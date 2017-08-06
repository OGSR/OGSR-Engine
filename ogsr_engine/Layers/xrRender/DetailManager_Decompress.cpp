#include "stdafx.h"
#pragma hdrstop

#include "DetailManager.h"
#include "cl_intersect.h"

#ifdef _EDITOR
#	include "scene.h"
#	include "sceneobject.h"
#	include "../../ETools/ETools.h"
#endif

//--------------------------------------------------- Decompression
IC float	Interpolate			(float* base,		u32 x, u32 y, u32 size)
{
	float	f	= float(size);
	float	fx	= float(x)/f; float ifx = 1.f-fx;
	float	fy	= float(y)/f; float ify = 1.f-fy;

	float	c01	= base[0]*ifx + base[1]*fx;
	float	c23	= base[2]*ifx + base[3]*fx;
	float	c02	= base[0]*ify + base[2]*fy;
	float	c13	= base[1]*ify + base[3]*fy;

	float	cx	= ify*c01 + fy*c23;
	float	cy	= ifx*c02 + fx*c13;
	return	(cx+cy)/2;
}

IC bool		InterpolateAndDither(float* alpha255,	u32 x, u32 y, u32 sx, u32 sy, u32 size, int dither[16][16] )
{
	clamp 	(x,(u32)0,size-1);
	clamp 	(y,(u32)0,size-1);
	int		c	= iFloor(Interpolate(alpha255,x,y,size)+.5f);
	clamp   (c,0,255);

	u32	row	= (y+sy) % 16;
	u32	col	= (x+sx) % 16;
	return	c	> dither[col][row];
}

