#include "stdafx.h"
#include "lzo\lzo1x.h"
#include <mmsystem.h>

#pragma warning(disable:4995)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <malloc.h>
#pragma warning(default:4995)

#pragma comment			(lib,"xrCore.lib")

//. #define MOD_COMPRESS

typedef void DUMMY_STUFF (const void*,const u32&,void*);
XRCORE_API DUMMY_STUFF	*g_temporary_stuff;
XRCORE_API DUMMY_STUFF	*g_dummy_stuff;


#define PROTECTED_BUILD

#ifdef PROTECTED_BUILD
#	define TRIVIAL_ENCRYPTOR_ENCODER
#	define TRIVIAL_ENCRYPTOR_DECODER
#	include "../xr_3da/trivial_encryptor.h"
#	undef TRIVIAL_ENCRYPTOR_ENCODER
#	undef TRIVIAL_ENCRYPTOR_DECODER
#endif // PROTECTED_BUILD

#ifndef MOD_COMPRESS
	extern int				ProcessDifference();
#endif

BOOL					bStoreFiles = FALSE;

IWriter*				fs				= 0;
CMemoryWriter			fs_desc;

u32						bytesSRC		= 0;
u32						bytesDST		= 0;
u32						filesTOTAL		= 0;
u32						filesSKIP		= 0;
u32						filesVFS		= 0;
u32						filesALIAS		= 0;
CStatTimer				t_compress;
u8*						c_heap			= NULL;
u32						dwTimeStart		= 0;

u32						XRP_MAX_SIZE	= 1024*1024*640; // bytes

DEFINE_VECTOR(_finddata_t,FFVec,FFIt);
IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y)
{	
	return xr_strcmp(x.name,y.name)<0;	
}


struct	ALIAS
{
	LPCSTR			path;
	u32				crc;
	u32				c_ptr;
	u32				c_size_real;
	u32				c_size_compressed;
};
xr_multimap<u32,ALIAS>	aliases;

xr_vector<shared_str>	exclude_exts;

BOOL	testSKIP		(LPCSTR path)
{
	string256			p_name;
	string256			p_ext;
	_splitpath			(path, 0, 0, p_name, p_ext );

	if (0==stricmp(p_ext,".swatch"))return TRUE;
	if (0==stricmp(p_ext,".db"))	return TRUE;
	if (PatternMatch(p_ext,"*avi*"))return TRUE;
	if (0==stricmp(p_ext,".key"))	return TRUE;
	if (0==stricmp(p_ext,".tga"))	return TRUE;
	if (0==stricmp(p_ext,".txt"))	return TRUE;
	if (0==stricmp(p_ext,".smf"))	return TRUE;
	if (0==stricmp(p_ext,".uvm"))	return TRUE;
	if (0==stricmp(p_ext,".raw"))	return TRUE;
	if (0==stricmp(p_name,"build"))	return TRUE;
	if ('~'==p_ext[1])				return TRUE;
	if ('_'==p_ext[1])				return TRUE;

	for (xr_vector<shared_str>::iterator it=exclude_exts.begin(); it!=exclude_exts.end(); it++)
		if (PatternMatch(p_ext,it->c_str()))return TRUE;

	return FALSE;
}

BOOL	testVFS			(LPCSTR path)
{
	if (bStoreFiles)
		return			(TRUE);

	string256			p_ext;
	_splitpath			(path,0,0,0,p_ext);

	if (!stricmp(p_ext,".xml"))
		return			(FALSE);

	if (!stricmp(p_ext,".ltx"))
		return			(FALSE);

	if (!stricmp(p_ext,".script"))
		return			(FALSE);

	return				(TRUE);
	/**
	// if (0==stricmp(p_name,"level") && (0==stricmp(p_ext,".") || 0==p_ext[0]) )	return TRUE;	// level.
	if (0==stricmp(p_name,"level") && 0==stricmp(p_ext,".ai"))					return TRUE;
	if (0==stricmp(p_name,"level") && 0==stricmp(p_ext,".gct"))					return TRUE;
	if (0==stricmp(p_name,"level") && 0==stricmp(p_ext,".details"))				return TRUE;
	if (0==stricmp(p_ext,".ogg"))												return TRUE;
	if (0==stricmp(p_name,"game") && 0==stricmp(p_ext,".graph"))				return TRUE;
	if (0==stricmp(p_name,"game") && 0==stricmp(p_ext,".graph"))				return TRUE;
	return				(FALSE);
	/**/
}

