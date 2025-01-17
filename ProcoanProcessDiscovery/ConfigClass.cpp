#include "stdafx.h"
#include "ConfigClass.h"
#include <string>

using namespace std;

ConfigClass::ConfigClass(char *fileName)
{
	bCaptureActiveMonitor = true;
	frameRate = 10;
	bCaptureAudio = true;
	bCaptureDirectX = false;
	bCaptureEvents = true;
	bCaptureVideo = true;
	selectedMonitorList.clear();

	bPlayActiveMonitor = true;
	xml_parse_result result = doc.load_file(fileName);

	string strValue = doc.child("config").child("recorder").child_value("captureactivemonitors");
	if (strValue == "OFF")
		bCaptureActiveMonitor = false;
	strValue = doc.child("config").child("recorder").child_value("captureaudio");
	if (strValue == "OFF")
		bCaptureAudio = false;
	strValue = doc.child("config").child("recorder").child_value("framerate");
	int val = 0;
	if (strValue != "")
		stoi(strValue);
	if (val == 30 || val == 15 || val == 10 || val == 5 || val == 2 || val == 1)
		frameRate = val;
	strValue = doc.child("config").child("recorder").child_value("captureevents");
	if (strValue == "OFF")
		bCaptureEvents = false;
	strValue = doc.child("config").child("recorder").child_value("capturevideo");
	if (strValue == "OFF")
		bCaptureVideo = false;

	strValue = doc.child("config").child("player").child_value("playactivemonitors");
	if (strValue == "OFF")
		bPlayActiveMonitor = false;

	xml_node selectedMonitors = doc.child("config").child("recorder").child("selectedmonitors");
	for (xml_node monitorName = selectedMonitors.child("monitorname"); monitorName; monitorName = monitorName.next_sibling("monitorname"))
	{
		string monitorNameValue = monitorName.value();
		selectedMonitorList.push_back(monitorNameValue);
	}

	if (strValue == "OFF")
		bPlayActiveMonitor = false;

	
}

ConfigClass::~ConfigClass()
{
}

bool ConfigClass::getPlayDisplayActiveMonitorMode()
{
	return bPlayActiveMonitor;
}

bool ConfigClass::getCaptureModeDirectX()
{
	return bCaptureDirectX;
}

bool ConfigClass::getCaptureVideo()
{
	return bCaptureVideo;
}


int ConfigClass::getCaptureFrameRate()
{
	return frameRate;
}

bool ConfigClass::getCaptureAudio()
{
	return bCaptureAudio;
}

bool ConfigClass::getCaptureEvents()
{
	return bCaptureEvents;
}

bool ConfigClass::getCaptureMonitorMode()
{
	return bCaptureActiveMonitor;
}

vector <string> *ConfigClass::getMonitorList()
{
	return &selectedMonitorList;
}