void		CDetailManager::cache_Decompress(Slot* S)
{
	VERIFY				(S);
	Slot&	D			= *S;
	D.type				= stReady;
	if (D.empty)		return;

	DetailSlot&	DS		= QueryDB(D.sx,D.sz);

	// Select polygons
	Fvector		bC,bD;
	D.vis.box.get_CD	(bC,bD);

#ifdef _EDITOR
	ETOOLS::box_options	(CDB::OPT_FULL_TEST); 
	// Select polygons
	SBoxPickInfoVec		pinf;
    Scene->BoxPickObjects(D.vis.box,pinf,GetSnapList());
	u32	triCount		= pinf.size();
#else
	xrc.box_options		(CDB::OPT_FULL_TEST); 
	xrc.box_query		(g_pGameLevel->ObjectSpace.GetStaticModel(),bC,bD);
	u32	triCount		= xrc.r_count	();
	CDB::TRI*	tris	= g_pGameLevel->ObjectSpace.GetStaticTris();
	Fvector*	verts	= g_pGameLevel->ObjectSpace.GetStaticVerts();
#endif

	if (0==triCount)	return;

	// Build shading table
	float		alpha255	[dm_obj_in_slot][4];
	for (int i=0; i<dm_obj_in_slot; i++)
	{
		alpha255[i][0]	= 255.f*float(DS.palette[i].a0)/15.f;
		alpha255[i][1]	= 255.f*float(DS.palette[i].a1)/15.f;
		alpha255[i][2]	= 255.f*float(DS.palette[i].a2)/15.f;
		alpha255[i][3]	= 255.f*float(DS.palette[i].a3)/15.f;
	}

	// Prepare to selection
	float		density		= ps_r__Detail_density;
	float		jitter		= density/1.7f;
	u32			d_size		= iCeil	(dm_slot_size/density);
	svector<int,dm_obj_in_slot>		selected;

    u32 p_rnd	= D.sx*D.sz; // нужно для того чтобы убрать полосы(ряды)
	CRandom				r_selection	(0x12071980^p_rnd);
	CRandom				r_jitter	(0x12071980^p_rnd);
	CRandom				r_yaw		(0x12071980^p_rnd);
	CRandom				r_scale		(0x12071980^p_rnd);

	// Prepare to actual-bounds-calculations
	Fbox				Bounds;
	Bounds.invalidate	();

	// Decompressing itself
	for (u32 z=0; z<=d_size; z++)
	{
		for (u32 x=0; x<=d_size; x++)
		{
			// shift
			u32 shift_x =  r_jitter.randI(16);
			u32 shift_z =  r_jitter.randI(16);

			// Iterpolate and dither palette
			selected.clear();
			if ((DS.id0!=DetailSlot::ID_Empty)&& InterpolateAndDither(alpha255[0],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(0);
			if ((DS.id1!=DetailSlot::ID_Empty)&& InterpolateAndDither(alpha255[1],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(1);
			if ((DS.id2!=DetailSlot::ID_Empty)&& InterpolateAndDither(alpha255[2],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(2);
			if ((DS.id3!=DetailSlot::ID_Empty)&& InterpolateAndDither(alpha255[3],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(3);

			// Select
			if (selected.empty())	continue;
			u32 index;
			if (selected.size()==1)	index = selected[0];
			else					index = selected[r_selection.randI(selected.size())];

			CDetail*	Dobj	= objects[DS.r_id(index)];
			SlotItem*	ItemP	= poolSI.create();
			SlotItem&	Item	= *ItemP;

			// Position (XZ)
			float		rx = (float(x)/float(d_size))*dm_slot_size + D.vis.box.min.x;
			float		rz = (float(z)/float(d_size))*dm_slot_size + D.vis.box.min.z;
			Fvector		Item_P;
			Item_P.set	(rx + r_jitter.randFs(jitter), D.vis.box.max.y, rz + r_jitter.randFs(jitter));

			// Position (Y)
			float y		= D.vis.box.min.y-5;
			Fvector	dir; dir.set(0,-1,0);

			float		r_u,r_v,r_range;
			for (u32 tid=0; tid<triCount; tid++)
			{
#ifdef _EDITOR
				Fvector verts[3];
				SBoxPickInfo& I=pinf[tid];
				for (int k=0; k<(int)I.inf.size(); k++){
					VERIFY(I.s_obj);
Device.Statistic->TEST0.Begin	();
					I.e_obj->GetFaceWorld(I.s_obj->_Transform(),I.e_mesh,I.inf[k].id,verts);
Device.Statistic->TEST0.End		();
					if (CDB::TestRayTri(Item_P,dir,verts,r_u,r_v,r_range,TRUE))
					{
						if (r_range>=0)	{
							float y_test	= Item_P.y - r_range;
							if (y_test>y)	y = y_test;
						}
					}
				}
#else
				CDB::TRI&	T		= tris[xrc.r_begin()[tid].id];
				Fvector		Tv[3]	= { verts[T.verts[0]],verts[T.verts[1]],verts[T.verts[2]] };
				if (CDB::TestRayTri(Item_P,dir,Tv,r_u,r_v,r_range,TRUE))
				{
					if (r_range>=0)	{
						float y_test	= Item_P.y - r_range;
						if (y_test>y)	y = y_test;
					}
				}
#endif
			}
			if (y<D.vis.box.min.y)			continue;
			Item_P.y	= y;

			// Angles and scale
			Item.scale	= r_scale.randF		(Dobj->m_fMinScale*0.5f,Dobj->m_fMaxScale*0.9f);

			// X-Form BBox
			Fmatrix		mScale,mXform;
			Fbox		ItemBB;
			Item.mRotY.rotateY				(r_yaw.randF	(0,PI_MUL_2));
			Item.mRotY.translate_over		(Item_P);
			mScale.scale					(Item.scale,Item.scale,Item.scale);
			mXform.mul_43					(Item.mRotY,mScale);
			ItemBB.xform					(Dobj->bv_bb,mXform);
			Bounds.merge					(ItemBB);

			// Color
			/*
			DetailPalette*	c_pal			= (DetailPalette*)&DS.color;
			float gray255	[4];
			gray255[0]						=	255.f*float(c_pal->a0)/15.f;
			gray255[1]						=	255.f*float(c_pal->a1)/15.f;
			gray255[2]						=	255.f*float(c_pal->a2)/15.f;
			gray255[3]						=	255.f*float(c_pal->a3)/15.f;
			*/
			//float c_f						=	1.f;	//Interpolate		(gray255,x,z,d_size)+.5f;
			//int c_dw						=	255;	//iFloor			(c_f);
			//clamp							(c_dw,0,255);
			//Item.C_dw						=	color_rgba		(c_dw,c_dw,c_dw,255);
#if RENDER==R_R1
			Item.c_rgb.x					=	DS.r_qclr	(DS.c_r,	15);
			Item.c_rgb.y					=	DS.r_qclr	(DS.c_g,	15);
			Item.c_rgb.z					=	DS.r_qclr	(DS.c_b,	15);
#endif
			Item.c_hemi						=	DS.r_qclr	(DS.c_hemi,	15);
			Item.c_sun						=	DS.r_qclr	(DS.c_dir,	15);

			// KD
			Item.need_to_render_anyway[0] = (::Random.randI(100) > 15) ? true : false;
			Item.need_to_render_anyway[1] = (::Random.randI(100) > 30) ? true : false;
			Item.need_to_render_anyway[2] = (::Random.randI(100) > 45) ? true : false;

			//? hack: RGB = hemi
			//? Item.c_rgb.add					(ps_r__Detail_rainbow_hemi*Item.c_hemi);

			// Vis-sorting
			if (!UseVS())
			{
				// Always still on CPU pipe
				Item.vis_ID	= 0;
			} else {
				if (Dobj->m_Flags.is(DO_NO_WAVING))	Item.vis_ID	= 0;
				else
				{
					if (::Random.randI(0,3)==0)	Item.vis_ID	= 2;	// Second wave
					else						Item.vis_ID = 1;	// First wave
				}
			}

			// Save it
			D.G[index].items.push_back(ItemP);
		}
	}

	// Update bounds to more tight and real ones
	D.vis.clear			();
	D.vis.box.set		(Bounds);
	D.vis.box.getsphere	(D.vis.sphere.P,D.vis.sphere.R);
}
