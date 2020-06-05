//********************************************************************************//
// OGSR Engine x64 Compressor
// KRodin (c) 2018
// Используется некоторый код из xrOxygen
//********************************************************************************//

#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <iterator>

#include "lzo\lzo1x.h"

#define TRIVIAL_ENCRYPTOR_ENCODER
#define TRIVIAL_ENCRYPTOR_DECODER
#include "trivial_encryptor.h"
#undef TRIVIAL_ENCRYPTOR_ENCODER
#undef TRIVIAL_ENCRYPTOR_DECODER


constexpr u32 XRP_MAX_SIZE_DEF = 1900; // Дефолтный максимальный размер создаваемого архива в МБ. Более ~1900 выставлять не рекомендую, т.к. архивы более 2гб двиг не поддерживает.

static const std::vector<const char*> NoCompress = { //Расширения файлов, которые нельзя сжимать.
	".geom", ".geomx", ".ogm"
};


bool bStoreFiles = false;
bool MOD_COMPRESS = false;

IWriter* fs = nullptr;
CMemoryWriter fs_desc;

u32 bytesSRC = 0;
u32 bytesDST = 0;
u32	filesTOTAL = 0;
u32	filesSKIP = 0;
u32	filesVFS = 0;
u32	filesALIAS = 0;
CTimer t_compress;
u8* c_heap = nullptr;

u32 XRP_MAX_SIZE = 1024 * 1024 * XRP_MAX_SIZE_DEF;

struct	ALIAS {
	LPCSTR			path;
	u32				crc;
	size_t			c_ptr;
	u32				c_size_real;
	u32				c_size_compressed;
};
xr_multimap<u32, ALIAS>	aliases;

std::vector<std::string> exclude_exts;


bool testSKIP(LPCSTR path)
{
	string_path p_name, p_ext;
	_splitpath(path, 0, 0, p_name, p_ext);

	if (!stricmp(p_name, "build"))
		return true;

	for (const auto& it : exclude_exts)
		if (PatternMatch(p_ext, it.c_str()))
			return true;

	return false;
}

bool testVFS(LPCSTR path)
{
	if (bStoreFiles)
		return true;

	string_path p_ext;
	_splitpath(path, 0, 0, 0, p_ext);

	for (const char* it : NoCompress)
		if (!stricmp(p_ext, it))
			return true;

	return false;
}

bool testEqual(LPCSTR path, IReader* base)
{
	bool res = false;

	IReader* test = FS.r_open(path);

	if (test->length() == base->length())
		if (0 == memcmp(test->pointer(), base->pointer(), base->length()))
			res = true;

	FS.r_close(test);

	return res;
}

ALIAS* testALIAS(IReader* base, u32 crc, u32& a_tests)
{
	auto I = aliases.lower_bound(base->length());

	while (I != aliases.end() && (I->first == base->length()))
	{
		if (I->second.crc == crc)
		{
			a_tests++;
			if (testEqual(I->second.path, base))
				return	&I->second;
		}
		I++;
	}
	return nullptr;
}


IC void write_file_header(LPCSTR file_name, const u32 &crc, const u32 &ptr, const u32 &size_real, const u32 &size_compressed)
{
	u32 file_name_size = (xr_strlen(file_name) + 0) * sizeof(char);
	u32 buffer_size = file_name_size + 4 * sizeof(u32);
	VERIFY(buffer_size <= 65535);
	u32 full_buffer_size = buffer_size + sizeof(u16);
	u8  *buffer = (u8*)_alloca(full_buffer_size);
	u8  *buffer_start = buffer;
	*(u16*)buffer = (u16)buffer_size;
	buffer += sizeof(u16);

	*(u32*)buffer = size_real;
	buffer += sizeof(u32);

	*(u32*)buffer = size_compressed;
	buffer += sizeof(u32);

	*(u32*)buffer = crc;
	buffer += sizeof(u32);

	Memory.mem_copy(buffer, file_name, file_name_size);
	buffer += file_name_size;

	*(u32*)buffer = ptr;

	fs_desc.w(buffer_start, full_buffer_size);
}

