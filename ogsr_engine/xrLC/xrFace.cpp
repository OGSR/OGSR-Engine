#include "stdafx.h"
#include "build.h"

bool	base_basis::similar			(const base_basis& o)
{
	s32 dx	= _abs(s32(x)-s32(o.x));
	s32 dy	= _abs(s32(y)-s32(o.y));
	s32 dz	= _abs(s32(z)-s32(o.z));
	return (dx<=1 && dy<=1 && dz<=1);
}


void	base_lighting::select	(xr_vector<R_Light>& dest, xr_vector<R_Light>& src, Fvector& P, float R)
{
	Fsphere		Sphere;
	Sphere.set	(P,R);
	dest.clear	();
	R_Light*	L			= &*src.begin();
	for (; L!=&*src.end(); L++)
	{
		if (L->type==LT_POINT) {
			float dist						= Sphere.P.distance_to(L->position);
			if (dist>(Sphere.R+L->range))	continue;
		}
		dest.push_back(*L);
	}
}
void	base_lighting::select	(base_lighting& from, Fvector& P, float R)
{
	select(rgb,from.rgb,P,R);
	select(hemi,from.hemi,P,R);
	select(sun,from.sun,P,R);
}

base_Vertex::~base_Vertex() {};

base_Face::base_Face()
{
	basis_tangent[0].set	(0,0,0);
	basis_tangent[1].set	(0,0,0);
	basis_tangent[2].set	(0,0,0);
	basis_binormal[0].set	(0,0,0);
	basis_binormal[1].set	(0,0,0);
	basis_binormal[2].set	(0,0,0);
}
base_Face::~base_Face()		{};

Shader_xrLC&	base_Face::Shader		()
{
	u32 shader_id = pBuild->materials[dwMaterial].reserved;
	return *(pBuild->shaders.Get(shader_id));
}
void			base_Face::CacheOpacity	()
{
	flags.bOpaque				= true;

	b_material& M		= pBuild->materials		[dwMaterial];
	b_texture&	T		= pBuild->textures		[M.surfidx];
	if (T.bHasAlpha)	flags.bOpaque = false;
	else				flags.bOpaque = true;
	if (!flags.bOpaque && (0==T.pSurface))		{
		flags.bOpaque	= true;
		clMsg			("Strange face detected... Has alpha without texture...");
	}
}

//
const int	edge2idx	[3][2]	= { {0,1},		{1,2},		{2,0}	};
const int	edge2idx3	[3][3]	= { {0,1,2},	{1,2,0},	{2,0,1}	};
const int	idx2edge	[3][3]  = {
	{-1,  0,  2},
	{ 0, -1,  1},
	{ 2,  1, -1}
};

volatile u32	dwInvalidFaces	= 0;

extern CBuild*	pBuild;

bool			g_bUnregister = true;

vecVertex		g_vertices;
vecFace			g_faces;

poolVertices	VertexPool;
poolFaces		FacePool;

Vertex::Vertex()
{
	g_vertices.push_back(this);
	adjacent.reserve	(4);
}
Vertex::~Vertex()
{
	if (g_bUnregister) {
		vecVertexIt F = std::find(g_vertices.begin(), g_vertices.end(), this);
		if (F!=g_vertices.end()) g_vertices.erase(F);
		else clMsg("* ERROR: Unregistered VERTEX destroyed");
	}
}
Vertex*	Vertex::CreateCopy_NOADJ()
{
	Vertex* V	= VertexPool.create();
	V->P.set	(P);
	V->N.set	(N);
	V->C		= C;
	return		V;
}

void	Vertex::normalFromAdj()
{
	N.set(0,0,0);
	for (vecFaceIt ad = adjacent.begin(); ad!=adjacent.end(); ad++)
		N.add( (*ad)->N );
	exact_normalize	(N);
}

