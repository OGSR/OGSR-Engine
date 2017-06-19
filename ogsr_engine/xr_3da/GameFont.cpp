#include "stdafx.h"
#pragma hdrstop

#include "GameFont.h"
#ifndef _EDITOR
    #include "Render.h"
#endif
#ifdef _EDITOR
unsigned short int mbhMulti2Wide
	( wide_char *WideStr , wide_char *WidePos , const unsigned short int WideStrSize , const char *MultiStr  ){return 0;};
#endif

extern ENGINE_API BOOL g_bRendering; 
ENGINE_API Fvector2		g_current_font_scale={1.0f,1.0f};

CGameFont::CGameFont(LPCSTR section, u32 flags)
{
	fCurrentHeight				= 0.0f;
	fXStep						= 0.0f;
	fYStep						= 0.0f;
	uFlags						= flags;
	nNumChars					= 0x100;
	TCMap						= NULL;
	Initialize	(pSettings->r_string(section,"shader"),pSettings->r_string(section,"texture"));
	if (pSettings->line_exist(section,"size")){
		float sz = pSettings->r_float(section,"size");
		if (uFlags&fsDeviceIndependent)	SetHeightI(sz);
		else							SetHeight(sz);
	}
	if (pSettings->line_exist(section,"interval"))
		SetInterval(pSettings->r_fvector2(section,"interval"));
}

CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, u32 flags)
{
	fCurrentHeight				= 0.0f;
	fXStep						= 0.0f;
	fYStep						= 0.0f;
	uFlags						= flags;
	nNumChars					= 0x100;
	TCMap						= NULL;
	Initialize					(shader,texture);
}

