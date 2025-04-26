// XR_IOConsole.cpp: implementation of the CConsole class.
// modify 15.05.2008 sea

#include "stdafx.h"
#include "XR_IOConsole.h"
#include "line_editor.h"

#include "igame_level.h"
#include "igame_persistent.h"

#include "x_ray.h"
#include "xr_input.h"
#include "xr_ioc_cmd.h"
#include "GameFont.h"

#include "../Include/xrRender/UIRender.h"

constexpr float UI_BASE_WIDTH = 1024.0f;
constexpr float UI_BASE_HEIGHT = 768.0f;

constexpr float LDIST = 0.05f;

constexpr u32 cmd_history_max = 64;

constexpr u32 prompt_font_color = color_rgba(228, 228, 255, 255);
constexpr u32 tips_font_color = color_rgba(230, 250, 230, 255);
constexpr u32 cmd_font_color = color_rgba(138, 138, 245, 255);
constexpr u32 cursor_font_color = color_rgba(255, 255, 255, 255);
constexpr u32 total_font_color = color_rgba(250, 250, 15, 180);
constexpr u32 default_font_color = color_rgba(250, 250, 250, 250);

constexpr u32 back_color = color_rgba(20, 20, 20, 200);
constexpr u32 tips_back_color = color_rgba(20, 20, 20, 200);
constexpr u32 tips_select_color = color_rgba(90, 90, 140, 230);
constexpr u32 tips_word_color = color_rgba(5, 100, 56, 200);
constexpr u32 tips_scroll_back_color = color_rgba(15, 15, 15, 230);
constexpr u32 tips_scroll_pos_color = color_rgba(70, 70, 70, 240);

ENGINE_API CConsole* Console = NULL;

extern char const* const ioc_prompt;
char const* const ioc_prompt = ">>> ";

extern char const* const ch_cursor;
char const* const ch_cursor = "_";

BOOL g_console_show_always = FALSE;


static inline void split_cmd(const std::string& str, std::string& out1, std::string& out2)
{
    size_t it{}, start{}, end{};
    while ((start = str.find_first_not_of(' ', end)) != std::string::npos)
    {
        if (it == 0)
        {
            end = str.find(' ', start);
            out1 = str.substr(start, end - start);
        }
        else
        {
            end = str.length();
            out2 = str.substr(start, end - start);
            return;
        }
        it++;
    }
}


text_editor::line_edit_control& CConsole::ec()
{
	return m_editor->control();
}

u32 CConsole::get_mark_color(Console_mark type)
{
	u32 color = default_font_color;
	switch (type)
	{
	case mark0:
		color = color_rgba(255, 255, 0, 255);
		break;
	case mark1:
		color = color_rgba(255, 0, 0, 255);
		break;
	case mark2:
		color = color_rgba(100, 100, 255, 255);
		break;
	case mark3:
		color = color_rgba(0, 222, 205, 155);
		break;
	case mark4:
		color = color_rgba(255, 0, 255, 255);
		break;
	case mark5:
		color = color_rgba(155, 55, 170, 155);
		break;
	case mark6:
		color = color_rgba(25, 200, 50, 255);
		break;
	case mark7:
		color = color_rgba(255, 255, 0, 255);
		break;
	case mark8:
		color = color_rgba(128, 128, 128, 255);
		break;
	case mark9:
		color = color_rgba(0, 255, 0, 255);
		break;
	case mark10:
		color = color_rgba(55, 155, 140, 255);
		break;
	case mark11:
		color = color_rgba(205, 205, 105, 255);
		break;
	case mark12:
		color = color_rgba(128, 128, 250, 255);
		break;
	case no_mark:
	default:
		break;
	}
	return color;
}

