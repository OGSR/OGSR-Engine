#include "StdAfx.h"
#include "UIOutfitInfo.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "../actor.h"
#include "../CustomOutfit.h"
#include "../string_table.h"

LPCSTR _imm_names []={
	"health_restore_speed",
	"radiation_restore_speed",
	"satiety_restore_speed",
	"thirst_restore_speed",
	"power_restore_speed",
	"bleeding_restore_speed",
	"psy_health_restore_speed",

	"burn_immunity",
	"shock_immunity",
	"strike_immunity",
	"wound_immunity",		
	"radiation_immunity",
	"telepatic_immunity",
	"chemical_burn_immunity",
	"explosion_immunity",
	"fire_wound_immunity",
};

LPCSTR _imm_st_names[]={
	"ui_inv_health",
	"ui_inv_radiation",
	"ui_inv_satiety",
	"ui_inv_thirst",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_psy_health",

	"ui_inv_outfit_burn_protection",
	"ui_inv_outfit_shock_protection",
	"ui_inv_outfit_strike_protection",
	"ui_inv_outfit_wound_protection",
	"ui_inv_outfit_radiation_protection",
	"ui_inv_outfit_telepatic_protection",
	"ui_inv_outfit_chemical_burn_protection",
	"ui_inv_outfit_explosion_protection",
	"ui_inv_outfit_fire_wound_protection",
};

LPCSTR _actor_param_names[] = {
	"satiety_health_v",
	"radiation_v",
	"satiety_v",
	"thirst_v",
	"satiety_power_v",
	"wound_incarnation_v",
	"psy_health_v"
};


CUIOutfitImmunity::CUIOutfitImmunity()
{
	m_name   = nullptr;
	m_value1 = nullptr;
	m_value2 = nullptr;
}

CUIOutfitImmunity::~CUIOutfitImmunity()
{
	xr_delete( m_name   );
	xr_delete( m_value1 );
	xr_delete( m_value2 );
}

void CUIOutfitImmunity::InitFromXml( CUIXml& xml_doc, LPCSTR base_str, u32 hit_type )
{
	string256 buf;
	
	m_name = xr_new<CUIStatic>();
	m_name->SetAutoDelete( false );
	AttachChild( m_name );
	strconcat( sizeof(buf), buf, base_str, ":static_", _imm_names[hit_type] );
	CUIXmlInit::InitWindow( xml_doc, buf, 0, this );
	CUIXmlInit::InitStatic( xml_doc, buf, 0, m_name );
	
	strconcat( sizeof(buf), buf, base_str, ":static_", _imm_names[hit_type], ":static_value1" );
	if ( xml_doc.NavigateToNode( buf, 0 ) ) {
	  m_value1 = xr_new<CUIStatic>();
	  m_value1->SetAutoDelete( false );
	  m_value1->SetTextComplexMode( true );
	  AttachChild( m_value1 );
	  CUIXmlInit::InitStatic( xml_doc, buf, 0, m_value1 );
	  m_value1->SetVisible( false );
	}

	strconcat( sizeof(buf), buf, base_str, ":static_", _imm_names[hit_type], ":static_value2" );
	if ( xml_doc.NavigateToNode( buf, 0 ) ) {
	  m_value2 = xr_new<CUIStatic>();
	  m_value2->SetAutoDelete( false );
	  m_value1->SetTextComplexMode( true );
	  AttachChild( m_value2 );
	  CUIXmlInit::InitStatic( xml_doc, buf, 0, m_value2 );
	  m_value2->SetVisible( false );
	}
}

CUIOutfitInfo::CUIOutfitInfo()
{
	m_listWnd = nullptr;
	Memory.mem_fill			(m_items, 0, sizeof(m_items));
}

CUIOutfitInfo::~CUIOutfitInfo()
{
	xr_delete( m_listWnd );
	for(u32 i=_item_start; i<_max_item_index; ++i)
	{
		xr_delete( m_items[i] );
	}
}

void CUIOutfitInfo::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base				= "outfit_info";

	string256					_buff;
	CUIXmlInit::InitWindow		(xml_doc, _base, 0, this);

        // для сохранения совместимости с ТЧ, будем scroll_view
        // использовать для определения позиции первого статика
	strconcat					(sizeof(_buff),_buff, _base, ":scroll_view");
	if ( xml_doc.NavigateToNode( _buff, 0 ) ) {
	  m_listWnd = xr_new<CUIStatic>();
	  CUIXmlInit::InitStatic( xml_doc, _buff, 0, m_listWnd );
	}

	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		strconcat(sizeof(_buff), _buff, _base, ":static_", _imm_names[i]);

		if (xml_doc.NavigateToNode(_buff, 0))
		{
			m_items[i] = xr_new<CUIOutfitImmunity>();
			m_items[i]->InitFromXml( xml_doc, _base, i );
		}
	}
}

