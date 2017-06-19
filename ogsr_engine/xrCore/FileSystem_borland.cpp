//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "FileSystem.h"

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

bool EFS_Utils::GetOpenName(LPCSTR initial, xr_string& buffer, bool bMulti, LPCSTR offset, int start_flt_ext )
{
	string_path		buf;
	strcpy			(buf,buffer.c_str());
	bool bRes		= GetOpenName(initial,buf,sizeof(buf),bMulti,offset,start_flt_ext);

	if (bRes) 
		buffer=(char*)buf;

	return bRes;
}

bool EFS_Utils::GetSaveName( LPCSTR initial, xr_string& buffer, LPCSTR offset, int start_flt_ext )
{
	string_path				buf;
	strcpy_s				(buf,sizeof(buf), buffer.c_str());
	bool bRes				= GetSaveName(initial,buf,offset,start_flt_ext);
	if (bRes) 
		buffer				= buf;

	return bRes;
}
//----------------------------------------------------

void EFS_Utils::MarkFile(LPCSTR fn, bool bDeleteSource)
{
	xr_string ext = strext(fn);
	ext.insert		(1,"~");
	xr_string backup_fn = EFS.ChangeFileExt(fn,ext.c_str());
	if (bDeleteSource){
		FS.file_rename(fn,backup_fn.c_str(),true);
	}else{
		FS.file_copy(fn,backup_fn.c_str());
	}
}

xr_string	EFS_Utils::AppendFolderToName(xr_string& tex_name, int depth, BOOL full_name)
{
	string1024 nm;
	strcpy(nm,tex_name.c_str());
	tex_name = AppendFolderToName(nm,depth,full_name);
	return tex_name;
}

BOOL EFS_Utils::CheckLocking(LPCSTR fname, bool bOnlySelf, bool bMsg)//, shared_str* owner)
{
	string256 fn; strcpy(fn,fname);

	if (bOnlySelf) return (m_LockFiles.find(fn)!=m_LockFiles.end());
	if (FS.exist(fn)){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(handle);
        if (INVALID_HANDLE_VALUE==handle){
			if (bMsg)	Msg("#!Access denied. File: '%s' currently locked by any user.",fn);
//.            if (owner) 	*owner = GetLockOwner(initial,fname);
        }
		return (INVALID_HANDLE_VALUE==handle);
	}
    return FALSE;
}

BOOL EFS_Utils::LockFile(LPCSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);

	BOOL bRes=false;
	if (m_LockFiles.find(fn)==m_LockFiles.end()){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (INVALID_HANDLE_VALUE!=handle){
			LPSTR lp_fn			= fn;
			std::pair<HANDLEPairIt, bool> I=m_LockFiles.insert(mk_pair(lp_fn,handle));
			R_ASSERT(			I.second);
/*            
			// register access              LockFile
            xr_string pref 		= initial?xr_string(FS.get_path(initial)->m_Add)+"\\":xr_string("");
            xr_string			m_AccessFN = xr_string("access\\")+pref+fname+xr_string(".desc");
            FS.update_path		(m_AccessFN,"$server_data_root$",m_AccessFN.c_str());
            
            CInifile*	ini		= CInifile::Create(m_AccessFN.c_str(),false);
            string512			buf0,buf1;
            string16			dt_buf, tm_buf;
            sprintf				(buf0,"%s-%s:%s-LOCK",_strdate(dt_buf),_strtime(tm_buf),"1");
            sprintf				(buf1,"\"Computer: '%s' User: '%s'\"",Core.CompName,Core.UserName);
            ini->w_string		("history",buf0,buf1);
            sprintf 			(buf0,"\\\\%s\\%s",Core.CompName,Core.UserName);
            ini->w_string		("locked","name",buf0);
            CInifile::Destroy	(ini);
*/            
			if (bLog) 			WriteAccessLog(fn,"Lock");
			bRes				= true;
		}
	}
	return bRes;
}

BOOL EFS_Utils::UnlockFile(LPCSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);

	HANDLEPairIt it 			= m_LockFiles.find(fn);
	if (it!=m_LockFiles.end()){
    	void* handle 			= it->second;
		m_LockFiles.erase		(it);
/*        
        // unregister access
        xr_string pref 			= initial?xr_string(FS.get_path(initial)->m_Add)+"\\":xr_string("");
        xr_string				m_AccessFN = xr_string("access\\")+pref+fname+xr_string(".desc");
        FS.update_path			(m_AccessFN,"$server_data_root$",m_AccessFN.c_str());
        CInifile*	ini			= CInifile::Create(m_AccessFN.c_str(),false);
        string512				buf0,buf1;
        string16				dt_buf, tm_buf;
        sprintf					(buf0,"%s-%s:%s-UNLOCK",_strdate(dt_buf),_strtime(tm_buf),"0");
        sprintf					(buf1,"\"Computer: '%s' User: '%s'\"",Core.CompName,Core.UserName);
        ini->w_string			("locked","name","");
        ini->w_string			("history",buf0,buf1);
        CInifile::Destroy		(ini);
*/
		if (bLog)				WriteAccessLog(fn,"Unlock");
		return CloseHandle		(handle);
	}
	return false;
}

void EFS_Utils::WriteAccessLog(LPCSTR fn, LPCSTR start_msg)
{
/*
	string1024		buf;
	string256		dt_buf, tm_buf;
	sprintf			(buf, "%s:   '%s' from computer: '%s' by user: '%s' at %s %s",start_msg,fn,Core.CompName,Core.UserName,_strdate(dt_buf),_strtime(tm_buf));
	xr_string		m_AccessLog;

    if(FS.path_exist("$server_data_root$"))
    {
        FS.update_path	(m_AccessLog,"$server_data_root$","access.log");
        int hf 			= open( m_AccessLog.c_str(), _O_WRONLY|_O_APPEND|_O_BINARY );
        if( hf<=0 )
            hf = open( m_AccessLog.c_str(),
            _O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY,
            _S_IREAD | _S_IWRITE );

        _write( hf, buf, xr_strlen(buf) );
        char el[2]={0x0d,0x0a};
        _write( hf, el, 2 );
        _close( hf );
    }
*/
}
/*
shared_str EFS_Utils::GetLockOwner(LPCSTR initial, LPCSTR fname)
{
    xr_string pref 				= initial?xr_string(FS.get_path(initial)->m_Add):xr_string("");
    xr_string					m_AccessFN = xr_string("access\\")+pref+fname+xr_string(".desc");
    FS.update_path				(m_AccessFN,"$server_data_root$",m_AccessFN.c_str());
    CInifile*	ini				= CInifile::Create(m_AccessFN.c_str(),false);
	static string256 			comp;
    LPCSTR l_name				= ini->r_string("locked","name");
    if (ini->line_exist("locked","name"))
		strcpy					(comp,l_name&&l_name[0]?l_name:"unknown");
	CInifile::Destroy			(ini);

	return comp;
}
*/
