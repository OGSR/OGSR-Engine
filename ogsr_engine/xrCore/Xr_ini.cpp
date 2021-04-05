#include "stdafx.h"

#include "fs_internal.h"

XRCORE_API CInifile *pSettings = nullptr;


CInifile* CInifile::Create( const char* szFileName, BOOL ReadOnly ) {
  return xr_new<CInifile>( szFileName, ReadOnly );
}

void CInifile::Destroy( CInifile* ini ) {
  xr_delete( ini );
}


//Тело функций Inifile
XRCORE_API void _parse( LPSTR dest, LPCSTR src ) {
  if ( src ) {
    bool bInsideSTR = false;
    while ( *src ) {
      if ( isspace( (u8)*src ) ) {
        if ( bInsideSTR ) { *dest++ = *src++; continue; }
        while ( *src && iswspace( *src ) ) src++;
        continue;
      }
      else if ( *src == '"' ) {
        bInsideSTR = !bInsideSTR;
      }
      *dest++ = *src++;
    }
  }
  *dest = 0;
}


XRCORE_API void _decorate( LPSTR dest, LPCSTR src ) {
  if (src) {
    bool bInsideSTR = false;
    while ( *src ) {
      if ( *src == ',' ) {
        if ( bInsideSTR )
          *dest++ = *src++;
        else {
          *dest++ = *src++;
          *dest++ = ' ';
        }
        continue;
      }
      else if ( *src=='"' ) {
        bInsideSTR = !bInsideSTR;
      }
      *dest++ = *src++;
    }
  }
  *dest = 0;
}


BOOL CInifile::Sect::line_exist( LPCSTR L, LPCSTR* val ) {
  const auto A = Data.find( L );
  if (A != Data.end() ){
    if ( val ) *val = *A->second;
    return TRUE;
  }
  return FALSE;
}

LPCSTR CInifile::Sect::r_string( LPCSTR L ) {
	if (!L || !strlen(L)) //--#SM+#-- [fix for one of "xrDebug - Invalid handler" error log]
		Msg("!![ERROR] CInifile::Sect::r_string: S = [%s], L = [%s]", Name.c_str(), L);

	const auto A = Data.find(L);
	if (A != Data.end())
		return A->second.c_str();
	else
		FATAL("Can't find variable %s in [%s]", L, Name.c_str());
	return 0;
}

float CInifile::Sect::r_float( LPCSTR L ) {
	LPCSTR C = r_string(L);
	return float(atof(C));
}

u32 CInifile::Sect::r_u32( LPCSTR L ) {
	LPCSTR C = r_string(L);
	return u32(atoi(C));
}

Fvector3 CInifile::Sect::r_fvector3( LPCSTR L ) {
	LPCSTR   C = r_string(L);
	Fvector3 V{};
	sscanf(C, "%f,%f,%f", &V.x, &V.y, &V.z);
	return V;
}

Ivector2 CInifile::Sect::r_ivector2( LPCSTR L ) {
	LPCSTR   C = r_string(L);
	Ivector2 V{};
	sscanf(C, "%d,%d", &V.x, &V.y);
	return V;
}

u32 CInifile::Sect::line_count() {
	return u32( Data.size() );
}

CInifile::CInifile( IReader* F, LPCSTR path ) {
  fName      = 0;
  bReadOnly  = true;
  bSaveAtEnd = FALSE;
  Load( F, path );
}


CInifile::CInifile( LPCSTR szFileName, BOOL ReadOnly, BOOL bLoad, BOOL SaveAtEnd ) {
  fName       = szFileName ? xr_strdup( szFileName ) : 0;
  bReadOnly   = !!ReadOnly;
  bSaveAtEnd  = SaveAtEnd;
  if ( bLoad ) {
    string_path path, folder;
    _splitpath( fName, path, folder, 0, 0 );
    strcat_s( path, folder );
    IReader* R = FS.r_open( szFileName );
    if ( R ) {
      Load( R, path );
      FS.r_close( R );
    }
  }
}


CInifile::~CInifile() {
  if ( !bReadOnly && bSaveAtEnd ) {
    if ( !save_as() )
      Log( "!Can't save inifile:", fName );
  }
  xr_free( fName );
  for ( auto &I : DATA ) xr_delete( I.second );
}


