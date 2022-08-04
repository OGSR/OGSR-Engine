#pragma once

//#include "../../../PHCharacter.h"
#include "../../../PHCapture.h"

#define TEMPLATE_SPECIALIZATION template <typename _Object>

#define CStateGroupDragAbstract CStateGroupDrag<_Object>

TEMPLATE_SPECIALIZATION
CStateGroupDragAbstract::CStateGroupDrag(_Object* obj) : inherited(obj) {}

TEMPLATE_SPECIALIZATION
CStateGroupDragAbstract::~CStateGroupDrag() {}

TEMPLATE_SPECIALIZATION
void CStateGroupDragAbstract::initialize()
{
    inherited::initialize();

    object->character_physics_support()->movement()->PHCaptureObject(const_cast<CEntityAlive*>(object->CorpseMan.get_corpse()));

    m_failed = false;

    CPHCapture* capture = object->character_physics_support()->movement()->PHCapture();
    if (capture && !capture->Failed())
    {
        m_cover_vertex_id = object->Home->get_place_in_min_home();
        if (m_cover_vertex_id != u32(-1))
        {
            m_cover_position = ai().level_graph().vertex_position(m_cover_vertex_id);
        }
        else
            m_cover_position = object->Position();
        if (m_cover_vertex_id == u32(-1) || object->Position().distance_to(m_cover_position) < 2.f || !object->Home->at_min_home(m_cover_position))
        {
            const CCoverPoint* point = object->CoverMan->find_cover(object->Home->get_home_point(), 1, object->Home->get_min_radius());
            if (point)
            {
                m_cover_vertex_id = point->level_vertex_id();
                if (m_cover_vertex_id != u32(-1))
                {
                    m_cover_position = ai().level_graph().vertex_position(m_cover_vertex_id);
                }
            }
        }
    }
    else
        m_failed = true;
    m_corpse_start_position = object->EatedCorpse->Position();
    object->path().prepare_builder();
}

TEMPLATE_SPECIALIZATION
void CStateGroupDragAbstract::execute()
{
    if (m_failed)
        return;

    // Установить параметры движения
    object->set_action(ACT_DRAG);
    object->anim().SetSpecParams(ASP_MOVE_BKWD);

    if (m_cover_vertex_id != u32(-1))
    {
        object->path().set_target_point(m_cover_position, m_cover_vertex_id);
    }
    else
    {
        object->path().set_retreat_from_point(object->EatedCorpse->Position());
    }

    object->path().set_generic_parameters();
    object->anim().accel_activate(eAT_Calm);
}

TEMPLATE_SPECIALIZATION
void CStateGroupDragAbstract::finalize()
{
    inherited::finalize();

    // бросить труп
    if (object->character_physics_support()->movement()->PHCapture())
        object->character_physics_support()->movement()->PHReleaseObject();
}

TEMPLATE_SPECIALIZATION
void CStateGroupDragAbstract::critical_finalize()
{
    inherited::critical_finalize();

    // бросить труп
    if (object->character_physics_support()->movement()->PHCapture())
        object->character_physics_support()->movement()->PHReleaseObject();
}

TEMPLATE_SPECIALIZATION
bool CStateGroupDragAbstract::check_completion()
{
    if (m_failed)
    {
        return true;
    }

    if (!object->character_physics_support()->movement()->PHCapture())
    {
        return true;
    }

    if (m_cover_vertex_id != u32(-1))
    { // valid vertex so wait path end
        if (object->Position().distance_to(m_cover_position) < 2.f)
            return true;
    }
    else
    { // invalid vertex so check distanced that passed
        if (m_corpse_start_position.distance_to(object->Position()) > object->Home->get_min_radius())
            return true;
    }

    return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateGroupDragAbstract
