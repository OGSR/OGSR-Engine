#include "stdafx.h"
#include "Light_Package.h"

void light_Package::clear()
{
    v_point.clear();
    v_spot.clear();
    v_shadowed.clear();
}

IC bool pred_light_cmp(light* _1, light* _2)
{
    return _1->range > _2->range; // sort by range
}

void light_Package::sort()
{
    // resort lights (pending -> at the end), maintain stable order
    std::stable_sort(v_point.begin(), v_point.end(), pred_light_cmp);
    std::stable_sort(v_spot.begin(), v_spot.end(), pred_light_cmp);
    std::stable_sort(v_shadowed.begin(), v_shadowed.end(), pred_light_cmp);
}

// Создаем запросы к окклюдеру. Потом мы будем получать от них данные
// в обратном порядке, так, что бы мы сначала дождались ответа от
// самого свежего запроса, т.е. от v_spot[ -1 ]. Полагаю, что если
// готовы данные для самого свежего запроса, то для более старых они
// будут готовы тем более и мы будет ждать ответа на запрос только
// один раз за кадр.
void light_Package::vis_prepare(CBackend& cmd_list) const
{
    ZoneScoped;

    for (light* L : v_shadowed)
        L->vis_prepare(cmd_list);

    for (light* L : v_point)
        L->vis_prepare(cmd_list);

    for (light* L : v_spot)
        L->vis_prepare(cmd_list);
    
}

// Получаем ответы от запросов к окклюдеру в обратном порядке, от
// самого свежего запроса, к самому старому. См. комментарии выше.
void light_Package::vis_update() const
{
    ZoneScoped;

    if (!v_spot.empty())
    {
        for (int it = v_spot.size() - 1; it >= 0; it--)
        {
            light* L = v_spot[it];
            L->vis_update();
        }
    }
    if (!v_point.empty())
    {
        for (int it = v_point.size() - 1; it >= 0; it--)
        {
            light* L = v_point[it];
            L->vis_update();
        }
    }
    if (!v_shadowed.empty())
    {
        for (int it = v_shadowed.size() - 1; it >= 0; it--)
        {
            light* L = v_shadowed[it];
            L->vis_update();
        }
    }
}
