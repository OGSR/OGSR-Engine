#include "stdafx.h"
#include "UICellCustomItems.h"
#include "UIInventoryUtilities.h"
#include "../Weapon.h"
#include "UIDragDropListEx.h"

#include "../game_object_space.h"
#include "../script_callback_ex.h"
#include "../script_game_object.h"
#include "../Actor.h"
#include "UIInventoryWnd.h"
#include "UICursor.h"

#pragma optimize("", off) //KRodin: добавлено специально, не убирать!

#define INV_GRID_WIDTHF			50.0f
#define INV_GRID_HEIGHTF		50.0f

CUIInventoryCellItem::CUIInventoryCellItem(CInventoryItem* itm)
{
	m_pData											= (void*)itm;
	itm->m_cell_item								= this;

	itm->m_icon_params.set_shader( this );

	m_grid_size.set									(itm->GetGridWidth(),itm->GetGridHeight());
	b_auto_drag_childs = true;
}

bool CUIInventoryCellItem::EqualTo(CUICellItem* itm)
{
	CUIInventoryCellItem* ci = smart_cast<CUIInventoryCellItem*>(itm);
	if(!itm)				return false;

	// Real Wolf: Колбек на группировку и само регулирование группировкой предметов. 12.08.2014.
	auto item1 = (CInventoryItem*)m_pData;
	auto item2 = (CInventoryItem*)itm->m_pData;

	if ( item1->m_always_ungroupable || item2->m_always_ungroupable )
		return false;
	if (item1->m_flags.test(CInventoryItem::FIUngroupable) || item2->m_flags.test(CInventoryItem::FIUngroupable) )
		return false;

	g_actor->callback(GameObject::eUIGroupItems)(item1->object().lua_game_object(), item2->object().lua_game_object() );

	auto fl1 = item1->m_flags;
	auto fl2 = item2->m_flags;

	item1->m_flags.set(CInventoryItem::FIUngroupable, false);
	item2->m_flags.set(CInventoryItem::FIUngroupable, false);

	if (fl1.test(CInventoryItem::FIUngroupable) || fl2.test(CInventoryItem::FIUngroupable) )
		return false;

	return					(
								fsimilar(object()->GetCondition(), ci->object()->GetCondition(), 0.01f) &&
								(object()->object().cNameSect() == ci->object()->object().cNameSect())
							);
}

CUIInventoryCellItem::~CUIInventoryCellItem()
{
	if (auto item = object() )
		item->m_cell_item = NULL;
}

void CUIInventoryCellItem::OnFocusReceive()
{
	m_selected = true;

	if (auto InvWnd = smart_cast<CUIInventoryWnd*>(this->OwnerList()->GetTop()))
	{
		InvWnd->HideSlotsHighlight();
		InvWnd->ShowSlotsHighlight(object());
	}

	inherited::OnFocusReceive();

	if (object()->object().m_spawned)
	{
		auto script_obj = object()->object().lua_game_object();
		g_actor->callback(GameObject::eCellItemFocus)(script_obj);
	}
}

void CUIInventoryCellItem::OnFocusLost()
{
	m_selected = false;

	if (auto InvWnd = smart_cast<CUIInventoryWnd*>(this->OwnerList()->GetTop()))
	{
		auto CellPos = this->m_pParentList->m_container->PickCell(GetUICursor()->GetCursorPosition());
		if (!this->m_pParentList->m_container->ValidCell(CellPos) || this->m_pParentList->m_container->GetCellAt(CellPos).Empty())
			InvWnd->HideSlotsHighlight();
	}

	inherited::OnFocusLost();

	if (object()->object().m_spawned)
	{
		auto script_obj = object()->object().lua_game_object();
		g_actor->callback(GameObject::eCellItemFocusLost)(script_obj);
	}
}

bool CUIInventoryCellItem::OnMouse(float x, float y, EUIMessages action)
{
	bool r = inherited::OnMouse(x, y, action);

	g_actor->callback(GameObject::eOnCellItemMouse)(object()->object().lua_game_object(), x, y, action);

	return r;
}

