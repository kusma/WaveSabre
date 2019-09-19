#include <WaveSabreCore/MacOSHelpers.h>

#ifdef LGCM_MAC

#include <dlfcn.h>
#include <unistd.h>
#include <string>
#include <syslog.h>
#include "sndfile.h"

#define LGCM_SAMPLE_RATE        44100
#define LGCM_MAX_SAMPLE_SIZE    16 * 1024
#define LGCM_BUFFER_LEN         4096    /*-(1<<16)-*/

#define GSM610_BLOCKSIZE        65
#define GSM610_SAMPLES          320
#define GSM610_WAVE_FORMAT      0x0031

#pragma pack(push, 1)
typedef struct
{
	unsigned int ChunkID;       // "RIFF" (0x52494646 big-endian form)
	unsigned int ChunkSize;     // size of the rest
	unsigned int Format;        // "WAVE" (0x57415645 big-endian form)
	unsigned int Subchunk1ID;   // "fmt " (0x666d7420 big-endian form)
	unsigned int Subchunk1Size; // 16 for PCM, rest of the Subchunk which follows
	WAVEFORMATEX Fmt;
	unsigned short Pad;
	unsigned int Subchunk2ID;   // "data" (0x64617461 big-endian form)
	unsigned int Subchunk2Size; // size of the rest
} WavHeader;
#pragma pack(pop)

// Note(alkama): can only hold 16 megs of data
typedef struct
{
	sf_count_t offset;
	sf_count_t length;
	unsigned char data[LGCM_MAX_SAMPLE_SIZE];
} VioData;

static sf_count_t VioGetFilelen(void *user_data)
{
	VioData *vf = (VioData *)user_data;
	return vf->length;
}

static sf_count_t VioSeek(sf_count_t offset, int whence, void *user_data)
{
	VioData *vf = (VioData *)user_data;

	switch (whence)
	{
	case SEEK_SET: vf->offset = offset; break;
	case SEEK_CUR: vf->offset = vf->offset + offset; break;
	case SEEK_END: vf->offset = vf->length + offset; break;
	default: break;
	}

	return vf->offset;
}

static sf_count_t VioRead(void *ptr, sf_count_t count, void *user_data)
{
	VioData *vf = (VioData *)user_data;
	
	// This will breack badly for files over 2Gig in length.
	if (vf->offset + count > vf->length)
		count = vf->length - vf->offset;
	
	memcpy(ptr, vf->data + vf->offset, count);
	vf->offset += count;
	
	return count;
}

static sf_count_t VioWrite(const void *ptr, sf_count_t count, void *user_data)
{
	VioData *vf = (VioData *) user_data;
	
	// This will breack badly for files over 2Gig in length.
	if (vf->offset >= (int)sizeof(vf->data))
		return 0;
	
	if (vf->offset + count > (int)sizeof(vf->data))
		count = sizeof(vf->data) - vf->offset;
	
	memcpy(vf->data + vf->offset, ptr, (size_t) count);
	vf->offset += count;
	
	if (vf->offset > vf->length)
		vf->length = vf->offset;
	
	return count;
}

static sf_count_t VioTell(void *user_data)
{
	VioData *vf = (VioData *) user_data;
	return vf->offset;
}

SF_VIRTUAL_IO vio = {
	VioGetFilelen, // get_filelen
	VioSeek, // seek
	VioRead, // read
	VioWrite, // write
	VioTell // tell
};

namespace WaveSabreCore
{
	void MacOSHelpers::LogError(const char* format, ...)
	{
		va_list args;
		openlog("WaveSabre", LOG_CONS, LOG_USER);
		va_start (args, format);
		vsyslog(LOG_ERR, format, args);
		va_end (args);
		closelog();
	}
	
	void MacOSHelpers::ChangeToResourcesDirectory()
	{
		Dl_info info;
		if (dladdr((const void*)MacOSHelpers::ChangeToResourcesDirectory, &info))
		{
			if (info.dli_fname)
			{
				std::string name;
				name.assign(info.dli_fname);
				for (int i = 0; i < 2; i++)
				{
					int delPos = name.find_last_of('/');
					if (delPos == -1)
					{
						// something's rotten in the french realm
						return;
					}
					name.erase(delPos, name.length() - delPos);
				}
				name.append("/Resources");
				
				chdir(name.c_str());
			}
		}
	}
	