void Compress(LPCSTR path, LPCSTR base, BOOL bFast)
{
	filesTOTAL++;

	if (testSKIP(path))
	{
		filesSKIP++;
		Msg("!![%s] - SKIP", path);
		return;
	}

	string_path fn;
	strconcat(sizeof(fn), fn, base, "\\", path);

	if (::GetFileAttributes(fn) == u32(-1))
	{
		filesSKIP++;
		Msg("!![%s] - CAN'T OPEN", path);
		return;
	}

	IReader* src = FS.r_open(fn);
	if (0 == src)
	{
		filesSKIP++;
		Msg("!![%s] - CAN'T OPEN", path);
		return;
	}
	bytesSRC += src->length();

	u32 c_crc32 = crc32(src->pointer(), src->length());
	//Msg("CRC32 for [%s] pointer: [%x], length: [%d] is: [%u]", fn, src->pointer(), src->length(), c_crc32);

	size_t c_ptr = 0;
	u32 c_size_real = 0;
	u32 c_size_compressed = 0;
	u32 a_tests = 0;

	ALIAS* A = testALIAS(src, c_crc32, a_tests);

	if (A)
	{
		filesALIAS++;
		Msg("~~[%s] - ALIAS (%s)", path, A->path);

		// Alias found
		c_ptr = A->c_ptr;
		c_size_real = A->c_size_real;
		c_size_compressed = A->c_size_compressed;
	}
	else
	{
		if (testVFS(path))
		{
			filesVFS++;

			// Write into BaseFS
			c_ptr = fs->tell();
			c_size_real = src->length();
			c_size_compressed = src->length();
			fs->w(src->pointer(), c_size_real);
			Msg("~~[%s] - ALIAS VFS", path);
		}
		else
		{
			// Compress into BaseFS
			c_ptr = fs->tell();
			c_size_real = src->length();
			if (0 != c_size_real)
			{
				u32 c_size_max = rtc_csize(src->length());
				u8*	c_data = xr_alloc<u8>(c_size_max);
				c_size_compressed = c_size_max;

				if (bFast)
					R_ASSERT(LZO_E_OK == lzo1x_1_compress(reinterpret_cast<const lzo_bytep>(src->pointer()), c_size_real, c_data, reinterpret_cast<lzo_uintp>(&c_size_compressed), c_heap));
				else
					R_ASSERT(LZO_E_OK == lzo1x_999_compress(reinterpret_cast<const lzo_bytep>(src->pointer()), c_size_real, c_data, reinterpret_cast<lzo_uintp>(&c_size_compressed), c_heap));

				if ((c_size_compressed + 16) >= c_size_real)
				{
					// Failed to compress - revert to VFS
					filesVFS++;
					c_size_compressed = c_size_real;
					fs->w(src->pointer(), c_size_real);
					Msg("~~[%s] - VFS (R)", path);
				}
				else
				{
					// Compressed OK - optimize
					if (!bFast) {
						u8* c_out = xr_alloc<u8>(c_size_real);
						u32 c_orig = c_size_real;
						R_ASSERT(LZO_E_OK == lzo1x_optimize(c_data, c_size_compressed, c_out, reinterpret_cast<lzo_uintp>(&c_orig), nullptr));
						R_ASSERT(c_orig == c_size_real);
						xr_free(c_out);
					}
					fs->w(c_data, c_size_compressed);
					Msg("--[%s] - OK (%3.1f%%)", path, 100.f*float(c_size_compressed) / float(src->length()));
				}

				// cleanup
				xr_free(c_data);
			}
			else
			{
				filesVFS++;
				c_size_compressed = c_size_real;
				Msg("!![%s] - EMPTY FILE", path);
			}
		}
	}

	// Write description
	write_file_header(path, c_crc32, (u32)c_ptr, c_size_real, c_size_compressed);

	if (0 == A)
	{
		// Register for future aliasing
		ALIAS				R;
		R.path = xr_strdup(fn);
		R.crc = c_crc32;
		R.c_ptr = c_ptr;
		R.c_size_real = c_size_real;
		R.c_size_compressed = c_size_compressed;
		aliases.insert(mk_pair(R.c_size_real, R));
	}

	FS.r_close(src);
}

