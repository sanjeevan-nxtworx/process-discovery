#include "stdafx.h"
#include "SpyProcessData.h"
#include "SpyControlData.h"
#include "SpyControlProxy.h"

SpyProcessData::SpyProcessData(string JSONString)
{
	string err;
	Json jSonElement;

	jSonElement = Json::parse(JSONString, err);
	ID = jSonElement["ID"].int_value();
	sysName = jSonElement["SysName"].string_value();
	userName = jSonElement["UserName"].string_value();
	description = jSonElement["Description"].string_value();
	type = jSonElement["Type"].string_value();
	platform = jSonElement["Platform"].string_value();
	url = jSonElement["HomeURL"].string_value();
	bIgnoreEvents = jSonElement["IgnoreEvents"].bool_value();
	bIgnoreVideo = jSonElement["IgnoreVideo"].bool_value();
	fPos = 0LL;
	fPosNext = 0LL;
	fPosWindow = 0LL;
}

SpyProcessData::SpyProcessData()
{
	ID = 0;
	sysName = "";
	userName = "";
	description = "";
	type = "";
	platform = "";
	url = "";
	bIgnoreEvents = false;
	bIgnoreVideo = false;
	fPos = 0LL;
	fPosNext = 0LL;
	fPosWindow = 0LL;
	//windows.clear();
}

SpyProcessData::~SpyProcessData()
{

}

//void SpyProcessData::SetWindowData(string JSONString)
//{
//	string err;
//	Json jSonElement;
//
//	jSonElement = Json::parse(JSONString, err);
//	vector <Json> prcWindowDefs = jSonElement.array_items();
//	for (int i = 0; i < prcWindowDefs.size(); i++)
//	{
//		SpyWindowData *pWindowData = DBG_NEW SpyWindowData;
//
//		Json windowDef = prcWindowDefs[i];
//		pWindowData->SetID(windowDef["ID"].int_value());
//		pWindowData->SetTitle(windowDef["Title"].string_value());
//		pWindowData->SetName(windowDef["WindowName"].string_value());
//		pWindowData->SetDescription(windowDef["Description"].string_value());
//		pWindowData->SetClassname(windowDef["ClassName"].string_value());
//		pWindowData->SetAutomationID(windowDef["AutomationID"].string_value());
//		pWindowData->SetWindowType(windowDef["AutomationID"].string_value());
//		pWindowData->SetAutomationID(windowDef["Type"].string_value());
//		pWindowData->SetMatchingRule(windowDef["MatchRule"].string_value());
//		pWindowData->SetIncludeClassName(windowDef["IncludeClassName"].bool_value());
//		pWindowData->SetIncludeAutomationID(windowDef["IncludeAutoID"].bool_value());
//		pWindowData->SetAnnotationRule(windowDef["AnnotationRule"].string_value());
//
//		vector <Json> prcVarDefs = jSonElement["Variables"].array_items();
//		for (int j = 0; j < prcVarDefs.size(); j++)
//		{
//			Json varDef = prcVarDefs[j];
//			string varName = varDef["VarName"].string_value();
//			string varRule = varDef["Rule"].string_value();
//			pWindowData->SetVar(varName, varRule);
//		}
//
//		
//		vector <Json> winControlDefs = jSonElement["Controls"].array_items();
//		for (int j = 0; j < prcVarDefs.size(); j++)
//		{
//			Json varDef = winControlDefs[j];
//			SpyControlData *pControlDef = new SpyControlData();
//			pControlDef->SetControlID(varDef["ControlID"].int_value());
//			pControlDef->SetID(varDef["ID"].int_value());
//			pControlDef->SetName(varDef["Name"].string_value());
//			pControlDef->SetDescription(varDef["Description"].string_value());
//			map <string, Json> rectData = varDef["Rect"].object_items();
//			map <string, Json>::iterator itRectData = rectData.begin();
//			while (itRectData != rectData.end())
//			{
//				Json rectVal = itRectData->second;
////				sprintf_s(str, "\"Rect\": { \"Left\": %d, \"Top\": %d, \"Right\": %d, \"Bottom\": %d},",
//				itRectData++;
//			}
//			pControlDef->SetType(varDef["Type"].int_value());
//			pControlDef->SetClassname (varDef["ClassName"].string_value());
//			pControlDef->SetAutoID(varDef["AutoID"].string_value());
//			pControlDef->SetClickAnnotation(varDef["ClickAnnotation"].string_value());
//			pControlDef->SetEnterAnnotation(varDef["EnterAnnotation"].string_value());
//			pControlDef->SetUseName(varDef["UseName"].bool_value());
//			pControlDef->SetUseRect(varDef["UseRect"].bool_value());
//			pControlDef->SetUseValue(varDef["UseValue"].bool_value());
//			pControlDef->SetDisableFlg(varDef["Disable"].bool_value());
//			pControlDef->SetCreateWindow(varDef["CreatesWindow"].bool_value());
//			pControlDef->SetCloseWindow(varDef["ClosesWindow"].bool_value());
//			pWindowData->AddControl(pControlDef);
//		}
//
//		vector <Json> layoutDefs = jSonElement["Layouts"].array_items();
//		for (int j = 0; j < layoutDefs.size(); j++)
//		{
//			SpyWindowLayout *pLayoutDef = DBG_NEW SpyWindowLayout();
//			Json layoutDef = layoutDefs[j];
//			pLayoutDef->SetLayoutName(layoutDef["Name"].string_value());
//			vector <Json> proxyDefs = jSonElement["Controls"].array_items();
//			SetLayoutData(pLayoutDef->GetControlData(), &proxyDefs);
//			pWindowData->AddLayout(pLayoutDef);
//		}
//
//		windows.insert({ pWindowData->GetTitle(), pWindowData });
//
//	}
//}
//
//void SpyProcessData::SetLayoutData(vector < SpyControlProxy *> *pControlList, vector <Json> *pProxyDefs)
//{
//	vector <Json>::iterator itProxyDefs = pProxyDefs->begin();
//
//	while (itProxyDefs != pProxyDefs->end())
//	{
//		Json proxyDef = *itProxyDefs;
//		DWORD id = proxyDef["ControlID"].int_value();
//		SpyControlProxy *pControlProxy = DBG_NEW SpyControlProxy(id);
//		vector <Json> proxyDefs = proxyDef["Controls"].array_items();
//		SetLayoutData(pControlProxy->GetChildControls(), &proxyDefs);
//		pControlList->push_back(pControlProxy);
//		itProxyDefs++;
//	}
//}
//
//


