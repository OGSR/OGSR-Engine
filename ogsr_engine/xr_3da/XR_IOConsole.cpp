// XR_IOConsole.cpp: implementation of the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_input.h"
#include "xr_ioc_cmd.h"
#include "GameFont.h"
#include "xr_trims.h"
#include "CustomHUD.h"
#pragma warning(push)
#pragma warning(disable:4995)
//#include <locale>
#pragma warning(pop)

#define  LDIST .05f

ENGINE_API CConsole*	Console		=	NULL;
const char *			ioc_prompt	=	">>> ";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CConsole::AddCommand(IConsole_Command* C)
{
	Commands[C->Name()] = C;
}
void CConsole::RemoveCommand(IConsole_Command* C)
{
	vecCMD_IT it = Commands.find(C->Name());
	if(Commands.end()!=it)
		Commands.erase(it);
}
void CConsole::Reset()
{
	if(pFont)
		xr_delete(pFont);
}

void CConsole::Initialize()
{
	scroll_delta	= cmd_delta = old_cmd_delta = 0;
	editor[0]       = 0;
	bShift			= false;
	RecordCommands	= false;
	editor[0]       = 0;
	cur_time		= rep_time = 0;
	fAccel			= 1.0f;
	bVisible		= false;
	rep_time		= 0;
	pFont			= 0;
	last_mm_timer	= 0;
	// Commands
	extern void CCC_Register();
	CCC_Register	();
}

void CConsole::Destroy	()
{
	Execute						("cfg_save");

	xr_delete					(pFont);

	Commands.clear				();
}

void CConsole::OnFrame	()
{
	u32 mm_timer = Device.dwTimeContinual;
	float fDelta = (mm_timer - last_mm_timer)/1000.0f;
	if (fDelta>.06666f) fDelta=.06666f;
	last_mm_timer = mm_timer;

	cur_time+=fDelta;
	rep_time+=fDelta*fAccel;
	if (cur_time>0.1f) { cur_time-=0.1f; bCursor=!bCursor;	}
	if (rep_time>0.15f) { rep_time-=0.15f; bRepeat=true;	fAccel+=0.4f;	}
/*
	cur_time+=Device.fTimeDelta;
	rep_time+=Device.fTimeDelta*fAccel;
	if (cur_time>0.1f) { cur_time-=0.1f; bCursor=!bCursor;	}
	if (rep_time>0.2f) { rep_time-=0.2f; bRepeat=true;	fAccel+=0.2f;	}
*/
}

void CConsole::out_font(const char* text, float& pos_y)
{
	float str_length = pFont->SizeOf_(text);
	float scr_width = 1.98f * Device.fWidth_2;
	if (str_length > scr_width) // 1024.0f
	{
		float f = 0.0f;
		int sz = 0;
		int ln = 0;
		char* one_line = (char*)_alloca((MAX_LEN + 1) * sizeof(char));

		while (text[sz] && (ln + sz < MAX_LEN - 5)) // перенос строк
		{
			one_line[ln + sz] = text[sz];
			one_line[ln + sz + 1] = 0;

			float t = pFont->SizeOf_(one_line + ln);
			if (t > scr_width)
			{
				out_font(text + sz + 1, pos_y);
				pos_y -= LDIST;
				pFont->OutI(-1.0f, pos_y, "%s", one_line + ln);
				ln = sz + 1;
				f = 0.0f;
			}
			else
			{
				f = t;
			}

			++sz;
		}
	}
	else
	{
		pFont->OutI(-1.0f, pos_y, "%s", text);
	}
}