bool CConsole::is_mark(Console_mark type)
{
	switch (type)
	{
	case mark0:
	case mark1:
	case mark2:
	case mark3:
	case mark4:
	case mark5:
	case mark6:
	case mark7:
	case mark8:
	case mark9:
	case mark10:
	case mark11:
	case mark12:
		return true;
		break;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CConsole::CConsole()
	: m_hShader_back(NULL)
{
	m_editor = xr_new<text_editor::line_editor>((u32)CONSOLE_BUF_SIZE);
	m_cmd_history_max = cmd_history_max;
	m_disable_tips = false;
	Register_callbacks();
	Device.seqResolutionChanged.Add(this);
    lineDistance = LDIST;
}

void CConsole::Initialize()
{
	scroll_delta = 0;
	bVisible = false;
	pFont = NULL;
	pFont2 = NULL;

	//m_mouse_pos.x = 0;
	//m_mouse_pos.y = 0;
	m_last_cmd = nullptr;

	m_cmd_history.reserve(m_cmd_history_max + 2);
	m_cmd_history.clear();
	reset_cmd_history_idx();

	m_tips.reserve(MAX_TIPS_COUNT + 1);
    m_tips.clear();
	m_temp_tips.reserve(MAX_TIPS_COUNT + 1);
    m_temp_tips.clear();

	m_tips_mode = 0;
	m_prev_length_str = 0;
	m_cur_cmd = NULL;
	reset_selected_tip();

	// Commands
	extern void CCC_Register();
	CCC_Register();
}

CConsole::~CConsole()
{
	xr_delete(m_hShader_back);
	xr_delete(m_editor);
    Execute("cfg_save");

    xr_delete(pFont);
    xr_delete(pFont2);
    Commands.clear();

    if (g_console_show_always)
        Device.seqRender.Remove(this);

	Device.seqResolutionChanged.Remove(this);
}

//void CConsole::Destroy()
//{
//    
//}

void CConsole::AddCommand(IConsole_Command* cc)
{
	Commands[cc->Name()] = cc;
}

void CConsole::RemoveCommand(IConsole_Command* cc)
{
	vecCMD_IT it = Commands.find(cc->Name());
	if (Commands.end() != it)
	{
		Commands.erase(it);
	}
}

void CConsole::OnFrame()
{
	m_editor->on_frame();

	if (Device.dwFrame % 10 == 0)
	{
		update_tips();
	}
}

void CConsole::OutFont(LPCSTR text, float& pos_y)
{
	float str_length = pFont->SizeOf_(text);
	float scr_width = 1.98f * Device.fWidth_2;
	if (str_length > scr_width) //1024.0f
	{
		int sz = 0;
		int ln = 0;
		LPSTR one_line = (LPSTR)_alloca((CONSOLE_BUF_SIZE + 1) * sizeof(char));

		while (text[sz] && (ln + sz < CONSOLE_BUF_SIZE - 5)) // перенос строк
		{
			one_line[ln + sz] = text[sz];
			one_line[ln + sz + 1] = 0;

			float t = pFont->SizeOf_(one_line + ln);
			if (t > scr_width)
			{
				OutFont(text + sz + 1, pos_y);
				pos_y -= lineDistance;
				pFont->OutI(-1.0f, pos_y, "%s", one_line + ln);
				ln = sz + 1;
            }

			++sz;
		}
	}
	else
	{
		pFont->OutI(-1.0f, pos_y, "%s", text);
	}
}

void CConsole::OnScreenResolutionChanged()
{
	xr_delete(pFont);
	xr_delete(pFont2);
}

void CConsole::OnRender()
{
    if (!bVisible && !g_console_show_always)
	{
		return;
	}
	
	if (!m_hShader_back)
	{
		m_hShader_back = xr_new<FactoryPtr<IUIShader>>();
		(*m_hShader_back)->create("hud\\default", "ui\\ui_console"); // "ui\\ui_empty"
	}

	if (!pFont)
	{
		pFont = xr_new<CGameFont>("hud_font_di", CGameFont::fsDeviceIndependent);
		pFont->SetHeightI(0.025f);

		lineDistance = LDIST * pFont->GetInterval().y;
	}

	if (!pFont2)
	{
		pFont2 = xr_new<CGameFont>(pSettings->section_exist("hud_font_di2") ? "hud_font_di2" : "hud_font_di", CGameFont::fsDeviceIndependent);
		pFont2->SetHeightI(0.025f);
	}

    constexpr bool bGame = true;

	if (bVisible)
		DrawBackgrounds(bGame);

	float fMaxY{0.0f};

	float ypos = fMaxY - LDIST; // тут нужно всегда вычитать оригинальную высоту без учета интервала
	float scr_x = 1.0f / Device.fWidth_2;

	//---------------------------------------------------------------------------------
	float scr_width = 1.9f * Device.fWidth_2;
	float ioc_d = pFont->SizeOf_(ioc_prompt);
	float d1 = pFont->SizeOf_("_");

	if (bVisible)
    {
        LPCSTR s_cursor = ec().str_before_cursor();

        LPCSTR s_b_mark = ec().str_before_mark();
        LPCSTR s_mark = ec().str_mark();
        LPCSTR s_mark_a = ec().str_after_mark();

        // strncpy_s( buf1, cur_pos, editor, MAX_LEN );
        float str_length = ioc_d + pFont->SizeOf_(s_cursor);
        float out_pos = 0.0f;

        if (str_length > scr_width)
        {
            out_pos -= (str_length - scr_width);
            str_length = scr_width;
        }

        pFont->SetColor(prompt_font_color);
        pFont->OutI(-1.0f + out_pos * scr_x, ypos, "%s", ioc_prompt);
        out_pos += ioc_d;

        if (!m_disable_tips && !m_tips.empty())
        {
            pFont->SetColor(tips_font_color);

            float shift_x = 0.0f;
            switch (m_tips_mode)
            {
            case 0: shift_x = scr_x * 1.0f; break;
            case 1: shift_x = scr_x * out_pos; break;
            case 2: shift_x = scr_x * (ioc_d + pFont->SizeOf_(m_cur_cmd.c_str()) + d1); break;
            case 3: shift_x = scr_x * str_length; break;
            }

			float start = fMaxY;

			start = start - (1.f - pFont->GetInterval().y) * lineDistance; // какая то магия что б первая строка подсказок не прыгала при измении интервала

            vecTipsEx::iterator itb = m_tips.begin() + m_start_tip;
            vecTipsEx::iterator ite = m_tips.end();
            for (u32 i = 0; itb != ite; ++itb, ++i) // tips
            {
                pFont->OutI(-1.0f + shift_x, start + i * lineDistance, "%s", (*itb).text.c_str());
                if (i >= VIEW_TIPS_COUNT - 1)
                {
                    break; // for
                }
            }
        }

        // ===== ==============================================
        pFont->SetColor(cmd_font_color);
        pFont2->SetColor(cmd_font_color);

		// из за того что тут строка по факту состоит из 3х, и из за того что ширина строк округляется, при рендере все плянет
		// переделал тут на символьный вывод. в таком случае оно получше все выглядит

		auto draw_string = [&](CGameFont* f, LPCSTR str) {
            for (size_t c = 0; c < strlen(str); c++)
            {
                f->OutI(-1.0f + out_pos * scr_x, ypos, "%c", str[c]);
                out_pos += f->SizeOf_(str[c]);
            }
        };

		draw_string(pFont, s_b_mark);
        draw_string(pFont2, s_mark);
        draw_string(pFont, s_mark_a);

        if (ec().cursor_view())
        {
            pFont->SetColor(cursor_font_color);
            pFont->OutI(-1.0f + str_length * scr_x, ypos, "%s", ch_cursor);
        }
    }

	// ---------------------
	u32 log_line = LogFile.size() - 1;
	ypos -= lineDistance;
	for (int i = log_line - scroll_delta; i >= 0; --i)
	{
		ypos -= lineDistance;
		if (ypos < -1.0f)
		{
			break;
		}
		LPCSTR ls = ((LogFile)[i]).c_str();

		if (!ls)
		{
			continue;
		}
		Console_mark cm = (Console_mark)ls[0];
		pFont->SetColor(get_mark_color(cm));
		//u8 b = (is_mark( cm ))? 2 : 0;
		//OutFont( ls + b, ypos );
		OutFont(ls, ypos);
	}

	string16 q;
	itoa(log_line, q, 10);
	u32 qn = xr_strlen(q);
	pFont->SetColor(total_font_color);
    pFont->OutI(0.95f - 0.03f * qn, fMaxY - 2.0f * (LDIST), "[%d]", log_line + 1);

	pFont->OnRender();
	pFont2->OnRender();
}

void CConsole::DrawBackgrounds(bool bGame)
{
	float ky = (bGame) ? 0.5f : 1.0f;

	Frect r;
	r.set(0.0f, 0.0f, float(Device.dwWidth), ky * float(Device.dwHeight));

	UIRender->SetShader(**m_hShader_back);
	// 6 = back, 12 = tips, (VIEW_TIPS_COUNT+1)*6 = highlight_words, 12 = scroll
	UIRender->StartPrimitive(6 + 12 + (VIEW_TIPS_COUNT + 1) * 6 + 12, IUIRender::ptTriList, IUIRender::pttTL);

	DrawRect(r, back_color);

	if (m_tips.empty() || m_disable_tips)
	{
		UIRender->FlushPrimitive();
		return;
	}

	LPCSTR max_str = "xxxxx";
	vecTipsEx::iterator itb = m_tips.begin();
	vecTipsEx::iterator ite = m_tips.end();
	for (; itb != ite; ++itb)
	{
		if (pFont->SizeOf_((*itb).text.c_str()) > pFont->SizeOf_(max_str))
		{
			max_str = (*itb).text.c_str();
		}
	}

	float w1 = pFont->SizeOf_("_");
	float ioc_w = pFont->SizeOf_(ioc_prompt) - w1;
	float cur_cmd_w = pFont->SizeOf_(m_cur_cmd.c_str());
	cur_cmd_w += (cur_cmd_w > 0.01f) ? w1 : 0.0f;

	float list_w = pFont->SizeOf_(max_str) + 2.0f * w1;

	float font_h = pFont->CurrentHeight_();
    float tips_h = std::min(m_tips.size(), (size_t)VIEW_TIPS_COUNT) * font_h;
	//tips_h += (!m_tips.empty()) ? 5.0f : 0.0f; // убрал, хз зачем там 5 пикслелей снизу добовлялось. не красиво )

	Frect pr, sr;
	pr.x1 = ioc_w + cur_cmd_w;
	pr.x2 = pr.x1 + list_w;

	pr.y1 = UI_BASE_HEIGHT * 0.5f;
	pr.y1 *= float(Device.dwHeight) / UI_BASE_HEIGHT;

	pr.y2 = pr.y1 + tips_h;

	DrawRect(pr, tips_back_color);

	float select_y = 0.0f;
	float select_h = 0.0f;

	if (m_select_tip >= 0 && m_select_tip < (int)m_tips.size())
	{
		int sel_pos = m_select_tip - m_start_tip;

		select_y = sel_pos * font_h;
		select_h = font_h; //1 string
	}

	sr.x1 = pr.x1;
	sr.y1 = pr.y1 + select_y;

	sr.x2 = pr.x2;
	sr.y2 = sr.y1 + select_h;

	DrawRect(sr, tips_select_color);

	// --------------------------- highlight words --------------------

	if (m_select_tip < (int)m_tips.size())
	{
		Frect r;
		xr_string tmp;
		vecTipsEx::iterator itb = m_tips.begin() + m_start_tip;
		vecTipsEx::iterator ite = m_tips.end();
		for (u32 i = 0; itb != ite; ++itb, ++i) // tips
		{
			TipString const& ts = (*itb);
			if ((ts.HL_start < 0) || (ts.HL_finish < 0) || (ts.HL_start > ts.HL_finish))
			{
				continue;
			}
			int str_size = (int)ts.text.size();
			if ((ts.HL_start >= str_size) || (ts.HL_finish > str_size))
			{
				continue;
			}

			r.null();
			tmp.assign(ts.text.c_str(), ts.HL_start);
			r.x1 = pr.x1 + w1 + pFont->SizeOf_(tmp.c_str());
			r.y1 = pr.y1 + i * font_h;

			tmp.assign(ts.text.c_str(), ts.HL_finish);
			r.x2 = pr.x1 + w1 + pFont->SizeOf_(tmp.c_str());
			r.y2 = r.y1 + font_h;

			DrawRect(r, tips_word_color);

			if (i >= VIEW_TIPS_COUNT - 1)
			{
				break; // for itb
			}
		} // for itb
	} // if

	// --------------------------- scroll bar --------------------

	u32 tips_sz = m_tips.size();
	if (tips_sz > VIEW_TIPS_COUNT)
	{
		Frect rb, rs;

		rb.x1 = pr.x2;
		rb.y1 = pr.y1;
		rb.x2 = rb.x1 + 2 * w1;
		rb.y2 = pr.y2;
		DrawRect(rb, tips_scroll_back_color);

		VERIFY(rb.y2 - rb.y1 >= 1.0f);
		float back_height = rb.y2 - rb.y1;
		float u_height = (back_height * static_cast<float>(VIEW_TIPS_COUNT)) / float(tips_sz);
		if (u_height < 0.5f * font_h)
		{
			u_height = 0.5f * font_h;
		}

		//float u_pos = (back_height - u_height) * float(m_start_tip) / float(tips_sz);
		float u_pos = back_height * float(m_start_tip) / float(tips_sz);

		//clamp( u_pos, 0.0f, back_height - u_height );

		rs = rb;
		rs.y1 = pr.y1 + u_pos;
		rs.y2 = rs.y1 + u_height;
		DrawRect(rs, tips_scroll_pos_color);
	}

	UIRender->FlushPrimitive();
}

void CConsole::DrawRect(Frect const& r, u32 color)
{
	UIRender->PushPoint(r.x1, r.y1, 0.0f, color, 0.0f, 0.0f);
	UIRender->PushPoint(r.x2, r.y1, 0.0f, color, 1.0f, 0.0f);
	UIRender->PushPoint(r.x2, r.y2, 0.0f, color, 1.0f, 1.0f);

	UIRender->PushPoint(r.x1, r.y1, 0.0f, color, 0.0f, 0.0f);
	UIRender->PushPoint(r.x2, r.y2, 0.0f, color, 1.0f, 1.0f);
	UIRender->PushPoint(r.x1, r.y2, 0.0f, color, 0.0f, 1.0f);
}

//extern bool CCC_OnUnknownCommand(std::string c, std::string a);
bool CCC_OnUnknownCommand(std::string c, std::string a)
{
#pragma todo("Simp: перенести функцию CCC_OnUnknownCommand в xrGame и добавить engine_callback")
   // static engine_callback<bool> ec("on_console_command");

   // return ec(c.c_str(), a.c_str());
   return false;
}

void CConsole::ExecuteCommand(LPCSTR cmd_str, bool record_cmd, bool allow_disabled)
{
    std::string edt{cmd_str};

	xr_string_utils::trim(edt);

	if (edt.empty())
		return;

	if (record_cmd)
	{
		scroll_delta = 0;
		reset_cmd_history_idx();
		reset_selected_tip();

		char c[2];
		c[0] = mark2;
		c[1] = 0;

		if (m_last_cmd.c_str() == 0 || xr_strcmp(m_last_cmd, edt.c_str()) != 0)
		{
            Msg("%s %s", c, edt.c_str());
            add_cmd_history(edt.c_str());
            m_last_cmd = edt.c_str();
		}
	}

    std::string first, last;
    split_cmd(edt, first, last);

	// search
	vecCMD_IT it = Commands.find(first.c_str());
	if (it != Commands.end())
	{
		IConsole_Command* cc = it->second;
        if (cc && (cc->bEnabled || allow_disabled))
		{
			if (cc->bLowerCaseArgs)
			{
                xr_string_utils::strlwr(last);
			}
			if (last.empty())
			{
				if (cc->bEmptyArgsHandled)
				{
					cc->Execute(last.c_str());
				}
				else
				{
					IConsole_Command::TStatus stat;
					cc->Status(stat);
					Msg("- %s %s", cc->Name(), stat);
				}
			}
			else
			{
				cc->Execute(last.c_str());
				if (record_cmd)
				{
					cc->add_to_LRU(last.c_str());
				}
			}
		}
		else
		{
            Msg("! Command disabled: %s", first.c_str());
		}
	}
	else
	{
		if (!CCC_OnUnknownCommand(first.c_str(), last.c_str()))
            Msg("! Unknown command: %s", first.c_str());
	}

	if (record_cmd)
	{
		ec().clear_states();
	}
}

void CConsole::Show()
{
	//SECUROM_MARKER_HIGH_SECURITY_ON(11)

	if (bVisible)
	{
		return;
	}
	bVisible = true;

	//GetCursorPos(&m_mouse_pos);

	ec().clear_states();
	scroll_delta = 0;
	reset_cmd_history_idx();
	reset_selected_tip();
	update_tips();

	m_editor->IR_Capture();

	if (!g_console_show_always)
		Device.seqRender.Add(this, 1);
	Device.seqFrame.Add(this);

	//SECUROM_MARKER_HIGH_SECURITY_OFF(11)
}

extern CInput* pInput;

void CConsole::Hide()
{
	if (!bVisible)
	{
		return;
	}

	//if (pInput->exclusive_mode())
	//{
	//	SetCursorPos(m_mouse_pos.x, m_mouse_pos.y);
	//}

	bVisible = false;
	reset_selected_tip();
	update_tips();

	Device.seqFrame.Remove(this);
    if (!g_console_show_always)
		Device.seqRender.Remove(this);
	m_editor->IR_Release();
}

void CConsole::SelectCommand()
{
	if (m_cmd_history.empty())
	{
		return;
	}
	VERIFY(0 <= m_cmd_history_idx && m_cmd_history_idx < (int)m_cmd_history.size());

	vecHistory::reverse_iterator it_rb = m_cmd_history.rbegin() + m_cmd_history_idx;
	ec().set_edit((*it_rb).c_str());
	reset_selected_tip();
}

void CConsole::Execute(LPCSTR cmd)
{ 
	ExecuteCommand(cmd, false, true); 
}

void CConsole::Execute(LPCSTR cmd, LPCSTR arg)
{
    string1024 full_cmd;
	xr_strconcat(full_cmd, cmd, " ", arg);

    ExecuteCommand(full_cmd, false, true);
}

void CConsole::ExecuteScript(LPCSTR str)
{
    string_path cmd;
    xr_strconcat(cmd, "cfg_load ", str);
    Execute(cmd);
	
	if (g_console_show_always)
        Device.seqRender.Add(this, 1);
}

// -------------------------------------------------------------------------------------------------

IConsole_Command* CConsole::find_next_cmd(LPCSTR in_str, shared_str& out_str)
{
	string_path t2;
    xr_strconcat(t2, in_str, " ");

	vecCMD_IT it = Commands.lower_bound(t2);
	if (it != Commands.end())
	{
		IConsole_Command* cc = it->second;
		LPCSTR name_cmd = cc->Name();

		out_str = name_cmd;
		return cc;
	}
	return nullptr;
}

bool CConsole::add_next_cmds(LPCSTR in_str, vecTipsEx& out_v)
{
	u32 cur_count = out_v.size();
	if (cur_count >= MAX_TIPS_COUNT)
	{
		return false;
	}

	string_path t2;
    xr_strconcat(t2, in_str, " ");

	shared_str temp;
	IConsole_Command* cc = find_next_cmd(t2, temp);
	if (!cc || temp.empty())
	{
		return false;
	}

	bool res = false;
	for (u32 i = cur_count; i < MAX_TIPS_COUNT * 2; ++i) //fake=protect
	{
        temp = cc->Name();
		bool dup = (std::find(out_v.begin(), out_v.end(), temp) != out_v.end());
		if (!dup)
		{
			TipString ts(temp);
			out_v.push_back(ts);
			res = true;
		}
		if (out_v.size() >= MAX_TIPS_COUNT)
		{
			break; // for
		}
        string_path t3;
        xr_strconcat(t3, out_v.back().text.c_str(), " ");
		cc = find_next_cmd(t3, temp);
		if (!cc)
		{
			break; // for
		}
	} // for
	return res;
}

bool CConsole::add_internal_cmds(LPCSTR in_str, vecTipsEx& out_v)
{
	u32 cur_count = out_v.size();
	if (cur_count >= MAX_TIPS_COUNT)
	{
		return false;
	}
	u32 in_sz = xr_strlen(in_str);

	bool res = false;
	// word in begin
	xr_string name2;
	vecCMD_IT itb = Commands.begin();
	vecCMD_IT ite = Commands.end();
	for (; itb != ite; ++itb)
	{
		LPCSTR name = itb->first;
		u32 name_sz = xr_strlen(name);
		if (name_sz >= in_sz)
		{
			name2.assign(name, in_sz);
			if (!stricmp(name2.c_str(), in_str))
			{
				shared_str temp;
                temp = name;
				bool dup = (std::find(out_v.begin(), out_v.end(), temp) != out_v.end());
				if (!dup)
				{
					out_v.emplace_back(temp, 0, in_sz);
					res = true;
				}
			}
		}

		if (out_v.size() >= MAX_TIPS_COUNT)
		{
			return res;
		}
	} // for

	// word in internal
	itb = Commands.begin();
	ite = Commands.end();
	for (; itb != ite; ++itb)
	{
		LPCSTR name = itb->first;
		LPCSTR fd_str = strstr(name, in_str);
		if (fd_str)
		{
			shared_str temp;
            temp = name;
			bool dup = (std::find(out_v.begin(), out_v.end(), temp) != out_v.end());
			if (!dup)
			{
				u32 name_sz = xr_strlen(name);
				int fd_sz = name_sz - xr_strlen(fd_str);
				out_v.emplace_back(temp, fd_sz, fd_sz + in_sz);
				res = true;
			}
		}
		if (out_v.size() >= MAX_TIPS_COUNT)
		{
			return res;
		}
	} // for

	return res;
}

void CConsole::reset_tips()
{
    m_temp_tips.clear();
    m_tips.clear();

    m_cur_cmd = NULL;
}

void CConsole::update_tips()
{
	if (!bVisible)
	{
        reset_tips();

		return;
	}

	LPCSTR cur = ec().str_edit();
	size_t cur_length = strlen(cur);

	if (cur_length == 0)
	{
        reset_tips();

		m_prev_length_str = 0;

		return;
	}

	if (cur_length == m_prev_str.size() && m_prev_str.equal(cur))
	{
        return;
	}

	m_prev_str = cur;

    reset_tips();

	if (m_prev_length_str != cur_length)
	{
		reset_selected_tip();
	}

	m_prev_length_str = cur_length;

    std::string s_cur{cur}, first, last;
    split_cmd(s_cur, first, last);

	size_t first_lenght = first.length();

	if ((first_lenght > 2) && (first_lenght + 1 <= cur_length)) // param
	{
		if (cur[first_lenght] == ' ')
		{
			if (m_tips_mode != 2)
			{
				reset_selected_tip();
			}

			vecCMD_IT it = Commands.find(first.c_str());
			if (it != Commands.end())
			{
				IConsole_Command* cc = it->second;

				u32 mode = 0;
				if ((first_lenght + 2 <= cur_length) && (cur[first_lenght] == ' ') && (cur[first_lenght + 1] == ' '))
				{
					mode = 1;
                    last.erase(0, 1); // fake: next char
				}

				cc->fill_tips(m_temp_tips, mode);
				m_tips_mode = 2;
                m_cur_cmd = first.c_str();
                select_for_filter(last.c_str(), m_temp_tips, m_tips);

				if (m_tips.empty())
				{
					m_tips.emplace_back("(empty)");
				}
				if ((int)m_tips.size() <= m_select_tip)
				{
					reset_selected_tip();
				}
				return;
			}
		}
	}

	// cmd name
	{
		add_internal_cmds(cur, m_tips);
		//add_next_cmds( cur, m_tips );
		m_tips_mode = 1;
	}

	if (m_tips.empty())
	{
		m_tips_mode = 0;
		reset_selected_tip();
	}
	if ((int)m_tips.size() <= m_select_tip)
	{
		reset_selected_tip();
	}
}

void CConsole::select_for_filter(LPCSTR filter_str, vecTips& in_v, vecTipsEx& out_v)
{
	out_v.clear();
	u32 in_count = in_v.size();
	if (in_count == 0 || !filter_str)
	{
		return;
	}

	bool all = (xr_strlen(filter_str) == 0);

	vecTips::iterator itb = in_v.begin();
	vecTips::iterator ite = in_v.end();
	for (; itb != ite; ++itb)
	{
		shared_str const& str = (*itb);
		if (all)
		{
			out_v.emplace_back(str);
		}
		else
		{
            if (LPCSTR fd_str = strstr(str.c_str(), filter_str))
			{
				u32 fd_sz = str.size() - xr_strlen(fd_str);
				TipString ts(str, fd_sz, fd_sz + xr_strlen(filter_str));
				out_v.push_back(ts);
			}
		}
	} //for
}