BOOL	testEqual		(LPCSTR path, IReader* base)
{
	IReader*	test	= FS.r_open	(path);
	if (test->length() != base->length())
	{
		return FALSE;
	}
	return 0==memcmp(test->pointer(),base->pointer(),base->length());
}

ALIAS*	testALIAS		(IReader* base, u32 crc, u32& a_tests)
{
	xr_multimap<u32,ALIAS>::iterator I = aliases.lower_bound(base->length());

	while (I!=aliases.end() && (I->first==base->length()))
	{
		if (I->second.crc == crc)
		{
			a_tests	++;
			if (testEqual(I->second.path,base))	
			{
				return	&I->second;
			}
		}
		I++;
	}
	return 0;
}

IC	void write_file_header	(LPCSTR file_name, const u32 &crc, const u32 &ptr, const u32 &size_real, const u32 &size_compressed)
{
#ifndef PROTECTED_BUILD
	fs_desc.w_stringZ	(file_name);
	fs_desc.w_u32		(crc);	// crc
	fs_desc.w_u32		(ptr);
	fs_desc.w_u32		(size_real);
	fs_desc.w_u32		(size_compressed);
#else // PROTECTED_BUILD
	u32					file_name_size = (xr_strlen(file_name) + 0)*sizeof(char);
	u32					buffer_size = file_name_size + 4*sizeof(u32);
	VERIFY				(buffer_size <= 65535);
	u32					full_buffer_size = buffer_size + sizeof(u16);
	u8					*buffer = (u8*)_alloca(full_buffer_size);
	u8					*buffer_start = buffer;
	*(u16*)buffer		= (u16)buffer_size;
	buffer				+= sizeof(u16);

	*(u32*)buffer		= size_real;
	buffer				+= sizeof(u32);

	*(u32*)buffer		= size_compressed;
	buffer				+= sizeof(u32);

	*(u32*)buffer		= crc;
	buffer				+= sizeof(u32);

	Memory.mem_copy		(buffer,file_name,file_name_size);
	buffer				+= file_name_size;

	*(u32*)buffer		= ptr;

//	trivial_encryptor::encode	(buffer_start,full_buffer_size,buffer_start);

	fs_desc.w			(buffer_start,full_buffer_size);
#endif // PROTECTED_BUILD
}

