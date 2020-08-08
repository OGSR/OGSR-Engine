#pragma once

struct SCallbackInfo;
class CUIWindow;

class CUIWndCallback
{
public:
	using void_function = fastdelegate::FastDelegate<void(CUIWindow*, void*)>;
private:
	typedef xr_vector<SCallbackInfo*>	CALLBACKS;
	typedef CALLBACKS::iterator			CALLBACK_IT;
private:
			CALLBACKS			m_callbacks;
			SCallbackInfo*		NewCallback			();


public:
	virtual						~CUIWndCallback		();
	virtual void				OnEvent				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void				Register			(CUIWindow* pChild);
			void				AddCallback			(LPCSTR control_id, s16 event, const void_function &f);
			void				AddCallback			(const shared_str& control_id, s16 event, const void_function &f);
};