// UIMainIngameWnd.h:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIProgressBar.h"
#include "UIGameLog.h"

#include "../alife_space.h"

#include "UICarPanel.h"
#include "UIMotionIcon.h"
#include "../hudsound.h"
#include "../script_export_space.h"

//для режима настройки HUD
//extern int				g_bHudAdjustMode;
//extern float			g_fHudAdjustValue;

struct					GAME_NEWS_DATA;

class					CUIPdaMsgListItem;
class					CLAItem;
class					CUIZoneMap;
class					CUIArtefactPanel;
class					CUIScrollView;
class					CActor;
class					CWeapon;
class					CMissile;
class					CInventoryItem;

class CUIMainIngameWnd: public CUIWindow  
{
public:
	CUIMainIngameWnd();
	virtual ~CUIMainIngameWnd();

	virtual void Init();
	virtual void Draw();
	virtual void Update();

	bool OnKeyboardPress(int dik);

protected:
	
	CUIStatic			UIStaticDiskIO;
	CUIStatic			UIStaticHealth;
	CUIStatic			UIStaticArmor;
	CUIStatic			UIStaticQuickHelp;
	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;
	CUICarPanel			UICarPanel;
	CUIMotionIcon		UIMotionIcon;	
	CUIZoneMap*			UIZoneMap;

	//иконка, показывающая количество активных PDA
	CUIStatic			UIPdaOnline;
	
	//изображение оружия
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponIcon;
	Frect				UIWeaponIcon_rect;
public:
	CUIStatic*			GetPDAOnline					() { return &UIPdaOnline; };
	CUIZoneMap*			GetUIZoneMap					() { return UIZoneMap; }
protected:


	// 5 статиков для отображения иконок:
	// - сломанного оружия
	// - радиации
	// - ранения
	// - голода
	// - усталости
	CUIStatic			UIWeaponJammedIcon;
	CUIStatic			UIRadiaitionIcon;
	CUIStatic			UIWoundIcon;
	CUIStatic			UIStarvationIcon;
	CUIStatic			UIPsyHealthIcon;
	CUIStatic			UIInvincibleIcon;
	CUIStatic			UIThirstIcon;
//	CUIStatic			UISleepIcon;
//	CUIStatic			UIArtefactIcon;

	CUIScrollView* m_UIIcons{};
public:	
	CUIArtefactPanel*    m_artefactPanel;
	
public:
	
	// Енумы соответсвующие предупреждающим иконкам 
	enum EWarningIcons
	{
		ewiAll				= 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiPsyHealth,
		ewiInvincible,
		ewiThirst,
//		ewiSleep,
//		ewiArtefact,
	};

	// Задаем цвет соответствующей иконке
	void				SetWarningIconColor				(EWarningIcons icon, const u32 cl);
	void				TurnOffWarningIcon				(EWarningIcons icon);

	// Пороги изменения цвета индикаторов, загружаемые из system.ltx
	typedef				xr_map<EWarningIcons, xr_vector<float> >	Thresholds;
	typedef				Thresholds::iterator						Thresholds_it;
	Thresholds			m_Thresholds;

	// Енум перечисления возможных мигающих иконок
	enum EFlashingIcons
	{
		efiPdaTask	= 0,
		efiMail
	};
	
	void				SetFlashIconState_				(EFlashingIcons type, bool enable);

	void				AnimateContacts					(bool b_snd);
	HUD_SOUND			m_contactSnd;

	void				ReceiveNews						(GAME_NEWS_DATA* news);
	
protected:
	void				SetWarningIconColor				(CUIStatic* s, const u32 cl);
	void				InitFlashingIcons				(CUIXml* node);
	void				DestroyFlashingIcons			();
	void				UpdateFlashingIcons				();
	void				UpdateActiveItemInfo			();

	void				SetAmmoIcon						(const shared_str& seсt_name);

	// first - иконка, second - анимация
	DEF_MAP				(FlashingIcons, EFlashingIcons, CUIStatic*);
	FlashingIcons		m_FlashingIcons;

	//для текущего активного актера и оружия
	CActor*				m_pActor;	
	CWeapon*			m_pWeapon;
	CMissile*			m_pGrenade;
	CInventoryItem*		m_pItem;

	// Отображение подсказок при наведении прицела на объект
	void				RenderQuickInfos();

public:
	CUICarPanel&		CarPanel							(){return UICarPanel;};
	CUIMotionIcon&		MotionIcon							(){return UIMotionIcon;}
	void				OnConnected							();
	void				reset_ui							();
protected:
	CInventoryItem*		m_pPickUpItem;
	CUIStatic			UIPickUpItemIcon;

	float				m_iPickUpItemIconX{};
	float				m_iPickUpItemIconY{};
	float				m_iPickUpItemIconWidth{};
	float				m_iPickUpItemIconHeight{};

	void				UpdatePickUpItem();
public:
	void				SetPickUpItem	(CInventoryItem* PickUpItem);

	DECLARE_SCRIPT_REGISTER_FUNCTION
#ifdef DEBUG
	void				draw_adjust_mode					();
#endif
};

add_to_type_list(CUIMainIngameWnd)
#undef script_type_list
#define script_type_list save_type_list(CUIMainIngameWnd)
