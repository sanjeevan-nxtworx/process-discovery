#include "stdafx.h"
#include "SpyUrlData.h"
#include "SpyControlData.h"
#include "SpyControlProxy.h"
#include "Utility.h"

SpyUrlData::SpyUrlData(string JSONString)
{
	string err;
	Json jSonElement;

	jSonElement = Json::parse(JSONString, err);
	ID = jSonElement["ID"].int_value();
	rule = jSonElement["Rule"].string_value();
	userName = jSonElement["UserName"].string_value();
	description = jSonElement["Description"].string_value();
	appName = jSonElement["AppName"].string_value();
	appDescription = jSonElement["AppDesc"].string_value();
	url = EscapeJSonAnnotationString(jSonElement["URL"].string_value());
	bIgnoreEvents = jSonElement["IgnoreEvents"].bool_value();
	bIgnoreVideo = jSonElement["IgnoreVideo"].bool_value();
	bIsHomePage = jSonElement["IsHomePage"].bool_value();
	fPos = 0LL;
	fPosNext = 0LL;
	fPosMask = 0LL;
	fPosControl = 0LL;
}

SpyUrlData::SpyUrlData()
{
	ID = 0;
	userName = "";
	description = "";
	appName = "";
	appDescription = "";
	url = "";
	bIgnoreEvents = false;
	bIgnoreVideo = false;
	bIsHomePage = false;
	fPos = 0LL;
	fPosNext = 0LL;
	fPosMask = 0LL;
	fPosControl = 0LL;
	//windows.clear();
}

SpyUrlData::~SpyUrlData()
{

}

void SpyUrlData::SetID(int value)
{
	ID = value;
}

void SpyUrlData::SetUserDefinedName(string value)
{
	userName = value;
}

void SpyUrlData::SetDescription(string value)
{
	description = value;
}

void SpyUrlData::SetApplicationName(string value)
{
	appName = value;
}

void SpyUrlData::SetApplicationDescription(string value)
{
	appDescription = value;
}

void SpyUrlData::SetURLRuleValue(string value)
{
	urlRuleValue = value;
}

void SpyUrlData::SetURL(string value)
{
	url = value;
}

void SpyUrlData::SetRule(string value)
{
	rule = value;
}

void SpyUrlData::SetIgnoreEvents(bool value)
{
	bIgnoreEvents = value;
}

void SpyUrlData::SetIgnoreVideo(bool value)
{
	bIgnoreVideo = value;
}

void SpyUrlData::SetHomePage(bool value)
{
	bIsHomePage = value;
}

int SpyUrlData::GetID()
{
	return ID;
}

void SpyUrlData::SetFilePosition(long long value)
{
	fPos = value;
}

void SpyUrlData::SetFileNextURLPosition(long long value)
{
	fPosNext = value;
}

void SpyUrlData::SetMaskFPos(long long value)
{
	fPosMask = value;
}

void SpyUrlData::SetControlFPos(long long value)
{
	fPosControl = value;
}

string SpyUrlData::GetUserDefinedName()
{
	return userName;
}

string SpyUrlData::GetDescription()
{
	return description;
}

string SpyUrlData::GetApplicationName()
{
	return appName;
}

string SpyUrlData::GetApplicationDescription()
{
	return appDescription;
}

string SpyUrlData::GetURL()
{
	return url;
}

string SpyUrlData::GetRule()
{
	return rule;
}

string SpyUrlData::GetURLRuleValue()
{
	return urlRuleValue;
}

bool SpyUrlData::GetIgnoreEvents()
{
	return bIgnoreEvents;
}

bool SpyUrlData::GetIgnoreVideo()
{
	return bIgnoreVideo;
}

bool SpyUrlData::GetHomePage()
{
	return bIsHomePage;
}

long long SpyUrlData::GetFilePosition()
{
	return fPos;
}

long long SpyUrlData::GetFileNextURLPosition()
{
	return fPosNext;
}

long long SpyUrlData::GetMaskFPos()
{
	return fPosMask;
}

long long SpyUrlData::GetControlFPos()
{
	return fPosControl;
}


