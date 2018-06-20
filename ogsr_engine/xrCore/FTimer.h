#pragma once

#define FTIMER_USE_STD_CHRONO //ћаловеро€тно, но вдруг проблемы будут, мало ли что.

#ifdef FTIMER_USE_STD_CHRONO

class CTimer_paused;

class XRCORE_API pauseMngr {
	xr_vector<CTimer_paused*> m_timers;
	bool paused;
public:
	pauseMngr();
	bool Paused() const { return paused; }
	void Pause(const bool b);
	void Register(CTimer_paused& t);
	void UnRegister(CTimer_paused& t);
};

extern XRCORE_API pauseMngr g_pauseMngr;

class XRCORE_API CTimerBase {
public:
	using Clock = std::chrono::high_resolution_clock;
	using Time = std::chrono::time_point<Clock>;
	using Duration = Time::duration;
protected:
	Time startTime;
	Duration pauseDuration;
	Duration pauseAccum;
	bool paused;

public:
	constexpr CTimerBase() noexcept : startTime(), pauseDuration(), pauseAccum(), paused(false) {}
	void Start() {
		if (paused)
			return;
		startTime = Now() - pauseAccum;
	}

	virtual Duration getElapsedTime() const {
		if (paused)
			return pauseDuration;
		else
			return Now() - startTime - pauseAccum;
	}

	u32 GetElapsed_ms() const {
		using namespace std::chrono;
		return u32(duration_cast<milliseconds>(getElapsedTime()).count());
	}

	float GetElapsed_sec() const {
		using namespace std::chrono;
		return duration_cast<duration<float>>(getElapsedTime()).count();
	}

	Time Now() const { return Clock::now(); }

	void Dump() const { Msg("* Elapsed time (sec): %f", GetElapsed_sec()); }
};

class XRCORE_API CTimer : public CTimerBase {
	float m_time_factor;
	Duration realTime;
	Duration time;

	inline Duration getElapsedTime(const Duration& current) const {
		const auto delta = current - realTime;
		const double deltaD = double(delta.count());
		const double time = deltaD * m_time_factor + .5;
		const auto result = u64(time);
		return Duration(this->time.count() + result);
	}

public:
	constexpr CTimer() noexcept : m_time_factor(1.f), realTime(0), time(0) {}

	void Start() noexcept {
		if (paused)
			return;

		realTime = std::chrono::nanoseconds(0);
		time = std::chrono::nanoseconds(0);
		__super::Start();
	}

	float time_factor() const noexcept { return m_time_factor; }

	void time_factor(const float time_factor) noexcept {
		const Duration current = __super::getElapsedTime();
		time = getElapsedTime(current);
		realTime = current;
		m_time_factor = time_factor;
	}

	virtual Duration getElapsedTime() const {
		return getElapsedTime(__super::getElapsedTime());
	}
};

class XRCORE_API CTimer_paused_ex : public CTimer {
	Time save_clock;
public:
	CTimer_paused_ex() noexcept : save_clock() {}
	virtual ~CTimer_paused_ex() = default;
	bool Paused() const noexcept { return paused; }

	void Pause(const bool b) noexcept {
		if (paused == b)
			return;

		const auto current = Now();
		if (b) {
			save_clock = current;
			pauseDuration = CTimerBase::getElapsedTime();
		}
		else {
			pauseAccum += current - save_clock;
		}
		paused = b;
	}
};

class XRCORE_API CTimer_paused : public CTimer_paused_ex {
public:
	CTimer_paused() { g_pauseMngr.Register(*this); }
	virtual ~CTimer_paused() { g_pauseMngr.UnRegister(*this); }
};

extern XRCORE_API BOOL g_bEnableStatGather;

class XRCORE_API CStatTimer {
	using Duration = CTimerBase::Duration;

public:
	CTimer T;
	Duration accum;
	float result;
	u32 count;

	CStatTimer() : T(), accum(), result(.0f), count(0) {}
	void FrameStart();
	void FrameEnd();

	void Begin() {
		if (!g_bEnableStatGather)
			return;
		count++;
		T.Start();
	}

	void End() {
		if (!g_bEnableStatGather)
			return;
		accum += T.getElapsedTime();
	}

	Duration getElapsedTime() const {
		return accum;
	}

	u32 GetElapsed_ms() const {
		using namespace std::chrono;
		return u32(duration_cast<milliseconds>(getElapsedTime()).count());
	}

	float GetElapsed_sec() const {
		using namespace std::chrono;
		return duration_cast<duration<float>>(getElapsedTime()).count();
	}
};

#else

class CTimerBase
{
protected:
	u64 qwStartTime;
	u64 qwPausedTime;
	u64 qwPauseAccum;
	BOOL bPause;
public:
	CTimerBase() : qwStartTime(0), qwPausedTime(0), qwPauseAccum(0), bPause(FALSE) { }

	ICF void Start()
	{
		if (bPause)
			return;
		qwStartTime = CPU::QPC() - qwPauseAccum;
	}

