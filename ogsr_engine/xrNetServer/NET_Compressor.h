#if !defined(AFX_NET_COMPRESSOR_H__21E1ED1C_BF92_4BF0_94A8_18A27486EBFD__INCLUDED_)
#define AFX_NET_COMPRESSOR_H__21E1ED1C_BF92_4BF0_94A8_18A27486EBFD__INCLUDED_
#pragma once

#include "../xrCore/xrSyncronize.h"

#if defined(XRSE_FACTORY_EXPORTS) || defined(AI_COMPILER)
struct	NET_Compressor_FREQ
{
	u32	table	[257];

	NET_Compressor_FREQ		()
	{
		setZero				();
	}
	void	setIdentity		()
	{
		for (u32 I=0; I<256; I++) table[I] = 1;
	}
	void	setZero			()
	{
		for (u32 I=0; I<256; I++) table[I] = 0;
	}
	void	setFromWORDS	(u16* data)
	{
		for (u32 I=0; I<256; I++) table[I] = u32(data[I]);
	}
	void	Cumulate		()
	{
		// summarize counters
		u32 I, total	= 0;
		for (I=0; I<256; I++)	total += table[I];

		// calculate cumulative freq
        table[256] = total;
        for (I=256; I; I--)		table[I-1] = table[I] - table[I-1];
	}
	void	Normalize		();

	IC u32&	operator[]	(int id)	{ return table[id]; }
};

// typedefs
class	NET_Compressor
{
public:
	typedef u32			code_value;		/* Type of an rangecode value			*/
	typedef u32			freq;
private:
	xrCriticalSection		CS;

	// main structure
	struct rangecoder {
		u32					low,range,help;
		BYTE				buffer;
		u32					bytecount;
		BYTE*				ptr;

		IC void				byte_out		(BYTE B)	{ *ptr++ = B;		}
		IC BYTE				byte_in			()			{ return *ptr++;	}
	} RNGC;

	NET_Compressor_FREQ		freqCompress;	// used in compression
	NET_Compressor_FREQ		freqDecompress;	// used in decompression
private:
	/* Start the encoder                                         */
	/* c is written as first byte in the datastream (header,...) */
	void				start_encoding		( BYTE* dest, u32 header_size );

	/* Encode a symbol using frequencies                         */
	/* sy_f is the interval length (frequency of the symbol)     */
	/* lt_f is the lower end (frequency sum of < symbols)        */
	/* tot_f is the total interval length (total frequency sum)  */
	/* or (a lot faster): tot_f = 1<<shift                       */
	void				encode_freq			( freq sy_f, freq lt_f, freq tot_f );
	void				encode_shift		( freq sy_f, freq lt_f, freq shift );
	void				encode_normalize	( );

	/* Encode a byte/short without modelling                     */
	/* b,s is the data to be encoded                             */
	void				encode_byte			(freq b)			{ encode_shift( freq(1), freq(b), freq(8) );	}
	void				encode_short		(freq s)			{ encode_shift( freq(1), freq(s), freq(16) );	}

	/* Finish encoding                                           */
	/* returns number of bytes written                           */
	u32					done_encoding		( );

	/* Start the decoder                                         */
	/* returns the char from start_encoding or EOF               */
	int					start_decoding		( BYTE* src, u32 header_size );

	/* Calculate culmulative frequency for next symbol. Does NO update!*/
	/* tot_f is the total frequency                              */
	/* or: totf is 1<<shift                                      */
	/* returns the <= culmulative frequency                      */
	freq				decode_culfreq		( freq tot_f );
	freq				decode_culshift		( freq shift );
	void				decode_normalize	( );

	/* Update decoding state                                     */
	/* sy_f is the interval length (frequency of the symbol)     */
	/* lt_f is the lower end (frequency sum of < symbols)        */
	/* tot_f is the total interval length (total frequency sum)  */
	void				decode_update		( freq sy_f, freq lt_f, freq tot_f);
	void				decode_update_shift	( freq f1, freq f2, freq f3)	{ decode_update(f1,f2,freq(1)<<f3); }

	/* Decode a byte/short without modelling                     */
	BYTE				decode_byte			( );
	u16					decode_short		( );

	/* Finish decoding                                           */
	void				done_decoding		( );

public:
	NET_Compressor			();
	~NET_Compressor			();

	void					Initialize	(NET_Compressor_FREQ& compress, NET_Compressor_FREQ& decompress);
	u16					Compress	(BYTE* dest, BYTE* src, u32 count);	// return size of compressed
	u16					Decompress	(BYTE* dest, BYTE* src, u32 count);	// return decompressed size
};

#else

class XRNETSERVER_API NET_Compressor
{
	xrCriticalSection		CS;
	
	struct SCompressorStats
	{
		u32				total_uncompressed_bytes;
		u32				total_compressed_bytes;
		
		struct SStatPacket{
			u32			hit_count;
			u32			unlucky_attempts;
			u32			compressed_size;
			SStatPacket ():hit_count(0),unlucky_attempts(0),compressed_size(0){}
		};
		xr_map<u32,SStatPacket>	m_packets;

		SCompressorStats():total_uncompressed_bytes(0),total_compressed_bytes(0){}
		SStatPacket*	get(u32 size) {return &(m_packets[size]);};
	}					m_stats;

public:
						NET_Compressor		();
						~NET_Compressor		();

	u16					compressed_size		(const u32 &count);
	u16					Compress			(BYTE* dest, const u32 &dest_size, BYTE* src, const u32 &count);	// return size of compressed
	u16					Decompress			(BYTE* dest, const u32 &dest_size, BYTE* src, const u32 &count);	// return size of compressed
	void				DumpStats			(bool brief);
};
#endif

#endif // !defined(AFX_NET_COMPRESSOR_H__21E1ED1C_BF92_4BF0_94A8_18A27486EBFD__INCLUDED_)