void	Compress			(LPCSTR path, LPCSTR base, BOOL bFast)
{
	filesTOTAL		++;

	if (testSKIP(path))	
	{
		filesSKIP	++;
		printf		(" - a SKIP");
		Msg			("%-80s   - SKIP",path);
		return;
	}

	string_path		fn;				
	strconcat		(sizeof(fn),fn,base,"\\",path);

	if (::GetFileAttributes(fn)==u32(-1))
	{
		filesSKIP	++;
		printf		(" - CAN'T OPEN");
		Msg			("%-80s   - CAN'T OPEN",path);
		return;
	}

	IReader*		src				=	FS.r_open	(fn);
	if (0==src)
	{
		filesSKIP	++;
		printf		(" - CAN'T OPEN");
		Msg			("%-80s   - CAN'T OPEN",path);
		return;
	}
	bytesSRC						+=	src->length	();
	u32			c_crc32				=	crc32		(src->pointer(),src->length());
	u32			c_ptr				=	0;
	u32			c_size_real			=	0;
	u32			c_size_compressed	=	0;
	u32			a_tests				=	0;

	ALIAS*		A					=	testALIAS	(src,c_crc32,a_tests);
	printf							("%3da ",a_tests);
	if (A) 
	{
		filesALIAS			++;
		printf				("ALIAS");
		Msg					("%-80s   - ALIAS (%s)",path,A->path);

		// Alias found
		c_ptr				= A->c_ptr;
		c_size_real			= A->c_size_real;
		c_size_compressed	= A->c_size_compressed;
	} else 
	{
		if (testVFS(path))	
		{
			filesVFS			++;

			// Write into BaseFS
			c_ptr				= fs->tell	();
			c_size_real			= src->length();
			c_size_compressed	= src->length();
			fs->w				(src->pointer(),c_size_real);
			printf				("VFS");
			Msg					("%-80s   - VFS",path);
		} else 
		{
			// Compress into BaseFS
			c_ptr				=	fs->tell();
			c_size_real			=	src->length();
			if (0!=c_size_real)
			{
				u32 c_size_max		=	rtc_csize		(src->length());
				u8*	c_data			=	xr_alloc<u8>	(c_size_max);
				t_compress.Begin	();
				{
					// c_size_compressed	=	rtc_compress	(c_data,c_size_max,src->pointer(),c_size_real);
					c_size_compressed	= c_size_max;
					if (bFast){		
						R_ASSERT(LZO_E_OK == lzo1x_1_compress	((u8*)src->pointer(),c_size_real,c_data,&c_size_compressed,c_heap));
					}else{
						R_ASSERT(LZO_E_OK == lzo1x_999_compress	((u8*)src->pointer(),c_size_real,c_data,&c_size_compressed,c_heap));
					}
				}
				t_compress.End		();

				if ((c_size_compressed+16) >= c_size_real)
				{
					// Failed to compress - revert to VFS
					filesVFS			++;
					c_size_compressed	= c_size_real;
					fs->w				(src->pointer(),c_size_real);
					printf				("VFS (R)");
					Msg					("%-80s   - VFS (R)",path);
				} else 
				{
					// Compressed OK - optimize
					if (!bFast){
						u8*		c_out	= xr_alloc<u8>	(c_size_real);
						u32		c_orig	= c_size_real;
						R_ASSERT		(LZO_E_OK	== lzo1x_optimize	(c_data,c_size_compressed,c_out,&c_orig, NULL));
						R_ASSERT		(c_orig		== c_size_real		);
						xr_free			(c_out);
					}
					fs->w				(c_data,c_size_compressed);
					printf				("%3.1f%%",	100.f*float(c_size_compressed)/float(src->length()));
					Msg					("%-80s   - OK (%3.1f%%)",path,100.f*float(c_size_compressed)/float(src->length()));
				}

				// cleanup
				xr_free		(c_data);
			}else
			{
				filesVFS				++;
				c_size_compressed		= c_size_real;
				//				fs->w					(src->pointer(),c_size_real);
				printf					("VFS (R)");
				Msg						("%-80s   - EMPTY",path);
			}
		}
	}

	// Write description
	write_file_header		(path,c_crc32,c_ptr,c_size_real,c_size_compressed);

	if (0==A)	
	{
		// Register for future aliasing
		ALIAS				R;
		R.path				= xr_strdup	(fn);
		R.crc				= c_crc32;
		R.c_ptr				= c_ptr;
		R.c_size_real		= c_size_real;
		R.c_size_compressed	= c_size_compressed;
		aliases.insert		(mk_pair(R.c_size_real,R));
	}

	FS.r_close	(src);
}

void	OpenPack			(LPCSTR tgt_folder, int num)
{
	VERIFY			(0==fs);

	string_path		fname;
	string128		s_num;
#ifdef MOD_COMPRESS
	strconcat		(sizeof(fname),fname,tgt_folder,".xdb",itoa(num,s_num,10));
#else
	strconcat		(sizeof(fname),fname,tgt_folder,".pack_#",itoa(num,s_num,10));
#endif
	unlink			(fname);
	fs				= FS.w_open	(fname);
	fs_desc.clear	();
	aliases.clear	();

	bytesSRC		= 0;
	bytesDST		= 0;
	filesTOTAL		= 0;
	filesSKIP		= 0;
	filesVFS		= 0;
	filesALIAS		= 0;

	dwTimeStart		= timeGetTime();
	fs->open_chunk	(0);
}