void SpyProcessData::SetID(int value)
{
	ID = value;
}

void SpyProcessData::SetSystemName(string value)
{
	sysName = value;
}

void SpyProcessData::SetUserDefinedName(string value)
{
	userName = value;
}

void SpyProcessData::SetDescription(string value)
{
	description = value;
}

void SpyProcessData::SetType(string value)
{
	type = value;
}

void SpyProcessData::SetPlatform(string value)
{
	platform = value;
}

void SpyProcessData::SetURL(string value)
{
	url = value;
}

void SpyProcessData::SetIgnoreEvents(bool value)
{
	bIgnoreEvents = value;
}

void SpyProcessData::SetIgnoreVideo(bool value)
{
	bIgnoreVideo = value;
}

int SpyProcessData::GetID()
{
	return ID;
}

void SpyProcessData::SetFilePosition(long long value)
{
	fPos = value;
}

void SpyProcessData::SetFileNextProcessPosition(long long value)
{
	fPosNext = value;
}

void SpyProcessData::SetWindowFPos(long long value)
{
	fPosWindow = value;
}


string SpyProcessData::GetSystemName()
{
	return sysName;
}

string SpyProcessData::GetUserDefinedName()
{
	return userName;
}

string SpyProcessData::GetDescription()
{
	return description;
}

string SpyProcessData::GetType()
{
	return type;
}

string SpyProcessData::GetPlatform()
{
	return platform;
}

string SpyProcessData::GetURL()
{
	return url;
}

bool SpyProcessData::GetIgnoreEvents()
{
	return bIgnoreEvents;
}

bool SpyProcessData::GetIgnoreVideo()
{
	return bIgnoreVideo;
}

long long SpyProcessData::GetFilePosition()
{
	return fPos;
}

long long SpyProcessData::GetFileNextProcessPosition()
{
	return fPosNext;
}

long long SpyProcessData::GetWindowFPos()
{
	return fPosWindow;
}

