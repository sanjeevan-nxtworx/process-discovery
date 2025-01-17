#pragma once
#include "SpyWindowData.h"
#include "json11.hpp"
#include <string>
#include <map>
#include <vector>
using namespace std;
using namespace json11;

class SpyControlProxy;

class SpyProcessData
{
	DWORD ID;
	string sysName;
	string userName;
	string description;
	string type;
	string platform;
	string url;
	bool   bIgnoreEvents;
	bool   bIgnoreVideo;
	long long fPos;
	long long fPosNext;
	long long fPosWindow;

	//string GetControlData(vector < SpyControlProxy *> *pControlList);
	//void SetLayoutData(vector < SpyControlProxy *> *pControlList, vector <Json> *pProxyDefs);
public:
	SpyProcessData(string JSONString);
	SpyProcessData();
	~SpyProcessData();

	//void SetWindowData(string JSONString);
	void SetID(int ID);
	void SetSystemName(string value);
	void SetUserDefinedName(string value);
	void SetDescription(string value);
	void SetType(string value);
	void SetPlatform(string value);
	void SetURL(string value);
	void SetIgnoreEvents(bool value);
	void SetIgnoreVideo(bool value);
	void SetFilePosition(long long value);
	void SetFileNextProcessPosition(long long value);
	void SetWindowFPos(long long value);

	int GetID();
	string GetSystemName();
	string GetUserDefinedName();
	string GetDescription();
	string GetType();
	string GetPlatform();
	string GetURL();
	bool GetIgnoreEvents();
	bool GetIgnoreVideo();
	long long GetFilePosition();
	long long GetFileNextProcessPosition();
	long long GetWindowFPos();

	//string GetWindowsJSON();
};