CUIDragItem* CUIInventoryCellItem::CreateDragItem()
{
	CUIDragItem* i = inherited::CreateDragItem();
	CUIStatic* s = NULL;
	if (!b_auto_drag_childs) return i;   

	for (auto it = this->m_ChildWndList.begin(); it != this->m_ChildWndList.end(); ++it)
	{
		if (auto s_child = smart_cast<CUIStatic*>(*it))
		{
			if (Core.Features.test(xrCore::Feature::show_inv_item_condition))
				if ( s_child == m_text ) continue;

			s = xr_new<CUIStatic>();
			s->SetAutoDelete(true);
			if ( s_child->GetShader() )
				s->SetShader(s_child->GetShader());

			s->SetWndRect(s_child->GetWndRect());
			s->SetOriginalRect(s_child->GetOriginalRect());

			s->SetStretchTexture(s_child->GetStretchTexture());
			s->SetText(s_child->GetText());

			if (auto text = s_child->GetTextureName())
				s->InitTextureEx(text, s_child->GetShaderName());

			s->SetColor(i->wnd()->GetColor());
			s->SetHeading(i->wnd()->Heading());
			i->wnd()->AttachChild(s);
		}
	}

	return	i;
}


void CUIInventoryCellItem::Update() {
  inherited::Update();
  if (Core.Features.test(xrCore::Feature::show_inv_item_condition))
	  UpdateConditionProgressBar();
}


CUIAmmoCellItem::CUIAmmoCellItem(CWeaponAmmo* itm)
:inherited(itm)
{}

bool CUIAmmoCellItem::EqualTo(CUICellItem* itm)
{
	if(!inherited::EqualTo(itm))	return false;

	CUIAmmoCellItem* ci				= smart_cast<CUIAmmoCellItem*>(itm);
	if(!ci)							return false;

	return					( (object()->cNameSect() == ci->object()->cNameSect()) );
}

void CUIAmmoCellItem::Update()
{
	inherited::Update	();
	UpdateItemText		();
}

void CUIAmmoCellItem::UpdateItemText()
{
	if(NULL==m_custom_draw)
	{
		xr_vector<CUICellItem*>::iterator it = m_childs.begin();
		xr_vector<CUICellItem*>::iterator it_e = m_childs.end();
		
		u16 total				= object()->m_boxCurr;
		for(;it!=it_e;++it)
			total				= total + ((CUIAmmoCellItem*)(*it))->object()->m_boxCurr;

		string32				str;
		sprintf_s					(str,"%d",total);

		if (Core.Features.test(xrCore::Feature::show_inv_item_condition)) {
			m_text->SetText(str);
			m_text->Show(true);
		} else
			SetText(str);
	}else
	{
		if (Core.Features.test(xrCore::Feature::show_inv_item_condition)) {
			m_text->SetText("");
			m_text->Show(false);
		} else
			SetText("");
	}
}


CUIWeaponCellItem::CUIWeaponCellItem(CWeapon* itm)
:inherited(itm)
{
	b_auto_drag_childs = false;
	m_addons[eSilencer]		= NULL;
	m_addons[eScope]		= NULL;
	m_addons[eLauncher]		= NULL;

	m_cell_size.set(INV_GRID_WIDTHF, INV_GRID_HEIGHTF);

	if(itm->SilencerAttachable())
		m_addon_offset[eSilencer].set(object()->GetSilencerX(), object()->GetSilencerY());

	if(itm->ScopeAttachable())
		m_addon_offset[eScope].set(object()->GetScopeX(), object()->GetScopeY());

	if(itm->GrenadeLauncherAttachable())
		m_addon_offset[eLauncher].set(object()->GetGrenadeLauncherX(), object()->GetGrenadeLauncherY());
}

#include "../object_broker.h"
CUIWeaponCellItem::~CUIWeaponCellItem()
{
}

bool CUIWeaponCellItem::is_scope()
{
	return object()->ScopeAttachable()&&object()->IsScopeAttached();
}

bool CUIWeaponCellItem::is_silencer()
{
	return object()->SilencerAttachable()&&object()->IsSilencerAttached();
}

bool CUIWeaponCellItem::is_launcher()
{
	return object()->GrenadeLauncherAttachable()&&object()->IsGrenadeLauncherAttached();
}

void CUIWeaponCellItem::CreateIcon(eAddonType t, CIconParams &params)
{
	if (GetIcon(t)) return;
	m_addons[t]					= xr_new<CUIStatic>();	
	m_addons[t]->SetAutoDelete	(true);
	AttachChild					(m_addons[t]);
	params.set_shader( m_addons[ t ] );
}

