#pragma once

class CBlender_fluid_advect : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_advect_velocity : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_simulate : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_obst : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_emitter : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid emitters"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_obstdraw : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_raydata : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_raycast : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};
