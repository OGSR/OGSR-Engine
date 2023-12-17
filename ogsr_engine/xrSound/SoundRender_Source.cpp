#include "stdafx.h"

#include "soundrender_core.h"
#include "soundrender_source.h"

CSoundRender_Source::CSoundRender_Source()
{
    m_fMinDist = 1.f;
    m_fMaxDist = 300.f;
    m_fMaxAIDist = 300.f;
    m_fBaseVolume = 1.f;
    m_uGameType = 0;
    fname = 0;
    CAT.table = 0;
    CAT.size = 0;
}

CSoundRender_Source::~CSoundRender_Source() { unload(); }

bool ov_can_continue_read(int res)
{
    switch (res)
    {
    case 0:
        return false;
        // info
    case OV_HOLE:
        Msg("Vorbisfile encoutered missing or corrupt data in the bitstream. Recovery is normally automatic and this return code is for informational purposes only.");
        return true;
    case OV_EBADLINK:
        Msg("The given link exists in the Vorbis data stream, but is not decipherable due to garbacge or corruption.");
        return true;
        // error
    case OV_FALSE: Msg("Not true, or no data available"); return false;
    case OV_EREAD: Msg("Read error while fetching compressed data for decode"); return false;
    case OV_EFAULT: Msg("Internal inconsistency in decode state. Continuing is likely not possible."); return false;
    case OV_EIMPL: Msg("Feature not implemented"); return false;
    case OV_EINVAL: Msg("Either an invalid argument, or incompletely initialized argument passed to libvorbisfile call"); return false;
    case OV_ENOTVORBIS: Msg("The given file/data was not recognized as Ogg Vorbis data."); return false;
    case OV_EBADHEADER: Msg("The file/data is apparently an Ogg Vorbis stream, but contains a corrupted or undecipherable header."); return false;
    case OV_EVERSION: Msg("The bitstream format revision of the given stream is not supported."); return false;
    case OV_ENOSEEK: Msg("The given stream is not seekable"); return false;
    }
    return false;
}

void CSoundRender_Source::i_decompress(OggVorbis_File* ovf, char* _dest, u32 size) const
{
    long TotalRet = 0;

    // Read loop
    // Read loop
    while (TotalRet < static_cast<long>(size))
    {
        const auto ret = ov_read(ovf, _dest + TotalRet, size - TotalRet, 0, 2, 1, nullptr);
        if (ret <= 0 && !ov_can_continue_read(ret))
            break;
        TotalRet += ret;
    }
}

void CSoundRender_Source::i_decompress(OggVorbis_File* ovf, float* _dest, u32 size) const
{
    s32 left = s32(size / m_wformat.nBlockAlign);
    while (left)
    {
        float** pcm;
        long samples = ov_read_float(ovf, &pcm, left, nullptr);

        if (samples <= 0 && !ov_can_continue_read(samples))
            break;

        if (samples > left)
            samples = left;

        for (long j = 0; j < samples; j++)
            for (long i = 0; i < m_wformat.nChannels; i++)
                *_dest++ = clampr(pcm[i][j], -1.0f, 1.0f);

        left -= samples;
    }
}