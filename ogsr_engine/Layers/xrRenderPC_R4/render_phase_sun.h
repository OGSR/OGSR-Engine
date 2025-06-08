#pragma once

#include "../xrRender/r_sun_cascades.h"
#include "../xrRender/Light_DB.h"

// TODO: move it into separate file.
struct i_render_phase
{
    explicit i_render_phase(const xr_string& name_in) : name(name_in)
    {
        o.active = false;
        o.mt_calc_enabled = false;
        o.mt_draw_enabled = false;
    }

    virtual ~i_render_phase() = default;

    ICF void run()
    {
        if (!o.active)
            return;

        auto main = [this] {
            calculate();

            if (o.mt_draw_enabled)
            {
                draw_task = TTAPI->submit([this] { render(); });
                draw_task_started = true;
            }
        };

        if (o.mt_calc_enabled)
        {
            main_task = TTAPI->submit(main);
            main_task_started = true;
        }
        else
        {
            main();
        }
    }

    ICF void sync()
    {
        if (main_task_started)
        {
            main_task.wait();
            main_task_started = false;
        }

        if (draw_task_started)
        {
            draw_task.wait();
            draw_task_started = false;
        }
        else
        {
            render();
        }

        flush();

        o.active = false;
    }

    virtual void init() = 0;
    virtual void calculate() = 0;
    virtual void render() = 0;
    virtual void flush() {}

    struct options_t
    {
        u32 active : 1;
        u32 mt_calc_enabled : 1;
        u32 mt_draw_enabled : 1;
    } o;

    std::future<void> main_task;
    bool main_task_started{};
    std::future<void> draw_task;
    bool draw_task_started{};

    xr_string name{"<UNKNOWN>"};
};

struct render_rain : public i_render_phase
{
    render_rain() : i_render_phase("rain_render") {}

    void init() override;
    void calculate() override;
    void render() override;
    void flush() override;

    light RainLight;
    u32 context_id{CHW::INVALID_CONTEXT_ID};
    float rain_factor{0.0f};
};

struct render_sun : public i_render_phase
{
    render_sun() : i_render_phase("sun_render") {}

    void init() override;
    void calculate() override;
    void render() override;
    void flush() override;

    void accumulate_cascade(u32 cascade_ind);

    sun::cascade m_sun_cascades[R__NUM_SUN_CASCADES]{};
    light* sun{nullptr};
    bool need_to_render_sunshafts{false};
    bool last_cascade_chain_mode{false};

    u32 contexts_ids[R__NUM_SUN_CASCADES]{};
    task_thread_pool::task_thread_pool sun_cascade_pool{"MT_SUN", R__NUM_SUN_CASCADES};
};