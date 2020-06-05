#include "stdafx.h"
#include "igame_level.h"

#include "xr_object.h"
#include "xr_area.h"
#include "render.h"
#include "xrLevel.h"

#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/Kinematics.h"

#include "x_ray.h"
#include "GameFont.h"

void CObject::MakeMeCrow_internal	()
{
	g_pGameLevel->Objects.o_crow	(this);
}

void CObject::cName_set			(shared_str N)
{ 
	NameObject	=	N; 
}
void CObject::cNameSect_set		(shared_str N)
{ 
	NameSection	=	N; 
}

//#include "SkeletonCustom.h"
void CObject::cNameVisual_set(shared_str N)
{
	// check if equal
	if (*N && *NameVisual)
		if (N == NameVisual)		return;

	// replace model
	if (*N && N[0])
	{
		IRenderVisual			*old_v = renderable.visual;

		NameVisual = N;
		renderable.visual = Render->model_Create(*N);

		IKinematics* old_k = old_v ? old_v->dcast_PKinematics() : NULL;
		IKinematics* new_k = renderable.visual->dcast_PKinematics();

		/*
		if(old_k && new_k){
			new_k->Update_Callback			= old_k->Update_Callback;
			new_k->Update_Callback_Param	= old_k->Update_Callback_Param;
		}
		*/
		if (old_k && new_k)
		{
			new_k->SetUpdateCallback(old_k->GetUpdateCallback());
			new_k->SetUpdateCallbackParam(old_k->GetUpdateCallbackParam());
		}

		::Render->model_Delete(old_v);
	}
	else
	{
		::Render->model_Delete(renderable.visual);
		NameVisual = 0;
	}
	OnChangeVisual();
}

// flagging
void CObject::processing_activate	()
{
	VERIFY3	(255!= Props.bActiveCounter, "Invalid sequence of processing enable/disable calls: overflow",*cName());
	Props.bActiveCounter			++;
	if (0==(Props.bActiveCounter-1))	g_pGameLevel->Objects.o_activate	(this);
}
void CObject::processing_deactivate	()
{
	VERIFY3	(0	!= Props.bActiveCounter, "Invalid sequence of processing enable/disable calls: underflow",*cName());
	Props.bActiveCounter			--;
	if (0==Props.bActiveCounter)		g_pGameLevel->Objects.o_sleep		(this);
}

void CObject::setEnabled			(BOOL _enabled)
{
	if (_enabled){
		Props.bEnabled							=	1;	
		if (collidable.model)	spatial.type	|=	STYPE_COLLIDEABLE;
	}else{
		Props.bEnabled							=	0;
		spatial.type							&=	~STYPE_COLLIDEABLE;
	}
}
void CObject::setVisible			(BOOL _visible)
{
	if (_visible){				// Parent should control object visibility itself (??????)
		Props.bVisible							= 1;
		if (renderable.visual)	spatial.type	|=	STYPE_RENDERABLE;
	}else{
		Props.bVisible							= 0;
		spatial.type							&=	~STYPE_RENDERABLE;
	}
}

void CObject::Center(Fvector& C) const {
	ASSERT_FMT(renderable.visual, "[%s]: %s[%u] has no renderable.visual", __FUNCTION__, cName().c_str(), ID());
	renderable.xform.transform_tiny(C, renderable.visual->getVisData().sphere.P);
}
float CObject::Radius() const {
	ASSERT_FMT(renderable.visual, "[%s]: %s[%u] has no renderable.visual", __FUNCTION__, cName().c_str(), ID());
	return renderable.visual->getVisData().sphere.R;
}
const Fbox& CObject::BoundingBox() const {
	ASSERT_FMT(renderable.visual, "[%s]: %s[%u] has no renderable.visual", __FUNCTION__, cName().c_str(), ID());
	return renderable.visual->getVisData().box;
}

//----------------------------------------------------------------------
// Class	: CXR_Object
// Purpose	:
//----------------------------------------------------------------------
CObject::CObject		( )		: ISpatial(g_SpatialSpace)
{
	// Transform
	Props.storage				= 0;

	Parent						= NULL;

	NameObject					= NULL;
	NameSection					= NULL;
	NameVisual					= NULL;

#ifdef DEBUG
	dbg_update_shedule			= u32(-1)/2;
	dbg_update_cl				= u32(-1)/2;
#endif
}

