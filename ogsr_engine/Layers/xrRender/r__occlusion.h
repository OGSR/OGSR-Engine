#pragma once

#include <wrl.h>

// must conform to following order of allocation/free
// a(A), a(B), a(C), a(D), ....
// f(A), f(B), f(C), f(D), ....
// a(A), a(B), a(C), a(D), ....
//	this mean:
//		use as litle of queries as possible
//		first try to use queries allocated first
//	assumption:
//		used queries number is much smaller than total count

class R_occlusion
{
private:
    struct _Q
    {
        Microsoft::WRL::ComPtr<ID3DQuery> Q;
        u32 ttl{};
    };

    static constexpr u32 iInvalidHandle = 0xFFFFFFFF;

    bool enabled;
    xr_vector<_Q> pool; // sorted (max ... min), insertions are usually at the end
    xr_vector<_Q> used; // id's are generated from this and it is cleared from back only
    xr_vector<u32> fids; // free id's
    u32 last_frame;

    std::recursive_mutex lock;

public:
    using occq_result = u64;
    static constexpr auto OCC_NOT_AVAIL{occq_result(-1)};
    static constexpr auto OCC_CONTINUE_WAIT{OCC_NOT_AVAIL - 1};

    R_occlusion();
    ~R_occlusion();

    void occq_destroy();
    void occq_begin(u32& ID, const u32 context_id);
    void occq_end(const u32& ID, u32 context_id);
    occq_result occq_get(u32& ID, const bool for_smapvis = false);
    void occq_free(const u32 ID, const bool get_data = false);

    void set_enabled(bool v)
    {
        enabled = v;
        if (!v)
            cleanup_lost(true);
    }

    void cleanup_lost(const bool full_cleanup = false);
};