static void insert_item( CInifile::Sect *tgt, const CInifile::Item& I ) {
  auto sect_it = tgt->Data.find( I.first );
  if ( sect_it != tgt->Data.end() && sect_it->first.equal( I.first ) ) {
    sect_it->second = I.second;
    auto found = std::find_if(
      tgt->Unordered.begin(), tgt->Unordered.end(),
      [&]( const auto &it ) {
        return xr_strcmp( *it.first, *I.first ) == 0;
      }
    );
    if ( found != tgt->Unordered.end() ) {
      found->second = I.second;
    }
  }
  else {
    tgt->Data.emplace( I.first, I.second );
    tgt->Unordered.push_back( I );
  }
}


void CInifile::Load ( IReader* F, LPCSTR path ) {
  R_ASSERT( F );
  Sect       *Current = nullptr;
  string4096 str;
  string4096 str2;

  while ( !F->eof() ) {
    F->r_string( str, sizeof( str ) );
    _Trim( str );
    LPSTR semi   = strchr( str, ';' );
    LPSTR semi_1 = strchr( str, '/' );

    if ( semi_1 && ( *(semi_1 + 1) == '/' ) && ( (!semi) || ( semi && ( semi_1 < semi ) ) ) )
      semi = semi_1;

    if ( semi )
      *semi = 0;

    if ( str[ 0 ] && ( str[ 0 ] == '#' ) && strstr( str, "#include" ) ) {
      string64 inc_name;
      R_ASSERT( path && path[ 0 ] );
      if ( _GetItem( str, 1, inc_name, '"' ) ) {
        string_path fn, inc_path, folder;
        strconcat( sizeof( fn ), fn, path, inc_name );
        _splitpath( fn, inc_path, folder, 0, 0 );
        strcat_s( inc_path, folder );
        IReader* I = FS.r_open( fn );
        R_ASSERT3( I, "Can't find include file:", inc_name );
        Load( I, inc_path );
        FS.r_close( I );
      }
    }
    else if ( str[ 0 ] && ( str[ 0 ] == '[' ) ) {
      // insert previous filled section
      if ( Current ) {
        auto I = DATA.find( Current->Name );
        if ( I != DATA.end() )
          FATAL( "Duplicate section '%s' found.", Current->Name.c_str() );
        DATA.emplace( Current->Name, Current );
      }
      Current = xr_new<Sect>();
      Current->Name = 0;
      // start new section
      R_ASSERT3( strchr( str, ']' ), "Bad ini section found: ", str );
      LPCSTR inherited_names = strstr( str, "]:" );
      if ( 0 != inherited_names ) {
        VERIFY2( bReadOnly, "Allow for readonly mode only." );
        inherited_names += 2;
        int cnt = _GetItemCount( inherited_names );
        for ( int k = 0; k < cnt; ++k ) {
          xr_string tmp;
          _GetItem( inherited_names, k, tmp );
          Sect& inherited = r_section( tmp.c_str() );
          for ( auto &it : inherited.Data ) {
            Item I = { it.first, it.second };
            insert_item( Current, I );
          }
        }
      }
      *strchr( str, ']' ) = 0;
      Current->Name = strlwr( str + 1 );
    }
    else {
      if ( Current ) {
        char* name = str;
        char* t    = strchr( name, '=' );
        if ( t ) {
          *t = 0;
          _Trim( name );
          _parse( str2, ++t );
        }
        else {
          _Trim( name );
          str2[ 0 ] = 0;
        }

        if ( name[ 0 ] ) {
          Item I;
          I.first  = name;
          I.second = str2[ 0 ] ? str2 : 0;
          if ( bReadOnly ) {
            if ( *I.first )
              insert_item( Current, I );
          }
          else {
            if ( *I.first || *I.second )
              insert_item( Current, I );
          }
        }
      }
    }
  }

  if ( Current ) {
    auto I = DATA.find( Current->Name );
    if ( I != DATA.end() )
      FATAL( "Duplicate section '%s' found.", Current->Name.c_str() );
    DATA.emplace( Current->Name, Current );
  }
}


