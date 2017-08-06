#include "stdafx.h"
#include "elight_def.h"

b_params				g_params;
extern volatile u32		dwInvalidFaces;

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

struct R_Control
{
	string64				name;
	xr_vector<u32>			data;
};
struct R_Layer
{
	R_Control				control;
	xr_vector<R_Light>		lights;
};

static bool is_thm_missing = false;
static bool is_tga_missing = false;

void CBuild::Load	(const b_params& Params, const IReader& _in_FS)
{
	IReader&	fs	= const_cast<IReader&>(_in_FS);
	// HANDLE		hLargeHeap	= HeapCreate(0,64*1024*1024,0);
	// clMsg		("* <LargeHeap> handle: %X",hLargeHeap);

	u32				i			= 0;

	float			p_total		= 0;
	float			p_cost		= 1.f/3.f;
	
	IReader*		F			= 0;

	// 
	string_path				sh_name;
	FS.update_path			(sh_name,"$game_data$","shaders_xrlc.xr");
	shaders.Load			(sh_name);

	//*******
	Status					("Vertices...");
	{
		F = fs.open_chunk		(EB_Vertices);
		u32 v_count			=	F->length()/sizeof(b_vertex);
		g_vertices.reserve		(3*v_count/2);
		scene_bb.invalidate		();
		for (i=0; i<v_count; i++)
		{
			Vertex*	pV			= VertexPool.create();
			F->r_fvector3		(pV->P);
			pV->N.set			(0,0,0);
			scene_bb.modify		(pV->P);
		}
		Progress			(p_total+=p_cost);
		clMsg				("* %16s: %d","vertices",g_vertices.size());
		F->close			();
	}

	//*******
	Status					("Faces...");
	{
		F = fs.open_chunk		(EB_Faces);
		R_ASSERT				(F);
		u32 f_count			=	F->length()/sizeof(b_face);
		g_faces.reserve			(f_count);
		for (i=0; i<f_count; i++)
		{
			try 
			{
				Face*	_F			= FacePool.create();
				b_face	B;
				F->r				(&B,sizeof(B));
				R_ASSERT			(B.dwMaterialGame<65536);

				_F->dwMaterial		= u16(B.dwMaterial);
				_F->dwMaterialGame	= B.dwMaterialGame;

				// Vertices and adjacement info
				for (u32 it=0; it<3; it++)
				{
					int id			= B.v[it];
					R_ASSERT		(id<(int)g_vertices.size());
					_F->SetVertex	(it,g_vertices[id]);
				}

				// transfer TC
				Fvector2				uv1,uv2,uv3;
				uv1.set				(B.t[0].x,B.t[0].y);
				uv2.set				(B.t[1].x,B.t[1].y);
				uv3.set				(B.t[2].x,B.t[2].y);
				_F->AddChannel		( uv1, uv2, uv3 );
			} catch (...)
			{
				err_save	();
				Debug.fatal	(DEBUG_INFO,"* ERROR: Can't process face #%d",i);
			}
		}
		Progress			(p_total+=p_cost);
		clMsg				("* %16s: %d","faces",g_faces.size());
		F->close			();

		if (dwInvalidFaces)	
		{
			err_save		();
			if (!b_skipinvalid)
				Debug.fatal		(DEBUG_INFO,"* FATAL: %d invalid faces. Compilation aborted",dwInvalidFaces);
			else
				clMsg		("* Total %d invalid faces. Do something.",dwInvalidFaces);
		}
	}

	//*******
	Status	("Models and References");
	F = fs.open_chunk		(EB_MU_models);
	if (F)
	{
		while (!F->eof())
		{
			mu_models.push_back				(xr_new<xrMU_Model>());
			mu_models.back()->Load			(*F);
		}
		F->close				();
	}
	F = fs.open_chunk		(EB_MU_refs);
	if (F)
	{
		while (!F->eof())
		{
			mu_refs.push_back				(xr_new<xrMU_Reference>());
			mu_refs.back()->Load			(*F);
		}		
		F->close				();
	}

	//*******
	Status	("Other transfer...");
	transfer("materials",	materials,			fs,		EB_Materials);
	transfer("shaders",		shader_render,		fs,		EB_Shaders_Render);
	transfer("shaders_xrlc",shader_compile,		fs,		EB_Shaders_Compile);
	transfer("glows",		glows,				fs,		EB_Glows);
	transfer("portals",		portals,			fs,		EB_Portals);
	transfer("LODs",		lods,				fs,		EB_LOD_models);

	// Load lights
	Status	("Loading lights...");
	{
		xr_vector<R_Layer>			L_layers;
		xr_vector<BYTE>				L_control_data;

		// Controlles/Layers
		{
			F = fs.open_chunk		(EB_Light_control);
			L_control_data.assign	(LPBYTE(F->pointer()),LPBYTE(F->pointer())+F->length());

			R_Layer					temp;

			while (!F->eof())
			{
				F->r				(temp.control.name,sizeof(temp.control.name));
				u32 cnt				= F->r_u32();
				temp.control.data.resize(cnt);
				F->r				(&*temp.control.data.begin(),cnt*sizeof(u32));

				L_layers.push_back	(temp);
			}

			F->close		();
		}
		// Static
		{
			F = fs.open_chunk	(EB_Light_static);
			b_light_static		temp;
			u32 cnt				= F->length()/sizeof(temp);
			for	(i=0; i<cnt; i++)
			{
				R_Light		RL;
				F->r		(&temp,sizeof(temp));
				Flight	L	= temp.data;

				// type
				if			(L.type == D3DLIGHT_DIRECTIONAL)	RL.type	= LT_DIRECT;
				else											
					RL.type = LT_POINT;
				RL.level	= 0;

				// split energy/color
				float			_e		=	(L.diffuse.r+L.diffuse.g+L.diffuse.b)/3.f;
				Fvector			_c		=	{L.diffuse.r,L.diffuse.g,L.diffuse.b};
				if (_abs(_e)>EPS_S)		_c.div	(_e);
				else					{ _c.set(0,0,0); _e=0; }

				// generic properties
				RL.diffuse.set				(_c);
				RL.position.set				(L.position);
				RL.direction.normalize_safe	(L.direction);
				RL.range				=	L.range*1.1f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	L.attenuation0;
				RL.attenuation1			=	L.attenuation1;
				RL.attenuation2			=	L.attenuation2;
				RL.energy				=	_e;

				// place into layer
				R_ASSERT	(temp.controller_ID<L_layers.size());
				L_layers	[temp.controller_ID].lights.push_back	(RL);
			}
			F->close		();
		}

		// ***Search LAYERS***
		for (u32 LH=0; LH<L_layers.size(); LH++)
		{
			R_Layer&	TEST	= L_layers[LH];
			if (0==stricmp(TEST.control.name,LCONTROL_HEMI))
			{
				// Hemi found
				L_static.hemi			= TEST.lights;
			}
			if (0==stricmp(TEST.control.name,LCONTROL_SUN))
			{
				// Sun found
				L_static.sun			= TEST.lights;
			}
			if (0==stricmp(TEST.control.name,LCONTROL_STATIC))
			{
				// Static found
				L_static.rgb			= TEST.lights;
			}
		}
		clMsg	("*lighting*: HEMI:   %d lights",L_static.hemi.size());
		clMsg	("*lighting*: SUN:    %d lights",L_static.sun.size());
		clMsg	("*lighting*: STATIC: %d lights",L_static.rgb.size());
		R_ASSERT(L_static.hemi.size());
		R_ASSERT(L_static.sun.size());
		R_ASSERT(L_static.rgb.size());

		// Dynamic
		transfer("d-lights",	L_dynamic,			fs,		EB_Light_dynamic);
	}
	
	// process textures
	Status			("Processing textures...");
	{
		Surface_Init		();
		F = fs.open_chunk	(EB_Textures);
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
				string_path			th_name;
#ifdef PRIQUEL
				FS.update_path	(th_name,"$game_textures$",strconcat(sizeof(th_name),th_name,N,".thm"));
#else // PRIQUEL
				FS.update_path	(th_name,"$textures$",strconcat(sizeof(th_name),th_name,N,".thm"));
#endif // PRIQUEL
//				clMsg			("processing: %s",th_name);			// commented by KD
				IReader* THM	= FS.r_open(th_name);

	// KD: first part of textures fix - start
	//  no nead to die if there is no texture
	//			R_ASSERT2		(THM,th_name);						// commented by KD
				if (!THM)
				{
					clMsg			("cannot find thm: %s",th_name);
					is_thm_missing = true;
					continue;
				} else
				{
					clMsg			("processing: %s",th_name);
				}
	// KD: first part of textures fix - end

				// version
				u32 version = 0;
				R_ASSERT2(THM->r_chunk(THM_CHUNK_VERSION,&version),th_name);
				// if( version!=THM_CURRENT_VERSION )	FATAL	("Unsupported version of THM file.");

				// analyze thumbnail information
				R_ASSERT2(THM->find_chunk(THM_CHUNK_TEXTUREPARAM),th_name);
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
				BT.dwWidth	= BT.THM.width;
				BT.dwHeight	= BT.THM.height;
				BT.bHasAlpha= BT.THM.HasAlphaChannel();
				if (!bLOD) 
				{
					if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted) || b_radiosity)
					{
						clMsg		("- loading: %s",N);
						u32			w=0, h=0;
						BT.pSurface = Surface_Load(N,w,h);
	//					R_ASSERT2	(BT.pSurface,"Can't load surface");
						if (!BT.pSurface)
						{
							clMsg			("cannot find tga texture: %s",th_name);
							is_tga_missing = true;
							continue;
						}
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
			textures.push_back	(BT);
		}
		// KD: last part of textures fix - start
		R_ASSERT2(!is_thm_missing,  "Some of required thm's are missing. See log for details.");
		R_ASSERT2(!is_tga_missing,  "Some of required tga_textures are missing. See log for details.");
		// KD: last part of textures fix - end
	}

	// post-process materials
	Status	("Post-process materials...");
	for (u32 m=0; m<materials.size(); m++)
	{
		b_material &M	= materials[m];

		if (65535==M.shader_xrlc)	{
			// No compiler shader
			M.reserved	= u16(-1);
			// clMsg	(" *  %20s",shader_render[M.shader].name);
		} else {
			// clMsg	(" *  %20s / %-20s",shader_render[M.shader].name, shader_compile[M.shader_xrlc].name);
			int id = shaders.GetID(shader_compile[M.shader_xrlc].name);
			if (id<0) {
				clMsg	("ERROR: Shader '%s' not found in library",shader_compile[M.shader].name);
				R_ASSERT(id>=0);
			}
			M.reserved = u16(id);
		}
	}
	Progress(p_total+=p_cost);

	// Parameter block
	CopyMemory(&g_params,&Params,sizeof(b_params));

	// 
	clMsg	("* sizes: V(%d),F(%d)",sizeof(Vertex),sizeof(Face));
}