void	_TCF::barycentric	(Fvector2 &P, float &u, float &v, float &w)
{
	Fvector2 	kV02; kV02.sub(uv[0],uv[2]);
	Fvector2 	kV12; kV12.sub(uv[1],uv[2]);
	Fvector2 	kPV2; kPV2.sub(P,    uv[2]);

	float		fM00 = kV02.dot(kV02);
	float		fM01 = kV02.dot(kV12);
	float		fM11 = kV12.dot(kV12);
	float		fR0  = kV02.dot(kPV2);
	float		fR1  = kV12.dot(kPV2);
	float		fDet = fM00*fM11 - fM01*fM01;

	u			= (fM11*fR0 - fM01*fR1)/fDet;
	v			= (fM00*fR1 - fM01*fR0)/fDet;
	w			= 1.0f - u - v;
}

Face::Face()
{
	pDeflector				= 0;
	flags.bSplitted			= false;
	g_faces.push_back		(this);
}
Face::~Face()
{
	if (g_bUnregister) {
		vecFaceIt F = std::find(g_faces.begin(), g_faces.end(), this);
		if (F!=g_faces.end()) g_faces.erase(F);
		else clMsg("* ERROR: Unregistered FACE destroyed");
	}
	// Remove 'this' from adjacency info in vertices
	for (int i=0; i<3; i++)
		v[i]->prep_remove(this);
}

#define VPUSH(a) a.x,a.y,a.z
void	Face::Failure		()
{
	dwInvalidFaces			++;

	clMsg		("* ERROR: Invalid face. (A=%f,e0=%f,e1=%f,e2=%f)",
		CalcArea(),
		v[0]->P.distance_to(v[1]->P),
		v[0]->P.distance_to(v[2]->P),
		v[1]->P.distance_to(v[2]->P)
		);
	clMsg		("*        v0[%f,%f,%f], v1[%f,%f,%f], v2[%f,%f,%f]",
		VPUSH(v[0]->P),
		VPUSH(v[1]->P),
		VPUSH(v[2]->P)
		);
	pBuild->err_invalid.w_fvector3	(v[0]->P);
	pBuild->err_invalid.w_fvector3	(v[1]->P);
	pBuild->err_invalid.w_fvector3	(v[2]->P);
}

void	Face::Verify		()
{
	// 1st :: area
	float	_a	= CalcArea();
	if		(!_valid(_a) || (_a<EPS))		{ Failure(); return; }

	// 2nd :: TC0
	Fvector2*	tc			= getTC0();
	float	eps				= .5f / 4096.f;		// half pixel from 4096 texture (0.0001220703125)
	float	e0				= tc[0].distance_to(tc[1]);	
	float	e1				= tc[1].distance_to(tc[2]);
	float	e2				= tc[2].distance_to(tc[0]);
	float	p				= e0+e1+e2;
	if		(!_valid(_a) || (p<eps))		{ Failure(); return; }

	// 3rd :: possibility to calc normal
	CalcNormal				();
	if (!_valid(N))			{ Failure(); return; }
}

void	Face::CalcNormal	()
{
	Fvector t1,t2;

	Fvector*	v0 = &(v[0]->P);
	Fvector*	v1 = &(v[1]->P);
	Fvector*	v2 = &(v[2]->P);
	t1.sub			(*v1,*v0);
	t2.sub			(*v2,*v1);
	N.crossproduct	(t1,t2);
	float mag		= N.magnitude();
	if (mag<EPS_S)
	{
		Fvector3		save_N	= N;
		if (exact_normalize(save_N))	N			=save_N;
		else							CalcNormal2	();
	} else {
		N.div		(mag);
		N.normalize	();
	}
};

void	Face::CalcNormal2	()
{
	FPU::m64r		();
	Dvector			v0,v1,v2,t1,t2,dN;
	v0.set			(v[0]->P);
	v1.set			(v[1]->P);
	v2.set			(v[2]->P);
	t1.sub			(v1,v0);
	t2.sub			(v2,v1);
	dN.crossproduct	(t1,t2);
	double mag		= dN.magnitude	();
	if (mag<dbl_zero)
	{
		Failure		();
		Dvector Nabs;
		Nabs.abs	(dN);

#define SIGN(a) ((a>=0.f)?1.f:-1.f)
		if (Nabs.x>Nabs.y && Nabs.x>Nabs.z)			N.set(SIGN(N.x),0.f,0.f);
		else if (Nabs.y>Nabs.x && Nabs.y>Nabs.z)	N.set(0.f,SIGN(N.y),0.f);
		else if (Nabs.z>Nabs.x && Nabs.z>Nabs.y)	N.set(0.f,0.f,SIGN(N.z));
		else {
			N.set	(0,1,0); 
		}
#undef SIGN
	} else {
		dN.div	(mag);
		N.set	(dN);
	}
}