float CUIOutfitInfo::GetArtefactParam(ActorRestoreParams params, u32 i)
{
	float r = 0;
	switch (i)
	{
	case _item_health_restore_speed:
		r = params.HealthRestoreSpeed; break;
	case _item_radiation_restore_speed:
		r = params.RadiationRestoreSpeed; break;
	case _item_satiety_restore_speed:
		r = params.SatietyRestoreSpeed; break;
	case _item_thirst_restore_speed:
		r = params.ThirstRestoreSpeed; break;
	case _item_power_restore_speed:
		r = params.PowerRestoreSpeed; break;
	case _item_bleeding_restore_speed:
		r = params.BleedingRestoreSpeed; break;
	case _item_psy_health_restore_speed:
		r = params.PsyHealthRestoreSpeed; break;
	}
	return r;
}

#include "script_game_object.h"

void CUIOutfitInfo::Update( CCustomOutfit* outfit, bool af )
{
	string128 _buff;

	auto artefactEffects = Actor()->ActiveArtefactsOnBelt( 1 );
	auto outfitEffects   = Actor()->ActiveArtefactsOnBelt( 2 );

	for ( auto it : m_items )
	  if ( it && it->GetParent() )
	    DetachChild( it );

	float _h = 0.0f;
	if ( m_listWnd )
	  _h = m_listWnd->GetWndPos().y;
	else if ( GetChildWndList().size() ) {
	  const auto it = GetChildWndList().back();
	  _h = it->GetWndPos().y + it->GetWndSize().y;
	}

	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		if ( !m_items[ i ] ) continue;

		float _val_outfit = 0.0f;
		float _val_af = 0.0f;

		if (i < _max_item_index1)
		{
			_val_outfit = GetArtefactParam( outfitEffects, i );

			float _actor_val = pSettings->r_float("actor_condition", _actor_param_names[i]);
			_val_outfit = (_val_outfit / _actor_val);

			if ( af ) {
			  _val_af = GetArtefactParam( artefactEffects, i );
			  _val_af = _val_af / _actor_val;
			}
		}
		else
		{
			_val_outfit = outfit ? outfit->GetDefHitTypeProtection(ALife::EHitType(i - _max_item_index1)) : 1.0f;
			_val_outfit = 1.0f - _val_outfit;

			if ( af ) {
			  _val_af = Actor()->HitArtefactsOnBelt(1.0f, ALife::EHitType(i - _max_item_index1));
			  _val_af = 1.0f - _val_af;
			}
		}

		if (fsimilar(_val_outfit, 0.0f) && fsimilar(_val_af, 0.0f))
		{
			continue;
		}

		LPCSTR _sn = "";
		if (i != _item_radiation_restore_speed
			&& i != _item_power_restore_speed)
		{
			_val_outfit *= 100.0f;
			_val_af *= 100.0f;
			_sn = "%";
		}

		if ( i == _item_bleeding_restore_speed ) {
			_val_outfit *= -1.0f;
			_val_af *= -1.0f;
		}

		LPCSTR _color    = ( _val_outfit > 0 ) ? "%c[green]" : "%c[red]";
		LPCSTR _af_color = ( _val_af     > 0 ) ? "%c[green]" : "%c[red]";

		if ( i == _item_bleeding_restore_speed || i == _item_radiation_restore_speed ) {
		  _color    = _val_outfit > 0 ? "%c[red]" : "%c[green]";
		  _af_color = _val_af     > 0 ? "%c[red]" : "%c[green]";
		}

		LPCSTR _imm_name = *CStringTable().translate(_imm_st_names[i]);

		if ( m_items[ i ]->m_value1 ) {
		  m_items[ i ]->m_name->SetText( _imm_name );
		  sprintf_s( _buff, sizeof( _buff ), "%s%+.0f%s", _color, _val_outfit, _sn );
		  m_items[ i ]->m_value1->SetText( _buff );
		  m_items[ i ]->m_value1->SetVisible( !fsimilar( _val_outfit, 0.0f )  );
		  if ( m_items[ i ]->m_value2 ) {
		    sprintf_s( _buff, sizeof( _buff ), "%s%+.0f%s", _af_color, _val_af, _sn );
		    m_items[ i ]->m_value2->SetText( _buff );
		    m_items[ i ]->m_value2->SetVisible( !fsimilar( _val_af, 0.0f ) );
		  }
		}
		else {
		  int _sz = sprintf_s( _buff, sizeof( _buff ), "%s ", _imm_name );
		  _sz += sprintf_s( _buff + _sz, sizeof( _buff ) - _sz, "%s %+3.0f%s", _color, _val_outfit, _sn );
		  if ( !fsimilar( _val_af, 0.0f ) )
		    _sz += sprintf_s( _buff + _sz, sizeof( _buff ) - _sz, "%s %+3.0f%s", _af_color, _val_af, _sn );
		  m_items[ i ]->m_name->SetText( _buff );
		}
		m_items[ i ]->SetWndPos( m_items[ i ]->GetWndPos().x, _h );
		_h += m_items[ i ]->GetWndSize().y;
		AttachChild( m_items[ i ] );
	}
	SetHeight( _h );

	if (pSettings->line_exist("engine_callbacks", "ui_actor_info_callback"))
	{
		const char* callback = pSettings->r_string("engine_callbacks", "ui_actor_info_callback");
		if (luabind::functor<void> lua_function; ai().script_engine().functor(callback, lua_function))
		{
                  lua_function( this, outfit ? outfit->lua_game_object() : nullptr, _h );
		}
	}
}