void OpenPack(LPCSTR tgt_folder, int num)
{
	t_compress.Start();

	VERIFY(0 == fs);

	string_path fname;
	const char* ext = MOD_COMPRESS ? ".xdb" : ".db_pack_#";
	strconcat(sizeof(fname), fname, "!Ready\\", tgt_folder, ext, std::to_string(num).c_str());

	unlink(fname);
	fs = FS.w_open(fname);
	fs_desc.clear();

	bytesSRC = 0;
	bytesDST = 0;
	filesTOTAL = 0;
	filesSKIP = 0;
	filesVFS = 0;
	filesALIAS = 0;

	fs->open_chunk(0);
}

void ClosePack()
{
	fs->close_chunk();
	// save list
	bytesDST = fs->tell();

	DUMMY_STUFF* _dummy_stuff_tmp;
	if (MOD_COMPRESS) {
		_dummy_stuff_tmp = g_dummy_stuff;
		g_dummy_stuff = nullptr;
	}

	fs->w_chunk(1 | CFS_CompressMark, fs_desc.pointer(), fs_desc.size());

	if (MOD_COMPRESS)
		g_dummy_stuff = _dummy_stuff_tmp;

	FS.w_close(fs);

	Log("\n");
	Msg("Files total/skipped/VFS/aliased: [%d/%d/%d/%d]\nOveral: [%dKB / %dKB], [%3.1f%%]\nElapsed time: [%d:%d]\nCompression speed: [%3.1f Mb/s]",
		filesTOTAL, filesSKIP, filesVFS, filesALIAS,
		bytesDST / 1024, bytesSRC / 1024,
		100.f*float(bytesDST) / float(bytesSRC),
		(t_compress.GetElapsed_ms() / 1000) / 60,
		(t_compress.GetElapsed_ms() / 1000) % 60,
		float((float(bytesDST) / float(1024 * 1024)) / (t_compress.GetElapsed_sec()))
	);
	Log("\n");

	for (auto& it : aliases)
		xr_free(it.second.path);
	aliases.clear();
}

void CompressList(LPCSTR in_name, xr_vector<char*>* list, xr_vector<char*>* fl_list, BOOL bFast)
{
	if (!list->empty() && in_name && in_name[0]) {
		string256 caption;

		VERIFY('\\' != in_name[xr_strlen(in_name) - 1]);
		string_path tgt_folder;
		_splitpath(in_name, 0, 0, tgt_folder, 0);

		int pack_num = 0;
		OpenPack(tgt_folder, pack_num++);

		for (char* it : *fl_list)
			write_file_header(it, 0, 0, 0, 0);

		c_heap = xr_alloc<u8>(LZO1X_999_MEM_COMPRESS);
		//***main process***: BEGIN
		u32 count = 0;
		for (char* it : *list) {
			count++;
			sprintf(caption, "Compress files: %d/%d - %d%%", count, list->size(), (count * 100) / list->size());
			SetWindowText(GetConsoleWindow(), caption);
			if (fs->tell() > XRP_MAX_SIZE) {
				ClosePack();
				OpenPack(tgt_folder, pack_num++);
			}
			Compress(it, in_name, bFast);
		}

		ClosePack();

		xr_free(c_heap);
		//***main process***: END
	}
	else {
		Msg("!!ERROR: folder not found.");
	}
}