void CUIWeaponCellItem::DestroyIcon(eAddonType t)
{
	DetachChild		(m_addons[t]);
	m_addons[t]		= NULL;
}

CUIStatic* CUIWeaponCellItem::GetIcon(eAddonType t)
{
	return m_addons[t];
}

void CUIWeaponCellItem::Update()
{
	bool b = Heading();
	inherited::Update			();
	bool bForceReInitAddons = (b != Heading());

	if (object()->SilencerAttachable()){
		if (object()->IsSilencerAttached())
		{
			if (!GetIcon(eSilencer) || bForceReInitAddons)
			{
				CIconParams params(object()->GetSilencerName());
				CreateIcon	(eSilencer, params);
				InitAddon	(GetIcon(eSilencer), params, m_addon_offset[eSilencer], Heading());
			}
		}
		else
		{
			if (m_addons[eSilencer])
				DestroyIcon(eSilencer);
		}
	}

	if (object()->ScopeAttachable()){
		if (object()->IsScopeAttached())
		{
			if (!GetIcon(eScope) || bForceReInitAddons)
			{
				CIconParams params(object()->GetScopeName());
				CreateIcon	(eScope, params);
				InitAddon	(GetIcon(eScope), params, m_addon_offset[eScope], Heading());
			}
		}
		else
		{
			if (m_addons[eScope])
				DestroyIcon(eScope);
		}
	}

	if (object()->GrenadeLauncherAttachable()){
		if (object()->IsGrenadeLauncherAttached())
		{
			if (!GetIcon(eLauncher) || bForceReInitAddons)
			{
				CIconParams params(object()->GetGrenadeLauncherName());
				CreateIcon	(eLauncher, params);
				InitAddon	(GetIcon(eLauncher), params, m_addon_offset[eLauncher], Heading());
			}
		}
		else
		{
			if (m_addons[eLauncher])
				DestroyIcon(eLauncher);
		}
	}
}

void CUIWeaponCellItem::OnAfterChild(CUIDragDropListEx* parent_list)
{
	const Ivector2 &cs = parent_list->CellSize();
	m_cell_size.set((float)cs.x, (float)cs.y);
	CUIStatic* s_silencer = is_silencer() ? GetIcon(eSilencer) : NULL;
	CUIStatic* s_scope = is_scope() ? GetIcon(eScope) : NULL;
	CUIStatic* s_launcher = is_launcher() ? GetIcon(eLauncher) : NULL;
	
	InitAllAddons(s_silencer, s_scope, s_launcher, parent_list->GetVerticalPlacement());
}

void CUIWeaponCellItem::InitAllAddons(CUIStatic* s_silencer, CUIStatic* s_scope, CUIStatic* s_launcher, bool b_vertical)
{
	CIconParams params;

	if (s_silencer) {
		params.Load(*object()->GetSilencerName());
		InitAddon(s_silencer, params, m_addon_offset[eSilencer], b_vertical);
	}
	if (s_scope) {
		params.Load(*object()->GetScopeName());
		InitAddon(s_scope, params, m_addon_offset[eScope], b_vertical);
	}
	if (s_launcher) {
		params.Load(*object()->GetGrenadeLauncherName());
		InitAddon(s_launcher, params, m_addon_offset[eLauncher], b_vertical);
	}
}