bool CInifile::save_as( LPCSTR new_fname ) {
  // save if needed
  if ( new_fname && new_fname[ 0 ] ) {
    xr_free( fName );
    fName = xr_strdup( new_fname );
  }
  R_ASSERT( fName && fName[ 0 ] );
  IWriter* F = FS.w_open_ex( fName );
  if ( F ) {
    string512 temp, val;
    for ( const auto &r_it : DATA ) {
      sprintf_s( temp, "[%s]", r_it.first.c_str() );
      F->w_string( temp );
      for ( const auto &I : r_it.second->Unordered ) {
        if ( *I.first ) {
          if ( *I.second ) {
            _decorate( val, *I.second );
            {
              // only name and value
              sprintf_s( temp, "%-32s = %-32s", *I.first, val );
            }
          }
          else {
            {
              // only name
              sprintf_s( temp, "%-32s = ", *I.first );
            }
          }
        }
        else {
          // no name, so no value
          temp[ 0 ] = 0;
        }
        _TrimRight( temp );
        if ( temp[ 0 ] ) F->w_string( temp );
      }
      F->w_string( " " );
    }
    FS.w_close( F );
    return true;
  }
  return false;
}


BOOL CInifile::section_exist( LPCSTR S ) {
  return DATA.find(S) != DATA.end();
}


BOOL CInifile::line_exist( LPCSTR S, LPCSTR L ) {
  if ( !section_exist( S ) ) return FALSE;
  Sect&   I = r_section( S );
  const auto A = I.Data.find( L );
  return A != I.Data.end();
}

u32 CInifile::line_count( LPCSTR Sname ) {
  Sect& S = r_section( Sname );
  return S.line_count();
}


CInifile::Sect& CInifile::r_section ( const shared_str& S ) {
  return r_section( S.c_str() );
}


BOOL CInifile::line_exist ( const shared_str& S, const shared_str& L ) {
  return line_exist( S.c_str(), L.c_str() );
}


u32 CInifile::line_count ( const shared_str& S ) {
  return line_count( S.c_str() );
}


BOOL CInifile::section_exist ( const shared_str& S ) {
  return section_exist( S.c_str() );
}


CInifile::Sect& CInifile::r_section( LPCSTR S ) {
  R_ASSERT( S && strlen( S ), "Empty section (null\\'') passed into CInifile::r_section(). See info above ^, check your configs and 'call stack'." ); //--#SM+#--

  char section[ 256 ];
  strcpy_s( section, S );
  shared_str k = strlwr( section );
  const auto I = DATA.find( k );
  if ( I == DATA.end() )
    FATAL( "Can't open section '%s'", S );
  return  *I->second;
}

LPCSTR CInifile::r_string ( LPCSTR S, LPCSTR L ) {
  if ( !S || !L || !strlen( S ) || !strlen( L ) ) //--#SM+#-- [fix for one of "xrDebug - Invalid handler" error log]
    Msg( "!![ERROR] CInifile::r_string: S = [%s], L = [%s]", S, L );

  Sect&   I = r_section( S );
  const auto A = I.Data.find( L );
  if ( A != I.Data.end() )
    return A->second.c_str();
  else
    FATAL( "Can't find variable %s in [%s]", L, S );
  return 0;
}


shared_str CInifile::r_string_wb( LPCSTR S, LPCSTR L ) {
  LPCSTR _base = r_string( S, L );
  if ( 0 == _base ) return  shared_str( 0 );
  string512 _original;
  strcpy_s( _original, _base );
  u32 _len = xr_strlen( _original );
  if ( 0 == _len ) return  shared_str( "" );
  if ( '"' == _original[ _len - 1 ] ) _original[ _len - 1 ] = 0; // skip end
  if ( '"' == _original[ 0 ] ) return  shared_str( &_original[ 0 ] + 1 ); // skip begin
  return shared_str( _original );
}


u8 CInifile::r_u8 ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return u8( atoi( C ) );
}


u16 CInifile::r_u16 ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return u16( atoi( C ) );
}


u32 CInifile::r_u32 ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return u32( atoi( C ) );
}


s8 CInifile::r_s8 ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return s8( atoi( C ) );
}


s16 CInifile::r_s16 ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return s16( atoi( C ) );
}


s32 CInifile::r_s32 ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return s32( atoi( C ) );
}


float CInifile::r_float ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  return float( atof( C ) );
}


Fcolor CInifile::r_fcolor ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  Fcolor V{};
  sscanf( C, "%f,%f,%f,%f", &V.r, &V.g, &V.b, &V.a );
  return V;
}


u32 CInifile::r_color ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  u32    r = 0, g = 0, b = 0, a = 255;
  sscanf( C, "%d,%d,%d,%d", &r, &g, &b, &a );
  return color_rgba( r, g, b, a );
}


Ivector2 CInifile::r_ivector2 ( LPCSTR S, LPCSTR L ) {
  LPCSTR   C = r_string( S, L );
  Ivector2 V{};
  sscanf( C, "%d,%d", &V.x, &V.y );
  return V;
}


