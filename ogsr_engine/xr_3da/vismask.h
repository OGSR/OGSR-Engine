#ifndef VisMaskH
#define VisMaskH

struct /* ENGINE_API*/ VisMask
{
	Flags64						_visimask;
	Flags64						_visimask_ex;
	IC 	VisMask() { _visimask.zero();  _visimask_ex.zero(); }
	IC 	VisMask(const VisMask& _second) { _visimask.flags = _second._visimask.flags;  _visimask_ex.flags = _second._visimask_ex.flags; }
	IC 	VisMask(u64 _low, u64 _high) { _visimask.assign(_low); _visimask_ex.assign(_high);	};

	IC bool							operator!=	(const VisMask& _second) const
	{
		if (_visimask.flags != _second._visimask.flags)
			return true;
		if (_visimask_ex.flags != _second._visimask_ex.flags)
			return true;
		return false;
	}
	IC VisMask&						operator=	(const VisMask& _second)
	{
		_visimask.flags = _second._visimask.flags;
		_visimask_ex.flags = _second._visimask_ex.flags;
		return *this;
	}
	IC	 void						set(u16 _digit, bool _set)
	{
		if (_digit < 64)
			_visimask.set(u64(1) << _digit, _set);
		else
			_visimask_ex.set(u64(1) << (_digit - 64), _set);
	}
	IC	void						set(u64 _low, u64 _high)
	{
		_visimask.assign(_low);
		_visimask_ex.assign(_high);
	}
	IC	bool						is(u16 _digit)
	{
		if (_digit < 64)
			return _visimask.is(u64(1) << _digit);
		else
			return _visimask_ex.is(u64(1) << (_digit - 64));
	}
	IC	void						zero()
	{
		_visimask.zero();
		_visimask_ex.zero();
	}
	IC	u16							count()
	{
		u16 _c = 0;
		for (u16 i = 0; i < 64; ++i)
			if (_visimask.is(i))
				++_c;
		for (u16 j = 0; j < 64; ++j)
			if (_visimask_ex.is(j))
				++_c;
		return _c;
	}
	IC	void						set_all()
	{
		for (u8 i = 0; i < 64; ++i)
			_visimask.set(0xffffffffffffffffL, true);
		for (u8 j = 0; j < 64; ++j)
			_visimask_ex.set(0xffffffffffffffffL, true);
	}
	IC	void						and (const VisMask& _second)
	{
		_visimask. and (_second._visimask.flags);
		_visimask_ex. and (_second._visimask_ex.flags);
	}
	IC	void						invert()
	{
		_visimask.invert();
		_visimask_ex.invert();
	}
};

#endif