void CUIWeaponCellItem::InitAddon(CUIStatic* s, CIconParams &params, Fvector2 addon_offset, bool b_rotate)
{
	
		Frect					tex_rect;
		Fvector2				base_scale;
		Fvector2				inventory_size;
		Fvector2				expected_size;
		static int method = 1;
		
		inventory_size.x = INV_GRID_WIDTHF  * m_grid_size.x;
		inventory_size.y = INV_GRID_HEIGHTF * m_grid_size.y;
		
		expected_size.x = m_cell_size.x * m_grid_size.x; // vert: cell_size = 40x50, grid_size = 5x2 
		expected_size.y = m_cell_size.y * m_grid_size.y;
		if (Heading())
		{   // h = 250, w = 80, i.x = 300, i.y = 100	
			if (1 == method)
			{   // пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ height & width.
				expected_size.x = m_cell_size.y * m_grid_size.x;
				expected_size.y = m_cell_size.x * m_grid_size.y;
			}
			if (2 == method)
			{   // пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ height & width + пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
				expected_size.x = m_cell_size.y * m_grid_size.y;
				expected_size.y = m_cell_size.x * m_grid_size.x;
			}
			
			base_scale.x = GetHeight() / inventory_size.x; 
			base_scale.y = GetWidth() / inventory_size.y;  
		}
		else
		{
			base_scale.x = GetWidth() / inventory_size.x;
			base_scale.y = GetHeight() / inventory_size.y;
		}
		
		if (method > 0)
		 {   
			base_scale.x = expected_size.x / inventory_size.x;
			base_scale.y = expected_size.y / inventory_size.y;
		}

		Fvector2				cell_size;
		Frect rect = params.original_rect();

		cell_size.x				= rect.width(); // это допущение, что ячейки инвентаря 50х50 всегда? )
		cell_size.y				= rect.height();

		tex_rect.x1				= rect.x1;
		tex_rect.y1				= rect.y1;

		tex_rect.rb.add			(tex_rect.lt,cell_size);

		cell_size.mul			(base_scale);
		if (b_rotate)
		{
			s->SetWndSize(Fvector2().set(cell_size.y, cell_size.x));
			Fvector2 new_offset;
			new_offset.x = addon_offset.y*base_scale.x;
			new_offset.y = GetHeight() - addon_offset.x*base_scale.x - cell_size.x;
			addon_offset = new_offset;
			addon_offset.x *= UI()->get_current_kx();
		}
		else
		{
			s->SetWndSize(cell_size);
			addon_offset.mul(base_scale);
		}
		s->SetWndPos			(addon_offset);
		s->SetOriginalRect		(tex_rect);
		s->SetStretchTexture	(true);

		s->EnableHeading(b_rotate);

		if (b_rotate)
		{
			s->SetHeading(GetHeading());
			Fvector2 offs;
			offs.set(0.0f, s->GetWndSize().y);
			s->SetHeadingPivot(Fvector2().set(0.0f, 0.0f), /*Fvector2().set(0.0f,0.0f)*/offs, true);
		}

		s->SetWindowName("wpn_addon");
}

CUIStatic *MakeAddonStatic(CUIDragItem* i, ref_shader &shader)
{
	CUIStatic* s = xr_new<CUIStatic>();
	s->SetShader(shader);
	s->SetAutoDelete(true);
	s->SetColor(i->wnd()->GetColor());
	i->wnd()->AttachChild(s);
	return s;
}

CUIDragItem* CUIWeaponCellItem::CreateDragItem()
{
	CUIDragItem* i = inherited::CreateDragItem();
	CUIStatic* s_silencer	= GetIcon(eSilencer) ? MakeAddonStatic(i, GetShader()) : NULL;
	CUIStatic* s_scope	= GetIcon(eScope)    ? MakeAddonStatic(i, GetShader()) : NULL;
	CUIStatic* s_launcher	= GetIcon(eLauncher) ? MakeAddonStatic(i, GetShader()) : NULL;		
	if (Heading()) m_cell_size.set(m_cell_size.y, m_cell_size.x);   // swap before	
	InitAllAddons(s_silencer, s_scope, s_launcher, false);
	if (Heading()) m_cell_size.set(m_cell_size.y, m_cell_size.x);  // swap after
	
	return				i;
}

bool CUIWeaponCellItem::EqualTo(CUICellItem* itm)
{
	if(!inherited::EqualTo(itm))	return false;

	CUIWeaponCellItem* ci			= smart_cast<CUIWeaponCellItem*>(itm);
	if(!ci)							return false;

	bool b_addons					= ( (object()->GetAddonsState() == ci->object()->GetAddonsState()) );
	bool b_place					= ( (object()->m_eItemPlace == ci->object()->m_eItemPlace) );
	
	return							b_addons && b_place;
}

CBuyItemCustomDrawCell::CBuyItemCustomDrawCell	(LPCSTR str, CGameFont* pFont)
{
	m_pFont		= pFont;
	VERIFY		(xr_strlen(str)<16);
	strcpy		(m_string,str);
}

void CBuyItemCustomDrawCell::OnDraw(CUICellItem* cell)
{
	Fvector2							pos;
	cell->GetAbsolutePos				(pos);
	UI()->ClientToScreenScaled			(pos, pos.x, pos.y);
	m_pFont->Out						(pos.x, pos.y, m_string);
	m_pFont->OnRender					();
}
