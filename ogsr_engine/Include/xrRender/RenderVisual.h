#pragma once

class IKinematics;
class IKinematicsAnimated;
class IParticleCustom;
struct vis_data;

class IRenderVisual
{
public:
    virtual ~IRenderVisual() { ; }

    virtual vis_data& getVisData() = 0;
    virtual u32 getType() = 0;

    virtual shared_str getDebugName() const = 0;
    virtual shared_str getDebugInfo() const = 0;

    virtual void MarkAsHot(bool is_hot){};

    virtual IKinematics* dcast_PKinematics() { return nullptr; }
    virtual IKinematicsAnimated* dcast_PKinematicsAnimated() { return nullptr; }
    virtual IParticleCustom* dcast_ParticleCustom() { return nullptr; }

    bool ignore_optimization{false};
};