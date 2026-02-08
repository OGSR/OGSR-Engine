#pragma once

class smapvis : public R_feedback
{
public:
    enum
    {
        state_sleeping = 0,
        state_working = 1,
        state_using = 3,
        state_waiting = 4,
    } state;
    xr_vector<dxRender_Visual*> invisible;

    u32 frame_sleep{};

    u32 test_total{};
    u32 test_current{};

    dxRender_Visual* testQ_V{};
    u32 testQ_id{};

    u32 context_id{CHW::INVALID_CONTEXT_ID};

private:
    IC bool ready_to_work() const { return Device.dwFrame > frame_sleep; }

    void mark_invisible() const;

    void handle_occ_result(const R_occlusion::occq_result fragments);

public:
    smapvis();
    ~smapvis();

    void invalidate();

    void begin(); // should be called before 'marker++' and before graph-build
    void end();

    void flush(); // should be called when no rendering of light is supposed
    void finish(); // вызывается в дектрукторе smapvis, соотв при удалении лампочки и в CRender::reset_begin

    void feedback_callback(dxRender_Visual* V) override;
};