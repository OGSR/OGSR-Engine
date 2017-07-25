#include "stdafx.h"
#pragma hdrstop

#include "CustomObject.h"
#include "../ECore/Editor/ui_main.h"
#include "../ECore/Editor/ui_toolscustom.h"
#include "../ECore/Editor/editorpreferences.h"
#include "scene.h"

//------------------------------------------------------------------------------
// static part
//------------------------------------------------------------------------------
void CCustomObject::SnapMove(Fvector& pos, Fvector& rot, const Fmatrix& rotRP, const Fvector& amount)
{
// !!! Hide object before test
    SRayPickInfo pinf;
    Fvector up,dn={0,-1,0};
    rotRP.transform_dir(dn);
    up.invert(dn);
    Fvector s2,s1=pos;
    s1.add(amount);
    s2.mad(s1,up,EPrefs->snap_moveto);

    pinf.inf.range=EPrefs->snap_moveto;
    if (Scene->RayPickObject( pinf.inf.range, s1, dn, OBJCLASS_SCENEOBJECT, &pinf, Scene->GetSnapList(false))||Scene->RayPickObject( pinf.inf.range, s2, dn, OBJCLASS_SCENEOBJECT, &pinf, Scene->GetSnapList(false))){
            pos.set(pinf.pt);
            if (Tools->GetSettings(etfNormalAlign)){
                Fvector verts[3];
                pinf.e_obj->GetFaceWorld(pinf.s_obj->_Transform(),pinf.e_mesh,pinf.inf.id,verts);
                Fvector vR,vD,vN;
                vN.mknormal(verts[0],verts[1],verts[2]);

                vD.set(rotRP.k);
                vR.crossproduct	(vN,vD);
                vR.normalize();
                vD.crossproduct	(vR,vN);
                vD.normalize();

                Fmatrix M;
                M.set(vR,vN,vD,vR);
                M.getXYZ(rot);
            }
        }
    else pos.add(amount);
}
void CCustomObject::NormalAlign(Fvector& rot, const Fvector& up)
{
    Fmatrix	mR;
    Fvector vR,vD,vN;
    vN.set(up);
    vD.set(0,0,1);
    if (fabsf(vN.z)>0.99f) vD.set(1,0,0);
    vR.crossproduct(vN,vD); vR.normalize();
    vD.crossproduct(vR,vN); vD.normalize();
    mR.set(vR,vN,vD,vR);
    mR.getXYZ(rot);
}
//------------------------------------------------------------------------------

void CCustomObject::OnDetach()
{
	if (m_pOwnerObject){
        m_pOwnerObject 		= 0;
        string64 			new_name;
        Scene->GenObjectName(ClassID,new_name,Name);
        Name 				= new_name;
        Scene->AppendObject	(this,false);
    }
    Select					(true);
}

void CCustomObject::OnAttach(CCustomObject* owner)
{
	R_ASSERT(owner);
    R_ASSERT2(!m_pOwnerObject,"Object already has owner!");
    m_pOwnerObject 		= owner;
    Scene->RemoveObject(this,false);
    Select				(false);
}

void CCustomObject::Move(Fvector& amount)
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    Fvector v=PPosition;
    Fvector r=PRotation;
    if (Tools->GetSettings(etfMTSnap)){
        BOOL bVis	= Visible();
        BOOL bSel	= Selected();
        Show		(FALSE);
        Select		(FALSE);
    	SnapMove	(v,r,FTransformRP,amount);
        Show		(bVis);
        Select		(bSel);
    }else{
	    v.add(amount);
    }
    PPosition = v;
    PRotation = r;
}

void CCustomObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    Fvector v=PPosition;
    v.set(pos);
    if (Tools->GetSettings(etfNormalAlign)){
        Fvector r=PRotation;
    	NormalAlign(r,up);
        PRotation = r;
    }
    PPosition = v;
}