void ProcessFolder(xr_vector<char*>& list, LPCSTR path)
{
	auto i_list = FS.file_list_open("$target_folder$", path, FS_ListFiles | FS_RootOnly);
	if (!i_list) {
		Msg("!!ERROR: Unable to open file list: [%s]", path);
		return;
	}

	for (char* it : *i_list) {
		string_path buf;
		strconcat(sizeof(buf), buf, path, it);
		list.push_back(xr_strdup(buf));
	}
	FS.file_list_close(i_list);
}

bool IsFolderAccepted(CInifile& ltx, LPCSTR path, BOOL& recurse)
{
	// exclude folders
	if (ltx.section_exist("exclude_folders"))
	{
		const auto& ef_sect = ltx.r_section("exclude_folders");
		for (const auto&[key, value] : ef_sect.Data)
		{
			recurse = CInifile::IsBOOL(value.c_str());
			if (recurse) {
				if (path == strstr(path, key.c_str()))
					return false;
			}
			else {
				if (0 == xr_strcmp(path, key.c_str()))
					return false;
			}
		}
	}
	return true;
}

void ProcessLTX(LPCSTR tgt_name, LPCSTR params, BOOL bFast)
{
	LPCSTR ltx_nm = strstr(params, ".ltx");
	VERIFY(ltx_nm != 0);
	string_path ltx_fn, tmp;
	strncpy_s(tmp, params, ltx_nm - params); tmp[ltx_nm - params] = 0;
	_Trim(tmp);
	strcat_s(tmp, ".ltx");
	strcpy_s(ltx_fn, tmp);
	_strlwr_s(ltx_fn, sizeof(ltx_fn));

	// append ltx path (if exist)
	string_path fn, dr, di;
	_splitpath(ltx_fn, dr, di, 0, 0);
	strconcat(sizeof(fn), fn, dr, di);
	if (0 != fn[0])
		FS.append_path("ltx_path", fn, 0, false);

	ASSERT_FMT(FS.exist(ltx_fn) && FS.exist(ltx_fn, "$app_root$", tmp), "ERROR: Can't find ltx file: [%s]", ltx_fn);

	CInifile ltx(ltx_fn);
	Msg("Processing LTX [%s]...", ltx_fn);
	Log("\n");

	if (ltx.line_exist("options", "exclude_exts"))
	{
		std::string input(ltx.r_string("options", "exclude_exts"));
		std::replace(input.begin(), input.end(), ',', ' ');
		std::istringstream iss(input);
		using it = std::istream_iterator<std::string>;
		std::copy(it(iss), it(), std::back_inserter(exclude_exts));
		//for (const auto& str : exclude_exts)
		//	Msg("--Found exclude: [%s]", str.c_str());
	}

	xr_vector<char*> list;
	xr_vector<char*> fl_list;
	if (ltx.section_exist("include_folders"))
	{
		const auto& if_sect = ltx.r_section("include_folders");
		for (const auto&[key, value] : if_sect.Data)
		{
			BOOL ifRecurse = CInifile::IsBOOL(value.c_str());
			u32 folder_mask = FS_ListFolders | (ifRecurse ? 0 : FS_RootOnly);

			string_path path;
			LPCSTR _path = 0 == xr_strcmp(key.c_str(), ".\\") ? "" : key.c_str();
			strcpy_s(path, _path);
			u32 path_len = xr_strlen(path);
			if ((0 != path_len) && (path[path_len - 1] != '\\')) strcat_s(path, "\\");

			Msg("~~Processing folder: [%s]", path);
			BOOL efRecurse;
			BOOL val = IsFolderAccepted(ltx, path, efRecurse);
			if (val || (!val && !efRecurse))
			{
				if (val)
					ProcessFolder(list, path);

				auto i_fl_list = FS.file_list_open("$target_folder$", path, folder_mask);
				if (!i_fl_list)
				{
					Msg("!!ERROR: Unable to open folder list: [%s]", path);
					continue;
				}

				for (char* it : *i_fl_list) {
					string_path tmp_path;
					strconcat(sizeof(tmp_path), tmp_path, path, it);
					bool val = IsFolderAccepted(ltx, tmp_path, efRecurse);
					if (val)
					{
						fl_list.push_back(xr_strdup(tmp_path));
						Msg("--+F: %s", tmp_path);
						// collect files
						if (ifRecurse)
							ProcessFolder(list, tmp_path);
					}
					else
					{
						Msg("!!-F: %s", tmp_path);
					}
				}
				FS.file_list_close(i_fl_list);
			}
			else
			{
				Msg("!!-F: %s", path);
			}
		}
	}

	// compress

	if (ltx.section_exist("include_files"))
	{
		const auto& if_sect = ltx.r_section("include_files");
		for (const auto& pair : if_sect.Data)
			list.push_back(xr_strdup(pair.first.c_str()));
	}

	Log("\n");

	CompressList(tgt_name, &list, &fl_list, bFast);

	// free
	for (char* it : list)
		xr_free(it);
	list.clear_and_free();

	for (char* it : fl_list)
		xr_free(it);
	fl_list.clear_and_free();

	exclude_exts.clear();
}


