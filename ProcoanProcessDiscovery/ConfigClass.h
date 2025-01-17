#pragma once
#include "pugixml.hpp"
#include <string>
#include <vector>

using namespace std;
using namespace pugi;

class ConfigClass
{

// Recorder Config
	int frameRate;
	bool bCaptureAudio;
	bool bCaptureDirectX;
	bool bCaptureEvents;
	bool bCaptureActiveMonitor;
	bool bCaptureVideo;

// Player config
	bool bPlayActiveMonitor;
	xml_document doc;

	vector <string> selectedMonitorList;

public:
	bool getPlayDisplayActiveMonitorMode();

	bool getCaptureModeDirectX();
	int  getCaptureFrameRate();
	bool getCaptureAudio();
	bool getCaptureEvents();
	bool getCaptureMonitorMode();
	bool getCaptureVideo();
	vector <string> *getMonitorList();

	ConfigClass(char *fileName);
	~ConfigClass();
};