void	ClosePack			()
{
	fs->close_chunk	(); 
	// save list
	bytesDST		= fs->tell	();
	Log				("...Writing pack desc");
#ifdef MOD_COMPRESS
	DUMMY_STUFF*		_dummy_stuff_tmp;
	_dummy_stuff_tmp	= g_dummy_stuff;
	g_dummy_stuff		 = NULL;
#endif
	fs->w_chunk		(1|CFS_CompressMark, fs_desc.pointer(),fs_desc.size());
#ifdef MOD_COMPRESS
	g_dummy_stuff	= _dummy_stuff_tmp;
#endif

	Msg				("Data size: %d. Desc size: %d.",bytesDST,fs_desc.size());
	FS.w_close		(fs);
	Log				("Pack saved.");
	u32	dwTimeEnd	= timeGetTime();
	printf			("\n\nFiles total/skipped/VFS/aliased: %d/%d/%d/%d\nOveral: %dK/%dK, %3.1f%%\nElapsed time: %d:%d\nCompression speed: %3.1f Mb/s",
		filesTOTAL,filesSKIP,filesVFS,filesALIAS,
		bytesDST/1024,bytesSRC/1024,
		100.f*float(bytesDST)/float(bytesSRC),
		((dwTimeEnd-dwTimeStart)/1000)/60,
		((dwTimeEnd-dwTimeStart)/1000)%60,
		float((float(bytesDST)/float(1024*1024))/(t_compress.GetElapsed_sec()))
		);
	Msg			("\n\nFiles total/skipped/VFS/aliased: %d/%d/%d/%d\nOveral: %dK/%dK, %3.1f%%\nElapsed time: %d:%d\nCompression speed: %3.1f Mb/s\n\n",
		filesTOTAL,filesSKIP,filesVFS,filesALIAS,
		bytesDST/1024,bytesSRC/1024,
		100.f*float(bytesDST)/float(bytesSRC),
		((dwTimeEnd-dwTimeStart)/1000)/60,
		((dwTimeEnd-dwTimeStart)/1000)%60,
		float((float(bytesDST)/float(1024*1024))/(t_compress.GetElapsed_sec()))
		);
}

void CompressList(LPCSTR in_name, xr_vector<char*>* list, xr_vector<char*>* fl_list, BOOL bFast, BOOL make_pack, LPCSTR copy_path)
{
	if (!list->empty() && in_name && in_name[0]){
		string256		caption;

		VERIFY			('\\'!=in_name[xr_strlen(in_name)-1]);
		string_path		tgt_folder;
		_splitpath		(in_name,0,0,tgt_folder,0);

		int pack_num	= 0;
		if (make_pack)
			OpenPack	(tgt_folder,pack_num++);

		for (u32 it=0; it<fl_list->size(); it++)
			write_file_header	((*fl_list)[it],0,0,0,0);

		c_heap			= xr_alloc<u8> (LZO1X_999_MEM_COMPRESS);
		//***main process***: BEGIN
		for (u32 it=0; it<list->size(); it++){
			sprintf				(caption,"Compress files: %d/%d - %d%%",it,list->size(),(it*100)/list->size());
			SetWindowText		(GetConsoleWindow(),caption);
			printf				("\n%-80s   ",(*list)[it]);
			if (make_pack){
				if (fs->tell()>XRP_MAX_SIZE){
					ClosePack	();
					OpenPack	(tgt_folder,pack_num++);
				}
				Compress		((*list)[it],in_name,bFast);
			}
			if (copy_path && copy_path[0]){
				string_path		src_fn, dst_fn; 
				strconcat		(sizeof(src_fn),src_fn,in_name,"\\",(*list)[it]);
				strconcat		(sizeof(dst_fn),dst_fn,copy_path,tgt_folder,"\\",(*list)[it]);
				printf			(" + COPY");
				int age			= FS.get_file_age(src_fn);
				FS.file_copy	(src_fn,dst_fn);
				FS.set_file_age	(dst_fn,age);
			}
		}
		if (make_pack)
			ClosePack			();

		xr_free					(c_heap);
		//***main process***: END
	} else {
		Msg						("ERROR: folder not found.");
	}
}

void ProcessNormal(LPCSTR tgt_name, BOOL bFast)
{
	// collect files
	xr_vector<char*>*	list	= FS.file_list_open	("$target_folder$",FS_ListFiles);
	R_ASSERT2			(list,	"Unable to open folder!!!");
	// collect folders
	xr_vector<char*>*	fl_list	= FS.file_list_open	("$target_folder$",FS_ListFolders);
	R_ASSERT2			(fl_list,	"Unable to open folder!!!");
	// compress
	CompressList		(tgt_name,list,fl_list,bFast,TRUE,0);
	// free lists
	FS.file_list_close	(fl_list);
	FS.file_list_close	(list);
}

#define OUT_LOG(s,p0){printf(s,p0); printf("\n"); Msg(s,p0);}

