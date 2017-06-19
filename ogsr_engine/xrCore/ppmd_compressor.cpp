#include "stdafx.h"
#include "ppmd_compressor.h"
#include "ppmd.h"

const u32		suballocator_size			= 32;
const u32		order_model					= 8;
const MR_METHOD	restoration_method_cut_off	= MRM_RESTART;

typedef compression::ppmd::stream	stream;

extern compression::ppmd::stream	*trained_model;

void _STDCALL PrintInfo		(_PPMD_FILE* DecodedFile,_PPMD_FILE* EncodedFile)
{
}



static LONG PPMd_Locked = 0;

static inline void
PPMd_Lock()
{
    while( ::InterlockedExchange( &PPMd_Locked, 1 ) )
        ::Sleep( 0 );
}

static inline void
PPMd_Unlock()
{
    ::InterlockedExchange( &PPMd_Locked, 0 );
}


void ppmd_initialize		()
{
	if (trained_model)
		trained_model->rewind	();

	static bool	initialized	= false;
	if (initialized)
		return;

	string_path			file_name;

	FS.update_path		(file_name,"$game_config$","mp\\!PPMd.mdl");
	if (FS.exist(file_name)) {
		IReader			*reader = FS.r_open(file_name);
		R_ASSERT		(reader);
		u32				buffer_size = reader->length();
		u8				*buffer = (u8*)xr_malloc(buffer_size);
		reader->r		(buffer,buffer_size);
		FS.r_close		(reader);
		trained_model	= xr_new<stream>(buffer,buffer_size);
	}

	initialized		= true;
	if (StartSubAllocator(suballocator_size))
		return;
	
	exit			(-1);
}

u32 ppmd_compress	(void *dest_buffer, const u32 &dest_buffer_size, const void *source_buffer, const u32 &source_buffer_size)
{
    PPMd_Lock();
	ppmd_initialize	();

	stream			source(source_buffer,source_buffer_size);
	stream			dest(dest_buffer,dest_buffer_size);
	EncodeFile		(&dest,&source,order_model,restoration_method_cut_off);

	PPMd_Unlock();
	return			(dest.tell());
}

u32 ppmd_decompress	(void *dest_buffer, const u32 &dest_buffer_size, const void *source_buffer, const u32 &source_buffer_size)
{
    PPMd_Lock();
	ppmd_initialize	();

	stream			source(source_buffer,source_buffer_size);
	stream			dest(dest_buffer,dest_buffer_size);
	DecodeFile		(&dest,&source,order_model,restoration_method_cut_off);

	PPMd_Unlock();
	return			(dest.tell());
}