//string SpyProcessData::GetWindowsJSON()
//{
//	string JSONstr = "";
//
//	map <string, SpyWindowData *>::iterator itvars = windows.begin();
//	bool bFirst = true;
//	if (itvars == windows.end())
//		return JSONstr;
//
//	JSONstr = JSONstr + "[";
//	while (itvars != windows.end())
//	{
//		char str[100];
//		SpyWindowData *pWindowData = itvars->second;
//		sprintf_s(str, "%d,", pWindowData->GetID());
//		if (!bFirst)
//			JSONstr += ", {\"ID\":";
//		else
//			JSONstr += "{\"ID\":";
//		bFirst = false;
//		JSONstr = JSONstr + str;
//		JSONstr = JSONstr + "\"Title\": \"";
//		JSONstr = JSONstr + pWindowData->GetTitle() + "\",";
//		JSONstr = JSONstr + "\"WindowName\": \"";
//		JSONstr = JSONstr + pWindowData->GetName() + "\",";
//		JSONstr = JSONstr + "\"Description\": \"";
//		JSONstr = JSONstr + pWindowData->GetDescription() + "\",";
//		JSONstr = JSONstr + "\"ClassName\": \"";
//		JSONstr = JSONstr + pWindowData->GetClassname() + "\",";
//		JSONstr = JSONstr + "\"AutomationID\": \"";
//		JSONstr = JSONstr + pWindowData->GetAutomationID() + "\",";
//		JSONstr = JSONstr + "\"Type\": \"";
//		JSONstr = JSONstr + pWindowData->GetWindowType() + "\",";
//		JSONstr = JSONstr + "\"MatchRule\": \"";
//		JSONstr = JSONstr + pWindowData->GetMatchingRule() + "\",";
//		JSONstr = JSONstr + "\"IncludeClassName\": ";
//		if (pWindowData->GetIncludeClassName())
//			JSONstr = JSONstr + "true,";
//		else
//			JSONstr = JSONstr + "false,";
//		JSONstr = JSONstr + "\"IncludeAutoID\": ";
//		if (pWindowData->GetIncludeAutomationID())
//			JSONstr = JSONstr + "true,";
//		else
//			JSONstr = JSONstr + "false,";
//		JSONstr = JSONstr + "\"Variables\": [";
//		vector <string> varKeys = pWindowData->GetVarNames();
//		vector <string>::iterator itVarKeys = varKeys.begin();
//		bool bVarFirst = true;
//		while (itVarKeys != varKeys.end())
//		{
//			string strKey = *itVarKeys;
//
//			string strRule = pWindowData->GetVarRule(strKey);
//			if (bVarFirst)
//				JSONstr = JSONstr + "{\"VarName\": \"" + strKey + "\", \"Rule\": \"" + strRule + "\"}";
//			else
//				JSONstr = JSONstr + ", {\"VarName\": \"" + strKey + "\", \"Rule\": \"" + strRule + "\"}";
//			itVarKeys++;
//		}
//
//		JSONstr = JSONstr + "],";
//		vector <SpyControlData *> pControls = pWindowData->GetControls();
//		vector <SpyControlData *>::iterator itControls = pControls.begin();
//		JSONstr = JSONstr + "\"Controls\": [";
//		while (itControls != pControls.end())
//		{
//			SpyControlData *pControlData = *itControls;
//			
//			string controlsString = "";
//			if (itControls == pControls.begin())
//			{
//				controlsString = "{";
//			}
//			else
//			{
//				controlsString = ",{";
//			}
//			JSONstr = JSONstr + controlsString + "\"ControlID\": ";
//			sprintf_s(str, "%d,", pControlData->GetControlID());
//			JSONstr = JSONstr + str;
//			sprintf_s(str, "\"ID\":%d,", pControlData->GetID());
//			JSONstr = JSONstr + str;
//			JSONstr = JSONstr + "\"Name\": " + pControlData->GetName() + ",";
//			JSONstr = JSONstr + "\"Description\": " + pControlData->GetDescription() + ",";
//			sprintf_s(str, "\"Type\":%d,", pControlData->GetType());
//			JSONstr = JSONstr + str;
//			sprintf_s(str, "\"Rect\": { \"Left\": %d, \"Top\": %d, \"Right\": %d, \"Bottom\": %d},",
//				pControlData->GetRect()->left,
//				pControlData->GetRect()->top,
//				pControlData->GetRect()->right,
//				pControlData->GetRect()->bottom);
//			JSONstr = JSONstr + str;
//			JSONstr = JSONstr + "\"ClassName\": " + pControlData->GetClassname() + ",";
//			JSONstr = JSONstr + "\"AutoID\": " + pControlData->GetAutoID() + ",";
//			JSONstr = JSONstr + "\"ClickAnnotation\": " + pControlData->GetClickAnnotation() + ",";
//			JSONstr = JSONstr + "\"EnterAnnotation\": " + pControlData->GetEnterAnnotation() + ",";
//			if (pControlData->GetUseName())
//				JSONstr = JSONstr + "\"UseName\": true,";
//			else
//				JSONstr = JSONstr + "\"UseName\": false,";
//			if (pControlData->GetUseRect())
//				JSONstr = JSONstr + "\"UseRect\": true,";
//			else
//				JSONstr = JSONstr + "\"UseRect\": false,";
//			if (pControlData->GetUseValue())
//				JSONstr = JSONstr + "\"UseValue\": true,";
//			else
//				JSONstr = JSONstr + "\"UseValue\": false,";
//			if (pControlData->GetDisableFlg())
//				JSONstr = JSONstr + "\"Disable\": true,";
//			else
//				JSONstr = JSONstr + "\"Disable\": false,";
//			if (pControlData->GetCreateWindow())
//				JSONstr = JSONstr + "\"CreatesWindow\": true,";
//			else
//				JSONstr = JSONstr + "\"CreatesWindow\": false,";
//			if (pControlData->GetCloseWindow())
//				JSONstr = JSONstr + "\"ClosesWindow\": true";
//			else
//				JSONstr = JSONstr + "\"ClosesWindow\": false";
//			JSONstr += "}";
//			itControls++;
//		}
//		JSONstr = JSONstr + "],";
//		vector <string> pLayouts = pWindowData->GetLayouts();
//		vector <string>::iterator itLayouts = pLayouts.begin();
//		JSONstr = JSONstr + "\"Layouts\": [";
//		while (itLayouts != pLayouts.end())
//		{
//			string layoutName = *itLayouts;
//			SpyWindowLayout *pLayoutDef = pWindowData->GetLayout(layoutName);
//			string controlsString = "";
//			if (itLayouts == pLayouts.begin())
//			{
//				controlsString = "{";
//			}
//			else
//			{
//				controlsString = ",{";
//			}
//			JSONstr = JSONstr + "\"Name\": " + pLayoutDef->GetLayoutName() + ",";
//			vector <SpyControlProxy *> *controlData = pLayoutDef->GetControlData();
//			JSONstr = JSONstr + GetControlData(controlData);
//			itLayouts++;
//		}
//		JSONstr = JSONstr + "],";
//		JSONstr = JSONstr + "\"AnnotationRule\": \"";
//		JSONstr = JSONstr + pWindowData->GetAnnotationRule() + "\"";
//		JSONstr = JSONstr + "}";
//		itvars++;
//	}
//	JSONstr = JSONstr + "]";
//	return JSONstr;
//
//}
//
//
//string SpyProcessData::GetControlData(vector <SpyControlProxy *> *pControlList)
//{
//	string JSONstr = "";
//	vector <SpyControlProxy *>::iterator itControlProxy = pControlList->begin();
//	JSONstr = JSONstr + "\"Controls\": [";
//	while (itControlProxy != pControlList->end())
//	{
//		string controlsString = "";
//		if (itControlProxy == pControlList->begin())
//		{
//			controlsString = "{";
//		}
//		else
//		{
//			controlsString = ",{";
//		}
//		JSONstr += controlsString;
//		SpyControlProxy *pControlProxy = *itControlProxy;
//		char str[100];
//		sprintf_s(str, "\"ControlID\": %ld,", pControlProxy->GetControlID());
//		JSONstr = JSONstr + str;
//		vector <SpyControlProxy *> *controlData = pControlProxy->GetChildControls();
//		JSONstr = JSONstr + GetControlData(controlData);
//		itControlProxy++;
//	}
//	return JSONstr;
//}
