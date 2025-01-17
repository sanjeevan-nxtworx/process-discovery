#pragma once
#include <mmsystem.h>
#include <vector>


class WavDeviceList
{
private:
	std::vector<WAVEINCAPS *> deviceList;

public:
	WavDeviceList();
	~WavDeviceList();
	int GetDevice(DWORD devCaps);
	void GetDeviceCaps(WORD deviceNum, WORD *numChannels, WORD *freq, WORD *dataSize);
};