void ProcessFolder(xr_vector<char*>& list, LPCSTR path)
{
	xr_vector<char*>*	i_list	= FS.file_list_open	("$target_folder$",path,FS_ListFiles|FS_RootOnly);
	if (!i_list){
		Log				("ERROR: Unable to open file list:", path);
		return;
	}
	xr_vector<char*>::iterator it	= i_list->begin();
	xr_vector<char*>::iterator itE	= i_list->end();
	for (;it!=itE;++it){ 
		xr_string		tmp_path	= xr_string(path)+xr_string(*it);
		if (!testSKIP(tmp_path.c_str())){
			list.push_back	(xr_strdup(tmp_path.c_str()));
//.			Msg				("+f: %s",tmp_path.c_str());
		}else{
			Msg				("-f: %s",tmp_path.c_str());
		}
	}
	FS.file_list_close	(i_list);
}

bool IsFolderAccepted(CInifile& ltx, LPCSTR path, BOOL& recurse)
{
	// exclude folders
	if( ltx.section_exist("exclude_folders") )
	{
		CInifile::Sect& ef_sect	= ltx.r_section("exclude_folders");
		for (CInifile::SectCIt ef_it=ef_sect.Data.begin(); ef_it!=ef_sect.Data.end(); ef_it++){
			recurse	= CInifile::IsBOOL(ef_it->second.c_str());
			if (recurse)	{
				if (path==strstr(path,ef_it->first.c_str()))	return false;
			}else{
				if (0==xr_strcmp(path,ef_it->first.c_str()))	return false;
			}
		}
	}
	return true;
}

void ProcessLTX(LPCSTR tgt_name, LPCSTR params, BOOL bFast)
{
	xr_string		ltx_name;
	LPCSTR ltx_nm	= strstr(params,".ltx");				VERIFY(ltx_nm!=0);
	string_path		ltx_fn;
	string_path		tmp;
	strncpy			(tmp,params,ltx_nm-params); tmp[ltx_nm-params]=0;
	_Trim			(tmp);
	strcat			(tmp,".ltx");
	strcpy			(ltx_fn,tmp);

	// append ltx path (if exist)
	string_path		fn,dr,di;
	_splitpath		(ltx_fn,dr,di,0,0);
	strconcat		(sizeof(fn),fn,dr,di);  
	if (0!=fn[0])
		FS.append_path	("ltx_path",fn,0,false);
	
	if (!FS.exist(ltx_fn)&&!FS.exist(ltx_fn,"$app_root$",tmp)) 
		Debug.fatal	(DEBUG_INFO,"ERROR: Can't find ltx file: '%s'",ltx_fn);

	CInifile ltx	(ltx_fn);
	printf			("Processing LTX...\n");

	BOOL make_pack	= ltx.line_exist("options","make_pack") ? ltx.r_bool("options","make_pack") : true;
	LPCSTR copy_path= ltx.line_exist("options","copy_path") ? ltx.r_string("options","copy_path") : 0;
	if (ltx.line_exist("options","exclude_exts"))
		_SequenceToList(exclude_exts,ltx.r_string("options","exclude_exts"));

	xr_vector<char*> list;
	xr_vector<char*> fl_list;
	if(ltx.section_exist("include_folders"))
	{
	CInifile::Sect& if_sect	= ltx.r_section("include_folders");
	for (CInifile::SectCIt if_it=if_sect.Data.begin(); if_it!=if_sect.Data.end(); if_it++)
	{
		BOOL ifRecurse		= CInifile::IsBOOL(if_it->second.c_str());
		u32 folder_mask		= FS_ListFolders | (ifRecurse?0:FS_RootOnly);

		string_path path;
		LPCSTR _path		= 0==xr_strcmp(if_it->first.c_str(),".\\")?"":if_it->first.c_str();
		strcpy				(path,_path);
		u32 path_len		= xr_strlen(path);
		if ((0!=path_len)&&(path[path_len-1]!='\\')) strcat(path,"\\");

		Log					("");
		OUT_LOG				("Processing folder: '%s'",path);
		BOOL efRecurse;
		BOOL val			= IsFolderAccepted(ltx,path,efRecurse);
		if (val || (!val&&!efRecurse))
		{ 
			if (val)		ProcessFolder	(list,path);

			xr_vector<char*>*	i_fl_list	= FS.file_list_open	("$target_folder$",path,folder_mask);
			if (!i_fl_list)
			{
				Log			("ERROR: Unable to open folder list:", path);
				continue;
			}

			xr_vector<char*>::iterator it	= i_fl_list->begin();
			xr_vector<char*>::iterator itE	= i_fl_list->end();
			for (;it!=itE;++it){ 
				xr_string tmp_path	= xr_string(path)+xr_string(*it);
				bool val		= IsFolderAccepted(ltx,tmp_path.c_str(),efRecurse);
				if (val)
				{
					fl_list.push_back(xr_strdup(tmp_path.c_str()));
					Msg			("+F: %s",tmp_path.c_str());
					// collect files
					if (ifRecurse) 
						ProcessFolder (list,tmp_path.c_str());
				}else
				{
					Msg			("-F: %s",tmp_path.c_str());
				}
			}
			FS.file_list_close	(i_fl_list);
		}else
		{
			Msg					("-F: %s",path);
		}
	}
}//if(ltx.section_exist("include_folders"))
	// compress
	{
		if(ltx.section_exist("include_files"))
		{
		CInifile::Sect& if_sect	= ltx.r_section("include_files");
		for (CInifile::SectCIt if_it=if_sect.Data.begin(); if_it!=if_sect.Data.end(); if_it++)
			{
				  list.push_back	(xr_strdup(if_it->first.c_str()));
			}	
		}
	
	}
	CompressList	(tgt_name,&list,&fl_list,bFast,make_pack,copy_path);

	// free
	xr_vector<char*>::iterator it	= list.begin();
	xr_vector<char*>::iterator itE	= list.end();
	for (;it!=itE;++it) xr_free(*it);
	it				= fl_list.begin();
	itE				= fl_list.end();
	for (;it!=itE;++it) xr_free(*it);

	exclude_exts.clear_and_free();
}


