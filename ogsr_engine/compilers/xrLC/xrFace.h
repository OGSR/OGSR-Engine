#pragma once

#include "shader_xrlc.h"
#include "r_light.h"

template<u32 range>
struct fixed16
{
	s16			_value;

	void		_w			(float a)	{ s32 _v=iFloor(a*32767.f/float(range));	clamp(_v,-32768,32767); _value=_v;	}
	float		_r			()			{ return float((_value/32767.f)*float(range));	}
};

#pragma pack(push,4)
class base_lighting
{
public:
	xr_vector<R_Light>		rgb;		// P,N	
	xr_vector<R_Light>		hemi;		// P,N	
	xr_vector<R_Light>		sun;		// P

	void					select		(xr_vector<R_Light>& dest, xr_vector<R_Light>& src, Fvector& P, float R);
	void					select		(base_lighting& from, Fvector& P, float R);
};
class base_color_c
{
public:
	Fvector					rgb;		// - all static lighting
	float					hemi;		// - hemisphere
	float					sun;		// - sun
	float					_tmp_;		// ???
	base_color_c()			{ rgb.set(0,0,0); hemi=0; sun=0; _tmp_=0;	}

	void					mul			(float s)									{	rgb.mul(s);	hemi*=s; sun*=s;				};
	void					add			(float s)									{	rgb.add(s);	hemi+=s; sun+=s;				};
	void					add			(base_color_c& s)							{	rgb.add(s.rgb);	hemi+=s.hemi; sun+=s.sun;	};
	void					scale		(int samples)								{	mul	(1.f/float(samples));					};
	void					max			(base_color_c& s)							{ 	rgb.max(s.rgb); hemi=_max(hemi,s.hemi); sun=_max(sun,s.sun); };
	void					lerp		(base_color_c& A, base_color_c& B, float s)	{ 	rgb.lerp(A.rgb,B.rgb,s); float is=1-s;  hemi=is*A.hemi+s*B.hemi; sun=is*A.sun+s*B.sun; };
};
class base_color
{
public:
	union{
		struct{
			fixed16<16>		r;
			fixed16<16>		g;
			fixed16<16>		b;
			fixed16<16>		h;
			fixed16<16>		s;
			fixed16<16>		t;
		};
		struct{
			float			_x;
			float			_y;
			float			_z;
		};
	};
	void					_set		(float x, float y, float z)	{
		_x					= x;
		_y					= y;
		_z					= z;
	}
	void					_set		(base_color_c& C)	{
		r._w(C.rgb.x);		g._w(C.rgb.y);		b._w(C.rgb.z);
		h._w(C.hemi);		s._w(C.sun);		t._w(C._tmp_);
	}
	void					_get		(base_color_c& C)	{
		C.rgb.x	=r._r();	C.rgb.y	=g._r();	C.rgb.z	=b._r();
		C.hemi	=h._r();	C.sun	=s._r();	C._tmp_	=t._r();
	}
};
IC	u8	u8_clr				(float a)	{ s32 _a = iFloor(a*255.f); clamp(_a,0,255); return u8(_a);		};

class base_Vertex
{
public: 
	Fvector					P;
	Fvector					N;
	base_color				C;			// all_lighting info
	int						handle;		// used in mesh-processing/optimization/conversion
public:
	base_Vertex()			{ }
	virtual ~base_Vertex()	= 0; 
};

class base_basis
{
public:
	u8						x,y,z;
	void	set				(Fvector	N)
	{
		N.normalize_safe	();
		N.add				(1.f);
		N.mul				(.5f*255.f);
		s32 nx				= iFloor(N.x);	clamp(nx,0,255);	x=u8(nx);
		s32 ny				= iFloor(N.y);	clamp(ny,0,255);	y=u8(ny);
		s32 nz				= iFloor(N.z);	clamp(nz,0,255);	z=u8(nz);
	}
	void	set				(float x, float y, float z)
	{
		Fvector	N			= { x,y,z };
		set					(N);
	}
	bool	similar			(const base_basis& o);
};
class base_Face
{
public: 
	base_basis				basis_tangent		[3];
	base_basis				basis_binormal		[3];

	u16						dwMaterial;			// index of material
	u16						dwMaterialGame;		// unique-id of game material (must persist up to game-CForm saving)

	struct					{
		u16					bSplitted			:		1;
		u16					bProcessed			:		1;
		u16					bOpaque				:		1;	// For ray-tracing speedup
		u16					bLocked				:		1;	// For tesselation
	}						flags;

	virtual	Shader_xrLC&	Shader				( );
	virtual void			CacheOpacity		( );
	virtual Fvector2*		getTC0				( ) = 0;

	base_Face();
	virtual ~base_Face() = 0; 
};		

class Vertex;
class Face;
class Material;
class Edge;
class CLightmap;

// Typedefs
typedef xr_vector<Vertex*>			vecVertex;
typedef vecVertex::iterator			vecVertexIt;

