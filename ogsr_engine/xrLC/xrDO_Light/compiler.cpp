#include "stdafx.h"
#include "xrlevel.h"
#include "shader_xrlc.h"
#include "communicate.h"
#include "xrThread.h"
#include "detailformat.h"
#include "xrhemisphere.h"
#include "cl_intersect.h"
#include "Etextureparams.h"
#include "r_light.h"
#include "global_options.h"

#define NUM_THREADS		3

enum
{
	LP_DEFAULT			= 0,
	LP_UseFaceDisable	= (1<<0),
	LP_dont_rgb			= (1<<1),
	LP_dont_hemi		= (1<<2),
	LP_dont_sun			= (1<<3),
};

// KD start
BOOL					b_norgb		= FALSE;
BOOL					b_nosun		= FALSE;
// KD end

float	color_intensity	(Fcolor& c)
{
	float	ntsc		= c.r * 0.2125f + c.g * 0.7154f + c.b * 0.0721f;
	float	absolute	= c.magnitude_rgb() / 1.7320508075688772935274463415059f;
	return	ntsc*0.5f + absolute*0.5f;
}
class base_lighting
{
public:
	xr_vector<R_Light>		rgb;		// P,N	
	xr_vector<R_Light>		hemi;		// P,N	
	xr_vector<R_Light>		sun;		// P

	void					select		(xr_vector<R_Light>& dest, xr_vector<R_Light>& src, Fvector& P, float R);
	void					select		(base_lighting& from, Fvector& P, float R);
};
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

class base_color
{
public:
	Fvector					rgb;		// - all static lighting
	float					hemi;		// - hemisphere
	float					sun;		// - sun
	float					_tmp_;		// ???
	base_color()			{ rgb.set(0,0,0); hemi=0; sun=0; _tmp_=0;	}

	void					mul			(float s)									{	rgb.mul(s);	hemi*=s; sun*=s;				};
	void					add			(float s)									{	rgb.add(s);	hemi+=s; sun+=s;				};
	void					add			(base_color& s)								{	rgb.add(s.rgb);	hemi+=s.hemi; sun+=s.sun;	};
	void					scale		(int samples)								{	mul	(1.f/float(samples));					};
	void					max			(base_color& s)								{ 	rgb.max(s.rgb); hemi=_max(hemi,s.hemi); sun=_max(sun,s.sun); };
	void					lerp		(base_color& A, base_color& B, float s)		{ 	rgb.lerp(A.rgb,B.rgb,s); float is=1-s;  hemi=is*A.hemi+s*B.hemi; sun=is*A.sun+s*B.sun; };
};
IC	u8	u8_clr				(float a)	{ s32 _a = iFloor(a*255.f); clamp(_a,0,255); return u8(_a);		};


//-----------------------------------------------------------------------------------------------------------------
const int	LIGHT_Count				=	7;

//-----------------------------------------------------------------
__declspec(thread)		u64			t_start	= 0;
__declspec(thread)		u64			t_time	= 0;
__declspec(thread)		u64			t_count	= 0;

struct b_BuildTexture : public b_texture
{
	STextureParams	THM;

	u32&	Texel	(u32 x, u32 y)
	{
		return pSurface[y*dwWidth+x];
	}
	void	Vflip		()
	{
		R_ASSERT(pSurface);
		for (u32 y=0; y<dwHeight/2; y++)
		{
			u32 y2 = dwHeight-y-1;
			for (u32 x=0; x<dwWidth; x++) 
			{
				u32		t	= Texel(x,y);
				Texel	(x,y)	= Texel(x,y2);
				Texel	(x,y2)	= t;
			}
		}
	}
};

//-----------------------------------------------------------------
base_lighting				g_lights;
CDB::MODEL					RCAST_Model;
Fbox						LevelBB;
CVirtualFileRW*				dtFS=0;
DetailHeader				dtH;
DetailSlot*					dtS;

Shader_xrLC_LIB*			g_shaders_xrlc;

b_params					g_params;

xr_vector<b_material>		g_materials;
xr_vector<b_shader>			g_shader_render;
xr_vector<b_shader>			g_shader_compile;
xr_vector<b_BuildTexture>	g_textures;
xr_vector<b_rc_face>		g_rc_faces;

