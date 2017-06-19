#pragma once

enum	EGameActions
{
	kLEFT,
	kRIGHT,
	kUP,
	kDOWN,
	kJUMP,
	kCROUCH,
	kCROUCH_TOGGLE,
	kACCEL,
	kSPRINT_TOGGLE,
						
	kFWD,
	kBACK,
	kL_STRAFE,
	kR_STRAFE,
						
	kL_LOOKOUT,
	kR_LOOKOUT,
						
	kENGINE,
	kCAM_1,
	kCAM_2,
	kCAM_3,
	kCAM_4,
	kCAM_ZOOM_IN,
	kCAM_ZOOM_OUT,
						
	kTORCH,
	kNIGHT_VISION,
	kWPN_1,
	kWPN_2,
	kWPN_3,
	kWPN_4,
	kWPN_5,
	kWPN_6,
	kWPN_8,
	kARTEFACT,
	kWPN_NEXT,
	kWPN_FIRE,
	kWPN_ZOOM,
	kWPN_ZOOM_INC,
	kWPN_ZOOM_DEC,
	kWPN_RELOAD,
	kWPN_FUNC,
	kWPN_FIREMODE_PREV,
	kWPN_FIREMODE_NEXT,
						
	kPAUSE,
	kDROP,
	kUSE,
	kSCORES,
	kCHAT,
	kCHAT_TEAM,
	kSCREENSHOT,
	kQUIT,
	kCONSOLE,
	kINVENTORY,
	kBUY,
	kSKIN,
	kTEAM,
	kACTIVE_JOBS,
	kMAP,
	kCONTACTS,
	kEXT_1,
						
	kVOTE_BEGIN,
	kVOTE,
	kVOTEYES,
	kVOTENO,
						
	kNEXT_SLOT,
	kPREV_SLOT,
						
	kSPEECH_MENU_0,
	kSPEECH_MENU_1,
	kSPEECH_MENU_2,
	kSPEECH_MENU_3,
	kSPEECH_MENU_4,
	kSPEECH_MENU_5,
	kSPEECH_MENU_6,
	kSPEECH_MENU_7,		
	kSPEECH_MENU_8,		
	kSPEECH_MENU_9,
						
	kUSE_BANDAGE,
	kUSE_MEDKIT,	

	// KD - OGSE
	kUSE_B190,
	kUSE_BIPSIZON,
	kUSE_ANTIRAD,
	kUSE_ENERGY_DRINK,
	kUSE_RADIO,
	// KD - OGSE
	
	kQUICK_SAVE,
	kQUICK_LOAD,

	kLASTACTION,
	kNOTBINDED,
	kFORCEDWORD		= u32(-1)
};

struct _keyboard		
{
	LPCSTR		key_name;
	int			dik;
	xr_string	key_local_name;
};
enum _key_group{
	_both	=	(1<<0)			,
	_sp		=	_both | (1<<1)	,
	_mp		=	_both | (1<<2)	,
};

extern _key_group g_current_keygroup;

bool is_group_not_conflicted(_key_group g1, _key_group g2);

struct _action
{
	LPCSTR			action_name;
	EGameActions	id;
	_key_group		key_group;
};

LPCSTR			dik_to_keyname			(int _dik);
int				keyname_to_dik			(LPCSTR _name);
_keyboard*		keyname_to_ptr			(LPCSTR _name);
_keyboard*		dik_to_ptr				(int _dik, bool bSafe);

LPCSTR			id_to_action_name		(EGameActions _id);
EGameActions	action_name_to_id		(LPCSTR _name);
_action*		action_name_to_ptr		(LPCSTR _name);

extern _action		actions		[];
//extern _keyboard	keyboards	[];
//extern xr_vector< _keyboard >	keyboards;

#define bindings_count kLASTACTION
struct _binding
{
	_action*		m_action;
	_keyboard*		m_keyboard[2];
};

extern _binding g_key_bindings[];

bool				is_binded			(EGameActions action_id, int dik);
int					get_action_dik		(EGameActions action_id);
EGameActions		get_binded_action	(int dik);

extern void		CCC_RegisterInput();

struct _conCmd	{
	shared_str	cmd;
};

class ConsoleBindCmds{
public:
	xr_map<int,_conCmd>		m_bindConsoleCmds;

	void 	bind			(int dik, LPCSTR N);
	void 	unbind			(int dik);
	bool 	execute			(int dik);
	void 	clear			();
	void 	save			(IWriter* F);
};

void GetActionAllBinding	(LPCSTR action, char* dst_buff, int dst_buff_sz);

extern ConsoleBindCmds		bindConsoleCmds;

// 0xED - max vavue in DIK* enum
#define MOUSE_1		(0xED + 100)
#define MOUSE_2		(0xED + 101)
#define MOUSE_3		(0xED + 102)

#define MOUSE_4		(0xED + 103)
#define MOUSE_5		(0xED + 104)
#define MOUSE_6		(0xED + 105)
#define MOUSE_7		(0xED + 106)
#define MOUSE_8		(0xED + 107)
