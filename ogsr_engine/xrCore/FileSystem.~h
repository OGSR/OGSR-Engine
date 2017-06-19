//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

#define BACKUP_FILE_LEVEL 5

class XRCORE_API EFS_Utils {
	DEFINE_MAP	(xr_string,void*,HANDLEMap,HANDLEPairIt);

    HANDLEMap 	m_LockFiles;
public:
				EFS_Utils		();
	virtual 	~EFS_Utils		();
	void 		_initialize		(){}
    void 		_destroy		(){}

	LPCSTR		GenerateName	(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name);

	bool 		GetOpenName		(LPCSTR initial, string_path& buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetOpenName		(LPCSTR initial, xr_string& buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );

	bool 		GetSaveName		(LPCSTR initial, string_path& buffer, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(LPCSTR initial, xr_string& buf, LPCSTR offset=0, int start_flt_ext=-1 );

	void 		MarkFile		(LPCSTR fn, bool bDeleteSource);

	xr_string 	AppendFolderToName(xr_string& tex_name, int depth, BOOL full_name);

	LPCSTR		AppendFolderToName(LPSTR tex_name, int depth, BOOL full_name);
	LPCSTR		AppendFolderToName(LPCSTR src_name, LPSTR dest_name, int depth, BOOL full_name);

	BOOL		LockFile		(LPCSTR fn, bool bLog=true);
	BOOL		UnlockFile		(LPCSTR fn, bool bLog=true);
	BOOL		CheckLocking	(LPCSTR fn, bool bOnlySelf, bool bMsg);//, shared_str* owner=0);
	void 		WriteAccessLog	(LPCSTR fn, LPCSTR start_msg);
//.	shared_str 	GetLockOwner	(LPCSTR initial, LPCSTR fn);

    xr_string	ChangeFileExt	(LPCSTR src, LPCSTR ext);
    xr_string	ChangeFileExt	(const xr_string& src, LPCSTR ext);

    xr_string	ExtractFileName		(LPCSTR src);
    xr_string	ExtractFilePath		(LPCSTR src);
    xr_string	ExtractFileExt		(LPCSTR src);
    xr_string	ExcludeBasePath		(LPCSTR full_path, LPCSTR excl_path);
};
extern XRCORE_API	EFS_Utils*	xr_EFS;
#define EFS (*xr_EFS)

#endif /*_INCDEF_FileSystem_H_*/

