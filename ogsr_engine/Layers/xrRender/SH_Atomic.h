#pragma once

#include "../../xrCore/xr_resource.h"
#include "tss_def.h"

#include "../xrRenderDX10/StateManager/dx10State.h"

#pragma pack(push, 4)

//////////////////////////////////////////////////////////////////////////
// Atomic resources
//////////////////////////////////////////////////////////////////////////
struct ECORE_API SInputSignature : public xr_resource_flagged
{
    ID3DBlob* signature;

    SInputSignature(ID3DBlob* pBlob);
    ~SInputSignature();
};
typedef resptr_core<SInputSignature, resptr_base<SInputSignature>> ref_input_sign;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API SVS : public xr_resource_named
{
    ID3DVertexShader* sh;
    R_constant_table constants;

    ref_input_sign signature;

    SVS();
    ~SVS();
};
typedef resptr_core<SVS, resptr_base<SVS>> ref_vs;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API SPS : public xr_resource_named
{
    ID3DPixelShader* sh;
    R_constant_table constants;
    ~SPS();
};
typedef resptr_core<SPS, resptr_base<SPS>> ref_ps;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API SGS : public xr_resource_named
{
    ID3DGeometryShader* sh;
    R_constant_table constants;
    ~SGS();
};
typedef resptr_core<SGS, resptr_base<SGS>> ref_gs;

struct ECORE_API SHS : public xr_resource_named
{
    ID3D11HullShader* sh;
    R_constant_table constants;
    ~SHS();
};
typedef resptr_core<SHS, resptr_base<SHS>> ref_hs;

struct ECORE_API SDS : public xr_resource_named
{
    ID3D11DomainShader* sh;
    R_constant_table constants;
    ~SDS();
};
typedef resptr_core<SDS, resptr_base<SDS>> ref_ds;

struct ECORE_API SCS : public xr_resource_named
{
    ID3D11ComputeShader* sh;
    R_constant_table constants;
    ~SCS();
};
typedef resptr_core<SCS, resptr_base<SCS>> ref_cs;


//////////////////////////////////////////////////////////////////////////
struct ECORE_API SState : public xr_resource_flagged
{
    ID3DState* state;
    SimulatorStates state_code;
    ~SState();
};
typedef resptr_core<SState, resptr_base<SState>> ref_state;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API SDeclaration : public xr_resource_flagged
{
    //	Maps input signature to input layout
    xr_map<ID3DBlob*, ID3DInputLayout*> vs_to_layout;
    xr_vector<D3D_INPUT_ELEMENT_DESC> dx10_dcl_code;

    //	Use this for DirectX10 to cache DX9 declaration for comparison purpose only
    xr_vector<D3DVERTEXELEMENT9> dcl_code;
    ~SDeclaration();
};
typedef resptr_core<SDeclaration, resptr_base<SDeclaration>> ref_declaration;

#pragma pack(pop)