Ivector3 CInifile::r_ivector3 ( LPCSTR S, LPCSTR L ) {
  LPCSTR  C = r_string( S, L );
  Ivector V{};
  sscanf( C, "%d,%d,%d", &V.x, &V.y, &V.z );
  return V;
}


Ivector4 CInifile::r_ivector4 ( LPCSTR S, LPCSTR L ) {
  LPCSTR   C = r_string( S, L );
  Ivector4 V{};
  sscanf( C, "%d,%d,%d,%d", &V.x, &V.y, &V.z, &V.w );
  return V;
}


Fvector2 CInifile::r_fvector2 ( LPCSTR S, LPCSTR L ) {
  LPCSTR   C = r_string( S, L );
  Fvector2 V{};
  sscanf( C, "%f,%f", &V.x, &V.y );
  return V;
}


Fvector3 CInifile::r_fvector3 ( LPCSTR S, LPCSTR L ) {
  LPCSTR   C = r_string( S, L );
  Fvector3 V{};
  sscanf( C, "%f,%f,%f", &V.x, &V.y, &V.z );
  return V;
}


Fvector4 CInifile::r_fvector4 ( LPCSTR S, LPCSTR L ) {
  LPCSTR   C = r_string( S, L );
  Fvector4 V{};
  sscanf( C, "%f,%f,%f,%f", &V.x, &V.y, &V.z, &V.w );
  return V;
}


BOOL CInifile::r_bool ( LPCSTR S, LPCSTR L ) {
  LPCSTR C = r_string( S, L );
  char   B[ 8 ];
  strncpy_s( B, C, 7 );
  strlwr( B );
  return IsBOOL( B );
}


CLASS_ID CInifile::r_clsid ( LPCSTR S, LPCSTR L) {
  LPCSTR C = r_string( S, L );
  return TEXT2CLSID( C );
}


int CInifile::r_token ( LPCSTR S, LPCSTR L, const xr_token *token_list ) {
  LPCSTR C = r_string( S, L );
  for( int i = 0; token_list[ i ].name; i++ )
    if( !stricmp( C, token_list[ i ].name ) )
      return token_list[ i ].id;
  return 0;
}


BOOL CInifile::r_line( LPCSTR S, int L, const char** N, const char** V ) {
  Sect& SS = r_section( S );
  if ( L >= (int)SS.Unordered.size() || L < 0 ) return FALSE;
  const auto &I = SS.Unordered.at( L );
  *N = I.first.c_str();
  *V = I.second.c_str();
  return TRUE;
}


BOOL CInifile::r_line( const shared_str& S, int L, const char** N, const char** V ) {
  return r_line( S.c_str(), L, N, V );
}


void CInifile::w_string ( LPCSTR S, LPCSTR L, LPCSTR V ) {
  R_ASSERT( !bReadOnly );

  // section
  char sect[ 256 ];
  _parse( sect, S );
  _strlwr( sect );
  ASSERT_FMT( sect[ 0 ], "[%s]: wrong section name [%s]", __FUNCTION__, S );
  if ( !section_exist( sect ) ) {
    // create _new_ section
    Sect *NEW = xr_new<Sect>();
    NEW->Name = sect;
    DATA.emplace( NEW->Name, NEW );
  }

  // parse line/value
  char line [ 256 ];
  _parse( line, L );
  ASSERT_FMT( line[ 0 ], "[%s]: wrong param name [%s]", __FUNCTION__, L );
  char value[ 256 ];
  _parse( value, V );

  // duplicate & insert
  Item I;
  I.first  = line;
  I.second = value[ 0 ] ? value : 0;
  Sect* data = &r_section( sect );
  insert_item( data, I );
}


void CInifile::w_u8 ( LPCSTR S, LPCSTR L, u8 V ) {
  string128 temp;
  sprintf_s( temp, "%d", V );
  w_string( S, L, temp );
}


void CInifile::w_u16 ( LPCSTR S, LPCSTR L, u16 V ) {
  string128 temp;
  sprintf_s( temp, "%d", V );
  w_string( S, L, temp );
}


void CInifile::w_u32( LPCSTR S, LPCSTR L, u32 V ) {
  string128 temp;
  sprintf_s( temp, "%d", V );
  w_string( S, L, temp );
}


void CInifile::w_s8 ( LPCSTR S, LPCSTR L, s8 V ) {
  string128 temp;
  sprintf_s( temp, "%d", V );
  w_string( S, L, temp );
}


