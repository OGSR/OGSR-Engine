#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"


#include "../PhraseDialogDefs.h"

class CActor;
class CInventoryOwner;
class CPhraseDialogManager;
class CUITalkDialogWnd;
class CUITradeWnd;
///////////////////////////////////////
//
///////////////////////////////////////

class CUITalkWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
	ref_sound			m_sound;
	void				PlaySnd					(LPCSTR text);
	void				StopSnd					();
public:
						CUITalkWnd();
	virtual				~CUITalkWnd();

	virtual void		Init();

	virtual bool		StopAnyMove					(){return true;}
	virtual void		SendMessage					(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void		Draw();
	virtual void		Update();
		
	virtual void		Show();
	virtual void		Hide();
	
	void				Stop();					//deffered

	void				UpdateQuestions();
	void				NeedUpdateQuestions();
	//инициализации начального диалога собеседника
	void				InitOthersStartDialog();
	virtual bool		IR_OnKeyboardPress(int dik);
	virtual bool		OnKeyboard(int dik, EUIMessages keyboard_action);
	void				SwitchToTrade();
	void				AddIconedMessage(LPCSTR text, LPCSTR texture_name, Frect texture_rect, LPCSTR templ_name);

protected:
	//диалог
	void				InitTalkDialog			();
	void				AskQuestion				();

	void				SayPhrase				(const shared_str& phrase_id);

	// Функции добавления строк в листы вопросов и ответов
public:
	void				AddQuestion				(const shared_str& text, const shared_str& id);
	void				AddAnswer				(const shared_str& text, LPCSTR SpeakerName);

	IC CUITradeWnd*			GetTradeWnd				() { return UITradeWnd;	}
	IC CInventoryOwner*		GetSecondTalker			() { return m_pOthersInvOwner; }
protected:
	//для режима торговли
	CUITradeWnd*			UITradeWnd;
	CUITalkDialogWnd*		UITalkDialogWnd;


	//указатель на владельца инвентаря вызвавшего менюшку
	//и его собеседника
	CActor*				m_pActor;
	CInventoryOwner*	m_pOurInvOwner;
	CInventoryOwner*	m_pOthersInvOwner;
	
	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	//спец. переменная, нужна для того чтобы RemoveAll
	//могла быть корректно вызвана из SendMessage
	//так как иначе возникает ситуация, что класс, который
	//вызвал нам SendMessage обращается к удаленному объекту pListItem
	bool				m_bNeedToUpdateQuestions;

	//текущий диалог, если NULL, то переходим в режим выбора темы
	DIALOG_SHARED_PTR	m_pCurrentDialog;
	bool				TopicMode				();
	void				ToTopicMode				();
};