#include <WaveSabreCore/GmDls.h>
#include <WaveSabreCore/MacOSHelpers.h>

#ifndef LGCM_MAC
#include <Windows.h>

static char *gmDlsPaths[2] =
{
	"drivers/gm.dls",
	"drivers/etc/gm.dls"
};
#endif

namespace WaveSabreCore
{
	unsigned char *GmDls::Load()
	{
#ifndef LGCM_MAC
		HANDLE gmDlsFile = INVALID_HANDLE_VALUE;
		for (int i = 0; gmDlsFile == INVALID_HANDLE_VALUE; i++)
		{
			OFSTRUCT reOpenBuff;
			gmDlsFile = (HANDLE)OpenFile(gmDlsPaths[i], &reOpenBuff, OF_READ);
		}

		auto gmDlsFileSize = GetFileSize(gmDlsFile, NULL);
		auto gmDls = new unsigned char[gmDlsFileSize];
		unsigned int bytesRead;
		ReadFile(gmDlsFile, gmDls, gmDlsFileSize, (LPDWORD)&bytesRead, NULL);
		CloseHandle(gmDlsFile);
#else
		MacOSHelpers::ChangeToResourcesDirectory();
		auto gmDls = MacOSHelpers::LoadFile("./gm.dls");
#endif

		return gmDls;
	}
}
