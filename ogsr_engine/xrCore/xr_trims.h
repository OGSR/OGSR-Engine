#pragma once

// refs
struct xr_token;

XRCORE_API int		    	_GetItemCount			( LPCSTR , char separator=',');
XRCORE_API LPSTR	    	_GetItem				( LPCSTR, int, LPSTR, char separator=',', LPCSTR ="", bool trim=true );
XRCORE_API LPSTR	    	_GetItems				( LPCSTR, int, int, LPSTR, char separator=',');
XRCORE_API LPCSTR	    	_SetPos					( LPCSTR src, u32 pos, char separator=',' );
XRCORE_API LPCSTR	    	_CopyVal				( LPCSTR src, LPSTR dst, char separator=',' );
XRCORE_API LPSTR	    	_Trim					( LPSTR str );
XRCORE_API LPSTR	    	_TrimLeft				( LPSTR str );
XRCORE_API LPSTR	    	_TrimRight				( LPSTR str );
XRCORE_API LPSTR	    	_ChangeSymbol			( LPSTR name, char src, char dest );
XRCORE_API u32		    	_ParseItem				( LPCSTR src, xr_token* token_list );
XRCORE_API u32		    	_ParseItem				( LPSTR src, int ind, xr_token* token_list );
XRCORE_API LPSTR 	    	_ReplaceItem 			( LPCSTR src, int index, LPCSTR new_item, LPSTR dst, char separator );
XRCORE_API LPSTR 	    	_ReplaceItems 			( LPCSTR src, int idx_start, int idx_end, LPCSTR new_items, LPSTR dst, char separator );
XRCORE_API void 	    	_SequenceToList			( LPSTRVec& lst, LPCSTR in, char separator=',' );
XRCORE_API void 			_SequenceToList			( RStringVec& lst, LPCSTR in, char separator=',' );
XRCORE_API void 			_SequenceToList			( SStringVec& lst, LPCSTR in, char separator=',' );

XRCORE_API xr_string& 		_Trim					( xr_string& src );
XRCORE_API xr_string& 		_TrimLeft				( xr_string& src );
XRCORE_API xr_string&		_TrimRight				( xr_string& src );
XRCORE_API xr_string&   	_ChangeSymbol			( xr_string& name, char src, char dest );
XRCORE_API LPCSTR		 	_CopyVal 				( LPCSTR src, xr_string& dst, char separator=',' );
XRCORE_API LPCSTR			_GetItem				( LPCSTR src, int, xr_string& p, char separator=',', LPCSTR ="", bool trim=true );
XRCORE_API xr_string		_ListToSequence			( const SStringVec& lst );
XRCORE_API shared_str		_ListToSequence			( const RStringVec& lst );
