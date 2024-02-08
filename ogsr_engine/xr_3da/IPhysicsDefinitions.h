#pragma once

class IPhysicsGeometry
{
public:
    virtual void get_Box(Fmatrix& form, Fvector& sz) = 0;
    virtual bool collide_fluids() = 0;
};

class IPhysicsElement
{
public:
    virtual const Fmatrix& XFORM() const = 0;
    virtual void get_LinearVel(Fvector& velocity) = 0;
    virtual void get_AngularVel(Fvector& velocity) = 0;
    virtual const Fvector& mass_Center() = 0;
    virtual u16 numberOfGeoms() = 0;
    virtual IPhysicsGeometry* geometry(u16 i) const = 0;
};

class IPhysicsShell
{
public:
    virtual const Fmatrix& XFORM() const = 0;
    virtual IPhysicsElement& IElement(u16 index) = 0;
    virtual u16 get_ElementsNumber() = 0;
};

class IObjectPhysicsCollision
{
public:
    virtual IPhysicsShell* physics_shell() const = 0;
    virtual IPhysicsElement* physics_character() const = 0; // depricated
};