void CConsole::OnRender	()
{
	float			fMaxY;
	BOOL			bGame;

	if (!bVisible) return;
	if (0==pFont)
		pFont		= xr_new<CGameFont>	("hud_font_di",CGameFont::fsDeviceIndependent);

	bGame	=false;	
	if ( (g_pGameLevel && g_pGameLevel->bReady)||
		 ( g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive() ) )	
		 bGame = true;

	if (!m_pRender)
		m_pRender = xr_new<FactoryPtr<IConsoleRender>>();

	(*m_pRender)->OnRender(bGame);

	// float dwMaxX=float(Device.dwWidth/2);
	if (bGame) { fMaxY=0.f; } else fMaxY=1.f;

	char		buf	[MAX_LEN+5];
	strcpy_s		(buf,ioc_prompt);
	strcat_s(buf,editor);
	if (bCursor) strcat_s(buf,"|");

	pFont->SetColor( color_rgba(128  ,128  ,255, 255) );
	pFont->SetHeightI(0.025f);
	pFont->OutI	( -1.f, fMaxY-LDIST, "%s", buf );

	float ypos=fMaxY-LDIST-LDIST;
	for (int i=LogFile.size()-1-scroll_delta; i>=0; i--) 
	{
		ypos-=LDIST;
		if (ypos<-1.f) break;

		auto& ls = LogFile[i];
		if (!ls.c_str())
			continue;
		switch (ls.front()) {
		case '~':
			pFont->SetColor(color_rgba(255,255,0, 255));
			out_font(&ls[2], ypos);
			break;
		case '!':
			pFont->SetColor(color_rgba(255,0  ,0  , 255));
			out_font(&ls[2], ypos);
			break;
		case '*':
			pFont->SetColor(color_rgba(128,128,128, 255));
			out_font(&ls[2], ypos);
			break;
		case '-':
			pFont->SetColor(color_rgba(0  ,255,0  , 255));
			out_font(&ls[2], ypos);
			break;
		case '#':
			pFont->SetColor(color_rgba(0  ,222, 205  ,145));
			out_font(&ls[2], ypos);
			break;
		case '>':
			pFont->SetColor(color_rgba(128, 128, 255, 255));
			out_font(&ls[2], ypos);
			break;
		default:
			pFont->SetColor(color_rgba(255,255,255, 255));
			out_font(ls.c_str(), ypos);
		}
	}
	pFont->OnRender();
}


void CConsole::OnPressKey(int dik, BOOL bHold)
{
	if (!bHold)	fAccel			= 1.0f;

	switch (dik) {
	case DIK_GRAVE:
		if (bShift) { strcat_s(editor,"~"); break; }
	case DIK_ESCAPE:
		if (!bHold) {
			if  ( g_pGameLevel || 
				( g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive() ))
				Hide();
		}
		break;
	case DIK_PRIOR:
		scroll_delta++;
		if (scroll_delta>int(LogFile.size())-1) scroll_delta=LogFile.size()-1;
		break;
	case DIK_NEXT:
		scroll_delta--;
		if (scroll_delta<0) scroll_delta=0;
		break;
	case DIK_TAB:
		{
			LPCSTR radmin_cmd_name = "ra ";
			bool b_ra = (editor==strstr(editor, radmin_cmd_name));
			int offset = (b_ra)?xr_strlen(radmin_cmd_name):0;
			vecCMD_IT I = Commands.lower_bound(editor+offset);
			if (I!=Commands.end()) {
				IConsole_Command &O = *(I->second);
				strcpy_s(editor+offset, sizeof(editor)-offset, O.Name());
				strcat(editor+offset," ");
			}
		}
		break;
	case DIK_UP:
		cmd_delta--;
		SelectCommand();
		break;
	case DIK_DOWN:
		cmd_delta++;
		SelectCommand();
		break;
	case DIK_BACK:
		if (xr_strlen(editor)>0) editor[xr_strlen(editor)-1]=0;
		break;
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		bShift = true;
		break;
	case DIK_RETURN:
	case DIK_NUMPADENTER:
		ExecuteCommand( true );
		break;
	case DIK_INSERT:
		if( OpenClipboard(0) )
		{
			HGLOBAL hmem = GetClipboardData(CF_TEXT);
			if( hmem ){
				LPCSTR	clipdata = (LPCSTR)GlobalLock(hmem);
				strncpy_s(editor,clipdata,MAX_LEN-1); editor[MAX_LEN-1]=0;
//				std::locale loc ("English");
				for (u32 i=0; i<xr_strlen(editor); i++)
					if (isprint(editor[i]))	{
//						editor[i]=char(tolower(editor[i],loc));
						editor[i]=char(tolower(editor[i]));
					}else{
						editor[i]=' ';
					}
				
				GlobalUnlock( hmem );
				CloseClipboard();
			}
		}
		break;
	default:
		char symbol = pInput->DikToChar(dik);
		if (symbol) {
			const char ptr[]{ symbol, 0 };
			strcat_s(editor, ptr);
		}
		break;
	}

	constexpr u32 clip = MAX_LEN - 8;
	if	(xr_strlen(editor)>=clip) editor[clip-1]=0;

	bRepeat		= false;
	rep_time	= 0;
}

void CConsole::IR_OnKeyboardPress(int dik)
{
	OnPressKey(dik);
}

