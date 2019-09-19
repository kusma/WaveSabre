#ifndef __WAVESABRECORE_MACOSHELPERS_H__
#define __WAVESABRECORE_MACOSHELPERS_H__

#ifdef LGCM_MAC

#include <stdio.h>

#define WAVE_FORMAT_PCM 0x0001

#pragma pack(push, 1)
typedef struct
{
	unsigned short wFormatTag;
	unsigned short nChannels;
	unsigned int   nSamplesPerSec;
	unsigned int   nAvgBytesPerSec;
	unsigned short nBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX;
#pragma pack(pop)

namespace WaveSabreCore
{
	class MacOSHelpers
	{
	public:
		static void LogError(const char* format, ...);
		static void ChangeToResourcesDirectory();
		static unsigned char *LoadFile(const char *filename);
		static bool SaveFile(const char *filename, const unsigned char *data, size_t size);
		static unsigned char *PCMToGSM(const unsigned char *in_data, size_t in_size, size_t *out_size);
		static unsigned char *BuildWAV(const unsigned char *in_data, size_t in_size, size_t *out_size);
	};
}

#endif

#endif
