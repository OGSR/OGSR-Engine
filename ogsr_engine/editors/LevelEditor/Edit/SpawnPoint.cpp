//----------------------------------------------------
// file: rpoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "spawnpoint.h"
#include "ESceneSpawnTools.h"
#include "eshape.h"
#include "../../xr_3da/xrGame/xrServer_Objects_Abstract.h"
#include "../ECore/Editor/ui_main.h"
#include "SkeletonAnimated.h"
#include "ObjectAnimator.h"
#include "../../xr_3da/xrGame/xrMessages.h"
#include "scene.h"
#include "d3dutils.h"

#define SPAWNPOINT_VERSION   			0x0014
//----------------------------------------------------
#define SPAWNPOINT_CHUNK_VERSION		0xE411
#define SPAWNPOINT_CHUNK_POSITION		0xE412
#define SPAWNPOINT_CHUNK_RPOINT			0xE413
#define SPAWNPOINT_CHUNK_DIRECTION		0xE414
#define SPAWNPOINT_CHUNK_SQUADID		0xE415
#define SPAWNPOINT_CHUNK_GROUPID		0xE416
#define SPAWNPOINT_CHUNK_TYPE			0xE417
#define SPAWNPOINT_CHUNK_FLAGS			0xE418

#define SPAWNPOINT_CHUNK_ENTITYREF		0xE419
#define SPAWNPOINT_CHUNK_SPAWNDATA		0xE420

#define SPAWNPOINT_CHUNK_ATTACHED_OBJ	0xE421

#define SPAWNPOINT_CHUNK_ENVMOD			0xE422
#define SPAWNPOINT_CHUNK_ENVMOD2		0xE423

//----------------------------------------------------
#define RPOINT_SIZE 0.5f
#define ENVMOD_SIZE 0.25f
#define MAX_TEAM 6
const u32 RP_COLORS[MAX_TEAM]={0xff0000,0x00ff00,0x0000ff,0xffff00,0x00ffff,0xff00ff};
//----------------------------------------------------
void CSE_Visual::set_visual	   	(LPCSTR name, bool load)
{
	string_path					tmp;
    strcpy						(tmp,name);
    if (strext(tmp))		 	*strext(tmp) = 0;
	xr_strlwr					(tmp);
	visual_name					= tmp;
}

//------------------------------------------------------------------------------
// CLE_Visual
//------------------------------------------------------------------------------
CSpawnPoint::CLE_Visual::CLE_Visual(CSE_Visual* src)
{
	source			= src;
    visual			= 0;
    b_tmp_lock                  = false;
}
CSpawnPoint::CLE_Visual::~CLE_Visual()
{
	::Render->model_Delete	(visual,TRUE);
}
void CSpawnPoint::CLE_Visual::OnChangeVisual	()
{
    if(b_tmp_lock) return;
    ::Render->model_Delete	(visual,TRUE);
    if (source->visual_name.size())
    {
        visual				= ::Render->model_Create(source->visual_name.c_str());
        if(NULL==visual)
        {
         xr_string _msg = "Model [" + xr_string(source->visual_name.c_str())+"] not found. Do you want to select it from library?";
              int mr = ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo, _msg.c_str());
              LPCSTR _new_val = 0;
              b_tmp_lock = true;
              if (mr==mrYes && TfrmChoseItem::SelectItem(smVisual,_new_val, 1) )
              {
                  source->visual_name  =  _new_val;
                  visual = ::Render->model_Create(source->visual_name.c_str());
              }
              b_tmp_lock = false;

        }
        PlayAnimation		();
    }
    ExecCommand				(COMMAND_UPDATE_PROPERTIES);
}
void CSpawnPoint::CLE_Visual::PlayAnimation ()
{
     if(b_tmp_lock) return;
    // play motion if skeleton
    if (PKinematicsAnimated(visual)){ 
        MotionID			M = PKinematicsAnimated(visual)->ID_Cycle_Safe(source->startup_animation.c_str());
        if (M.valid())		PKinematicsAnimated(visual)->PlayCycle(M); 
    }
    if (PKinematics(visual))PKinematics(visual)->CalculateBones();
}

