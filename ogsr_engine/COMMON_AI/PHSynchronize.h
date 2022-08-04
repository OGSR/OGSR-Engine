#ifndef PH_SYNCHRONIZE_H
#define PH_SYNCHRONIZE_H

#include "PHNetState.h"

class NET_Packet;

class CPHSynchronize
{
public:
    virtual void get_State(SPHNetState& state) = 0;
    virtual void set_State(const SPHNetState& state) = 0;
    virtual void cv2obj_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform) = 0;
    virtual void cv2bone_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform) = 0;

protected:
private:
};

#endif