int __cdecl main	(int argc, char* argv[])
{
	g_temporary_stuff	= &trivial_encryptor::decode;
	g_dummy_stuff		= &trivial_encryptor::encode;

	Core._initialize("xrCompress",0,FALSE);
	printf			("\n\n");

	LPCSTR params = GetCommandLine();

#ifndef MOD_COMPRESS
	if(strstr(params,"-store"))
	{
		bStoreFiles = TRUE;
	};
	{
		LPCSTR					temp = strstr(params,"-max_size");
		if (temp) {
			u64					test = u64(1024*1024)*u64(atoi(temp+9));
			if (u64(test) >= u64(u32(1) << 31))
				printf			("! too large max_size (%I64u), restoring previous (%I64u)\n",test,u64(XRP_MAX_SIZE));
			else
				XRP_MAX_SIZE	= u32(test);
		};
	}
#else
	bStoreFiles = TRUE;
#endif
#ifndef MOD_COMPRESS
	if(strstr(params,"-diff"))
	{
		ProcessDifference	();
	}else
#endif
	{
		#ifndef MOD_COMPRESS
		if (argc<2)	
		{
			printf("ERROR: u must pass folder name as parameter.\n");
			printf("-diff /? option to get information about creating difference.\n");
			printf("-fast	- fast compression.\n");
			printf("-store	- store files. No compression.\n");
			printf("-ltx <file_name.ltx> - pathes to compress.\n");
			printf("\n");
			printf("LTX format:\n");
			printf("	[config]\n");
			printf("	;<path>     = <recurse>\n");
			printf("	.\\         = false\n");
			printf("	textures    = true\n");
			
			Core._destroy();
			return 3;
		}
		#endif

		string_path		folder;		
		strconcat		(sizeof(folder),folder,argv[1],"\\");
		_strlwr_s		(folder,sizeof(folder));
		printf			("\nCompressing files (%s)...\n\n",folder);

		FS._initialize	(CLocatorAPI::flTargetFolderOnly|CLocatorAPI::flScanAppRoot,folder);

		BOOL bFast		= 0!=strstr(params,"-fast");

		LPCSTR p		= strstr(params,"-ltx");
#ifndef MOD_COMPRESS
		if(0!=p)
		{
			ProcessLTX		(argv[1],p+4,bFast);
		}else{
			ProcessNormal	(argv[1],bFast);
		}
#else
		R_ASSERT2		(p, "wrong params passed. -ltx option needed");
		ProcessLTX		(argv[1],p+4,bFast);
#endif
	}

	Core._destroy		();
	return 0;
}
