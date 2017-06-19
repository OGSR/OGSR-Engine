//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "cderr.h"
#include "commdlg.h"

EFS_Utils*	xr_EFS	= NULL;
//----------------------------------------------------
EFS_Utils::EFS_Utils( )
{
}

EFS_Utils::~EFS_Utils()
{
}

xr_string	EFS_Utils::ExtractFileName(LPCSTR src)
{
	string_path name;
	_splitpath	(src,0,0,name,0);
    return xr_string(name);
}

xr_string	EFS_Utils::ExtractFileExt(LPCSTR src)
{
	string_path ext;
	_splitpath	(src,0,0,0,ext);
    return xr_string(ext);
}

xr_string	EFS_Utils::ExtractFilePath(LPCSTR src)
{
	string_path drive,dir;
	_splitpath	(src,drive,dir,0,0);
    return xr_string(drive)+dir;
}

xr_string	EFS_Utils::ExcludeBasePath(LPCSTR full_path, LPCSTR excl_path)
{
    LPCSTR sub		= strstr(full_path,excl_path);
	if (0!=sub) 	return xr_string(sub+xr_strlen(excl_path));
	else	   		return xr_string(full_path);
}

xr_string	EFS_Utils::ChangeFileExt(LPCSTR src, LPCSTR ext)
{
	xr_string	tmp;
	LPSTR src_ext	= strext(src);
    if (src_ext){
	    size_t		ext_pos	= src_ext-src;
        tmp.assign	(src,0,ext_pos);
    }else{
        tmp			= src;
    }
    tmp				+= ext;
    return tmp;
}

xr_string	EFS_Utils::ChangeFileExt(const xr_string& src, LPCSTR ext)
{
	return ChangeFileExt(src.c_str(),ext);
}

//----------------------------------------------------
LPCSTR MakeFilter(string1024& dest, LPCSTR info, LPCSTR ext)
{
	ZeroMemory(dest,sizeof(dest));
    if (ext){
        int icnt=_GetItemCount(ext,';');
		LPSTR dst=dest;
        if (icnt>1)
		{
            strconcat		(sizeof(dest),dst,info," (",ext,")");
            dst				+= (xr_strlen(dst)+1);
            strcpy			(dst,ext);
            dst				+= (xr_strlen(ext)+1);
        }
        for (int i=0; i<icnt; i++)
		{
            string64		buf;
            _GetItem		(ext,i,buf,';');
            strconcat		(sizeof(dest), dst,info," (",buf,")");
            dst				+= (xr_strlen(dst)+1);
            strcpy			(dst, buf);
            dst				+= (xr_strlen(buf)+1);
        }
    }
	return dest;
}

//------------------------------------------------------------------------------
// start_flt_ext = -1-all 0..n-indices
//------------------------------------------------------------------------------
bool EFS_Utils::GetOpenName( LPCSTR initial,  string_path& buffer, int sz_buf, bool bMulti, LPCSTR offset, int start_flt_ext )
{
	VERIFY(buffer&&(sz_buf>0));
	FS_Path& P			= *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt,P.m_FilterCaption?P.m_FilterCaption:"",P.m_DefExt);

	OPENFILENAME ofn;
	Memory.mem_fill		( &ofn, 0, sizeof(ofn) );
    if (xr_strlen(buffer)){ 
        string_path		dr;
        if (!(buffer[0]=='\\' && buffer[1]=='\\')){ // if !network
            _splitpath		(buffer,dr,0,0,0);
            if (0==dr[0])	P._update(buffer,buffer); 
        }
    }
    ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= P.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.nMaxFile 		= sz_buf;
	ofn.lpstrFilter 	= flt;
	ofn.nFilterIndex 	= start_flt_ext+2;
    ofn.lpstrTitle      = "Open a File";
    string512 path; 
	strcpy				(path,(offset&&offset[0])?offset:P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags =
    	OFN_PATHMUSTEXIST|
    	OFN_FILEMUSTEXIST|
		OFN_HIDEREADONLY|
		OFN_FILEMUSTEXIST|
		OFN_NOCHANGEDIR|(bMulti?OFN_ALLOWMULTISELECT|OFN_EXPLORER:0);
    ofn.FlagsEx			= OFN_EX_NOPLACESBAR;
    
	bool bRes = !!GetOpenFileName( &ofn );
    if (!bRes){
	    u32 err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	Log("Too many file selected."); break;
        }
	}
    if (bRes&&bMulti){
		int cnt		= _GetItemCount(buffer,0x0);
        if (cnt>1){
            string64  	buf;
            string64  	dir;
            string4096 	fns;
            strcpy		(dir, buffer);
            strcpy		(fns,dir);
            strcat		(fns,"\\");
            strcat		(fns,_GetItem(buffer,1,buf,0x0));
            for (int i=2; i<cnt; i++){
                strcat	(fns,",");
                strcat	(fns,dir);
                strcat	(fns,"\\");
                strcat	(fns,_GetItem(buffer,i,buf,0x0));
            }
            strcpy		(buffer,fns);
        }
    }
    strlwr(buffer);
    return bRes;
}