//------------------------------------------------------------------------------
// CLE_Motion
//------------------------------------------------------------------------------
CSpawnPoint::CLE_Motion::CLE_Motion	(CSE_Motion* src)
{
	source			= src;
    animator		= 0;
}
CSpawnPoint::CLE_Motion::~CLE_Motion()
{
}
void __stdcall	CSpawnPoint::CLE_Motion::OnChangeMotion	()
{
	xr_delete					(animator);
    if (source->motion_name.size()){
        animator				= xr_new<CObjectAnimator>();
        animator->Load			(*source->motion_name);
        PlayMotion				();
    }
	ExecCommand					(COMMAND_UPDATE_PROPERTIES);
}
void CSpawnPoint::CLE_Motion::PlayMotion()
{
    // play motion if skeleton
    if (animator) animator->Play(true);
}
//------------------------------------------------------------------------------
// SpawnData
//------------------------------------------------------------------------------
void CSpawnPoint::SSpawnData::Create(LPCSTR _entity_ref)
{
    m_Data 	= create_entity	(_entity_ref);
    if (m_Data){
    	m_Data->set_name	(_entity_ref);
        if (m_Data->visual()){
            m_Visual	= xr_new<CLE_Visual>(m_Data->visual());
            m_Data->set_editor_flag(ISE_Abstract::flVisualChange|ISE_Abstract::flVisualAnimationChange);
        }
        if (m_Data->motion()){
            m_Motion	= xr_new<CLE_Motion>(m_Data->motion());
            m_Data->set_editor_flag(ISE_Abstract::flMotionChange);
        }
        if (pSettings->line_exist(m_Data->name(),"$player")){
            if (pSettings->r_bool(m_Data->name(),"$player")){
				m_Data->flags().set(M_SPAWN_OBJECT_ASPLAYER,TRUE);
            }
        }
        m_ClassID 			= pSettings->r_clsid(m_Data->name(),"class");
    }else{
    	Log("!Can't create entity: ",_entity_ref);
    }
}
void CSpawnPoint::SSpawnData::Destroy()
{
    destroy_entity		(m_Data);
    xr_delete			(m_Visual);
    xr_delete			(m_Motion);
}
void CSpawnPoint::SSpawnData::get_bone_xform	(LPCSTR name, Fmatrix& xform)
{
	xform.identity		();
	if (name&&name[0]&&m_Visual&&m_Visual->visual){
    	CKinematics* P 	= PKinematics(m_Visual->visual);
    	if (P){
        	u16 id 		= P->LL_BoneID(name);
            if (id!=BI_NONE)
	        	xform 	= P->LL_GetTransform(id);
        }    	
    }
}
void CSpawnPoint::SSpawnData::Save(IWriter& F)
{
    F.open_chunk		(SPAWNPOINT_CHUNK_ENTITYREF);
    F.w_stringZ			(m_Data->name());
    F.close_chunk		();

    F.open_chunk		(SPAWNPOINT_CHUNK_SPAWNDATA);
    NET_Packet 			Packet;
    m_Data->Spawn_Write	(Packet,TRUE);
    F.w_u32				(Packet.B.count);
    F.w					(Packet.B.data,Packet.B.count);
    F.close_chunk		();
}
bool CSpawnPoint::SSpawnData::Load(IReader& F)
{
    string64 			temp;
    F.r_stringZ			(temp,sizeof(temp));

    NET_Packet 			Packet;
    R_ASSERT(F.find_chunk(SPAWNPOINT_CHUNK_SPAWNDATA));
    Packet.B.count 		= F.r_u32();
    F.r					(Packet.B.data,Packet.B.count);
    Create				(temp);
    if (Valid())
    	if (!m_Data->Spawn_Read(Packet))
        	Destroy		();

    return Valid();
}
bool CSpawnPoint::SSpawnData::ExportGame(SExportStreams* F, CSpawnPoint* owner)
{
	// set params
    m_Data->set_name_replace	(owner->Name);
    m_Data->position().set		(owner->PPosition);
    m_Data->angle().set			(owner->PRotation);

    // export cform (if needed)
    ISE_Shape* cform 			= m_Data->shape();
// SHAPE
    if (cform&&!(owner->m_AttachedObject&&(owner->m_AttachedObject->ClassID==OBJCLASS_SHAPE))){
		ELog.DlgMsg				(mtError,"Spawn Point: '%s' must contain attached shape.",owner->Name);
    	return false;
    }
    if (cform){
	    CEditShape* shape		= dynamic_cast<CEditShape*>(owner->m_AttachedObject); R_ASSERT(shape);
		shape->ApplyScale		();
        owner->PScale 			= shape->PScale;
    	cform->assign_shapes	(&*shape->GetShapes().begin(),shape->GetShapes().size());
    }
    // end

    NET_Packet					Packet;
    m_Data->Spawn_Write			(Packet,TRUE);

    F->spawn.stream.open_chunk	(F->spawn.chunk++);
    F->spawn.stream.w			(Packet.B.data,Packet.B.count);
    F->spawn.stream.close_chunk	();

    return true;
}
void CSpawnPoint::SSpawnData::FillProp(LPCSTR pref, PropItemVec& items)
{
	m_Data->FillProp			(pref,items);
}
void CSpawnPoint::SSpawnData::Render(bool bSelected, const Fmatrix& parent,int priority, bool strictB2F)
{
	if (m_Visual&&m_Visual->visual)	
    	::Render->model_Render	(m_Visual->visual,parent,priority,strictB2F,1.f);
    if (m_Motion&&m_Motion->animator&&bSelected&&(1==priority)&&(false==strictB2F))
        m_Motion->animator->DrawPath();
    RCache.set_xform_world		(Fidentity);
	Device.SetShader			(Device.m_WireShader);
    m_Data->on_render			(&DU,this,bSelected,parent,priority,strictB2F);
}
void CSpawnPoint::SSpawnData::OnFrame()
{
	if (m_Data->m_editor_flags.is(ISE_Abstract::flUpdateProperties))
    	ExecCommand				(COMMAND_UPDATE_PROPERTIES);
    // visual part
	if (m_Visual){
	    if (m_Data->m_editor_flags.is(ISE_Abstract::flVisualChange))
        	m_Visual->OnChangeVisual();
	    if (m_Data->m_editor_flags.is(ISE_Abstract::flVisualAnimationChange))
        	m_Visual->PlayAnimation();
    	if (m_Visual->visual&&PKinematics(m_Visual->visual))
	    	PKinematics			(m_Visual->visual)->CalculateBones(TRUE);
    }
    // motion part
    if (m_Motion){
	    if (m_Data->m_editor_flags.is(ISE_Abstract::flMotionChange))
        	m_Motion->OnChangeMotion();
    	if (m_Motion->animator)
    		m_Motion->animator->Update(Device.fTimeDelta);
    }
    // reset editor flags
    m_Data->m_editor_flags.zero	();
}
//------------------------------------------------------------------------------
CSpawnPoint::CSpawnPoint(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

void CSpawnPoint::Construct(LPVOID data)
{
	ClassID			= OBJCLASS_SPAWNPOINT;
    m_AttachedObject= 0;
    if (data){
        if (strcmp(LPSTR(data),RPOINT_CHOOSE_NAME)==0){
            m_Type 		= ptRPoint;
            m_RP_Type	= rptActorSpawn;
            m_RP_GameType= rpgtGameAny;
            m_RP_TeamID	= 0;
        }else if (strcmp(LPSTR(data),ENVMOD_CHOOSE_NAME)==0){
            m_Type 				= ptEnvMod;
            m_EM_Radius			= 10.f;
            m_EM_Power			= 1.f;
            m_EM_ViewDist		= 300.f;
            m_EM_FogColor		= 0x00808080;
            m_EM_FogDensity		= 1.f;
            m_EM_AmbientColor	= 0x00000000;
            m_EM_SkyColor		= 0x00FFFFFF;
            m_EM_HemiColor		= 0x00FFFFFF;
        }else{
            CreateSpawnData(LPCSTR(data));
            if (!m_SpawnData.Valid()){
            	SetValid(false);
            }else{
	        	m_Type			= ptSpawnPoint;
            }
        }
    }else{
		SetValid(false);
    }
}

CSpawnPoint::~CSpawnPoint()
{
	xr_delete(m_AttachedObject);
    OnDeviceDestroy();
}

void CSpawnPoint::Select(int  flag)
{
	inherited::Select(flag);
    if (m_AttachedObject) m_AttachedObject->Select(flag);
}

void CSpawnPoint::SetPosition(const Fvector& pos)
{
	inherited::SetPosition	(pos);
    if (m_AttachedObject) m_AttachedObject->PPosition = pos;
}
void CSpawnPoint::SetRotation(const Fvector& rot)
{
	inherited::SetRotation	(rot);
    if (m_AttachedObject) m_AttachedObject->PRotation = rot;
}
void CSpawnPoint::SetScale(const Fvector& scale)
{
	inherited::SetScale		(scale);
    if (m_AttachedObject) m_AttachedObject->PScale = scale;
}

bool CSpawnPoint::AttachObject(CCustomObject* obj)
{
	bool bAllowed = false;
    // большая проверялка
    if (m_SpawnData.Valid()){
    	switch(obj->ClassID){    
        case OBJCLASS_SHAPE:
	    	bAllowed = !!m_SpawnData.m_Data->shape();
        break;
//        case OBJCLASS_SCENEOBJECT:
//	    	bAllowed = !!dynamic_cast<xrSE_Visualed*>(m_SpawnData.m_Data);
//        break;
        }
    }
    // реальный атач
	if (bAllowed){
        DetachObject();
        m_AttachedObject = obj;           
        m_AttachedObject->OnAttach(this);
        PPosition 	= m_AttachedObject->PPosition;
        PRotation 	= m_AttachedObject->PRotation;
        PScale 		= m_AttachedObject->PScale;
    }
    return bAllowed;
}

void CSpawnPoint::DetachObject()
{
	if (m_AttachedObject){
		m_AttachedObject->OnDetach();
    	m_AttachedObject = 0;
    }
}

bool CSpawnPoint::RefCompare	(LPCSTR ref)
{
	return ref&&ref[0]&&m_SpawnData.Valid()?(strcmp(ref,m_SpawnData.m_Data->name())==0):false; 
}

LPCSTR CSpawnPoint::RefName	() 			
{
	return m_SpawnData.Valid()?m_SpawnData.m_Data->name():0;
}

bool CSpawnPoint::CreateSpawnData(LPCSTR entity_ref)
{
	R_ASSERT(entity_ref&&entity_ref[0]);
    m_SpawnData.Destroy	();
    m_SpawnData.Create	(entity_ref);
    if (m_SpawnData.Valid()) m_Type = ptSpawnPoint;
    return m_SpawnData.Valid();
}
//----------------------------------------------------

bool CSpawnPoint::GetBox( Fbox& box )
{
    switch (m_Type){
    case ptRPoint: 	
        box.set		( PPosition, PPosition );
        box.min.x 	-= RPOINT_SIZE;
        box.min.y 	-= 0;
        box.min.z 	-= RPOINT_SIZE;
        box.max.x 	+= RPOINT_SIZE;
        box.max.y 	+= RPOINT_SIZE*2.f;
        box.max.z 	+= RPOINT_SIZE;
    break;
    case ptEnvMod: 	
        box.set		(PPosition, PPosition);
        box.grow	(Selected()?m_EM_Radius:ENVMOD_SIZE);
    break;
    case ptSpawnPoint:
    	if (m_SpawnData.Valid()){
			if (m_SpawnData.m_Visual&&m_SpawnData.m_Visual->visual)
            {
            	box.set		(m_SpawnData.m_Visual->visual->vis.box);
                box.xform	(FTransformRP);
            }else{
			    CEditShape* shape	= dynamic_cast<CEditShape*>(m_AttachedObject);
                if (shape&&!shape->GetShapes().empty()){
                	CShapeData::ShapeVec& SV	= shape->GetShapes();
                	box.invalidate();
                    Fvector p;
                	for (CShapeData::ShapeIt it=SV.begin(); it!=SV.end(); it++){
                    	switch (it->type){
                        case CShapeData::cfSphere:
                        	p.add(it->data.sphere.P,it->data.sphere.R); shape->_Transform().transform_tiny(p); box.modify(p);
                        	p.sub(it->data.sphere.P,it->data.sphere.R); shape->_Transform().transform_tiny(p); box.modify(p);
                        break;
                        case CShapeData::cfBox:
                        	p.set( 0.5f, 0.5f, 0.5f);it->data.box.transform_tiny(p); shape->_Transform().transform_tiny(p); box.modify(p);
                        	p.set(-0.5f,-0.5f,-0.5f);it->data.box.transform_tiny(p); shape->_Transform().transform_tiny(p); box.modify(p);
                        break;
                        }
                    }
                }else{
                    box.set		( PPosition, PPosition );
                    box.min.x 	-= RPOINT_SIZE;
                    box.min.y 	-= 0;
                    box.min.z 	-= RPOINT_SIZE;
                    box.max.x 	+= RPOINT_SIZE;
                    box.max.y 	+= RPOINT_SIZE*2.f;
                    box.max.z 	+= RPOINT_SIZE;
                }
            }
        }else{
            box.set		( PPosition, PPosition );
            box.min.x 	-= RPOINT_SIZE;
            box.min.y 	-= 0;
            box.min.z 	-= RPOINT_SIZE;
            box.max.x 	+= RPOINT_SIZE;
            box.max.y 	+= RPOINT_SIZE*2.f;
            box.max.z 	+= RPOINT_SIZE;
        }
    break;
    default: NODEFAULT;
    }
    if (m_AttachedObject){ 		
    	Fbox 					bb;
    	m_AttachedObject->GetBox(bb);
        box.merge				(bb);
    }
	return true;
}

void CSpawnPoint::OnFrame()
{
	inherited::OnFrame();
    if (m_AttachedObject) 		m_AttachedObject->OnFrame	();
	if (m_SpawnData.Valid())    m_SpawnData.OnFrame			();
}

void CSpawnPoint::Render( int priority, bool strictB2F )
{
	inherited::Render			(priority, strictB2F);
	Scene->SelectLightsForObject(this);
    // render attached object
    if (m_AttachedObject) 		m_AttachedObject->Render(priority, strictB2F);
	if (m_SpawnData.Valid())    m_SpawnData.Render(Selected(),FTransformRP,priority, strictB2F);
	// render spawn point
    if (1==priority){
        if (strictB2F){
            RCache.set_xform_world(FTransformRP);
            if (m_SpawnData.Valid()){
                // render icon
                ESceneSpawnTools* st= dynamic_cast<ESceneSpawnTools*>(ParentTools); VERIFY(st);
                ref_shader s 	   	= st->GetIcon(m_SpawnData.m_Data->name());
                DU.DrawEntity		(0xffffffff,s);
            }else{
                switch (m_Type){
                case ptRPoint:{
                    float k = 1.f/(float(m_RP_TeamID+1)/float(MAX_TEAM));
                    int r = m_RP_TeamID%MAX_TEAM;
                    Fcolor c;
                    c.set(RP_COLORS[r]);
                    c.mul_rgb(k*0.9f+0.1f);
                    DU.DrawEntity(c.get(),Device.m_WireShader);
                }break;
                case ptEnvMod:{
                	Fvector pos={0,0,0};
	                Device.SetShader(Device.m_WireShader);
                    DU.DrawCross(pos,0.25f,0x20FFAE00,true);
                    if (Selected()) DU.DrawSphere(Fidentity,PPosition,m_EM_Radius,0x30FFAE00,0x00FFAE00,true,true);
                }break;
                default: THROW2("CSpawnPoint:: Unknown Type");
                }
            }
        }else{
            ESceneSpawnTools* st = dynamic_cast<ESceneSpawnTools*>(ParentTools); VERIFY(st);
            if (st->m_Flags.is(ESceneSpawnTools::flShowSpawnType)){ 
                AnsiString s_name;
                if (m_SpawnData.Valid()){
                    s_name	= m_SpawnData.m_Data->name();
                }else{
                    switch (m_Type){
                    case ptRPoint: 	s_name.sprintf("RPoint T:%d",m_RP_TeamID); break;
                    case ptEnvMod: 	
                    	s_name.sprintf("EnvMod V:%3.2f, F:%3.2f",m_EM_ViewDist,m_EM_FogDensity); 
					break;
                    default: THROW2("CSpawnPoint:: Unknown Type");
                    }
                }
                
                Fvector D;	D.sub(Device.vCameraPosition,PPosition);
                float dist 	= D.normalize_magn();
                if (!st->m_Flags.is(ESceneSpawnTools::flPickSpawnType)||
                    !Scene->RayPickObject(dist,PPosition,D,OBJCLASS_SCENEOBJECT,0,0))
                        DU.OutText	(PPosition,s_name.c_str(),0xffffffff,0xff000000);
            }
            if(Selected()){
                RCache.set_xform_world(Fidentity);
                Fbox bb; GetBox(bb);
                u32 clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                Device.SetShader(Device.m_WireShader);
                DU.DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CSpawnPoint::FrustumPick(const CFrustum& frustum)
{
    if (m_AttachedObject&&m_AttachedObject->FrustumPick(frustum)) return true;
    Fbox bb; GetBox(bb);
    u32 mask=0xff;
    return (frustum.testAABB(bb.data(),mask));
}

bool CSpawnPoint::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	bool bPick 	= false;
    if (m_AttachedObject){
    	bPick 	= m_AttachedObject->RayPick(distance, start, direction, pinf);
        return 	bPick;
    }

    Fbox 		bb;
    Fvector 	pos;
    float 		radius;
    GetBox		(bb);
    bb.getsphere(pos,radius);

	Fvector ray2;
	ray2.sub	(pos, start);

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (radius*radius)) && (d>radius) ){
            Fvector pt;
            if (Fbox::rpOriginOutside==bb.Pick2(start,direction,pt)){
            	d	= start.distance_to(pt);
            	if (d<distance){
                    distance	= d;
                    bPick 		= true;
	            }
            }
        }
    }

	return bPick;
}
//----------------------------------------------------
bool CSpawnPoint::OnAppendObject(CCustomObject* object)
{
	R_ASSERT(!m_AttachedObject);
    if (object->ClassID!=OBJCLASS_SHAPE) return false;
    // all right
    m_AttachedObject 		= object;
    object->m_pOwnerObject	= this;
    return true;
}

bool CSpawnPoint::Load(IReader& F){
	u32 version = 0;

    R_ASSERT(F.r_chunk(SPAWNPOINT_CHUNK_VERSION,&version));
    if(version!=SPAWNPOINT_VERSION){
        ELog.Msg( mtError, "SPAWNPOINT: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    // new generation
    if (F.find_chunk(SPAWNPOINT_CHUNK_ENTITYREF)){
        if (!m_SpawnData.Load(F)){
            ELog.Msg( mtError, "SPAWNPOINT: Can't load Spawn Data.");
            return false;
        }
        SetValid	(true);
        m_Type			= ptSpawnPoint;
    }else{
	    if (F.find_chunk(SPAWNPOINT_CHUNK_TYPE))     m_Type 		= (EPointType)F.r_u32();
        if (m_Type>=ptMaxType){
            ELog.Msg( mtError, "SPAWNPOINT: Unsupported spawn version.");
            return false;
        }
    	switch (m_Type){
        case ptRPoint:
		    if (F.find_chunk(SPAWNPOINT_CHUNK_RPOINT)){ 
            	m_RP_TeamID	= F.r_u8();
                m_RP_Type	= F.r_u8();
                m_RP_GameType=F.r_u8();
                F.advance	(1);
            }
        break;
        case ptEnvMod:
		    if (F.find_chunk(SPAWNPOINT_CHUNK_ENVMOD)){ 
                m_EM_Radius			= F.r_float();
                m_EM_Power			= F.r_float();
                m_EM_ViewDist		= F.r_float();
                m_EM_FogColor		= F.r_u32();
                m_EM_FogDensity		= F.r_float();
                m_EM_AmbientColor	= F.r_u32();
                m_EM_SkyColor		= F.r_u32();
                if (F.find_chunk(SPAWNPOINT_CHUNK_ENVMOD2))
                    m_EM_HemiColor	= F.r_u32();
            }
        break;
        default: THROW;
        }
    }

	// objects
    Scene->ReadObjects(F,SPAWNPOINT_CHUNK_ATTACHED_OBJ,OnAppendObject,0);

	UpdateTransform	();

	// BUG fix
    CEditShape* shape	= dynamic_cast<CEditShape*>(m_AttachedObject);
    if (shape) 	PScale 	= shape->PScale;
    
    return true;
}

void CSpawnPoint::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk		(SPAWNPOINT_CHUNK_VERSION);
	F.w_u16				(SPAWNPOINT_VERSION);
	F.close_chunk		();

    // save attachment
    if (m_AttachedObject){
	    ObjectList lst; lst.push_back(m_AttachedObject);
		Scene->SaveObjects(lst,SPAWNPOINT_CHUNK_ATTACHED_OBJ,F);
    }

	if (m_SpawnData.Valid()){
    	m_SpawnData.Save(F);
    }else{
		F.w_chunk	(SPAWNPOINT_CHUNK_TYPE,		&m_Type,	sizeof(u32));
    	switch (m_Type){
        case ptRPoint:
        	F.open_chunk(SPAWNPOINT_CHUNK_RPOINT);
            F.w_u8		(m_RP_TeamID);
            F.w_u8		(m_RP_Type);
            F.w_u8 		(m_RP_GameType);
            F.w_u8		(0);
            F.close_chunk();
        break;
        case ptEnvMod:
        	F.open_chunk(SPAWNPOINT_CHUNK_ENVMOD);
            F.w_float	(m_EM_Radius);
            F.w_float	(m_EM_Power);
            F.w_float	(m_EM_ViewDist);
            F.w_u32		(m_EM_FogColor);
            F.w_float	(m_EM_FogDensity);
        	F.w_u32		(m_EM_AmbientColor);
            F.w_u32		(m_EM_SkyColor);
            F.close_chunk();
        	F.open_chunk(SPAWNPOINT_CHUNK_ENVMOD2);
            F.w_u32		(m_EM_HemiColor);
            F.close_chunk();
        break;
        default: THROW;
        }
    }
}
//----------------------------------------------------

Fvector3 u32_3f(u32 clr)
{
	Fvector	tmp;
    float f	= 1.f / 255.f;
    tmp.x 	= f * float((clr >> 16)& 0xff);
    tmp.y 	= f * float((clr >>  8)& 0xff);
    tmp.z 	= f * float((clr >>  0)& 0xff);
    return	tmp;
}

bool CSpawnPoint::ExportGame(SExportStreams* F)
{
	// spawn
	if (m_SpawnData.Valid()){
    	if (m_SpawnData.m_Data->validate()){
	    	m_SpawnData.ExportGame		(F,this);
        }else{
        	Log	("!Invalid spawn data:",Name);
            return false;
        }
    }else{
        // game
        switch (m_Type){
        case ptRPoint:
	        F->rpoint.stream.open_chunk	(F->rpoint.chunk++);
            F->rpoint.stream.w_fvector3	(PPosition);
            F->rpoint.stream.w_fvector3	(PRotation);
            F->rpoint.stream.w_u8		(m_RP_TeamID);
            F->rpoint.stream.w_u8		(m_RP_Type);
            F->rpoint.stream.w_u8		(m_RP_GameType);
            F->rpoint.stream.w_u8		(0);
			F->rpoint.stream.close_chunk	();
        break;
        case ptEnvMod:
        	Fcolor tmp;
	        F->envmodif.stream.open_chunk(F->envmodif.chunk++);
            F->envmodif.stream.w_fvector3(PPosition);
            F->envmodif.stream.w_float	(m_EM_Radius);
            F->envmodif.stream.w_float	(m_EM_Power);
            F->envmodif.stream.w_float	(m_EM_ViewDist);
            F->envmodif.stream.w_fvector3(u32_3f(m_EM_FogColor));
            F->envmodif.stream.w_float	(m_EM_FogDensity);
            F->envmodif.stream.w_fvector3(u32_3f(m_EM_AmbientColor));
            F->envmodif.stream.w_fvector3(u32_3f(m_EM_SkyColor));
            F->envmodif.stream.w_fvector3(u32_3f(m_EM_HemiColor));
			F->envmodif.stream.close_chunk();
        break;
        default: THROW;
        }
    }
    return true;
}
//----------------------------------------------------

void CSpawnPoint::OnFillChooseItems		(ChooseValue* val)
{
    ESceneSpawnTools* st 		= dynamic_cast<ESceneSpawnTools*>(ParentTools); VERIFY(st);
    CLASS_ID cls_id				= m_SpawnData.m_ClassID;
    ESceneSpawnTools::ClassSpawnMapIt cls_it = st->m_Classes.find(cls_id); VERIFY(cls_it!=st->m_Classes.end());
    *val->m_Items				= cls_it->second;
}

shared_str CSpawnPoint::SectionToEditor(shared_str nm)
{
    ESceneSpawnTools* st 		= dynamic_cast<ESceneSpawnTools*>(ParentTools); 			VERIFY(st);
    ESceneSpawnTools::ClassSpawnMapIt cls_it = st->m_Classes.find(m_SpawnData.m_ClassID);	VERIFY(cls_it!=st->m_Classes.end());
    for (ESceneSpawnTools::SSVecIt ss_it=cls_it->second.begin(); ss_it!=cls_it->second.end(); ++ss_it)
        if (nm.equal(ss_it->hint)) return ss_it->name;
    return 0;
}

shared_str CSpawnPoint::EditorToSection(shared_str nm)
{
    ESceneSpawnTools* st  	= dynamic_cast<ESceneSpawnTools*>(ParentTools); 			VERIFY(st);
    ESceneSpawnTools::ClassSpawnMapIt cls_it = st->m_Classes.find(m_SpawnData.m_ClassID);	VERIFY(cls_it!=st->m_Classes.end());
    for (ESceneSpawnTools::SSVecIt ss_it=cls_it->second.begin(); ss_it!=cls_it->second.end(); ++ss_it)
        if (nm.equal(ss_it->name)) return ss_it->hint;
    return 0;
}

void CSpawnPoint::OnProfileChange(PropValue* prop)
{
	if (m_SpawnData.m_Profile.size()!=0){
        shared_str s_name		= EditorToSection(m_SpawnData.m_Profile);
        VERIFY					(s_name.size());
        if (0!=strcmp(m_SpawnData.m_Data->name(),*s_name)){
            ISE_Abstract* tmp	= create_entity	(*s_name); VERIFY(tmp);
            NET_Packet 			Packet;
            tmp->Spawn_Write	(Packet,TRUE);
            R_ASSERT			(m_SpawnData.m_Data->Spawn_Read(Packet));
            m_SpawnData.m_Data->set_editor_flag(ISE_Abstract::flVisualChange|ISE_Abstract::flVisualAnimationChange);
            destroy_entity		(tmp);
        }
    }else{
		m_SpawnData.m_Profile	= SectionToEditor(m_SpawnData.m_Data->name());
    }
}

void CSpawnPoint::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);

    if (m_SpawnData.Valid()){
	    shared_str pref1			= PrepareKey(pref,m_SpawnData.m_Data->name());
        m_SpawnData.m_Profile 		= SectionToEditor(m_SpawnData.m_Data->name());
        ChooseValue* C				= PHelper().CreateChoose(items,PrepareKey(pref1.c_str(),"Profile"),&m_SpawnData.m_Profile,smCustom,0,0,1,cfFullExpand);
        C->OnChooseFillEvent.bind	(this,&CSpawnPoint::OnFillChooseItems);
        C->OnChangeEvent.bind		(this,&CSpawnPoint::OnProfileChange);
    	m_SpawnData.FillProp		(pref,items);
    }else{
    	switch (m_Type){
        case ptRPoint:{
			PHelper().CreateU8		(items, PrepareKey(pref,"Respawn Point\\Team"), 		&m_RP_TeamID, 	0,7);
			PHelper().CreateToken8	(items, PrepareKey(pref,"Respawn Point\\Spawn Type"),	&m_RP_Type, 	rpoint_type);
			PHelper().CreateToken8	(items, PrepareKey(pref,"Respawn Point\\Game Type"), 	&m_RP_GameType, rpoint_game_type);
        }break;
        case ptEnvMod:{
        	PHelper().CreateFloat	(items, PrepareKey(pref,"Environment Modificator\\Radius"),			&m_EM_Radius, 	EPS_L,10000.f);
        	PHelper().CreateFloat	(items, PrepareKey(pref,"Environment Modificator\\Power"), 			&m_EM_Power, 	EPS,1000.f);
        	PHelper().CreateFloat	(items, PrepareKey(pref,"Environment Modificator\\View Distance"),	&m_EM_ViewDist, EPS_L,10000.f);
        	PHelper().CreateColor	(items, PrepareKey(pref,"Environment Modificator\\Fog Color"), 		&m_EM_FogColor);
        	PHelper().CreateFloat	(items, PrepareKey(pref,"Environment Modificator\\Fog Density"), 	&m_EM_FogDensity, 0.f,10000.f);
        	PHelper().CreateColor	(items, PrepareKey(pref,"Environment Modificator\\Ambient Color"), 	&m_EM_AmbientColor);
        	PHelper().CreateColor	(items, PrepareKey(pref,"Environment Modificator\\Sky Color"), 		&m_EM_SkyColor);
        	PHelper().CreateColor	(items, PrepareKey(pref,"Environment Modificator\\Hemi Color"), 	&m_EM_HemiColor);
        }break;
        default: THROW;
        }
    }
}
//----------------------------------------------------

bool CSpawnPoint::OnChooseQuery(LPCSTR specific)
{
	return (m_SpawnData.Valid()&&(0==strcmp(m_SpawnData.m_Data->name(),specific)));
}
///-----------------------------------------------------------------------------

