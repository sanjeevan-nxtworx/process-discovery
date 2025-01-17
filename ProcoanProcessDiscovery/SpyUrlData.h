#pragma once
#include "json11.hpp"
#include <string>
#include <map>
#include <vector>
using namespace std;
using namespace json11;

class SpyControlProxy;

class SpyUrlData
{
	DWORD ID;

	string url;
	string rule;
	string urlRuleValue;

	string userName;
	string description;

	bool bIsHomePage;
	string appName;
	string appDescription;

	bool   bIgnoreEvents;
	bool   bIgnoreVideo;
	long long fPos;
	long long fPosNext;
	long long fPosControl;
	long long fPosMask;

public:
	SpyUrlData(string JSONString);
	SpyUrlData();
	~SpyUrlData();

	void SetID(int ID);
	void SetRule(string value);
	void SetURL(string value);
	void SetUserDefinedName(string value);
	void SetDescription(string value);
	void SetApplicationName(string value);
	void SetApplicationDescription(string value);
	void SetURLRuleValue(string value);

	void SetIgnoreEvents(bool value);
	void SetIgnoreVideo(bool value);
	void SetHomePage(bool value);

	void SetFilePosition(long long value);
	void SetFileNextURLPosition(long long value);
	void SetMaskFPos(long long value);
	void SetControlFPos(long long value);

	int GetID();
	string GetRule();
	string GetUserDefinedName();
	string GetDescription();
	string GetURL();
	bool GetIgnoreEvents();
	bool GetIgnoreVideo();
	string GetApplicationName();
	string GetApplicationDescription();
	string GetURLRuleValue();
	bool GetHomePage();

	long long GetFilePosition();
	long long GetFileNextURLPosition();
	long long  GetMaskFPos();
	long long  GetControlFPos();

	//string GetWindowsJSON();
};