	ICF u64 GetElapsed_ticks()const
	{
		if (bPause)
		{
			return qwPausedTime;
		}
		else
		{
			return CPU::QPC() - qwStartTime - qwPauseAccum;
		}
	}

	ICF u32 GetElapsed_ms()const
	{
		return u32(GetElapsed_ticks()*u64(1000) / CPU::qpc_freq);
	}

	ICF float GetElapsed_sec()const
	{
		float _result = float(double(GetElapsed_ticks()) / double(CPU::qpc_freq));
		return _result;
	}

	ICF void Dump() const
	{
		Msg("* Elapsed time (sec): %f", GetElapsed_sec());
	}
};

class CTimer : public CTimerBase
{
private:
	typedef CTimerBase inherited;

private:
	float m_time_factor;
	u64 m_real_ticks;
	u64 m_ticks;

private:
	ICF u64 GetElapsed_ticks(const u64& current_ticks) const
	{
		u64 delta = current_ticks - m_real_ticks;
		double delta_d = (double)delta;
		double time_factor_d = time_factor();
		double time = delta_d * time_factor_d + .5;
		u64 result = (u64)time;
		return (m_ticks + result);
	}

public:
	ICF CTimer() : m_time_factor(1.f), m_real_ticks(0), m_ticks(0) {}

	ICF void Start()
	{
		if (bPause)
			return;

		inherited::Start();

		m_real_ticks = 0;
		m_ticks = 0;
	}

	IC const float& time_factor() const
	{
		return (m_time_factor);
	}

	ICF void time_factor(const float& time_factor)
	{
		u64 current = inherited::GetElapsed_ticks();
		m_ticks = GetElapsed_ticks(current);
		m_real_ticks = current;
		m_time_factor = time_factor;
	}

	ICF u64 GetElapsed_ticks() const
	{
		u64 result = GetElapsed_ticks(inherited::GetElapsed_ticks());

		return (result);
	}

	ICF u32 GetElapsed_ms() const
	{
		return (u32(GetElapsed_ticks()*u64(1000) / CPU::qpc_freq));
	}

	ICF float GetElapsed_sec() const
	{
		float result = float(double(GetElapsed_ticks()) / double(CPU::qpc_freq));
		return (result);
	}

	ICF void Dump() const
	{
		Msg("* Elapsed time (sec): %f", GetElapsed_sec());
	}
};

class CTimer_paused_ex : public CTimer
{
	u64 save_clock;
public:
	CTimer_paused_ex() { }
	virtual ~CTimer_paused_ex() { }
	ICF BOOL Paused()const { return bPause; }
	ICF void Pause(BOOL b)
	{
		if (bPause == b) return;

		u64 _current = CPU::QPC();
		if (b)
		{
			save_clock = _current;
			qwPausedTime = CTimerBase::GetElapsed_ticks();
		}
		else
		{
			qwPauseAccum += _current - save_clock;
		}
		bPause = b;
	}
};

class CTimer_paused;

class pauseMngr {
	xr_vector<CTimer_paused_ex*> m_timers;
	bool paused;
public:
	pauseMngr() :paused(false)
	{
		m_timers.reserve(3);
	}
	ICF bool Paused() const { return paused; }
	ICF void Pause(const bool b)
	{
		if (paused == b)return;

		xr_vector<CTimer_paused_ex*>::iterator it = m_timers.begin();
		for (; it != m_timers.end(); ++it)
			(*it)->Pause(b);

		paused = b;
	}

	ICF void Register(CTimer_paused* t)
	{
		m_timers.push_back(reinterpret_cast<CTimer_paused_ex*> (t));
	}

	ICF void UnRegister(CTimer_paused* t)
	{
		CTimer_paused_ex* timer = reinterpret_cast<CTimer_paused_ex*>(t);
		xr_vector<CTimer_paused_ex*>::iterator it = std::find(m_timers.begin(), m_timers.end(), timer);
		if (it != m_timers.end())
			m_timers.erase(it);
	}
};

extern XRCORE_API pauseMngr g_pauseMngr;

class CTimer_paused : public CTimer_paused_ex {
public:
	ICF CTimer_paused() { g_pauseMngr.Register(this); }
	ICF virtual ~CTimer_paused() { g_pauseMngr.UnRegister(this); }
};


extern XRCORE_API BOOL g_bEnableStatGather;

class XRCORE_API CStatTimer
{
public:
	CTimer T;
	u64 accum;
	float result;
	u32 count;
public:
	CStatTimer();
	void FrameStart();
	void FrameEnd();

	ICF void Begin() { if (!g_bEnableStatGather) return; count++; T.Start(); }
	ICF void End() { if (!g_bEnableStatGather) return; accum += T.GetElapsed_ticks(); }

	ICF u64 GetElapsed_ticks()const { return accum; }

	IC u32 GetElapsed_ms()const { return u32(GetElapsed_ticks()*u64(1000) / CPU::qpc_freq); }
	IC float GetElapsed_sec()const
	{
		float _result = float(double(GetElapsed_ticks()) / double(CPU::qpc_freq));
		return _result;
	}
};

#endif
