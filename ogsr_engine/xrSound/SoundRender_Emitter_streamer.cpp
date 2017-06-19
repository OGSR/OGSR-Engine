#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Core.h"
#include "SoundRender_Source.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"

void	CSoundRender_Emitter::fill_data		(u8* _dest, u32 offset, u32 size)
{
/*
	Msg				("stream: %10s - %d",*source->fname,size);
	CopyMemory	(_dest,&source->m_buffer.front()+offset,size);
	return;
//*/
/*
	Memory.mem_fill	(_dest,0,size);	// debug only
	//	Msg			("stream: %10s - %d",*source->fname,size);
	int				dummy;
	ov_pcm_seek		(source->ovf,(psSoundFreq==sf_22K)?offset:offset/2);
//	ov_pcm_seek		(source->ovf,0);
	char* dest		= (char*)_dest;
	u32	left		= size;
	while (left)
	{                     
		int ret		= ov_read(source->ovf,dest,left,0,2,1,&dummy);
//		Msg			("Part: %d - %d",left,ret);
		if (ret==0){	
        	ret=0;
        	break;
        }if (ret>0){
			left		-= ret;
			dest		+= ret;
		}else{
			switch (ret){
			case OV_HOLE:		Msg("OV_HOLE");		continue; break;
			case OV_EBADLINK:	Msg("OV_EBADLINK"); continue; break;
			}
			break;
		}
	}
//	Msg			("Final: %d - %d",size,size-left);
/*/
//*
	u32		line_size						= SoundRender->cache.get_linesize();
	u32		line							= offset / line_size;

	// prepare for first line (it can be unaligned)
	u32		line_offs						= offset - line*line_size;
	u32		line_amount						= line_size - line_offs;
	while	(size)
	{
		// cache access
		if (SoundRender->cache.request(source->CAT,line))		{
			source->decompress	(line,target->get_data());
		}
                                                
		// fill block
		u32		blk_size	= _min(size,line_amount);
		u8*		ptr			= (u8*)SoundRender->cache.get_dataptr(source->CAT,line);
		CopyMemory		(_dest,ptr+line_offs,blk_size);
		
		// advance
		line		++	;
		size		-=	blk_size;
		_dest		+=	blk_size;
		offset		+=	blk_size;
		line_offs	=	0;
		line_amount	=	line_size;
	}
//*/
	//  --- previously it was something like this
	//	CopyMemory		(ptr,wave+offset,size);
}

void	CSoundRender_Emitter::fill_block	(void* ptr, u32 size)
{
	//Msg			("stream: %10s - [%X]:%d, p=%d, t=%d",*source->fname,ptr,size,position,source->dwBytesTotal);
	LPBYTE		dest = LPBYTE(ptr);
	if ((position+size) > source->dwBytesTotal)
	{
		// We are reaching the end of data, what to do?
		switch (state)
		{
		case stPlaying:
			{
				// Fill as much data as we can, zeroing remainder
				if (position >= source->dwBytesTotal)
				{
					// ??? We requested the block after remainder - just zero
					Memory.mem_fill	(dest,0,size);
//					Msg				("        playing: zero");
				} else {
					// Calculate remainder
					u32	sz_data		= source->dwBytesTotal - position;
					u32 sz_zero		= (position+size) - source->dwBytesTotal;
					VERIFY			(size == (sz_data+sz_zero));
					fill_data		(dest,position,sz_data);
					Memory.mem_fill	(dest+sz_data,0,sz_zero);
//					Msg				("        playing: [%d]-normal,[%d]-zero",sz_data,sz_zero);
				}
				position			+= size;
			}
			break;
		case stPlayingLooped:
			{
            	u32 hw_position		= 0;
				do{
					u32	sz_data		= source->dwBytesTotal - position;
                    u32 sz_write	= _min(size-hw_position,sz_data);
					fill_data		(dest+hw_position,	position,	sz_write);
                    hw_position		+= sz_write;
                    position		+= sz_write;
					position		%= source->dwBytesTotal;
                }while(0!=(size-hw_position));
/*				            	
				// Fill in two parts - looping :)
				u32		sz_first	= source->dwBytesTotal - position;
                u32		sz_second	= 0;
                if (0==sz_first)
                {
					fill_data			(dest,0,size);
//					Msg					("        playing: zero");
                } else {
                    sz_second			= (position+size) - source->dwBytesTotal;
                    VERIFY				(size == (sz_first+sz_second));
                    VERIFY				(position<source->dwBytesTotal);
					fill_data			(dest,			position,	sz_first);
					fill_data			(dest+sz_first,	0,			sz_second);
                }
//				Msg					("        looping: [%d]-first,[%d]-second",sz_first,sz_second);
				position			+= size;
				position			%= source->dwBytesTotal;
*/                
			}
			break;
		default:
			FATAL					("SOUND: Invalid emitter state");
			break;
		}
	} else {
		// Everything OK, just stream
//		Msg				("        normal");
		fill_data			(dest,position,size);
		position			+= size;
	}
}