	unsigned char *MacOSHelpers::LoadFile(const char *filename)
	{
		FILE *file = fopen(filename, "rb");
		if (!file)
		{
			return nullptr;
		}
		
		long filesize = -1;
		fseek(file, 0, SEEK_END);
		filesize = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		if (filesize <= 0)
		{
			fclose(file);
			return nullptr;
		}
		
		auto data = new unsigned char[filesize];
		if (!data)
		{
			fclose(file);
			return nullptr;
		}
		fread(data, sizeof(unsigned char), filesize, file);
		fclose(file);
		
		return data;
	}
	
	bool MacOSHelpers::SaveFile(const char *filename, const unsigned char *data, size_t size)
	{
		FILE *file = fopen(filename, "wb");
		if (!file)
		{
			return false;
		}
		
		size_t n_written = fwrite(data , sizeof(unsigned char), size, file);
		if (n_written != size)
		{
			fclose(file);
			return false;
		}
		
		fclose(file);
		
		return true;
	}
	
	unsigned char *MacOSHelpers::PCMToGSM(const unsigned char *in_data, size_t in_size, size_t *out_size)
	{
		if (in_size > LGCM_MAX_SAMPLE_SIZE) {
			LogError("PCMToGSM: input is too big.");
			return nullptr;
		}
		
		static VioData vio_in_data;
		vio_in_data.offset = 0;
		vio_in_data.length = in_size;
		memcpy(vio_in_data.data, in_data, in_size);
		
		SF_INFO sfinfo;
		memset (&sfinfo, 0, sizeof(sfinfo));
		sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_GSM610;

		SNDFILE *in_file = sf_open_virtual(&vio, SFM_READ, &sfinfo, &vio_in_data);
		if (in_file == NULL)
		{
			LogError("PCMToGSM: sf_open for reading failed with error.");
			LogError(sf_strerror(NULL));
			return nullptr;
		}
		
		if (sfinfo.channels > 1)
		{
			LogError("PCMToGSM: input is not mono.");
			sf_close(in_file);
			return nullptr;
		}
		
		sfinfo.samplerate = LGCM_SAMPLE_RATE;
		sfinfo.channels = 1;
		sfinfo.format = SF_FORMAT_RAW | SF_FORMAT_PCM_16;

		static VioData vio_out_data;
		memset (&vio_out_data, 0, sizeof(vio_out_data));
		SNDFILE *out_file = sf_open_virtual(&vio, SFM_WRITE, &sfinfo, &vio_out_data);
		if (out_file == NULL)
		{
			LogError("PCMToGSM: sf_open for writing failed with error.");
			LogError(sf_strerror(NULL));
			sf_close(in_file);
			return nullptr;
		}
		
		static int buffer_data[LGCM_BUFFER_LEN];
		int frames = LGCM_BUFFER_LEN;
		int readcount = frames;
		while (readcount > 0)
		{
			readcount = sf_readf_int(in_file, buffer_data, frames) ;
			sf_writef_int(out_file, buffer_data, readcount) ;
		}

		*out_size = vio_out_data.length;
		auto out_data = new unsigned char[vio_out_data.length];
		memcpy(out_data, vio_out_data.data, vio_out_data.length);

		sf_close(out_file);
		sf_close(in_file);

		return out_data;
	}

	unsigned char *MacOSHelpers::BuildWAV(const unsigned char *in_data, size_t in_size, size_t *out_size)
	{
		*out_size = sizeof(WavHeader) + in_size;
		auto out_data = new unsigned char[*out_size];
		WavHeader *wh = (WavHeader *)out_data;
		wh->ChunkID = 0x46464952; // "RIFF"
		wh->ChunkSize = *out_size - 8; // rest
		wh->Format = 0x45564157; // "WAVE"
		wh->Subchunk1ID = 0x20746d66; // "fmt "
		wh->Subchunk1Size = 20; // 16 for PCM, rest
		wh->Fmt.wFormatTag = GSM610_WAVE_FORMAT;
		wh->Fmt.nChannels = 1;
		wh->Fmt.nSamplesPerSec = LGCM_SAMPLE_RATE;
		wh->Fmt.nAvgBytesPerSec = LGCM_SAMPLE_RATE * GSM610_BLOCKSIZE / GSM610_SAMPLES;
		wh->Fmt.nBlockAlign = GSM610_BLOCKSIZE;
		wh->Fmt.wBitsPerSample = 0;
		wh->Fmt.cbSize = 2;
		wh->Pad = 0x140; // <- eeeeh, horrible Note(alkama): dont know what it's about, but it fails if it's not there
		wh->Subchunk2ID = 0x61746164; // "data"
		wh->Subchunk2Size = in_size; // rest
		
		unsigned char *data = out_data+sizeof(WavHeader);
		memcpy(data, in_data, in_size);
		
		return out_data;
	}
}

#endif
