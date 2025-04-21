#pragma once

#include "../xrcdb/ispatial.h"
#include "isheduled.h"
#include "irenderable.h"

class ENGINE_API CPS_Instance : public ISpatial, public ISheduled, public IRenderable
{
    friend class IGame_Persistent;

private:
    bool m_destroy_on_game_load;

protected:
    int m_iLifeTime;
    BOOL m_bAutoRemove;
    BOOL m_bDead;
    virtual void PSI_internal_delete();

public:
    CPS_Instance(bool destroy_on_game_load);
    virtual ~CPS_Instance();

    virtual void PSI_destroy();

    IC const bool& destroy_on_game_load() const { return m_destroy_on_game_load; }

    IC BOOL PSI_alive() { return m_iLifeTime > 0; }

    virtual void Play(BOOL hudMode = FALSE) = 0;
    virtual BOOL Locked() { return FALSE; }

    virtual shared_str shedule_Name() const { return shared_str("particle_instance"); };

    virtual void shedule_Update(u32 dt);
    virtual IRenderable* dcast_Renderable() { return this; }

    virtual void PerformFrame() = 0;

    //virtual void PerformCreate() = 0;
};