//-----------------------------------------------------------------
template <class T>
void transfer(const char *name, xr_vector<T> &dest, IReader& F, u32 chunk)
{
	IReader*	O	= F.open_chunk(chunk);
	u32		count	= O?(O->length()/sizeof(T)):0;
	clMsg			("* %16s: %d",name,count);
	if (count)  
	{
		dest.reserve(count);
		dest.insert	(dest.begin(), (T*)O->pointer(), (T*)O->pointer() + count);
	}
	if (O)		O->close	();
}

extern u32*		Surface_Load	(char* name, u32& w, u32& h);
extern void		Surface_Init	();

// 
void xrLoad(LPCSTR name)
{
	string_path					N;
	FS.update_path				(N,"$game_data$","shaders_xrlc.xr");
	g_shaders_xrlc				= xr_new<Shader_xrLC_LIB> ();
	g_shaders_xrlc->Load		(N);

	// Load CFORM
	{
		FS.update_path			(N,"$level$","build.cform");
		IReader*			fs = FS.r_open("$level$","build.cform");
		
		R_ASSERT			(fs->find_chunk(0));
		hdrCFORM			H;
		fs->r				(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)fs->pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		RCAST_Model.build	( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",RCAST_Model.memory()/1024);

		g_rc_faces.resize	(H.facecount);
		R_ASSERT(fs->find_chunk(1));
		fs->r				(&*g_rc_faces.begin(),g_rc_faces.size()*sizeof(b_rc_face));

		LevelBB.set			(H.aabb);
	}
	
	// Load .details
	{
		// copy
		IReader*	R		= FS.r_open	("$level$","build.details");
		IWriter*	W		= FS.w_open	("$level$","level.details");
		W->w				(R->pointer(),R->length());
		FS.w_close			(W);
		FS.r_close			(R);

		// re-open
		FS.update_path		(N,"$level$","level.details");
		dtFS				= xr_new<CVirtualFileRW> (N);
		dtFS->r_chunk		(0,&dtH);
		R_ASSERT			(dtH.version==DETAIL_VERSION);

		dtFS->find_chunk	(2);
		dtS					= (DetailSlot*)dtFS->pointer();
	}

	// Lights
	{
		IReader*			fs = FS.r_open("$level$","build.lights");
		IReader*			F;	u32 cnt; R_Light* L;

		// rgb
		F		=			fs->open_chunk		(0);
		cnt		=			F->length()/sizeof(R_Light);
		L		=			(R_Light*)F->pointer();
		g_lights.rgb.assign	(L,L+cnt);
		F->close			();

		// hemi
		F		=			fs->open_chunk		(1);
		cnt		=			F->length()/sizeof(R_Light);
		L		=			(R_Light*)F->pointer();
		g_lights.hemi.assign(L,L+cnt);
		F->close			();

		// sun
		F		=			fs->open_chunk		(2);
		cnt		=			F->length()/sizeof(R_Light);
		L		=			(R_Light*)F->pointer();
		g_lights.sun.assign	(L,L+cnt);
		F->close			();

		FS.r_close			(fs);
	}

	
	// Load level data
	{
		IReader*	fs		= FS.r_open ("$level$","build.prj");
		IReader*	F;

		// Version
		u32 version;
		fs->r_chunk			(EB_Version,&version);
		R_ASSERT(XRCL_CURRENT_VERSION==version);

		// Header
		fs->r_chunk			(EB_Parameters,&g_params);

		// Load level data
		transfer("materials",	g_materials,			*fs,		EB_Materials);
		transfer("shaders_xrlc",g_shader_compile,		*fs,		EB_Shaders_Compile);

		// process textures
		Status			("Processing textures...");
		{
			Surface_Init		();
			F = fs->open_chunk	(EB_Textures);
#ifdef _WIN64
			u32 tex_count	= F->length()/sizeof(help_b_texture);
#else
			u32 tex_count	= F->length()/sizeof(b_texture);
#endif
			for (u32 t=0; t<tex_count; t++)
			{
				Progress		(float(t)/float(tex_count));

#ifdef _WIN64
				// workaround for ptr size mismatching
				help_b_texture	TEX;
				F->r			(&TEX,sizeof(TEX));

				b_BuildTexture	BT;
				CopyMemory		(&BT,&TEX,sizeof(TEX) - 4);	// ptr should be copied separately
				BT.pSurface		= (u32 *)TEX.pSurface;
#else
				b_texture		TEX;
				F->r			(&TEX,sizeof(TEX));

				b_BuildTexture	BT;
				CopyMemory		(&BT,&TEX,sizeof(TEX));
#endif

				// load thumbnail
				LPSTR N			= BT.name;
				if (strchr(N,'.')) *(strchr(N,'.')) = 0;
				strlwr			(N);

				if (0==xr_strcmp(N,"level_lods"))	{
					// HACK for merged lod textures
					BT.dwWidth	= 1024;
					BT.dwHeight	= 1024;
					BT.bHasAlpha= TRUE;
					BT.pSurface	= 0;
				} else {
					strcat			(N,".thm");
					IReader* THM	= FS.r_open("$game_textures$",N);
					R_ASSERT2		(THM,	N);

					// version
					u32 version				= 0;
					R_ASSERT				(THM->r_chunk(THM_CHUNK_VERSION,&version));
					// if( version!=THM_CURRENT_VERSION )	FATAL	("Unsupported version of THM file.");

					// analyze thumbnail information
					R_ASSERT(THM->find_chunk(THM_CHUNK_TEXTUREPARAM));
					THM->r                  (&BT.THM.fmt,sizeof(STextureParams::ETFormat));
					BT.THM.flags.assign		(THM->r_u32());
					BT.THM.border_color		= THM->r_u32();
					BT.THM.fade_color		= THM->r_u32();
					BT.THM.fade_amount		= THM->r_u32();
					BT.THM.mip_filter		= THM->r_u32();
					BT.THM.width			= THM->r_u32();
					BT.THM.height           = THM->r_u32();
					BOOL			bLOD=FALSE;
					if (N[0]=='l' && N[1]=='o' && N[2]=='d' && N[3]=='\\') bLOD = TRUE;

					// load surface if it has an alpha channel or has "implicit lighting" flag
					BT.dwWidth				= BT.THM.width;
					BT.dwHeight				= BT.THM.height;
					BT.bHasAlpha			= BT.THM.HasAlphaChannel();
					BT.pSurface				= 0;
					if (!bLOD) 
					{
						if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
						{
							clMsg		("- loading: %s",N);
							u32			w=0, h=0;
							BT.pSurface = Surface_Load(N,w,h); 
							R_ASSERT2	(BT.pSurface,"Can't load surface");
							// KD: in case of thm doesn't correspond to texture let's reset thm params to actual texture ones
							if ((w != BT.dwWidth) || (h != BT.dwHeight))
							{
								Msg		("! THM doesn't correspond to the texture: %dx%d -> %dx%d, reseting", BT.dwWidth, BT.dwHeight, w, h);
								BT.dwWidth = w;
								BT.dwHeight = h;
							}
							BT.Vflip	();
						} else {
							// Free surface memory
						}
					}
				}

				// save all the stuff we've created
				g_textures.push_back	(BT);
			}
		}
	}
}

IC bool RayPick(CDB::COLLIDER& DB, Fvector& P, Fvector& D, float r, R_Light& L)
{
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = CDB::TestRayTri(P,D,L.tri,_u,_v,range,true);
	if (res) {
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	t_start			= CPU::GetCLK();
	DB.ray_query	(&RCAST_Model,P,D,r);
	t_time			+=	CPU::GetCLK()-t_start-CPU::clk_overhead;
	t_count			+=	1;
	
	// 3. Analyze
	if (0==DB.r_count()) {
		return false;
	} else {
		// cache polygon
		CDB::RESULT&	rpinf	= *DB.r_begin();
		CDB::TRI&		T		= RCAST_Model.get_tris()[rpinf.id];
		L.tri[0].set	(rpinf.verts[0]);
		L.tri[1].set	(rpinf.verts[1]);
		L.tri[2].set	(rpinf.verts[2]);
		return true;
	}
}

float getLastRP_Scale(CDB::COLLIDER* DB, R_Light& L)//, Face* skip)
{
	u32	tris_count		= DB->r_count();
	float	scale		= 1.f;
	Fvector B;

//	X_TRY 
	{
		for (u32 I=0; I<tris_count; I++)
		{
			CDB::RESULT& rpinf = DB->r_begin()[I];
			// Access to texture
			CDB::TRI& clT								= RCAST_Model.get_tris()[rpinf.id];
			b_rc_face& F								= g_rc_faces[rpinf.id];
//			if (0==F)									continue;
//			if (skip==F)								continue;

			b_material& M	= g_materials				[F.dwMaterial];
			b_texture&	T	= g_textures				[M.surfidx];
			Shader_xrLCVec&	LIB = 		g_shaders_xrlc->Library	();
			if (M.shader_xrlc>=LIB.size()) return		0;		//. hack
			Shader_xrLC& SH	= LIB						[M.shader_xrlc];
			if (!SH.flags.bLIGHT_CastShadow)			continue;

			if (0==T.pSurface)	T.bHasAlpha = FALSE;
			if (!T.bHasAlpha)	{
				// Opaque poly - cache it
				L.tri[0].set	(rpinf.verts[0]);
				L.tri[1].set	(rpinf.verts[1]);
				L.tri[2].set	(rpinf.verts[2]);
				return 0;
			}

			// barycentric coords
			// note: W,U,V order
			B.set	(1.0f - rpinf.u - rpinf.v,rpinf.u,rpinf.v);

			// calc UV
			Fvector2*	cuv = F.t;
			Fvector2	uv;
			uv.x = cuv[0].x*B.x + cuv[1].x*B.y + cuv[2].x*B.z;
			uv.y = cuv[0].y*B.x + cuv[1].y*B.y + cuv[2].y*B.z;

			int U = iFloor(uv.x*float(T.dwWidth) + .5f);
			int V = iFloor(uv.y*float(T.dwHeight)+ .5f);
			U %= T.dwWidth;		if (U<0) U+=T.dwWidth;
			V %= T.dwHeight;	if (V<0) V+=T.dwHeight;

			u32 pixel		= T.pSurface[V*T.dwWidth+U];
			u32 pixel_a		= color_get_A(pixel);
			float opac		= 1.f - float(pixel_a)/255.f;
			scale			*= opac;
		}
	} 
//	X_CATCH
//	{
//		clMsg("* ERROR: getLastRP_Scale");
//	}

	return scale;
}

float rayTrace	(CDB::COLLIDER* DB, R_Light& L, Fvector& P, Fvector& D, float R)//, Face* skip)
{
	R_ASSERT	(DB);

	// 1. Check cached polygon
	float _u,_v,range;
	bool res = CDB::TestRayTri(P,D,L.tri,_u,_v,range,false);
	if (res) {
		if (range>0 && range<R) return 0;
	}

	// 2. Polygon doesn't pick - real database query
	DB->ray_query	(&RCAST_Model,P,D,R);

	// 3. Analyze polygons and cache nearest if possible
	if (0==DB->r_count()) {
		return 1;
	} else {
		return getLastRP_Scale(DB,L);//,skip);
	}
	return 0;
}

void LightPoint(CDB::COLLIDER* DB, base_color &C, Fvector &P, Fvector &N, base_lighting& lights, u32 flags)
{
	Fvector		Ldir,Pnew;
	Pnew.mad	(P,N,0.01f);

	if (0==(flags&LP_dont_rgb))
	{
		R_Light	*L	= &*lights.rgb.begin(), *E = &*lights.rgb.end();
		for (;L!=E; L++)
		{
			if (L->type==LT_DIRECT) {
				// Cos
				Ldir.invert	(L->direction);
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float scale	=	D*L->energy*rayTrace(DB,*L,Pnew,Ldir,1000.f);
				C.rgb.x		+=	scale * L->diffuse.x; 
				C.rgb.y		+=	scale * L->diffuse.y;
				C.rgb.z		+=	scale * L->diffuse.z;
			} else {
				// Distance
				float sqD	=	P.distance_to_sqr	(L->position);
				if (sqD > L->range2) continue;

				// Dir
				Ldir.sub	(L->position,P);
				Ldir.normalize_safe();
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float R		= _sqrt(sqD);
				float scale = D*L->energy*rayTrace(DB,*L,Pnew,Ldir,R);
				float A		= scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

				C.rgb.x += A * L->diffuse.x;
				C.rgb.y += A * L->diffuse.y;
				C.rgb.z += A * L->diffuse.z;
			}
		}
	}
	if (0==(flags&LP_dont_sun))
	{
		R_Light	*L	= &*(lights.sun.begin()), *E = &*(lights.sun.end());
		for (;L!=E; L++)
		{
			if (L->type==LT_DIRECT) {
				// Cos
				Ldir.invert	(L->direction);
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float scale	=	L->energy*rayTrace(DB,*L,Pnew,Ldir,1000.f);
				C.sun		+=	scale;
			} else {
				// Distance
				float sqD	=	P.distance_to_sqr(L->position);
				if (sqD > L->range2) continue;

				// Dir
				Ldir.sub			(L->position,P);
				Ldir.normalize_safe	();
				float D				= Ldir.dotproduct( N );
				if( D <=0 )			continue;

				// Trace Light
				float R		=	_sqrt(sqD);
				float scale =	D*L->energy*rayTrace(DB,*L,Pnew,Ldir,R);
				float A		=	scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

				C.sun		+=	A;
			}
		}
	}
	if (0==(flags&LP_dont_hemi))
	{
		R_Light	*L	= &*lights.hemi.begin(), *E = &*lights.hemi.end();
		for (;L!=E; L++)
		{
			if (L->type==LT_DIRECT) {
				// Cos
				Ldir.invert	(L->direction);
				float D		= Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				Fvector		PMoved;	PMoved.mad	(Pnew,Ldir,0.001f);
				float scale	=	L->energy*rayTrace(DB,*L,PMoved,Ldir,1000.f);
				C.hemi		+=	scale;
			} else {
				// Distance
				float sqD	=	P.distance_to_sqr(L->position);
				if (sqD > L->range2) continue;

				// Dir
				Ldir.sub			(L->position,P);
				Ldir.normalize_safe	();
				float D		=	Ldir.dotproduct( N );
				if( D <=0 ) continue;

				// Trace Light
				float R		=	_sqrt(sqD);
				float scale =	D*L->energy*rayTrace(DB,*L,Pnew,Ldir,R);
				float A		=	scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);

				C.hemi		+=	A;
			}
		}
	}
}

DEFINE_VECTOR(u32,DWORDVec,DWORDIt);
class	LightThread : public CThread
{
	u32		Nstart, Nend;
	DWORDVec	box_result;
public:
	LightThread			(u32 ID, u32 _start, u32 _end) : CThread(ID)
	{
		Nstart	= _start;
		Nend	= _end;
	}
	IC float			fromSlotX		(int x)		
	{
		return (x-dtH.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;
	}
	IC float			fromSlotZ		(int z)		
	{
		return (z-dtH.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;
	}
	void				GetSlotRect		(Frect& rect, int sx, int sz)
	{
		float x 		= fromSlotX(sx);
		float z 		= fromSlotZ(sz);
		rect.x1			= x-DETAIL_SLOT_SIZE_2+EPS_L;
		rect.y1			= z-DETAIL_SLOT_SIZE_2+EPS_L;
		rect.x2			= x+DETAIL_SLOT_SIZE_2-EPS_L;
		rect.y2			= z+DETAIL_SLOT_SIZE_2-EPS_L;
	}
	virtual void		Execute()
	{
//		DetailSlot::verify	();
		CDB::COLLIDER		DB;
		DB.ray_options		(CDB::OPT_CULL		);
		DB.box_options		(CDB::OPT_FULL_TEST	);

		base_lighting		Selected;
		for (u32 _z=Nstart; _z<Nend; _z++)
		{
			for (u32 _x=0; _x<dtH.size_x; _x++)
			{
				DetailSlot&	DS = dtS[_z*dtH.size_x+_x];

				if ((DS.id0==DetailSlot::ID_Empty)&&(DS.id1==DetailSlot::ID_Empty)&&(DS.id2==DetailSlot::ID_Empty)&&(DS.id3==DetailSlot::ID_Empty))
					continue;

				// Build slot BB & sphere
				int slt_z	= int(_z)-int(dtH.offs_z);
				int slt_x	= int(_x)-int(dtH.offs_x);
				
				Fbox		BB;
				BB.min.set	(slt_x*DETAIL_SLOT_SIZE,	DS.r_ybase(),				slt_z*DETAIL_SLOT_SIZE);
				BB.max.set	(BB.min.x+DETAIL_SLOT_SIZE,	DS.r_ybase()+DS.r_yheight(),BB.min.z+DETAIL_SLOT_SIZE);
				BB.grow		(0.05f);

				Fsphere		S;
				BB.getsphere(S.P,S.R);
				
				// Select polygons
				Fvector				bbC,bbD;
				BB.get_CD			(bbC,bbD);	bbD.add(0.01f);
				DB.box_query		(&RCAST_Model,bbC,bbD);

				box_result.clear	();
				for (CDB::RESULT* I=DB.r_begin(); I!=DB.r_end(); I++) box_result.push_back(I->id);
				if (box_result.empty())	continue;

				CDB::TRI*	tris	= RCAST_Model.get_tris();
				Fvector*	verts	= RCAST_Model.get_verts();
				
				// select lights
				Selected.select		(g_lights,S.P,S.R);
				
				// lighting itself
				base_color		amount;
				u32				count	= 0;
				float coeff		= DETAIL_SLOT_SIZE_2/float(LIGHT_Count);
				FPU::m64r		();
				for (int x=-LIGHT_Count; x<=LIGHT_Count; x++) 
				{
					Fvector		P;
					P.x			= bbC.x + coeff*float(x);

					for (int z=-LIGHT_Count; z<=LIGHT_Count; z++) 
					{
						// compute position
						Fvector t_n;	t_n.set(0,1,0);
						P.z				= bbC.z + coeff*float(z);
						P.y				= BB.min.y-5;
						Fvector	dir;	dir.set		(0,-1,0);
						Fvector start;	start.set	(P.x,BB.max.y+EPS,P.z);
						
						float		r_u,r_v,r_range;
						for (DWORDIt tit=box_result.begin(); tit!=box_result.end(); tit++)
						{
							CDB::TRI&	T		= tris	[*tit];
							Fvector		V[3]	= { verts[T.verts[0]], verts[T.verts[1]], verts[T.verts[2]] };
							if (CDB::TestRayTri(start,dir,V,r_u,r_v,r_range,TRUE))
							{
								if (r_range>=0.f)	{
									float y_test	= start.y - r_range;
									if (y_test>P.y)	{
										P.y			= y_test+EPS;
										t_n.mknormal(V[0],V[1],V[2]);
									}
								}
							}
						}
						if (P.y<BB.min.y) continue;
						
						// light point
						LightPoint		(&DB,amount,P,t_n,Selected,(b_norgb?LP_dont_rgb:0)|(b_nosun?LP_dont_sun:0)|LP_DEFAULT);
						count			+= 1;
					}
				}
				
				// calculation of luminocity
				amount.scale		(count);
				amount.mul			(.5f);
				DS.c_dir			= DS.w_qclr	(amount.sun,15);
				DS.c_hemi			= DS.w_qclr	(amount.hemi,15);
				DS.c_r				= DS.w_qclr	(amount.rgb.x,15);
				DS.c_g				= DS.w_qclr	(amount.rgb.y,15);
				DS.c_b				= DS.w_qclr	(amount.rgb.z,15);
				thProgress			= float(_z-Nstart)/float(Nend-Nstart);
				thPerformance		= float(double(t_count)/double(t_time*CPU::clk_to_seconds))/1000.f;
			}
		}
	}
};

void	xrLight			()
{
	u32	range				= dtH.size_z;

	// Start threads, wait, continue --- perform all the work
	CThreadManager		Threads;
	CTimer				start_time;
	u32	stride			= range/NUM_THREADS;
	u32	last			= range-stride*	(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)	{
		CThread*	T		= xr_new<LightThread> (thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride));
		T->thMessages		= FALSE;
		T->thMonitor		= FALSE;
		Threads.start		(T);
	}
	Threads.wait			();
	Msg						("%d seconds elapsed.",(start_time.GetElapsed_ms())/1000);
}

void xrCompiler(LPCSTR name)
{
	Phase		("Loading level...");
	xrLoad		(name);

	Phase		("Lighting nodes...");
	xrLight		();

	if (dtFS)	xr_delete(dtFS);
}