typedef xr_vector<Face*>			vecFace;
typedef vecFace::iterator			vecFaceIt;

typedef vecFace						vecAdj;
typedef vecAdj::iterator			vecAdjIt;

extern const int edge2idx			[3][2];

#include		"xrUVpoint.h"

class Vertex	: public base_Vertex
{
public:
	vecAdj		adjacent;

	Vertex*		CreateCopy_NOADJ	();
	IC	BOOL	similar				(Vertex &V, float eps)		{	return P.similar(V.P,eps);	}
	IC	Vertex*	CreateCopy			()							{	Vertex* V = CreateCopy_NOADJ();	V->adjacent = adjacent;	return V;	}
	IC	void	prep_add			(Face* F)					{	for (vecFaceIt I=adjacent.begin(); I!=adjacent.end(); I++)	if (F==(*I)) return;	adjacent.push_back(F);	}
	IC	void	prep_remove			(Face* F)					{	vecFaceIt	I = std::find(adjacent.begin(),adjacent.end(),F);	if (I!=adjacent.end())	adjacent.erase(I);	}
	void		normalFromAdj		();

	Vertex	();
	virtual ~Vertex();
};

struct _TCF {
	Fvector2			uv	[3];

	void	barycentric	(Fvector2 &P, float &u, float &v, float &w);
	IC void	barycentric	(Fvector2 &P, Fvector &B)		{	barycentric(P,B.x,B.y,B.z); }
	IC bool	isInside	(float u, float v, float w)		{	return (u>=0 && u<=1) && (v>=0 && v<=1) && (w>=0 && w<=1); }
	IC bool	isInside	(Fvector &B)					{	return	isInside	(B.x,B.y,B.z); }
	IC bool	isInside	(Fvector2 &P, Fvector &B)		{	barycentric(P,B);	return isInside(B);	}
};

class Face	: public base_Face
{
public:
	Vertex*					v[3];			// vertices
	Fvector					N;				// face normal
	svector<_TCF,2>			tc;				// TC

	void*					pDeflector;		// does the face has LM-UV map?
	CLightmap*				lmap_layer;

	virtual Fvector2*		getTC0			( ) { return tc[0].uv; }

	void					CalcNormal		();
	void					CalcNormal2		();

	//------------------------------//
	void					Failure			();
	void					Verify			();

	// Does the face contains this vertex?
	IC bool		VContains	(Vertex* pV)			{	return VIndex(pV)>=0;	};

	// Replace ONE vertex by ANOTHER
	IC void		VReplace	(Vertex* what, Vertex* to)
	{
		if (v[0]==what) { v[0]=to; what->prep_remove(this); to->prep_add(this); }
		if (v[1]==what) { v[1]=to; what->prep_remove(this); to->prep_add(this); }
		if (v[2]==what) { v[2]=to; what->prep_remove(this); to->prep_add(this); }
	};
	IC void		VReplace_not_remove(Vertex* what, Vertex* to)
	{
		if (v[0]==what) { v[0]=to; to->prep_add(this); }
		if (v[1]==what) { v[1]=to; to->prep_add(this); }
		if (v[2]==what) { v[2]=to; to->prep_add(this); }
	};
	IC	int		VIndex			(Vertex* pV)
	{
		if (v[0]==pV) return 0;
		if (v[1]==pV) return 1;
		if (v[2]==pV) return 2;
		return -1;
	};

	IC	void	SetVertex		(int idx, Vertex* V)
	{
		v[idx]=V; V->prep_add(this);
	};
	IC void		SetVertices		(Vertex *V1, Vertex *V2, Vertex *V3)
	{
		SetVertex(0,V1);
		SetVertex(1,V2);
		SetVertex(2,V3);
	};
	IC BOOL		isDegenerated	()
	{
		return (v[0]==v[1] || v[0]==v[2] || v[1]==v[2]);
	};
	IC float	EdgeLen			(int edge)
	{
		Vertex* V1 = v[edge2idx[edge][0]];
		Vertex* V2 = v[edge2idx[edge][1]];
		return V1->P.distance_to(V2->P);
	};
	IC void		EdgeVerts		(int e, Vertex** A, Vertex** B)
	{
		*A = v[edge2idx[e][0]];
		*B = v[edge2idx[e][1]];
	}

	float		CalcArea			();
	float		CalcMaxEdge			();
	void		CalcCenter			(Fvector &C);
	BOOL		RenderEqualTo		(Face *F);
	void		Unwarp				(int connect_edge, float u1, float v1, float u2, float v2);
	void		OA_Unwarp			();
	void		AddChannel			(Fvector2 &p1, Fvector2 &p2, Fvector2 &p3); 
	BOOL		hasImplicitLighting	();

	Face();
	virtual ~Face();
};

//
typedef poolSS<Vertex,8*1024>	poolVertices;
extern poolVertices				VertexPool;
typedef poolSS<Face,8*1024>		poolFaces;
extern poolFaces				FacePool;

extern bool						g_bUnregister;

#pragma pack(pop)
