#pragma once

struct ENGINE_API VisMask final
{
    Flags64 _visimask;
    Flags64 _visimask_ex;

    IC VisMask()
    {
        _visimask.zero();
        _visimask_ex.zero();
    }
    IC VisMask(const VisMask& _second)
    {
        _visimask.flags = _second._visimask.flags;
        _visimask_ex.flags = _second._visimask_ex.flags;
    }
    IC VisMask(u64 _low, u64 _high)
    {
        _visimask.assign(_low);
        _visimask_ex.assign(_high);
    };

    IC bool operator!=(const VisMask& _second) const noexcept
    {
        if (_visimask.flags != _second._visimask.flags)
            return true;
        if (_visimask_ex.flags != _second._visimask_ex.flags)
            return true;
        return false;
    }
    IC bool operator==(const VisMask& _second) const noexcept { return (_visimask.flags == _second._visimask.flags) && (_visimask_ex.flags == _second._visimask_ex.flags); }
    IC VisMask& operator=(const VisMask& _second)
    {
        _visimask.flags = _second._visimask.flags;
        _visimask_ex.flags = _second._visimask_ex.flags;
        return *this;
    }
    IC void set(u16 _digit, bool _set)
    {
        if (_digit < 64)
            _visimask.set(1ull << _digit, _set);
        else
            _visimask_ex.set(1ull << (_digit - 64), _set);
    }
    IC void set(u64 _low, u64 _high)
    {
        _visimask.assign(_low);
        _visimask_ex.assign(_high);
    }
    IC bool is(u16 _digit)
    {
        if (_digit < 64)
            return !!_visimask.is(1ull << _digit);
        else
            return !!_visimask_ex.is(1ull << (_digit - 64));
    }
    IC void zero()
    {
        _visimask.zero();
        _visimask_ex.zero();
    }
    IC u16 count()
    {
        u16 _c = 0;
        for (u16 i = 0; i < 64; ++i)
            if (_visimask.is(1ull << i))
                ++_c;
        for (u16 j = 0; j < 64; ++j)
            if (_visimask_ex.is(1ull << j))
                ++_c;
        return _c;
    }
    IC void set_all()
    {
        for (u16 i = 0; i < 128; ++i)
            this->set(i, true);
    }
    IC void And(const VisMask& _second)
    {
        _visimask.And(_second._visimask.flags);
        _visimask_ex.And(_second._visimask_ex.flags);
    }
    IC void invert()
    {
        _visimask.invert();
        _visimask_ex.invert();
    }
};
