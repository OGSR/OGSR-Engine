// XR_IOConsole.h: interface for the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/UIShader.h"

//refs
class ENGINE_API CGameFont;
class ENGINE_API IConsole_Command;

namespace text_editor
{
	class line_editor;
	class line_edit_control;
};

struct TipString
{
	shared_str text;
	int HL_start; // Highlight
	int HL_finish;

	TipString()
	{
		text = "";
		HL_start = 0;
		HL_finish = 0;
	}

	TipString(shared_str const& tips_text, int start_pos, int finish_pos)
	{
		text = tips_text;
		HL_start = start_pos;
		HL_finish = finish_pos;
	}

	TipString(LPCSTR tips_text, int start_pos, int finish_pos)
	{
		text = tips_text;
		HL_start = start_pos;
		HL_finish = finish_pos;
	}

	TipString(shared_str const& tips_text)
	{
		text = tips_text;
		HL_start = 0;
		HL_finish = 0;
	}

	IC bool operator==(shared_str const& tips_text) const
    {
		return (text == tips_text);
	}
};

class ENGINE_API CConsole :
	public pureRender,
	public pureFrame,
	public pureScreenResolutionChanged
{
public:

	typedef xr_map<LPCSTR, IConsole_Command*, pred_str> vecCMD;
	typedef vecCMD::iterator vecCMD_IT;
	typedef vecCMD::const_iterator vecCMD_CIT;
    typedef fastdelegate::FastDelegate<void()> Callback;
	typedef xr_vector<shared_str> vecHistory;
	typedef xr_vector<shared_str> vecTips;
	typedef xr_vector<TipString> vecTipsEx;

	enum { CONSOLE_BUF_SIZE = 1024 };

	enum { VIEW_TIPS_COUNT = 14, MAX_TIPS_COUNT = 220 };

protected:
	int scroll_delta;

	CGameFont* pFont;
	CGameFont* pFont2;

	FactoryPtr<IUIShader>* m_hShader_back;

	//POINT m_mouse_pos;
	bool m_disable_tips;

private:
	vecHistory m_cmd_history;
	u32 m_cmd_history_max;
	int m_cmd_history_idx;
	shared_str m_last_cmd;

	vecTips m_temp_tips;
	vecTipsEx m_tips;
	u32 m_tips_mode;
	shared_str m_cur_cmd;
	int m_select_tip;
	int m_start_tip;

	u32 m_prev_length_str;
    shared_str m_prev_str;

public:
	CConsole();
	virtual ~CConsole();
	virtual void Initialize();
	//virtual void Destroy();

	virtual void OnRender();
	virtual void OnFrame();
	virtual void OnScreenResolutionChanged();

	string64 ConfigFile;
	bool bVisible;
	vecCMD Commands;
    float lineDistance;

	void AddCommand(IConsole_Command* cc);
	void RemoveCommand(IConsole_Command* cc);

	void Show();
	void Hide();

	void Execute(LPCSTR cmd);
    void Execute(LPCSTR cmd, LPCSTR arg);

	void ExecuteScript(LPCSTR str);
    void ExecuteCommand(LPCSTR cmd, bool record_cmd = true, bool allow_disabled = false);
	void SelectCommand();

	bool GetBool(LPCSTR cmd) const;
	float GetFloat(LPCSTR cmd, float& min, float& max) const;
	int GetInteger(LPCSTR cmd, int& min, int& max) const;
	LPCSTR GetString(LPCSTR cmd) const;
	LPCSTR GetToken(LPCSTR cmd) const;
    const xr_token* GetXRToken(LPCSTR cmd) const;
	Fvector GetFVector3(LPCSTR cmd) const;
    void GetFVector3Bounds(LPCSTR cmd, Fvector& imin, Fvector& imax) const;
	Fvector* GetFVector3Ptr(LPCSTR cmd) const;
    Fvector4 GetFVector4(LPCSTR cmd) const;
    void GetFVector4Bounds(LPCSTR cmd, Fvector4& imin, Fvector4& imax) const;
    Fvector4* GetFVector4Ptr(LPCSTR cmd) const;

	IConsole_Command* GetCommand(LPCSTR cmd) const;
protected:
	text_editor::line_editor* m_editor;
	text_editor::line_edit_control& ec();

	enum Console_mark // (int)=char
	{
		no_mark = ' ',
		mark0 = '~',
		mark1 = '!',
		// error
		mark2 = '@',
		// console cmd
		mark3 = '#',
		mark4 = '$',
		mark5 = '%',
		mark6 = '^',
		mark7 = '&',
		mark8 = '*',
		mark9 = '-',
		// green = ok
		mark10 = '+',
		mark11 = '=',
		mark12 = '/'
	};

	bool is_mark(Console_mark type);
	u32 get_mark_color(Console_mark type);

	void DrawBackgrounds(bool bGame);
	void DrawRect(Frect const& r, u32 color);
	void OutFont(LPCSTR text, float& pos_y);
	void Register_callbacks();

protected:
	void Screenshot();

	void Prev_log();
	void Next_log();
	void Begin_log();
	void End_log();

	void Find_cmd();
	void Find_cmd_back();
	void Prev_cmd();
	void Next_cmd();
	void Prev_tip();
	void Next_tip();

	void Begin_tips();
	void End_tips();
	void PageUp_tips();
	void PageDown_tips();

	void Execute_cmd();
	void Show_cmd();
	void Hide_cmd();
	void Hide_cmd_esc();

	void GamePause();

protected:
	void add_cmd_history(shared_str const& str);
	void next_cmd_history_idx();
	void prev_cmd_history_idx();
	void reset_cmd_history_idx();

	void next_selected_tip();
	void check_next_selected_tip();
	void prev_selected_tip();
	void check_prev_selected_tip();
	void reset_selected_tip();

	IConsole_Command* find_next_cmd(LPCSTR in_str, shared_str& out_str);
	bool add_next_cmds(LPCSTR in_str, vecTipsEx& out_v);
	bool add_internal_cmds(LPCSTR in_str, vecTipsEx& out_v);

	void update_tips();
    void reset_tips();
	void select_for_filter(LPCSTR filter_str, vecTips& in_v, vecTipsEx& out_v);
}; // class CConsole

ENGINE_API extern CConsole* Console;