void CCustomObject::RotatePivot(const Fmatrix& prev_inv, const Fmatrix& current)
{
    Fmatrix 		Ol,On;
    Ol.mul			(prev_inv,FTransformRP);
    On.mul			(current,Ol);
    Fvector 		xyz;
    On.getXYZ		(xyz);
    PRotation		= xyz;
    PPosition		= On.c;
}

void CCustomObject::RotateParent(Fvector& axis, float angle)
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    Fvector r	= PRotation;
    r.mad		(axis,angle);
    PRotation		= r;
}

void CCustomObject::RotateLocal(Fvector& axis, float angle)
{
    Fmatrix m;
    Fvector r;
    m.rotation(axis,angle);
    FTransformRP.mulB_43(m);
    FTransformRP.getXYZ(r);
    PRotation		= r;    
}

void CCustomObject::ScalePivot( const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount )
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    Fvector p	= PPosition;
    Fvector s	= PScale;
	s.add(amount);
	if (s.x<EPS) s.x=EPS;
	if (s.y<EPS) s.y=EPS;
	if (s.z<EPS) s.z=EPS;
    PScale		= s;

    // translate position
    prev_inv.transform_tiny	(p);
    current.transform_tiny	(p);

    PPosition	= p;
}

void CCustomObject::Scale( Fvector& amount )
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    Fvector s	= PScale;
	s.add(amount);
    if (s.x<EPS) s.x=EPS;
    if (s.y<EPS) s.y=EPS;
    if (s.z<EPS) s.z=EPS;
    PScale		= s;
}

bool CCustomObject::OnObjectNameAfterEdit(PropValue* sender, shared_str& edit_val)
{
	RTextValue* V = dynamic_cast<RTextValue*>(sender); VERIFY(V);
    edit_val 	= (AnsiString(edit_val.c_str()).LowerCase()).c_str();
	return !Scene->FindObjectByName(edit_val.c_str(),(CCustomObject*)0);
}

void CCustomObject::OnNumChangePosition(PropValue* sender)
{
	NumSetPosition	(PPosition);
}
void CCustomObject::OnNumChangeRotation(PropValue* sender)
{
	NumSetRotation	(PRotation);
}
void CCustomObject::OnNumChangeScale(PropValue* sender)
{
	NumSetScale		(PScale);
}
void CCustomObject::OnNameChange(PropValue* sender)
{
	ExecCommand		(COMMAND_UPDATE_PROPERTIES);
}

void CCustomObject::FillProp(LPCSTR pref, PropItemVec& items)
{
    PropValue* V;
    V = PHelper().CreateNameCB	(items, PrepareKey(pref, "Name"),&FName,NULL,NULL,RTextValue::TOnAfterEditEvent(this,&CCustomObject::OnObjectNameAfterEdit));
    V->OnChangeEvent.bind		(this,&CCustomObject::OnNameChange);
    V = PHelper().CreateVector	(items, PrepareKey(pref,"Transform\\Position"),	&PPosition,	-10000,	10000,0.01,2);
    V->OnChangeEvent.bind		(this,&CCustomObject::OnNumChangePosition);
    V = PHelper().CreateAngle3	(items, PrepareKey(pref,"Transform\\Rotation"),	&PRotation,	-10000,	10000,0.1,1);
    V->OnChangeEvent.bind		(this,&CCustomObject::OnNumChangeRotation);
    V = PHelper().CreateVector	(items, PrepareKey(pref,"Transform\\Scale"),	&PScale, 	0.01,	10000,0.01,2);
    V->OnChangeEvent.bind		(this,&CCustomObject::OnNumChangeScale);
}
//----------------------------------------------------

void CCustomObject::OnShowHint(AStringVec& dest)
{
//	dest.push_back(AnsiString("Class: ")+AnsiString(ParentTools->ClassDesc()));
    dest.push_back(AnsiString("Name:  ")+AnsiString(Name));
    dest.push_back(AnsiString("-------"));
}
//----------------------------------------------------