bool EFS_Utils::GetSaveName( LPCSTR initial, string_path& buffer, LPCSTR offset, int start_flt_ext )
{
	FS_Path& P			= *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt,P.m_FilterCaption?P.m_FilterCaption:"",P.m_DefExt);
	OPENFILENAME ofn;
	Memory.mem_fill		( &ofn, 0, sizeof(ofn) );
    if (xr_strlen(buffer)){ 
        string_path		dr;
        if (!(buffer[0]=='\\' && buffer[1]=='\\')){ // if !network
            _splitpath		(buffer,dr,0,0,0);
            if (0==dr[0])	P._update(buffer,buffer); 
        }
    }
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= P.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.lpstrFilter 	= flt;
	ofn.lStructSize 	= sizeof(ofn);
	ofn.nMaxFile 		= sizeof(buffer);
	ofn.nFilterIndex 	= start_flt_ext+2;
    ofn.lpstrTitle      = "Save a File";
    string512 path; strcpy(path,(offset&&offset[0])?offset:P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags 			= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
    ofn.FlagsEx			= OFN_EX_NOPLACESBAR;

	bool bRes = !!GetSaveFileName( &ofn );
    if (!bRes){
	    u32 err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	Log("Too many file selected."); break;
        }
	}
    strlwr(buffer);
	return bRes;
}
//----------------------------------------------------
LPCSTR EFS_Utils::AppendFolderToName(LPSTR tex_name, int depth, BOOL full_name)
{
	string256 _fn;
	strcpy(tex_name,AppendFolderToName(tex_name, _fn, depth, full_name));
	return tex_name;
}

LPCSTR EFS_Utils::AppendFolderToName(LPCSTR src_name, LPSTR dest_name, int depth, BOOL full_name)
{
	shared_str tmp = src_name;
    LPCSTR s 	= src_name;
    LPSTR d 	= dest_name;
    int sv_depth= depth;
	for (; *s&&depth; s++, d++){
		if (*s=='_'){depth--; *d='\\';}else{*d=*s;}
	}
	if (full_name){
		*d			= 0;
		if (depth<sv_depth)	strcat(dest_name,*tmp);
	}else{
		for (; *s; s++, d++) *d=*s;
		*d			= 0;
	}
    return dest_name;
}

LPCSTR EFS_Utils::GenerateName(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name)
{
    int cnt = 0;
	string_path fn;
    if (base_name)	
		strconcat		(sizeof(fn), fn, base_path,base_name,def_ext);
	else 			
		sprintf_s		(fn, sizeof(fn), "%s%02d%s",base_path,cnt++,def_ext);

	while (FS.exist(fn))
	    if (base_name)	
			sprintf_s	(fn, sizeof(fn),"%s%s%02d%s",base_path,base_name,cnt++,def_ext);
        else 			
			sprintf_s	(fn, sizeof(fn), "%s%02d%s",base_path,cnt++,def_ext);
    strcpy(out_name,fn);
	return out_name;
}

//#endif