CObject::~CObject				( )
{
	cNameVisual_set				( 0 );
	cName_set					( 0 );
	cNameSect_set				( 0 );
}

void CObject::Load				(LPCSTR section )
{
	// Name
	R_ASSERT					(section);
	cName_set					(section);
	cNameSect_set				(section);
	
	// Visual and light-track
	if (pSettings->line_exist(section,"visual")){
		string_path					tmp;
		strcpy_s					(tmp, pSettings->r_string(section,"visual"));
		if(strext(tmp)) 
			*strext(tmp)			=0;
		xr_strlwr					(tmp);

		cNameVisual_set				(tmp);
	}
	setVisible					(false);
}

BOOL CObject::net_Spawn			(CSE_Abstract* data)
{
	PositionStack.clear			();

	VERIFY						(_valid(renderable.xform));

	if (0 == Visual() && pSettings->line_exist(cNameSect(), "visual")) {
		shared_str visual_name = pSettings->r_string(cNameSect(), "visual");
		Msg("! [%s]: zero Visual() in %s found, use %s instead", __FUNCTION__, cName().c_str(), visual_name.c_str());
		cNameVisual_set(visual_name);
	}

	if (0==collidable.model) 	{
		if (pSettings->line_exist(cNameSect(),"cform")) {
			VERIFY3				(*NameVisual, "Model isn't assigned for object, but cform requisted",*cName());
			collidable.model	= xr_new<CCF_Skeleton>	(this);
		}
	}
	R_ASSERT(spatial.space);	spatial_register();

	if (register_schedule())
		shedule_register		();

	// reinitialize flags
	processing_activate			();
	setDestroy					(FALSE);

	MakeMeCrow					();

	return TRUE					;
}

void CObject::net_Destroy		()
{
	VERIFY						(getDestroy());
	xr_delete					(collidable.model);
	if (register_schedule())
		shedule_unregister		();

	spatial_unregister			();
//	setDestroy					(true);
	// remove visual
	cNameVisual_set				( 0 );
}

//////////////////////////////////////////////////////////////////////////
const	float	base_spu_epsP		= 0.05f;
const	float	base_spu_epsR		= 0.05f;
void	CObject::spatial_update		(float eps_P, float eps_R)
{
	//
	BOOL	bUpdate=FALSE;
	if (PositionStack.empty())
	{
		// Empty
		bUpdate							= TRUE;
		PositionStack.push_back			(SavedPosition());
		PositionStack.back().dwTime		= Device.dwTimeGlobal;
		PositionStack.back().vPosition	= Position();
	} else {
		if (PositionStack.back().vPosition.similar(Position(),eps_P))
		{
			// Just update time
			PositionStack.back().dwTime		= Device.dwTimeGlobal;
		} else {
			// Register _new_ record
			bUpdate							= TRUE;
			if (PositionStack.size()<4)		{
				PositionStack.push_back			(SavedPosition());
			} else {
				PositionStack[0]				= PositionStack[1];
				PositionStack[1]				= PositionStack[2];
				PositionStack[2]				= PositionStack[3];
			}
			PositionStack.back().dwTime		= Device.dwTimeGlobal;
			PositionStack.back().vPosition	= Position();
		}
	}

	if (bUpdate)		{
		spatial_move	();
	} else {
		if (spatial.node_ptr)	
		{	// Object registered!
			if (!fsimilar(Radius(),spatial.sphere.R,eps_R))	spatial_move();
			else			{
				Fvector			C;
				Center			(C);
				if (!C.similar(spatial.sphere.P,eps_P))	spatial_move();
			}
			// else nothing to do :_)
		}
	}
}