void CInifile::w_s16 ( LPCSTR S, LPCSTR L, s16 V ) {
  string128 temp;
  sprintf_s( temp, "%d", V );
  w_string( S, L, temp );
}


void CInifile::w_s32 ( LPCSTR S, LPCSTR L, s32 V ) {
  string128 temp;
  sprintf_s( temp, "%d", V );
  w_string( S, L, temp);
}


void CInifile::w_float ( LPCSTR S, LPCSTR L, float V ) {
  string128 temp;
  sprintf_s( temp, "%f", V );
  w_string( S, L, temp );
}


void CInifile::w_fcolor ( LPCSTR S, LPCSTR L, const Fcolor& V ) {
  string128 temp;
  sprintf_s( temp, "%f,%f,%f,%f", V.r, V.g, V.b, V.a );
  w_string( S, L, temp );
}


void CInifile::w_color ( LPCSTR S, LPCSTR L, u32 V ) {
  string128 temp;
  sprintf_s( temp, "%d,%d,%d,%d", color_get_R( V ), color_get_G( V ), color_get_B( V ), color_get_A( V ) );
  w_string( S, L, temp );
}



void CInifile::w_ivector2 ( LPCSTR S, LPCSTR L, const Ivector2& V ) {
  string128 temp;
  sprintf_s( temp, "%d,%d", V.x, V.y );
  w_string( S, L, temp );
}


void CInifile::w_ivector3 ( LPCSTR S, LPCSTR L, const Ivector3& V ) {
  string128 temp;
  sprintf_s( temp, "%d,%d,%d", V.x, V.y, V.z );
  w_string( S, L, temp );
}


void CInifile::w_ivector4 ( LPCSTR S, LPCSTR L, const Ivector4& V ) {
  string128 temp;
  sprintf_s( temp, "%d,%d,%d,%d", V.x, V.y, V.z, V.w );
  w_string( S, L, temp );
}


void CInifile::w_fvector2 ( LPCSTR S, LPCSTR L, const Fvector2& V ) {
  string128 temp;
  sprintf_s( temp, "%f,%f", V.x, V.y );
  w_string( S, L, temp );
}


void CInifile::w_fvector3 ( LPCSTR S, LPCSTR L, const Fvector3& V ) {
  string128 temp;
  sprintf_s( temp, "%f,%f,%f", V.x, V.y, V.z );
  w_string( S, L, temp );
}


void CInifile::w_fvector4 ( LPCSTR S, LPCSTR L, const Fvector4& V ) {
  string128 temp;
  sprintf_s( temp, "%f,%f,%f,%f", V.x, V.y, V.z, V.w );
  w_string( S, L, temp );
}


void CInifile::w_bool ( LPCSTR S, LPCSTR L, bool V ) {
  w_string( S, L, V ? "true" : "false" );
}


void CInifile::remove_line( LPCSTR S, LPCSTR L ) {
  R_ASSERT( !bReadOnly );

  if ( line_exist( S, L ) ) {
    Sect& data = r_section( S );
    auto A = data.Data.find( L );
    R_ASSERT( A != data.Data.end() );
    data.Data.erase( A );
    auto found = std::find_if(
      data.Unordered.begin(), data.Unordered.end(),
      [&]( const auto& it ) {
        return xr_strcmp( *it.first, L ) == 0;
      }
    );
    R_ASSERT( found != data.Unordered.end() );
    data.Unordered.erase( found );
  }
}


void CInifile::remove_section( LPCSTR S ) {
  R_ASSERT( !bReadOnly );

  if ( section_exist( S ) ) {
    const auto I = DATA.find( S );
    R_ASSERT( I != DATA.end() );
    DATA.erase( I );
  }
}


#include <sstream>

std::string CInifile::get_as_string() {
  std::stringstream str;

  bool first_sect = true;
  for ( const auto &r_it : DATA ) {
    if ( !first_sect ) str << "\r\n";
    first_sect = false;
    str << "[" << r_it.first.c_str() << "]\r\n";
    for ( const auto &I : r_it.second->Unordered ) {
      if ( I.first.c_str() ) {
        if ( I.second.c_str() ) {
          string512 val;
          _decorate( val, I.second.c_str() );
          _TrimRight( val );
          // only name and value
          str << I.first.c_str() << " = " << val << "\r\n";
        }
        else {
          // only name
          str << I.first.c_str() << " =\r\n";
        }
      }
    }
  }

  return str.str();
}