int __cdecl main(int argc, char* argv[])
{
	g_temporary_stuff = &trivial_encryptor::decode;
	g_dummy_stuff = &trivial_encryptor::encode;

	Debug._initialize();

	auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
	auto LogCB = [handle](const char* s) {
		constexpr WORD FOREGROUND_YELLOW = 0x0006;
		WORD Clr = s[0] == '!' ? FOREGROUND_RED : s[0] == '-' ? FOREGROUND_GREEN : s[0] == '~' ? FOREGROUND_YELLOW : FOREGROUND_INTENSITY;
		SetConsoleTextAttribute(handle, Clr);

		auto ResStr = s;
		if (Clr != FOREGROUND_INTENSITY && strlen(s) > 2)
			ResStr = s + 2;

		auto buf = (char*)_alloca(strlen(ResStr) + 1);
		CharToOem(ResStr, buf);
		std::cout << buf << std::endl;
	};

	Core._initialize("xrCompress", LogCB, FALSE);

	Log("\n");
	Log("OGSR Engine x64 Compressor [ https://github.com/OGSR/OGSR-Engine ]");
	Log("\n");

	LPCSTR params = GetCommandLine();

	MOD_COMPRESS = !!strstr(params, "-xdb");

	bStoreFiles = !!strstr(params, "-store");

	if (const char* temp = strstr(params, "-max_size"))
	{
		u32 test = 1024 * 1024 * u32(atoi(temp + 9));
		if (test > XRP_MAX_SIZE)
			Msg("!!too large max_size [%u], restoring previous [%u]", test, XRP_MAX_SIZE);
		else
			XRP_MAX_SIZE = test;
	}

	if (argc < 2)
	{
		Msg(
			"!!ERROR: you must pass folder name as parameter.\n"
			"-ltx <file_name.ltx> - mandatory config for xrCompressor\n"
			"-xdb                 - pack files into .xdb format\n"
			"-fast                - fast compression.\n"
			"-store               - store files. No compression.\n"
			"-max_size <Mb>       - set maximum archive size. Default: [%u Mb]\n"
			, XRP_MAX_SIZE_DEF
		);

		Core._destroy();
		goto EXIT;
	}

	string_path folder;
	strconcat(sizeof(folder), folder, argv[1], "\\");
	_strlwr_s(folder, sizeof(folder));

	FS._initialize(CLocatorAPI::flTargetFolderOnly | CLocatorAPI::flScanAppRoot, folder);

	Log("\n");
	Msg("Compressing files [%s]...", folder);
	Log("\n");

	BOOL bFast = 0 != strstr(params, "-fast");

	LPCSTR p = strstr(params, "-ltx");
	R_ASSERT2(p, "wrong params passed. -ltx option needed");
	ProcessLTX(argv[1], p + 4, bFast);

	Core._destroy();

EXIT:
	ExitFromWinMain = true;
	return 0;
}
