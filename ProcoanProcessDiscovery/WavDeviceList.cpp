#include "stdafx.h"
#include <mmsystem.h>
#include "WavDeviceList.h"


WavDeviceList::WavDeviceList()
{
	int numDevices = waveInGetNumDevs();
	for (int i = 0; i < numDevices; i++)
	{
		WAVEINCAPS *pWavCaps = DBG_NEW WAVEINCAPS;
		ZeroMemory(pWavCaps, sizeof(WAVEINCAPS));
		if (waveInGetDevCaps(i, pWavCaps, sizeof(WAVEINCAPS)) == 0)
		{
			deviceList.push_back(pWavCaps);
		}
	}
}

void WavDeviceList::GetDeviceCaps(WORD deviceNum, WORD *numChannels, WORD *freq, WORD *dataSize)
{
	*numChannels = 0;
	*freq = 0;	
	*dataSize = 0;
	if (deviceNum == -1)
		return;

	WAVEINCAPS *pWavCaps = deviceList[deviceNum];
	if ((pWavCaps->dwFormats & WAVE_FORMAT_2S16) == WAVE_FORMAT_2S16)
	{
		*freq = 22050;
		*numChannels = 2;
		*dataSize = 16;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_2S08) == WAVE_FORMAT_2S08)
	{
		*freq = 22050;
		*numChannels = 2;
		*dataSize = 8;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_2M16) == WAVE_FORMAT_2M16)
	{
		*freq = 22050;
		*numChannels = 1;
		*dataSize = 16;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_2M08) == WAVE_FORMAT_2M08)
	{
		*freq = 22050;
		*numChannels = 1;
		*dataSize = 8;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_1S16) == WAVE_FORMAT_1S16)
	{
		*freq = 11025;
		*numChannels = 2;
		*dataSize = 16;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_1S08) == WAVE_FORMAT_1S08)
	{
		*freq = 11025;
		*numChannels = 2;
		*dataSize = 8;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_1M16) == WAVE_FORMAT_1M16)
	{
		*freq = 11025;
		*numChannels = 1;
		*dataSize = 16;
	}
	else if ((pWavCaps->dwFormats & WAVE_FORMAT_1M08) == WAVE_FORMAT_1M08)
	{
		*freq = 11025;
		*numChannels = 1;
		*dataSize = 8;
	}
}



int WavDeviceList::GetDevice(DWORD devCaps)
{
	int index = -1;
	int indexFound = -1;

	for (auto it = deviceList.cbegin();it != deviceList.cend();it++)
	{
		WAVEINCAPS *pWavCaps = *it;
		index++;
		if ((pWavCaps->dwFormats & devCaps) == devCaps)
		{
			indexFound = index;
			break;
		}

	}

	return indexFound;
}


WavDeviceList::~WavDeviceList()
{
	std::vector <WAVEINCAPS *>::iterator itList = deviceList.begin();
	while (itList != deviceList.end())
	{
		WAVEINCAPS *pPtr = *itList;
		delete pPtr;
		itList++;
	}

	deviceList.clear();
}