void CConsole::IR_OnKeyboardRelease(int dik)
{
	fAccel		= 1.0f;
	rep_time	= 0;
	switch (dik) 
	{
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		bShift = false;
		break;
	}
}

void CConsole::IR_OnKeyboardHold(int dik)
{
	float fRep	= rep_time;
	if (bRepeat) { OnPressKey(dik, true); bRepeat=false; }
	rep_time	= fRep;
}

void CConsole::ExecuteCommand( bool is_user_input ) //Надо переписать нафиг!
{
	char	first_word[MAX_LEN];
	char	last_word [MAX_LEN];
	char	converted [MAX_LEN];
	int		i,j,len;

	scroll_delta	= 0;
	cmd_delta		= 0;
	old_cmd_delta	= 0;

	len = xr_strlen(editor);
	for (i=0; i<len; i++) {
		if ((editor[i]=='\n')||(editor[i]=='\t')) editor[i]=' ';
	}
	j   = 0;
	for (i=0; i<len; i++) {
		switch (editor[i]) {
		case ' ':
			if (editor[i+1]==' ') continue;
			if (i==len-1) goto outloop;
			break;
//.		case ';':
//.			goto outloop;
		}
		converted[j++]=editor[i];
	}
outloop:
	converted[j]=0;
	if (converted[0]==' ')	strcpy_s(editor,&(converted[1]));
	else					strcpy_s(editor,converted);
	if (editor[0]==0)		return;
	if (RecordCommands)		Log(">",editor);
	
	// split into cmd/params
	editor[j++  ]	=	' ';
	editor[len=j]	=	0;
	for (i=0; i<len; i++) {
		if (editor[i]!=' ') first_word[i]=editor[i];
		else {
			// last 'word' - exit
			strcpy_s(last_word,editor+i+1);
			break;
		}
	}
	first_word[i]=0;
	//if (last_word[xr_strlen(last_word)-1]==' ') last_word[xr_strlen(last_word)-1]=0; //Вэ той строке творится какая-то хуита!
	auto str_len = strlen(last_word);
	if (last_word[str_len - 1] == ' ')
		last_word[str_len - 1] = 0;

	// search
	vecCMD_IT I = Commands.find(first_word);
	if (I!=Commands.end()) {
//		Log("! Executing command: ", first_word);
		IConsole_Command &C = *(I->second);
		if ( C.bEnabled || !is_user_input ) {
			if (C.bLowerCaseArgs) strlwr(last_word);
			if (last_word[0]==0) {
				if (C.bEmptyArgsHandled) C.Execute(last_word);
				else {
					IConsole_Command::TStatus S; C.Status(S);
					Msg("- %s %s",C.Name(),S);
				}
			} else C.Execute(last_word);
		} else {
			Log("! Command disabled.");
		}
	}
	else 
		Log("! Unknown command: ",first_word);
	editor[0]=0;
}

void CConsole::Show			()
{
	if (bVisible)			return;
	bVisible				= true;

	editor[0]				= 0;
	rep_time				= 0;
	fAccel					= 1.0f;
	cmd_delta				= 0;
	old_cmd_delta			= 0;

	IR_Capture				( );
	Device.seqRender.Add	(this, 1);
	Device.seqFrame.Add		(this);
}

void CConsole::Hide()
{
	if (!bVisible)
		return;

	bVisible				= false;
	Device.seqFrame.Remove	(this);
	Device.seqRender.Remove	(this);
	IR_Release				( );
}

void CConsole::SelectCommand()
{
	int		p,k;
	BOOL	found=false;
	for (p=LogFile.size()-1, k=0; p>=0; p--) {
		auto& str = LogFile[p];
		if (!str.c_str())
			continue;

		if (str.front() == '>') {
			k--;
			if (k == cmd_delta) {
				strcpy_s(editor, &str[2]);
				found = true;
			}
		}
	}
	if (!found) {
		if (cmd_delta == 0)
		{
			editor[0] = 0;
			old_cmd_delta = 0;
		}
		else
			cmd_delta=old_cmd_delta;

	} else {
		old_cmd_delta=cmd_delta;
	}
}

void CConsole::Execute		(LPCSTR cmd)
{
	strncpy_s(editor,cmd,MAX_LEN-1); editor[MAX_LEN-1]=0;
	RecordCommands	= false;
	ExecuteCommand	();
	RecordCommands	= true;
}
void CConsole::ExecuteScript(LPCSTR N)
{
	string128		cmd;
	strconcat		(sizeof(cmd),cmd,"cfg_load ",N);
	Execute			(cmd);
}