// Updates
void CObject::UpdateCL			()
{
	// consistency check
#ifdef DEBUG
	VERIFY2								(_valid(renderable.xform),*cName());

	if (Device.dwFrame==dbg_update_cl)								Debug.fatal	(DEBUG_INFO,"'UpdateCL' called twice per frame for %s",*cName());
	dbg_update_cl	= Device.dwFrame;

	if (Parent && spatial.node_ptr)									Debug.fatal	(DEBUG_INFO,"Object %s has parent but is still registered inside spatial DB",*cName());

	if ((0==collidable.model)&&(spatial.type&STYPE_COLLIDEABLE))	Debug.fatal	(DEBUG_INFO,"Object %s registered as 'collidable' but has no collidable model",*cName());
#endif

	spatial_update				(base_spu_epsP*5,base_spu_epsR*5);

	// crow
	if (Parent == g_pGameLevel->CurrentViewEntity())
		MakeMeCrow	();
	else if (AlwaysTheCrow())
		MakeMeCrow	();
	else
	{
		float dist = Device.vCameraPosition.distance_to_sqr(Position());
		if (dist < CROW_RADIUS*CROW_RADIUS)
			MakeMeCrow	();
		else
		if( (Visual() && Visual()->getVisData().hom_frame+2 > Device.dwFrame) && (dist < CROW_RADIUS2*CROW_RADIUS2) )
			MakeMeCrow	();
	}
}

void CObject::shedule_Update	( u32 T )
{
	// consistency check
	// Msg						("-SUB-:[%x][%s] CObject::shedule_Update",dynamic_cast<void*>(this),*cName());
	ISheduled::shedule_Update	(T);
	spatial_update				(base_spu_epsP*1,base_spu_epsR*1);

	// Always make me crow on shedule-update 
	// Makes sure that update-cl called at least with freq of shedule-update
	MakeMeCrow					();	
	/*
	if (AlwaysTheCrow())																	MakeMeCrow	();
	else if (Device.vCameraPosition.distance_to_sqr(Position()) < CROW_RADIUS*CROW_RADIUS)	MakeMeCrow	();
	*/
}

void	CObject::spatial_register	()
{
	Center						(spatial.sphere.P);
	spatial.sphere.R			= Radius();
	ISpatial::spatial_register	();
}

void	CObject::spatial_unregister()
{
	ISpatial::spatial_unregister();
}

void	CObject::spatial_move()
{
	Center						(spatial.sphere.P);
	spatial.sphere.R			= Radius();
	ISpatial::spatial_move		();
}

CObject::SavedPosition CObject::ps_Element(u32 ID) const
{
	VERIFY(ID<ps_Size());
	return PositionStack[ID];
}

void CObject::renderable_Render	()
{
	MakeMeCrow	();
}

CObject* CObject::H_SetParent	(CObject* new_parent, bool just_before_destroy)
{
	if (new_parent==Parent)	return new_parent;

	CObject* old_parent	= Parent; 
	
	VERIFY2((new_parent==0)||(old_parent==0),"Before set parent - execute H_SetParent(0)");

	// if (Parent) Parent->H_ChildRemove	(this);
	if (0==old_parent)	OnH_B_Chield		();	// before attach
	else				OnH_B_Independent	(just_before_destroy); // before detach
	if (new_parent)		spatial_unregister	();
	else				spatial_register	();
	Parent				= new_parent;
	if (0==old_parent)	OnH_A_Chield		();	// after attach
	else				OnH_A_Independent	(); // after detach
	// if (Parent)	Parent->H_ChildAdd		(this);
	MakeMeCrow			();
	return				old_parent;
}

void CObject::OnH_A_Chield		()
{
}
void CObject::OnH_B_Chield		()
{
	setVisible	(false);
}
void CObject::OnH_A_Independent	()
{
	setVisible	(true);
}
void CObject::OnH_B_Independent	(bool just_before_destroy)
{
}
void CObject::MakeMeCrow			()
{
		if (Props.crow)					return	;
		if (!processing_enabled())		return	;
		Props.crow						= true	;
		MakeMeCrow_internal				()		;
}

void CObject::setDestroy			(BOOL _destroy)
{
	if (_destroy == (BOOL)Props.bDestroy)
		return;

	Props.bDestroy	= _destroy?1:0;
	if (_destroy)
	{
		g_pGameLevel->Objects.register_object_to_destroy	(this);
#ifdef DEBUG
		Msg("cl setDestroy [%d][%d]",ID(),Device.dwFrame);
#endif
	}else
		VERIFY		(!g_pGameLevel->Objects.registered_object_to_destroy(this));
}