float Face::CalcArea()
{
	float	e1 = v[0]->P.distance_to(v[1]->P);
	float	e2 = v[0]->P.distance_to(v[2]->P);
	float	e3 = v[1]->P.distance_to(v[2]->P);

	float	p  = (e1+e2+e3)/2.f;
	return	_sqrt( p*(p-e1)*(p-e2)*(p-e3) );
}
float Face::CalcMaxEdge()
{
	float	e1 = v[0]->P.distance_to(v[1]->P);
	float	e2 = v[0]->P.distance_to(v[2]->P);
	float	e3 = v[1]->P.distance_to(v[2]->P);

	if (e1>e2 && e1>e3) return e1;
	if (e2>e1 && e2>e3) return e2;
	return e3;
}

BOOL	Face::RenderEqualTo	(Face *F)
{
	if (F->dwMaterial	!= dwMaterial		)	return FALSE;
	//if (F->tc.size()	!= F->tc.size()		)	return FALSE;	// redundant???
	return TRUE;
}

void	Face::CalcCenter	(Fvector &C)
{
	C.set(v[0]->P);
	C.add(v[1]->P);
	C.add(v[2]->P);
	C.div(3);
};

void	Face::AddChannel	(Fvector2 &p1, Fvector2 &p2, Fvector2 &p3) 
{
	_TCF	TC;
	TC.uv[0] = p1;	TC.uv[1] = p2;	TC.uv[2] = p3;
	tc.push_back(TC);
}

BOOL	Face::hasImplicitLighting()
{
	if (0==this)								return FALSE;
	if (!Shader().flags.bRendering)				return FALSE;
	b_material& M		= pBuild->materials		[dwMaterial];
	b_BuildTexture&	T	= pBuild->textures		[M.surfidx];
	return (T.THM.flags.test(STextureParams::flImplicitLighted));
}

/*
BOOL	exact_normalize	(Fvector3& a)	{	return exact_normalize(&a.x);	}
BOOL	exact_normalize (float* a)
{
	double	sqr_magnitude	= a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	double	epsilon			= 1.192092896e-05F;
	if		(sqr_magnitude > epsilon)
	{
		double	l	=	rsqrt(sqr_magnitude);
		a[0]		*=	l;
		a[1]		*=	l;
		a[2]		*=	l;
		return		TRUE;
	}

	double a0,a1,a2,aa0,aa1,aa2,l;
	a0 = a[0];
	a1 = a[1];
	a2 = a[2];
	aa0 = _abs(a0);
	aa1 = _abs(a1);
	aa2 = _abs(a2);
	if (aa1 > aa0) {
		if (aa2 > aa1) {
			goto aa2_largest;
		}
		else {		// aa1 is largest
			a0 /= aa1;
			a2 /= aa1;
			l = rsqrt (a0*a0 + a2*a2 + 1);
			a[0] = a0*l;
			a[1] = (double)_copysign(l,a1);
			a[2] = a2*l;
		}
	}
	else {
		if (aa2 > aa0) {
aa2_largest:	// aa2 is largest
			a0 /= aa2;
			a1 /= aa2;
			l = rsqrt (a0*a0 + a1*a1 + 1);
			a[0] = a0*l;
			a[1] = a1*l;
			a[2] = (double)_copysign(l,a2);
		}
		else {		// aa0 is largest
			if (aa0 <= 0) {
				// dDEBUGMSG ("vector has zero size"); ... this messace is annoying
				a[0] = 0;	// if all a's are zero, this is where we'll end up.
				a[1] = 1;	// return a default unit length vector.
				a[2] = 0;
				return	FALSE;
			}
			a1 /= aa0;
			a2 /= aa0;
			l = rsqrt (a1*a1 + a2*a2 + 1);
			a[0] = (double)_copysign(l,a0);
			a[1] = a1*l;
			a[2] = a2*l;
		}
	}
	return	TRUE;
}
*/