void CGameFont::Initialize		(LPCSTR cShader, LPCSTR cTextureName)
{
	string_path					cTexture;

	LPCSTR _lang				= pSettings->r_string("string_table", "font_prefix");
	bool is_di					= strstr(cTextureName, "ui_font_hud_01") || 
								  strstr(cTextureName, "ui_font_hud_02") ||
								  strstr(cTextureName, "ui_font_console_02");
	if(_lang && !is_di)
		strconcat				(sizeof(cTexture),cTexture, cTextureName, _lang);
	else
		strcpy_s				(cTexture, sizeof(cTexture), cTextureName);

	uFlags						&=~fsValid;
	vTS.set						(1.f,1.f); // обязательно !!!

	eCurrentAlignment			= alLeft;
	vInterval.set				(1.f,1.f);

	strings.reserve				(128);

	// check ini exist
	string_path fn,buf;
	strcpy_s		(buf,cTexture); if (strext(buf)) *strext(buf)=0;
	R_ASSERT2	(FS.exist(fn,"$game_textures$",buf,".ini"),fn);
	CInifile* ini				= CInifile::Create(fn);

	nNumChars = 0x100;
	TCMap = ( Fvector* ) xr_realloc( ( void* ) TCMap , nNumChars * sizeof( Fvector ) );

	if ( ini->section_exist( "mb_symbol_coords" ) ) {
		nNumChars = 0x10000;
		TCMap = ( Fvector* ) xr_realloc( ( void* ) TCMap , nNumChars * sizeof( Fvector ) );
		uFlags |= fsMultibyte;
		fHeight = ini->r_float( "mb_symbol_coords" , "height" );
		
		fXStep = ceil( fHeight / 2.0f );

		for ( u32 i=0 ; i < nNumChars ; i++ ) {
			sprintf_s( buf ,sizeof(buf), "%05d" , i );
			if ( ini->line_exist( "mb_symbol_coords" , buf ) ) {
				Fvector v = ini->r_fvector3( "mb_symbol_coords" , buf );
				TCMap[i].set( v.x , v.y , 1 + v[2] - v[0] );
			} else
				TCMap[i].set( 0 , 0 , 0 );
		}
	}else
	if (ini->section_exist("symbol_coords")){
		fHeight						= ini->r_float("symbol_coords","height");
		for (u32 i=0; i<nNumChars; i++){
			sprintf_s				(buf,sizeof(buf),"%03d",i);
			Fvector v				= ini->r_fvector3("symbol_coords",buf);
			TCMap[i].set			(v.x,v.y,v[2]-v[0]);
		}
	}else{
	if (ini->section_exist("char widths")){
		fHeight					= ini->r_float("char widths","height");
		int cpl					= 16;
		for (u32 i=0; i<nNumChars; i++){
			sprintf_s			(buf,sizeof(buf),"%d",i);
			float w				= ini->r_float("char widths",buf);
			TCMap[i].set		((i%cpl)*fHeight,(i/cpl)*fHeight,w);
		}
	}else{
		R_ASSERT(ini->section_exist("font_size"));
		fHeight					= ini->r_float("font_size","height");
		float width				= ini->r_float("font_size","width");
		const int cpl			= ini->r_s32	("font_size","cpl");
		for (u32 i=0; i<nNumChars; i++)
			TCMap[i].set		((i%cpl)*width,(i/cpl)*fHeight,width);
		}
	}

	fCurrentHeight				= fHeight;

	CInifile::Destroy			(ini);

	// Shading
	pShader.create				(cShader,cTexture);
	pGeom.create				(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
}

CGameFont::~CGameFont()
{
	if ( TCMap )
		xr_free( TCMap );

	// Shading
	pShader.destroy		();
	pGeom.destroy		();
}

#define DI2PX(x) float(iFloor((x+1)*float(::Render->getTarget()->get_width())*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(::Render->getTarget()->get_height())*0.5f))

void CGameFont::OutSet			(float x, float y)
{
	fCurrentX=x;
	fCurrentY=y;
}

void CGameFont::OutSetI			(float x, float y)
{
	OutSet(DI2PX(x),DI2PY(y));
}

u32 CGameFont::smart_strlen( const char* S )
{
	return ( IsMultibyte() ? mbhMulti2Wide( NULL , NULL , 0 , S ) : xr_strlen( S ) );
}

void CGameFont::OnRender()
{
	VERIFY				(g_bRendering);
	if (pShader)		RCache.set_Shader	(pShader);

	if (!(uFlags&fsValid)){
		CTexture* T		= RCache.get_ActiveTexture(0);
		vTS.set			((int)T->get_Width(),(int)T->get_Height());
		fTCHeight		= fHeight/float(vTS.y);
		uFlags			|= fsValid;
	}

	for (u32 i=0; i<strings.size(); ){
		// calculate first-fit
		int		count	=	1;

		int length = smart_strlen( strings[ i ].string );

		while	((i+count)<strings.size()) {
			int L = smart_strlen( strings[ i + count ].string );

			if ((L+length)<MAX_MB_CHARS){
				count	++;
				length	+=	L;
			}
			else		break;
		}

		// lock AGP memory
		u32	vOffset;
		FVF::TL* v		= (FVF::TL*)RCache.Vertex.Lock	(length*4,pGeom.stride(),vOffset);
		FVF::TL* start	= v;

		// fill vertices
		u32 last		= i+count;
		for (; i<last; i++) {
			String		&PS	= strings[i];
			wide_char wsStr[ MAX_MB_CHARS ];

			int	len	= IsMultibyte() ? 
				mbhMulti2Wide( wsStr , NULL , MAX_MB_CHARS , PS.string ) :
				xr_strlen( PS.string );

			if (len) {
				float	X	= float(iFloor(PS.x));
				float	Y	= float(iFloor(PS.y));
				float	S	= PS.height*g_current_font_scale.y;
				float	Y2	= Y+S;
				float fSize = 0;

				if ( PS.align )
					fSize = IsMultibyte() ? SizeOf_( wsStr ) : SizeOf_( PS.string );

				switch ( PS.align )
				{
				case alCenter:	
						X	-= ( iFloor( fSize * 0.5f ) ) * g_current_font_scale.x;	
						break;
				case alRight:	
						X	-=	iFloor( fSize );
						break;
				}

				u32	clr,clr2;
				clr2 = clr	= PS.c;
				if (uFlags&fsGradient){
					u32	_R	= color_get_R	(clr)/2;
					u32	_G	= color_get_G	(clr)/2;
					u32	_B	= color_get_B	(clr)/2;
					u32	_A	= color_get_A	(clr);
					clr2	= color_rgba	(_R,_G,_B,_A);
				}

				float	tu,tv;
				for (int j=0; j<len; j++)
				{
					Fvector l;

					l = IsMultibyte() ? GetCharTC( wsStr[ 1 + j ] ) : GetCharTC( ( u16 ) ( u8 ) PS.string[j] );

					float scw		= l.z * g_current_font_scale.x;

					float fTCWidth	= l.z/vTS.x;

					if (!fis_zero(l.z))
					{
						tu			= ( l.x / vTS.x ) + ( 0.5f / vTS.x );
						tv			= ( l.y / vTS.y ) + ( 0.5f / vTS.y );

						v->set( X , Y2 , clr2 , tu , tv + fTCHeight );						v++;
						v->set( X ,	Y , clr , tu , tv );									v++;
						v->set( X + scw , Y2 , clr2 , tu + fTCWidth , tv + fTCHeight );		v++;
						v->set( X + scw , Y , clr , tu + fTCWidth , tv );					v++;
					}
					X += scw * vInterval.x;
					if ( IsMultibyte() ) {
						X -= 2;
						if ( IsNeedSpaceCharacter( wsStr[ 1 + j ] ) )
							X += fXStep;
					}
				}
			}
		}

		// Unlock and draw
		u32 vCount = (u32)(v-start);
		RCache.Vertex.Unlock		(vCount,pGeom.stride());
		if (vCount){
			RCache.set_Geometry		(pGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		}
	}
	strings.clear_not_free			();
}

u16 CGameFont::GetCutLengthPos( float fTargetWidth , const char * pszText )
{
	VERIFY( pszText );

	wide_char wsStr[ MAX_MB_CHARS ], wsPos[ MAX_MB_CHARS ];
	float fCurWidth = 0.0f , fDelta = 0.0f;

	u16	len	= mbhMulti2Wide( wsStr , wsPos , MAX_MB_CHARS , pszText );
	u16 i = 1;
	for ( ; i <= len ; i++ ) {

		fDelta = GetCharTC( wsStr[ i ] ).z - 2;

		if ( IsNeedSpaceCharacter( wsStr[ i ] ) )
			fDelta += fXStep;

		if ( ( fCurWidth + fDelta ) > fTargetWidth )
			break;
		else 
			fCurWidth += fDelta;
	}

	return wsPos[ i - 1 ];
}

u16 CGameFont::SplitByWidth( u16 * puBuffer , u16 uBufferSize , float fTargetWidth , const char * pszText )
{
	VERIFY( puBuffer && uBufferSize && pszText );

	wide_char wsStr[ MAX_MB_CHARS ] , wsPos[ MAX_MB_CHARS ];
	float fCurWidth = 0.0f , fDelta = 0.0f;
	u16 nLines = 0;

	u16	len	= mbhMulti2Wide( wsStr , wsPos , MAX_MB_CHARS , pszText );

	for ( u16 i = 1 ; i <= len ; i++ ) {

		fDelta = GetCharTC( wsStr[ i ] ).z - 2;

		if ( IsNeedSpaceCharacter( wsStr[ i ] ) )
			fDelta += fXStep;

		if ( 
				( ( fCurWidth + fDelta ) > fTargetWidth ) && // overlength
				( ! IsBadStartCharacter( wsStr[ i ] ) ) && // can start with this character
				( i < len ) && // is not the last character
				( ( i > 1 ) && ( ! IsBadEndCharacter( wsStr[ i - 1 ] ) ) ) // && // do not stop the string on a "bad" character
//				( ( i > 1 ) && ( ! ( ( IsAlphaCharacter( wsStr[ i - 1 ] ) ) && (  IsAlphaCharacter( wsStr[ i ] ) ) ) ) ) // do not split numbers or words
		) {
			fCurWidth = fDelta;
			VERIFY( nLines < uBufferSize );
			puBuffer[ nLines++ ] = wsPos[ i - 1 ];
		} else 
			fCurWidth += fDelta;
	}

	return nLines;
}

void CGameFont::MasterOut(
	BOOL bCheckDevice , BOOL bUseCoords , BOOL bScaleCoords , BOOL bUseSkip , 
	float _x , float _y , float _skip , LPCSTR fmt , va_list p )
{
	if ( bCheckDevice && ( ! Device.b_is_Active ) )
		return;

	String rs;

	rs.x = ( bUseCoords ? ( bScaleCoords ? ( DI2PX( _x ) ) : _x ) : fCurrentX );
	rs.y = ( bUseCoords ? ( bScaleCoords ? ( DI2PY( _y ) ) : _y ) : fCurrentY );
	rs.c = dwCurrentColor;
	rs.height = fCurrentHeight;
	rs.align = eCurrentAlignment;

	int vs_sz = _vsnprintf( rs.string , sizeof( rs.string ) - 1 , fmt , p );

	VERIFY( ( vs_sz != -1 ) && ( rs.string[ vs_sz ] == '\0' ) );

	if ( vs_sz == -1 )
		return;

	if ( vs_sz )
		strings.push_back( rs );

	if ( bUseSkip )
		OutSkip( _skip );
}

#define MASTER_OUT(CHECK_DEVICE,USE_COORDS,SCALE_COORDS,USE_SKIP,X,Y,SKIP,FMT) \
	{ va_list p; va_start ( p , fmt ); \
	  MasterOut( CHECK_DEVICE , USE_COORDS , SCALE_COORDS , USE_SKIP , X , Y , SKIP , FMT, p ); \
	  va_end( p ); }

void __cdecl CGameFont::OutI( float _x , float _y , LPCSTR fmt , ... )
{
	MASTER_OUT( FALSE , TRUE , TRUE , FALSE , _x  , _y , 0.0f , fmt );
};

void __cdecl CGameFont::Out( float _x , float _y , LPCSTR fmt , ... )
{
	MASTER_OUT( TRUE , TRUE , FALSE , FALSE , _x , _y , 0.0f , fmt );
};

void __cdecl CGameFont::OutNext( LPCSTR fmt , ... )
{
	MASTER_OUT( TRUE , FALSE , FALSE , TRUE , 0.0f , 0.0f , 1.0f , fmt );
};

void __cdecl CGameFont::OutPrev( LPCSTR fmt , ... )
{
	MASTER_OUT( TRUE , FALSE , FALSE , TRUE , 0.0f , 0.0f , -1.0f , fmt );
};


void CGameFont::OutSkip( float val )
{
	fCurrentY += val*CurrentHeight_();
}

float CGameFont::SizeOf_( const char cChar )
{
	VERIFY( ! IsMultibyte() );

	return ( ( GetCharTC( ( u16 ) ( u8 ) cChar ).z * vInterval.x ) );
}

float CGameFont::SizeOf_( LPCSTR s )
{
	if ( ! ( s && s[ 0 ] ) )
		return 0;

	if ( IsMultibyte() ) {
		wide_char wsStr[ MAX_MB_CHARS ];

		mbhMulti2Wide( wsStr , NULL , MAX_MB_CHARS , s );

		return SizeOf_( wsStr );
	}

	int		len			= xr_strlen(s);
	float	X			= 0;
	if (len)
		for (int j=0; j<len; j++)
			X			+= GetCharTC( ( u16 ) ( u8 ) s[ j ] ).z;
	return				(X*vInterval.x/**vTS.x*/);
}

float CGameFont::SizeOf_( const wide_char *wsStr )
{
	if ( ! ( wsStr && wsStr[ 0 ] ) )
		return 0;

	unsigned int len = wsStr[ 0 ];
	float X	= 0.0f , fDelta = 0.0f;

	if ( len )
		for ( unsigned int j=1 ; j <= len ; j++ ) {
			fDelta = GetCharTC( wsStr[ j ] ).z - 2;
			if ( IsNeedSpaceCharacter( wsStr[ j ] ) )
				fDelta += fXStep;
			X += fDelta;
		}

	return ( X * vInterval.x );
}

float CGameFont::CurrentHeight_	()
{
	return fCurrentHeight * vInterval.y;
}

void CGameFont::SetHeightI(float S)
{
	VERIFY			( uFlags&fsDeviceIndependent );
	fCurrentHeight	= S*Device.dwHeight;
};

void CGameFont::SetHeight(float S)
{
	VERIFY			( uFlags&fsDeviceIndependent );
	fCurrentHeight	= S;
};