BOOL CConsole::GetBool(LPCSTR cmd, BOOL& val)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I!=Commands.end()) {
		IConsole_Command* C = I->second;
		CCC_Mask* cf = dynamic_cast<CCC_Mask*>(C);
		if(cf){
			val = cf->GetValue();
		}else{
			CCC_Integer* cf = dynamic_cast<CCC_Integer*>(C);
			val = !!cf->GetValue();
		}
	}
	return val;
}

float CConsole::GetFloat(LPCSTR cmd, float& val, float& min, float& max)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I!=Commands.end()) {
		IConsole_Command* C = I->second;
		CCC_Float* cf = dynamic_cast<CCC_Float*>(C);
		val = cf->GetValue();

		//min = cf->GetMin();
		//max = cf->GetMax();
		cf->GetBounds(min, max);
		return val;
	}
	return val;
}

int CConsole::GetInteger(LPCSTR cmd, int& val, int& min, int& max)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I!=Commands.end()) {
		IConsole_Command* C = I->second;
		CCC_Integer* cf = dynamic_cast<CCC_Integer*>(C);
		if(cf)
		{
			val = cf->GetValue();

			//min = cf->GetMin();
			//max = cf->GetMax();
			cf->GetBounds(min, max);
		}else{
			CCC_Mask* cm	= dynamic_cast<CCC_Mask*>(C);
			R_ASSERT		(cm);
			val = (0!=cm->GetValue())?1:0;
			min = 0;
			max = 1;
		}
		return val;
	}
	return val;
}


char * CConsole::GetString(LPCSTR cmd)
{
	static IConsole_Command::TStatus stat;
	vecCMD_IT I = Commands.find(cmd);
	if (I!=Commands.end()) {
		IConsole_Command* C = I->second;
		C->Status(stat);
		return stat;
	}

/*
	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdVALUE) {
		u32 *v = (u32 *) cmd->ptr; // pointer to value
		xr_token *tok=cmd->tok;
		while (tok->name) {
			if (tok->id==(int)(*v)) {
				return (char *)tok->name;
			}
			tok++;
		}
	}
*/
	return NULL;
}
char * CConsole::GetToken(LPCSTR cmd)
{
	return GetString(cmd);
}

const xr_token* CConsole::GetXRToken(LPCSTR cmd)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I!=Commands.end()) {
		IConsole_Command* C = I->second;
		CCC_Token* cf = dynamic_cast<CCC_Token*>(C);
		return cf->GetToken();
	}
	return NULL;
}

IConsole_Command* CConsole::GetCommand( LPCSTR cmd )
{
	vecCMD_IT it = Commands.find( cmd );
	if ( it == Commands.end() )
		return NULL;
	else
		return it->second;
}

Fvector* CConsole::GetFVectorPtr( LPCSTR cmd )
{
	IConsole_Command* cc	= GetCommand(cmd);
	CCC_Vector3* cf			= dynamic_cast<CCC_Vector3*>(cc);
	if ( cf )
	{
		return cf->GetValuePtr();
	}
	return					NULL;
}

Fvector CConsole::GetFVector( LPCSTR cmd )
{
	Fvector* pV = GetFVectorPtr( cmd );
	if ( pV )
	{
		return *pV;
	}
	return Fvector().set( 0.0f, 0.0f, 0.0f );
}
/*
char * CConsole::GetNextValue(LPCSTR cmd)
{

	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdVALUE) {
		u32 *v = (u32 *) cmd->ptr; // pointer to value
		xr_token *tok=cmd->tok;
		while (tok->name) {
			if (tok->id==(int)(*v)) {
				char *save = (char *)tok->name;
				tok++;
				if (tok->name!=0) return (char *)tok->name;
				else				 return save;
			}
			tok++;
		}
	}

	return GetValue(cmd);
}

char * CConsole::GetPrevValue(LPCSTR cmd)
{

	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdVALUE) {
		u32 *v = (u32 *) cmd->ptr; // pointer to value
		xr_token *tok=cmd->tok;
		while (tok->name) {
			if (tok->id==(int)(*v)) {
				if (tok!=cmd->tok) tok--;
				return (char *)tok->name;
			}
			tok++;
		}
	}

	return GetValue(cmd);
}

*/