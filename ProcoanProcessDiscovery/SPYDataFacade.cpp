#include "stdafx.h"
#include "SPYDataFacade.h"
#include "DataFile.h"
#include "Utility.h"
#include <string>
#include <list>
#include "SpyProcessData.h"
#include "SpyControlData.h"
#include "CaptureElementData.h"
#include "SpyMaskData.h"
#include "SpyUrlData.h"
#include <chrono>
#include <io.h>
#include <regex>

using namespace std;

SPYDataFacade::SPYDataFacade()
{
	string fileName = getcurrentpath() + "resources\\SensAI.dat";
	if (_access_s(fileName.c_str(), 0) == 0)
		pDataFile = DBG_NEW DataFile((char *)fileName.c_str(), FALSE);
	else
		pDataFile = DBG_NEW DataFile((char *)fileName.c_str(), TRUE);
	mapProcessData.clear();
	ZeroMemory(&rootIndex, sizeof(struct spyRoot));
}

SPYDataFacade::SPYDataFacade(char *fileName)
{
	if (_access_s(fileName, 0) == 0)
		pDataFile = DBG_NEW DataFile(fileName, FALSE);
	else
		pDataFile = DBG_NEW DataFile(fileName, TRUE);
	mapProcessData.clear();
	ZeroMemory(&rootIndex, sizeof(struct spyRoot));
}

void SPYDataFacade::ClearProcessData()
{
	SpyProcessData *pProcessData = NULL;
	map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.begin();
	while (itMapProcessData != mapProcessData.end())
	{
		pProcessData = itMapProcessData->second;
		delete pProcessData;
		itMapProcessData++;
	}
	mapProcessData.clear();
}

vector <SpyControlData *> SPYDataFacade::GetControls()
{
	vector <SpyControlData *> controlList;
	controlList.clear();

	map <DWORD, SpyControlData *>::iterator itControls = mapControlData.begin();
	while (itControls != mapControlData.end())
	{
		SpyControlData *pControlData = itControls->second;
		controlList.push_back(pControlData);
		itControls++;
	}

	return controlList;
}


DWORD SPYDataFacade::GetNextControlID()
{
	DWORD val = mapControlData.size() + 1;
	return val;
}

void SPYDataFacade::WriteControlData(SpyControlData *pControl)
{
	long long fPos = pControl->GetFilePosition();
	if (fPos == NULL)
		return;
	struct controlRecord controlIndex;
	ZeroMemory(&controlIndex, sizeof(struct controlRecord));

	pDataFile->ReadFromFile((char *)&controlIndex, sizeof(struct controlRecord), fPos);
	string JSONStr = pControl->GetJSonString();
	if (JSONStr.size() > 0)
	{
		controlIndex.dataSize = (ULONG)(JSONStr.size() + 10);
		controlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)JSONStr.c_str(), controlIndex.dataSize, &controlIndex.compSize);
	}
	pDataFile->WriteToFile(fPos, (char *)&controlIndex, sizeof(struct controlRecord));
}

void SPYDataFacade::AddControl(SpyControlData *pControlData)
{
	mapControlData.insert({ pControlData->GetControlID(), pControlData });
}


void SPYDataFacade::AddWindowMask(string windowName, SpyMaskData *pMaskData)
{
	long long fPos = pMaskData->GetFilePosition();
	struct maskRecord maskIndex;
	ZeroMemory(&maskIndex, sizeof(struct maskRecord));

	if (fPos == NULL)
	{
		fPos = pDataFile->AppendToFile((char *)&maskIndex, sizeof(struct maskRecord));
		mapMaskData.insert({ pMaskData->GetMaskName(), pMaskData });
	}
	else
	{
		pDataFile->ReadFromFile((char *)&maskIndex, sizeof(struct maskRecord), fPos);
	}

	string val = pMaskData->GetMaskName();
	CopyMemory(maskIndex.maskName, val.c_str(), val.length());
	val = pMaskData->GetMaskType();
	CopyMemory(maskIndex.maskType, val.c_str(), val.length());

	RECT wRect = pMaskData->GetImageRect();
	maskIndex.imageRect.top = wRect.top;
	maskIndex.imageRect.left = wRect.left;
	maskIndex.imageRect.bottom = wRect.bottom;
	maskIndex.imageRect.right = wRect.right;

	wRect = pMaskData->GetImage1Rect();
	maskIndex.match1Rect.top = wRect.top;
	maskIndex.match1Rect.left = wRect.left;
	maskIndex.match1Rect.bottom = wRect.bottom;
	maskIndex.match1Rect.right = wRect.right;

	wRect = pMaskData->GetImage2Rect();
	maskIndex.match2Rect.top = wRect.top;
	maskIndex.match2Rect.left = wRect.left;
	maskIndex.match2Rect.bottom = wRect.bottom;
	maskIndex.match2Rect.right = wRect.right;

	wRect = pMaskData->GetImage3Rect();
	maskIndex.match3Rect.top = wRect.top;
	maskIndex.match3Rect.left = wRect.left;
	maskIndex.match3Rect.bottom = wRect.bottom;
	maskIndex.match3Rect.right = wRect.right;

	wRect = pMaskData->GetImage4Rect();
	maskIndex.match4Rect.top = wRect.top;
	maskIndex.match4Rect.left = wRect.left;
	maskIndex.match4Rect.bottom = wRect.bottom;
	maskIndex.match4Rect.right = wRect.right;

	wRect = pMaskData->GetImage5Rect();
	maskIndex.match5Rect.top = wRect.top;
	maskIndex.match5Rect.left = wRect.left;
	maskIndex.match5Rect.bottom = wRect.bottom;
	maskIndex.match5Rect.right = wRect.right;

	wRect = pMaskData->GetImage6Rect();
	maskIndex.match6Rect.top = wRect.top;
	maskIndex.match6Rect.left = wRect.left;
	maskIndex.match6Rect.bottom = wRect.bottom;
	maskIndex.match6Rect.right = wRect.right;

	wRect = pMaskData->GetMaskRect();
	maskIndex.maskRect.top = wRect.top;
	maskIndex.maskRect.left = wRect.left;
	maskIndex.maskRect.bottom = wRect.bottom;
	maskIndex.maskRect.right = wRect.right;

	char *pImageBuffer = pMaskData->GetMaskImage(&maskIndex.bfh, &maskIndex.bmiHeader);
	int width = maskIndex.bmiHeader.biWidth;
	int height = maskIndex.bmiHeader.biHeight;
	if (height < 0)
		height = -height;


	maskIndex.dataSize = width * height * (maskIndex.bmiHeader.biBitCount / 8);
	maskIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)pImageBuffer, maskIndex.dataSize, &maskIndex.compSize);

	pDataFile->WriteToFile(fPos, (char *)&maskIndex, sizeof(struct maskRecord));

	if (pMaskData->GetFilePosition() == NULL)
	{
		// Update the previous Record
		SpyMaskData *pLastMask = GetLastMaskRecord();
		if (pLastMask == NULL)
		{
			SpyWindowData* pWindow = GetProcessWindow(windowName);
			long long fPosWindow = pWindow->GetFilePosition();
			struct windowRecord windowIndex;
			ZeroMemory((char *)&windowIndex, sizeof(struct windowRecord));
			pDataFile->ReadFromFile((char *)&windowIndex, sizeof(struct windowRecord), fPosWindow);
			pWindow->SetMaskFPos(fPos);
			windowIndex.fPosMask = fPos;
			pDataFile->WriteToFile(fPosWindow, (char *)&windowIndex, sizeof(struct windowRecord));
		}
		else
		{
			long long fPosPrevMask = pLastMask->GetFilePosition();
			struct maskRecord prevMaskIndex;
			ZeroMemory((char *)&prevMaskIndex, sizeof(struct maskRecord));
			pDataFile->ReadFromFile((char *)&prevMaskIndex, sizeof(struct maskRecord), fPosPrevMask);
			pLastMask->SetFileNextMaskPosition(fPos);
			prevMaskIndex.fPosNextMask = fPos;
			pDataFile->WriteToFile(fPosPrevMask, (char *)&prevMaskIndex, sizeof(struct maskRecord));
		}

	}
	pMaskData->SetFilePosition(fPos);

}

void SPYDataFacade::AddURLMask(string urlName, SpyMaskData* pMaskData)
{
	long long fPos = pMaskData->GetFilePosition();
	struct maskRecord maskIndex;
	ZeroMemory(&maskIndex, sizeof(struct maskRecord));

	if (fPos == NULL)
	{
		fPos = pDataFile->AppendToFile((char*)&maskIndex, sizeof(struct maskRecord));
		mapMaskData.insert({ pMaskData->GetMaskName(), pMaskData });
	}
	else
	{
		pDataFile->ReadFromFile((char*)&maskIndex, sizeof(struct maskRecord), fPos);
	}

	string val = pMaskData->GetMaskName();
	CopyMemory(maskIndex.maskName, val.c_str(), val.length());
	val = pMaskData->GetMaskType();
	CopyMemory(maskIndex.maskType, val.c_str(), val.length());

	RECT wRect = pMaskData->GetImageRect();
	maskIndex.imageRect.top = wRect.top;
	maskIndex.imageRect.left = wRect.left;
	maskIndex.imageRect.bottom = wRect.bottom;
	maskIndex.imageRect.right = wRect.right;

	wRect = pMaskData->GetImage1Rect();
	maskIndex.match1Rect.top = wRect.top;
	maskIndex.match1Rect.left = wRect.left;
	maskIndex.match1Rect.bottom = wRect.bottom;
	maskIndex.match1Rect.right = wRect.right;

	wRect = pMaskData->GetImage2Rect();
	maskIndex.match2Rect.top = wRect.top;
	maskIndex.match2Rect.left = wRect.left;
	maskIndex.match2Rect.bottom = wRect.bottom;
	maskIndex.match2Rect.right = wRect.right;

	wRect = pMaskData->GetImage3Rect();
	maskIndex.match3Rect.top = wRect.top;
	maskIndex.match3Rect.left = wRect.left;
	maskIndex.match3Rect.bottom = wRect.bottom;
	maskIndex.match3Rect.right = wRect.right;

	wRect = pMaskData->GetImage4Rect();
	maskIndex.match4Rect.top = wRect.top;
	maskIndex.match4Rect.left = wRect.left;
	maskIndex.match4Rect.bottom = wRect.bottom;
	maskIndex.match4Rect.right = wRect.right;

	wRect = pMaskData->GetImage5Rect();
	maskIndex.match5Rect.top = wRect.top;
	maskIndex.match5Rect.left = wRect.left;
	maskIndex.match5Rect.bottom = wRect.bottom;
	maskIndex.match5Rect.right = wRect.right;

	wRect = pMaskData->GetImage6Rect();
	maskIndex.match6Rect.top = wRect.top;
	maskIndex.match6Rect.left = wRect.left;
	maskIndex.match6Rect.bottom = wRect.bottom;
	maskIndex.match6Rect.right = wRect.right;

	wRect = pMaskData->GetMaskRect();
	maskIndex.maskRect.top = wRect.top;
	maskIndex.maskRect.left = wRect.left;
	maskIndex.maskRect.bottom = wRect.bottom;
	maskIndex.maskRect.right = wRect.right;

	char* pImageBuffer = pMaskData->GetMaskImage(&maskIndex.bfh, &maskIndex.bmiHeader);
	int width = maskIndex.bmiHeader.biWidth;
	int height = maskIndex.bmiHeader.biHeight;
	if (height < 0)
		height = -height;


	maskIndex.dataSize = width * height * (maskIndex.bmiHeader.biBitCount / 8);
	maskIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char*)pImageBuffer, maskIndex.dataSize, &maskIndex.compSize);

	pDataFile->WriteToFile(fPos, (char*)&maskIndex, sizeof(struct maskRecord));

	if (pMaskData->GetFilePosition() == NULL)
	{
		// Update the previous Record
		SpyMaskData* pLastMask = GetLastMaskRecord();
		if (pLastMask == NULL)
		{
			SpyUrlData* pURL = GetUrlData(urlName);
			long long fPosURL = pURL->GetFilePosition();
			struct URLRecord urlIndex;
			ZeroMemory((char*)&urlIndex, sizeof(struct URLRecord));
			pDataFile->ReadFromFile((char*)&urlIndex, sizeof(struct windowRecord), fPosURL);
			pURL->SetMaskFPos(fPos);
			urlIndex.fPosMask = fPos;
			pDataFile->WriteToFile(fPosURL, (char*)&urlIndex, sizeof(struct windowRecord));
		}
		else
		{
			long long fPosPrevMask = pLastMask->GetFilePosition();
			struct maskRecord prevMaskIndex;
			ZeroMemory((char*)&prevMaskIndex, sizeof(struct maskRecord));
			pDataFile->ReadFromFile((char*)&prevMaskIndex, sizeof(struct maskRecord), fPosPrevMask);
			pLastMask->SetFileNextMaskPosition(fPos);
			prevMaskIndex.fPosNextMask = fPos;
			pDataFile->WriteToFile(fPosPrevMask, (char*)&prevMaskIndex, sizeof(struct maskRecord));
		}

	}
	pMaskData->SetFilePosition(fPos);

}

SpyMaskData *SPYDataFacade::GetMaskData(string name)
{
	SpyMaskData *pMaskData = NULL;

	map <string, SpyMaskData *>::iterator itMask = mapMaskData.find(name);
	if (itMask != mapMaskData.end())
	{
		pMaskData = itMask->second;
	}
	return pMaskData;
}



void SPYDataFacade::ClearWindowData()
{
	SpyWindowData *pWindowData = NULL;
	map <string, SpyWindowData *>::iterator itMapWindowData = mapWindowData.begin();
	while (itMapWindowData != mapWindowData.end())
	{
		pWindowData = itMapWindowData->second;
		delete pWindowData;
		itMapWindowData++;
	}
	mapWindowData.clear();
}

void SPYDataFacade::ClearMaskData()
{
	SpyMaskData *pMaskData = NULL;
	map <string, SpyMaskData *>::iterator itMapMaskData = mapMaskData.begin();
	while (itMapMaskData != mapMaskData.end())
	{
		pMaskData = itMapMaskData->second;
		delete pMaskData;
		itMapMaskData++;
	}
	mapMaskData.clear();
}

void SPYDataFacade::ClearControlsData()
{
	SpyControlData *pControlData = NULL;
	map <DWORD, SpyControlData *>::iterator itMapControlData = mapControlData.begin();
	while (itMapControlData != mapControlData.end())
	{
		pControlData = itMapControlData->second;
		delete pControlData;
		itMapControlData++;
	}
	mapControlData.clear();
}


void SPYDataFacade::ReadDataFile()
{
	// Clear Map objects

	map <string, SpyProcessData*>::iterator itProcess = mapProcessData.begin();
	while (itProcess != mapProcessData.end())
	{
		SpyProcessData* pProcess = itProcess->second;
		delete pProcess;
		itProcess++;
	}
	mapProcessData.clear();

	map <string, SpyWindowData*>::iterator itWindow = mapWindowData.begin();
	while (itWindow != mapWindowData.end())
	{
		SpyWindowData* pPtr = itWindow->second;
		delete pPtr;
		itWindow++;
	}
	mapWindowData.clear();

	map <DWORD, SpyControlData*>::iterator itControl = mapControlData.begin();
	while (itControl != mapControlData.end())
	{
		SpyControlData* pPtr = itControl->second;
		delete pPtr;
		itControl++;
	}
	mapControlData.clear();

	map <string, SpyMaskData*>::iterator itMask = mapMaskData.begin();
	while (itMask != mapMaskData.end())
	{
		SpyMaskData* pPtr = itMask->second;
		delete pPtr;
		itMask++;
	}
	mapMaskData.clear();

	map <string, SpyUrlData*>::iterator itUrlData = mapUrlData.begin();
	while (itUrlData != mapUrlData.end())
	{
		SpyUrlData* pPtr = itUrlData->second;
		delete pPtr;
		itUrlData++;
	}
	mapUrlData.clear();

	// Now read the file


	ZeroMemory(&rootIndex, sizeof(struct spyRoot));
	pDataFile->ReadFromFile((char *)&rootIndex, sizeof(struct spyRoot), 0L);
	string strKey = rootIndex.key;
	struct processRecord prcIndex;
	struct URLRecord urlIndex;

	ZeroMemory(&prcIndex, sizeof(struct processRecord));
	ZeroMemory(&urlIndex, sizeof(struct URLRecord));

	if (strKey != "SensAI")
	{
		return;
	}

	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 204800L);
	long long fPos = rootIndex.fPosProcessData;
	while (fPos != NULL)
	{
		pDataFile->ReadFromFile((char *)&prcIndex, sizeof(struct processRecord), fPos);
		long long fPosData = prcIndex.fPosData;
		pDataFile->ReadDeflateBlockFromFile(pBuffer, prcIndex.dataSize, prcIndex.compSize, fPosData);
		string strJSON = pBuffer;
		SpyProcessData *pProcessData = DBG_NEW SpyProcessData(strJSON);
		pProcessData->SetFilePosition(fPos);
		pProcessData->SetFileNextProcessPosition(prcIndex.fPosNextProcess);
		pProcessData->SetWindowFPos(prcIndex.fPosWindow);
		string key = pProcessData->GetSystemName();
		map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.find(key);
		if (itMapProcessData == mapProcessData.end())
		{
			mapProcessData.insert(pair<string, SpyProcessData *>(key, pProcessData));
		}
		else
		{
			// delete since already exists and not saved in map
			delete pProcessData;
		}
		fPos = prcIndex.fPosNextProcess;
	}

	fPos = rootIndex.fPosURLData;
	while (fPos != NULL)
	{
		pDataFile->ReadFromFile((char *)&urlIndex, sizeof(struct URLRecord), fPos);
		long long fPosData = urlIndex.fPosData;
		pDataFile->ReadDeflateBlockFromFile(pBuffer, urlIndex.dataSize, urlIndex.compSize, fPosData);
		string strJSON = pBuffer;
		SpyUrlData *pURLData = DBG_NEW SpyUrlData(strJSON);
		pURLData->SetFilePosition(fPos);
		pURLData->SetFileNextURLPosition(urlIndex.fPosNextURL);
		pURLData->SetControlFPos(urlIndex.fPosControls);
		pURLData->SetMaskFPos(urlIndex.fPosMask);
		string key = pURLData->GetURL();
		map <string, SpyUrlData *>::iterator itMapUrlData = mapUrlData.find(key);
		if (itMapUrlData == mapUrlData.end())
		{
			mapUrlData.insert(pair<string, SpyUrlData *>(key, pURLData));
		}
		else
		{
			// Delete since already exists and not stored in map
			delete pURLData;
		}
		fPos = urlIndex.fPosNextURL;
	}

	HeapFree(GetProcessHeap(), 0, pBuffer);
}

void SPYDataFacade::WriteProcessDataFile(SpyProcessData *pProcessData)
{
	char str[100];
	string jSONVal = "{";

	jSONVal = jSONVal + "\"ID\": \"";
	sprintf_s(str, "%d", pProcessData->GetID());
	jSONVal = jSONVal + str;
	jSONVal = jSONVal + "\",";
	jSONVal = jSONVal + "\"SysName\":\"" + pProcessData->GetSystemName() +"\",";
	jSONVal = jSONVal + "\"UserName\":\"" + pProcessData->GetUserDefinedName() + "\",";
	jSONVal = jSONVal + "\"Description\":\"" + pProcessData->GetDescription() +"\",";
	jSONVal = jSONVal + "\"Type\":\"" + pProcessData->GetType() +"\",";
	jSONVal = jSONVal + "\"Platform\":\"" + pProcessData->GetPlatform() +"\",";
	jSONVal = jSONVal + "\"URL\":\"" + pProcessData->GetURL() +"\",";

	if (pProcessData->GetIgnoreVideo())
		jSONVal = jSONVal + "\"IgnoreEvents\":true,";
	else
		jSONVal = jSONVal + "\"IgnoreEvents\":false,";

	if (pProcessData->GetIgnoreVideo())
		jSONVal = jSONVal + "\"IgnoreVideo\":true";
	else
		jSONVal = jSONVal + "\"IgnoreVideo\":false";

	jSONVal = jSONVal + "}";

	//string windowJSON = pProcessData->GetWindowsJSON();

	string strKey = rootIndex.key;
	struct processRecord prcIndex;
	ZeroMemory(&prcIndex, sizeof(struct processRecord));
	prcIndex.recType = 'P';
	if (strKey != "SensAI")
	{
		ZeroMemory(&rootIndex, sizeof(struct spyRoot));
		strcpy_s(rootIndex.key, "SensAI");
		pDataFile->AppendToFile((char *)&rootIndex, sizeof(struct spyRoot));
		ZeroMemory(&prcIndex, sizeof(struct processRecord));
		prcIndex.dataSize = jSONVal.size() + 10;
		prcIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), prcIndex.dataSize, &prcIndex.compSize);
		rootIndex.fPosProcessData = pDataFile->AppendToFile((char *)&prcIndex, sizeof(struct processRecord));
		pProcessData->SetFilePosition(rootIndex.fPosProcessData);
	}
	else
	{
		if (IsProcessPresent(pProcessData->GetSystemName()))
		{
			pDataFile->ReadFromFile((char *)&prcIndex, sizeof(struct processRecord), pProcessData->GetFilePosition());
			prcIndex.dataSize = jSONVal.size() + 10;
			prcIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), prcIndex.dataSize, &prcIndex.compSize);
			pDataFile->WriteToFile(pProcessData->GetFilePosition(), (char *)&prcIndex, sizeof(struct processRecord));
		}
		else
		{
			long long fPosLast = 0L;
			SpyProcessData *pLastRecord = GetLastProcessRecord();
			
			prcIndex.dataSize = jSONVal.size() + 10;
			long long newRecPos = 0L;
			if (pLastRecord != NULL)
			{
				fPosLast = pLastRecord->GetFilePosition();
				prcIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), prcIndex.dataSize, &prcIndex.compSize);
				newRecPos = pDataFile->AppendToFile((char *)&prcIndex, sizeof(struct processRecord));
				pLastRecord->SetFileNextProcessPosition(newRecPos);
				pProcessData->SetFilePosition(newRecPos);
				struct processRecord prevPrcIndex;
				pDataFile->ReadFromFile((char *)&prevPrcIndex, sizeof(struct processRecord), fPosLast);
				prevPrcIndex.fPosNextProcess = newRecPos;
				pDataFile->WriteToFile(fPosLast, (char *)&prevPrcIndex, sizeof(struct processRecord));
			}
			else
			{
				prcIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), prcIndex.dataSize, &prcIndex.compSize);
				newRecPos = pDataFile->AppendToFile((char *)&prcIndex, sizeof(struct processRecord));
				pProcessData->SetFilePosition(newRecPos);
				rootIndex.fPosProcessData = newRecPos;
			}
			pDataFile->WriteToFile(newRecPos, (char *)&prcIndex, sizeof(struct processRecord));
		}
	}
	pDataFile->WriteToFile(0L, (char *)&rootIndex, sizeof(struct spyRoot));
}


void SPYDataFacade::WriteURLDataFile(SpyUrlData *pURLData)
{
	char str[100];
	string jSONVal = "{";

	jSONVal = jSONVal + "\"ID\": \"";
	sprintf_s(str, "%d", pURLData->GetID());
	jSONVal = jSONVal + str;
	jSONVal = jSONVal + "\",";
	jSONVal = jSONVal + "\"Rule\":\"" + EscapeJSonAnnotationString(pURLData->GetRule()) + "\",";
	jSONVal = jSONVal + "\"UserName\":\"" + pURLData->GetUserDefinedName() + "\",";
	jSONVal = jSONVal + "\"Description\":\"" + pURLData->GetDescription() + "\",";
	jSONVal = jSONVal + "\"AppName\":\"" + pURLData->GetApplicationName() + "\",";
	jSONVal = jSONVal + "\"AppDesc\":\"" + pURLData->GetApplicationDescription() + "\",";
	jSONVal = jSONVal + "\"URL\":\"" + pURLData->GetURL() + "\",";
	
	if (pURLData->GetIgnoreVideo())
		jSONVal = jSONVal + "\"IgnoreEvents\":true,";
	else
		jSONVal = jSONVal + "\"IgnoreEvents\":false,";

	if (pURLData->GetIgnoreVideo())
		jSONVal = jSONVal + "\"IgnoreVideo\":true,";
	else
		jSONVal = jSONVal + "\"IgnoreVideo\":false,";

	if (pURLData->GetHomePage())
		jSONVal = jSONVal + "\"IsHomePage\":true";
	else
		jSONVal = jSONVal + "\"IsHomePage\":false";

	jSONVal = jSONVal + "}";

	string strKey = rootIndex.key;
	struct URLRecord urlIndex;
	ZeroMemory(&urlIndex, sizeof(struct URLRecord));
	urlIndex.recType = 'U';

	if (strKey != "SensAI")
	{
		ZeroMemory(&rootIndex, sizeof(struct spyRoot));
		strcpy_s(rootIndex.key, "SensAI");
		pDataFile->AppendToFile((char *)&rootIndex, sizeof(struct spyRoot));
		ZeroMemory(&urlIndex, sizeof(struct URLRecord));
		urlIndex.dataSize = jSONVal.size() + 10;
		urlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), urlIndex.dataSize, &urlIndex.compSize);
		rootIndex.fPosURLData = pDataFile->AppendToFile((char *)&urlIndex, sizeof(struct URLRecord));
		pURLData->SetFilePosition(rootIndex.fPosURLData);
	}
	else
	{

		if (GetUrlData(pURLData->GetURL()) != NULL)
		{
			pDataFile->ReadFromFile((char *)&urlIndex, sizeof(struct URLRecord), pURLData->GetFilePosition());
			urlIndex.dataSize = jSONVal.size() + 10;
			urlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), urlIndex.dataSize, &urlIndex.compSize);
			pDataFile->WriteToFile(pURLData->GetFilePosition(), (char *)&urlIndex, sizeof(struct URLRecord));
		}
		else
		{
			long long fPosLast = 0L;
			SpyUrlData *pLastRecord = GetLastURLRecord();

			urlIndex.dataSize = jSONVal.size() + 10;
			long long newRecPos = 0L;
			if (pLastRecord != NULL)
			{
				fPosLast = pLastRecord->GetFilePosition();
				urlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), urlIndex.dataSize, &urlIndex.compSize);
				newRecPos = pDataFile->AppendToFile((char *)&urlIndex, sizeof(struct URLRecord));
				pLastRecord->SetFileNextURLPosition(newRecPos);
				pURLData->SetFilePosition(newRecPos);
				struct URLRecord prevURLIndex;
				pDataFile->ReadFromFile((char *)&prevURLIndex, sizeof(struct URLRecord), fPosLast);
				prevURLIndex.fPosNextURL = newRecPos;
				pDataFile->WriteToFile(fPosLast, (char *)&prevURLIndex, sizeof(struct URLRecord));
			}
			else
			{
				urlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)jSONVal.c_str(), urlIndex.dataSize, &urlIndex.compSize);
				newRecPos = pDataFile->AppendToFile((char *)&urlIndex, sizeof(struct URLRecord));
				pURLData->SetFilePosition(newRecPos);
				rootIndex.fPosURLData = newRecPos;
			}
			pDataFile->WriteToFile(newRecPos, (char *)&urlIndex, sizeof(struct URLRecord));
		}
	}
	pDataFile->WriteToFile(0L, (char *)&rootIndex, sizeof(struct spyRoot));
}


void SPYDataFacade::WriteWindowDataFile(SpyProcessData *pProcessData, SpyWindowData *pWindowData)
{
	struct windowRecord wndIndex;
	ZeroMemory(&wndIndex, sizeof(struct windowRecord));
	bool bNewRecord = false;
	long long fPos = pWindowData->GetFilePosition();
	if (fPos != NULL)
	{
		pDataFile->ReadFromFile((char *)&wndIndex, sizeof(struct windowRecord), fPos);
	}
	else
	{
		bNewRecord = true;
		fPos = pDataFile->AppendToFile((char *)&wndIndex, sizeof(struct windowRecord));
	}
	pWindowData->SetFilePosition(fPos);
	wndIndex.recType = 'W';
	string JSONstr = "";
	
	char str[100];
	sprintf_s(str, "%d,", pWindowData->GetID());
	JSONstr += "{\"ID\":";
	JSONstr = JSONstr + str;
	JSONstr = JSONstr + "\"Title\": \"";
	JSONstr = JSONstr + pWindowData->GetTitle() + "\",";
	JSONstr = JSONstr + "\"WindowName\": \"";
	JSONstr = JSONstr + pWindowData->GetName() + "\",";
	JSONstr = JSONstr + "\"Description\": \"";
	JSONstr = JSONstr + pWindowData->GetDescription() + "\",";
	JSONstr = JSONstr + "\"ClassName\": \"";
	JSONstr = JSONstr + pWindowData->GetClassname() + "\",";
	JSONstr = JSONstr + "\"AutomationID\": \"";
	JSONstr = JSONstr + pWindowData->GetAutomationID() + "\",";
	JSONstr = JSONstr + "\"Type\": \"";
	JSONstr = JSONstr + pWindowData->GetWindowType() + "\",";
	JSONstr = JSONstr + "\"MatchRule\": \"";
	JSONstr = JSONstr + pWindowData->GetMatchingRule() + "\",";
	JSONstr = JSONstr + "\"IsMainWindow\": ";
	if (pWindowData->GetMainWindowFlg())
		JSONstr = JSONstr + "true";
	else
		JSONstr = JSONstr + "false";
	JSONstr = JSONstr + "}";

	string strJSONVar = JSONstr;
	if (strJSONVar.size() > 0)
	{
		wndIndex.dataSize = (ULONG)(strJSONVar.size() + 10);
		wndIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char *)strJSONVar.c_str(), wndIndex.dataSize, &wndIndex.compSize);
	}
	pDataFile->WriteToFile(fPos, (char *)&wndIndex, sizeof(struct windowRecord));

	// Update the last window record to point to updated record if a new window has been added
	if (bNewRecord)
	{
		SpyWindowData *pLastWindow = GetLastWindowRecord();
		if (pLastWindow == NULL)
		{
			long long fPosIndex = pProcessData->GetFilePosition();
			struct processRecord prcIndex;
			ZeroMemory(&prcIndex, sizeof(struct processRecord));
			pDataFile->ReadFromFile((char *)&prcIndex, sizeof(struct processRecord), fPosIndex);
			prcIndex.fPosWindow = fPos;
			pDataFile->WriteToFile(fPosIndex, (char *)&prcIndex, sizeof(struct processRecord));
			pProcessData->SetWindowFPos(fPos);
		}
		else
		{
			pLastWindow->SetFileNextWindowPosition(fPos);
			long long fPosIndex = pLastWindow->GetFilePosition();
			pDataFile->ReadFromFile((char *)&wndIndex, sizeof(struct windowRecord), fPosIndex);
			wndIndex.fPosNextWindow = fPos;
			pDataFile->WriteToFile(fPosIndex, (char *)&wndIndex, sizeof(struct windowRecord));
		}
	}
}

SpyUrlData *SPYDataFacade::GetLastURLRecord()
{
	map <string, SpyUrlData *>::iterator itMapUrlData = mapUrlData.begin();
	SpyUrlData *pURLData = NULL;
	while (itMapUrlData != mapUrlData.end())
	{
		pURLData = itMapUrlData->second;
		if (pURLData->GetFileNextURLPosition() == 0LL && pURLData->GetFilePosition() != NULL)
			break;
		else
			pURLData = NULL;
		itMapUrlData++;
	}

	return pURLData;
}


SpyProcessData *SPYDataFacade::GetLastProcessRecord()
{
	map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.begin();
	SpyProcessData *pProcessData = NULL;
	while (itMapProcessData != mapProcessData.end())
	{
		pProcessData = itMapProcessData->second;
		if (pProcessData->GetFileNextProcessPosition() == 0LL && pProcessData->GetFilePosition() != NULL)
			break;
		else
			pProcessData = NULL;
		itMapProcessData++;
	}
	
	return pProcessData;
}

SpyWindowData *SPYDataFacade::GetLastWindowRecord()
{
	map <string, SpyWindowData *>::iterator itMapWindowData = mapWindowData.begin();
	SpyWindowData *pWindowData = NULL;
	while (itMapWindowData != mapWindowData.end())
	{
		pWindowData = itMapWindowData->second;
		if (pWindowData->GetFileNextWindowPosition() == 0LL && pWindowData->GetFilePosition() != 0LL)
			break;
		else
			pWindowData = NULL;
		itMapWindowData++;
	}

	return pWindowData;
}


SpyControlData *SPYDataFacade::GetLastControlRecord()
{
	map <DWORD, SpyControlData *>::iterator itMapControlData = mapControlData.begin();
	SpyControlData *pControlData = NULL;
	while (itMapControlData != mapControlData.end())
	{
		pControlData = itMapControlData->second;
		if (pControlData->GetFileNextControlPosition() == 0LL && pControlData->GetFilePosition() != 0LL)
			break;
		else
			pControlData = NULL;
		itMapControlData++;
	}

	return pControlData;
}

SpyMaskData *SPYDataFacade::GetLastMaskRecord()
{
	map <string, SpyMaskData *>::iterator itMapMaskData = mapMaskData.begin();
	SpyMaskData *pMaskData = NULL;
	while (itMapMaskData != mapMaskData.end())
	{
		pMaskData = itMapMaskData->second;
		if (pMaskData->GetFileNextMaskPosition() == 0LL && pMaskData->GetFilePosition() != 0LL)
			break;
		else
			pMaskData = NULL;
		itMapMaskData++;
	}

	return pMaskData;
}

void SPYDataFacade::AddWindow(string name, SpyWindowData *window)
{
	mapWindowData.insert(pair<string, SpyWindowData *>(name, window));
}

SpyProcessData *SPYDataFacade::GetProcessData(string key)
{
	SpyProcessData *pProcessData = NULL;
	map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.find(key);
	if (itMapProcessData != mapProcessData.end())
	{
		pProcessData = itMapProcessData->second;
	}
	return pProcessData;
}

SpyUrlData *SPYDataFacade::GetUrlData(string key)
{
	SpyUrlData *pUrlData = NULL;
	map <string, SpyUrlData *>::iterator itMapUrlData = mapUrlData.find(key);
	if (itMapUrlData != mapUrlData.end())
	{
		pUrlData = itMapUrlData->second;
	}
	return pUrlData;
}

void SPYDataFacade::ReadWindowControlsData(string key)
{
	ClearControlsData();

	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 204800L);
	SpyWindowData *pWindowData = NULL;
	map <string, SpyWindowData *>::iterator itMapWindowData = mapWindowData.find(key);
	if (itMapWindowData != mapWindowData.end())
	{
		pWindowData = itMapWindowData->second;
		long long controlPos = pWindowData->GetControlFPos();
		struct controlRecord controlIndex;
		ZeroMemory(&controlIndex, sizeof(struct controlRecord));

		while (controlPos != NULL)
		{
			pDataFile->ReadFromFile((char *)&controlIndex, sizeof(struct controlRecord), controlPos);
			long long fPosData = controlIndex.fPosData;
			pDataFile->ReadDeflateBlockFromFile(pBuffer, controlIndex.dataSize, controlIndex.compSize, fPosData);
			string strJSON = pBuffer;
			SpyControlData *pControlData = DBG_NEW SpyControlData();
			pControlData->SetData(strJSON);
			pControlData->SetFilePosition(controlPos);
			pControlData->SetFileNextControlPosition(controlIndex.fPosNextControl);
			DWORD keyControl = pControlData->GetControlID();
			map <DWORD, SpyControlData *>::iterator itMapControlData = mapControlData.find(keyControl);
			if (itMapControlData == mapControlData.end())
			{
				mapControlData.insert(pair<DWORD, SpyControlData *>(keyControl, pControlData));
			}
			controlPos = controlIndex.fPosNextControl;
		}
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);
}


void SPYDataFacade::ReadURLControlsData(string key)
{
	ClearControlsData();

	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 204800L);
	SpyUrlData *pURLData = NULL;
	map <string, SpyUrlData *>::iterator itMapURLData = mapUrlData.find(key);
	if (itMapURLData != mapUrlData.end())
	{
		pURLData = itMapURLData->second;
		long long controlPos = pURLData->GetControlFPos();
		struct controlRecord controlIndex;
		ZeroMemory(&controlIndex, sizeof(struct controlRecord));

		while (controlPos != NULL)
		{
			pDataFile->ReadFromFile((char *)&controlIndex, sizeof(struct controlRecord), controlPos);
			long long fPosData = controlIndex.fPosData;
			pDataFile->ReadDeflateBlockFromFile(pBuffer, controlIndex.dataSize, controlIndex.compSize, fPosData);
			string strJSON = pBuffer;
			SpyControlData *pControlData = DBG_NEW SpyControlData();
			pControlData->SetData(strJSON);
			pControlData->SetFilePosition(controlPos);
			pControlData->SetFileNextControlPosition(controlIndex.fPosNextControl);
			pControlData->SetParentFilePosition(controlIndex.fPosParent);
			pControlData->SetLeftSiblingFilePosition(controlIndex.fPosLeftSibling);
			pControlData->SetRightSiblingFilePosition(controlIndex.fPosRightSibling);
			DWORD keyControl = pControlData->GetControlID();
			map <DWORD, SpyControlData *>::iterator itMapControlData = mapControlData.find(keyControl);
			if (itMapControlData == mapControlData.end())
			{
				mapControlData.insert(pair<DWORD, SpyControlData *>(keyControl, pControlData));
			}
			controlPos = controlIndex.fPosNextControl;
		}
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);
}

void SPYDataFacade::ReadProcessWindowsData(string key)
{
	ClearWindowData();

	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 204800L);
	SpyProcessData *pProcessData = NULL;
	map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.find(key);
	if (itMapProcessData != mapProcessData.end())
	{
		pProcessData = itMapProcessData->second;
		long long windowPos = pProcessData->GetWindowFPos();
		struct windowRecord windowIndex;
		ZeroMemory(&windowIndex, sizeof(struct windowRecord));

		while (windowPos != NULL)
		{
			pDataFile->ReadFromFile((char *)&windowIndex, sizeof(struct windowRecord), windowPos);
			long long fPosData = windowIndex.fPosData;
			pDataFile->ReadDeflateBlockFromFile(pBuffer, windowIndex.dataSize, windowIndex.compSize, fPosData);
			string strJSON = pBuffer;
			SpyWindowData *pWindowData = DBG_NEW SpyWindowData(strJSON.c_str());
			pWindowData->SetFilePosition(windowPos);
			pWindowData->SetFileNextWindowPosition(windowIndex.fPosNextWindow);
			pWindowData->SetMaskFPos(windowIndex.fPosMask);
			pWindowData->SetControlFPos(windowIndex.fPosControls);
			string keyTitle = pWindowData->GetTitle();
			map <string, SpyWindowData *>::iterator itMapWindowData = mapWindowData.find(keyTitle);
			if (itMapWindowData == mapWindowData.end())
			{
				mapWindowData.insert(pair<string, SpyWindowData *>(keyTitle, pWindowData));
			}
			windowPos = windowIndex.fPosNextWindow;
		}
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);
}


void SPYDataFacade::ReadWindowMaskInfo(string key)
{
	ClearMaskData();

	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 2048L);
	SpyWindowData* pWindowData = NULL;
	map <string, SpyWindowData*>::iterator itMapWindowData = mapWindowData.find(key);
	if (itMapWindowData != mapWindowData.end())
	{
		pWindowData = itMapWindowData->second;
		long long maskPos = pWindowData->GetMaskFPos();

		struct maskRecord maskIndex;
		ZeroMemory(&maskIndex, sizeof(struct maskRecord));

		while (maskPos != NULL)
		{
			pDataFile->ReadFromFile((char *)&maskIndex, sizeof(struct maskRecord), maskPos);
			SpyMaskData *pMaskData = DBG_NEW SpyMaskData();

			pMaskData->SetFilePosition(maskPos);
			pMaskData->SetFileNextMaskPosition(maskIndex.fPosNextMask);
			pMaskData->SetMaskName(maskIndex.maskName);
			pMaskData->SetMaskType(maskIndex.maskType);
			RECT wRect;
			wRect.left = maskIndex.maskRect.left;
			wRect.right = maskIndex.maskRect.right;
			wRect.top = maskIndex.maskRect.top;
			wRect.bottom = maskIndex.maskRect.bottom;
			pMaskData->SetMaskRect(wRect);

			wRect.left = maskIndex.match1Rect.left;
			wRect.right = maskIndex.match1Rect.right;
			wRect.top = maskIndex.match1Rect.top;
			wRect.bottom = maskIndex.match1Rect.bottom;
			pMaskData->SetImage1Rect(wRect);

			wRect.left = maskIndex.match2Rect.left;
			wRect.right = maskIndex.match2Rect.right;
			wRect.top = maskIndex.match2Rect.top;
			wRect.bottom = maskIndex.match2Rect.bottom;
			pMaskData->SetImage2Rect(wRect);

			wRect.left = maskIndex.match3Rect.left;
			wRect.right = maskIndex.match3Rect.right;
			wRect.top = maskIndex.match3Rect.top;
			wRect.bottom = maskIndex.match3Rect.bottom;
			pMaskData->SetImage3Rect(wRect);

			wRect.left = maskIndex.match4Rect.left;
			wRect.right = maskIndex.match4Rect.right;
			wRect.top = maskIndex.match4Rect.top;
			wRect.bottom = maskIndex.match4Rect.bottom;
			pMaskData->SetImage4Rect(wRect);

			wRect.left = maskIndex.match5Rect.left;
			wRect.right = maskIndex.match5Rect.right;
			wRect.top = maskIndex.match5Rect.top;
			wRect.bottom = maskIndex.match5Rect.bottom;
			pMaskData->SetImage5Rect(wRect);

			wRect.left = maskIndex.match6Rect.left;
			wRect.right = maskIndex.match6Rect.right;
			wRect.top = maskIndex.match6Rect.top;
			wRect.bottom = maskIndex.match6Rect.bottom;
			pMaskData->SetImage6Rect(wRect);

			wRect.left = maskIndex.imageRect.left;
			wRect.right = maskIndex.imageRect.right;
			wRect.top = maskIndex.imageRect.top;
			wRect.bottom = maskIndex.imageRect.bottom;
			pMaskData->SetImageRect(wRect);

			string keyMask = pMaskData->GetMaskName();
			map <string, SpyMaskData *>::iterator itMapMaskData = mapMaskData.find(keyMask);
			if (itMapMaskData == mapMaskData.end())
			{
				mapMaskData.insert(pair<string, SpyMaskData *>(keyMask, pMaskData));
			}
			maskPos = maskIndex.fPosNextMask;
		}
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);
}



void SPYDataFacade::ReadURLMaskInfo(string key)
{
	ClearMaskData();

	char* pBuffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 2048L);
	SpyUrlData* pWindowData = NULL;
	map <string, SpyUrlData*>::iterator itMapUrlData = mapUrlData.find(key);
	if (itMapUrlData != mapUrlData.end())
	{
		pWindowData = itMapUrlData->second;
		long long maskPos = pWindowData->GetMaskFPos();

		struct maskRecord maskIndex;
		ZeroMemory(&maskIndex, sizeof(struct maskRecord));

		while (maskPos != NULL)
		{
			pDataFile->ReadFromFile((char*)&maskIndex, sizeof(struct maskRecord), maskPos);
			SpyMaskData* pMaskData = DBG_NEW SpyMaskData();

			pMaskData->SetFilePosition(maskPos);
			pMaskData->SetFileNextMaskPosition(maskIndex.fPosNextMask);
			pMaskData->SetMaskName(maskIndex.maskName);
			pMaskData->SetMaskType(maskIndex.maskType);
			RECT wRect;
			wRect.left = maskIndex.maskRect.left;
			wRect.right = maskIndex.maskRect.right;
			wRect.top = maskIndex.maskRect.top;
			wRect.bottom = maskIndex.maskRect.bottom;
			pMaskData->SetMaskRect(wRect);

			wRect.left = maskIndex.match1Rect.left;
			wRect.right = maskIndex.match1Rect.right;
			wRect.top = maskIndex.match1Rect.top;
			wRect.bottom = maskIndex.match1Rect.bottom;
			pMaskData->SetImage1Rect(wRect);

			wRect.left = maskIndex.match2Rect.left;
			wRect.right = maskIndex.match2Rect.right;
			wRect.top = maskIndex.match2Rect.top;
			wRect.bottom = maskIndex.match2Rect.bottom;
			pMaskData->SetImage2Rect(wRect);

			wRect.left = maskIndex.match3Rect.left;
			wRect.right = maskIndex.match3Rect.right;
			wRect.top = maskIndex.match3Rect.top;
			wRect.bottom = maskIndex.match3Rect.bottom;
			pMaskData->SetImage3Rect(wRect);

			wRect.left = maskIndex.match4Rect.left;
			wRect.right = maskIndex.match4Rect.right;
			wRect.top = maskIndex.match4Rect.top;
			wRect.bottom = maskIndex.match4Rect.bottom;
			pMaskData->SetImage4Rect(wRect);

			wRect.left = maskIndex.match5Rect.left;
			wRect.right = maskIndex.match5Rect.right;
			wRect.top = maskIndex.match5Rect.top;
			wRect.bottom = maskIndex.match5Rect.bottom;
			pMaskData->SetImage5Rect(wRect);

			wRect.left = maskIndex.match6Rect.left;
			wRect.right = maskIndex.match6Rect.right;
			wRect.top = maskIndex.match6Rect.top;
			wRect.bottom = maskIndex.match6Rect.bottom;
			pMaskData->SetImage6Rect(wRect);

			wRect.left = maskIndex.imageRect.left;
			wRect.right = maskIndex.imageRect.right;
			wRect.top = maskIndex.imageRect.top;
			wRect.bottom = maskIndex.imageRect.bottom;
			pMaskData->SetImageRect(wRect);

			string keyMask = pMaskData->GetMaskName();
			map <string, SpyMaskData*>::iterator itMapMaskData = mapMaskData.find(keyMask);
			if (itMapMaskData == mapMaskData.end())
			{
				mapMaskData.insert(pair<string, SpyMaskData*>(keyMask, pMaskData));
			}
			maskPos = maskIndex.fPosNextMask;
		}
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);
}

void SPYDataFacade::GetMaskImageData(SpyMaskData *pData)
{
	struct maskRecord maskIndex;
	ZeroMemory(&maskIndex, sizeof(struct maskRecord));
	long long maskPos = pData->GetFilePosition();
	pDataFile->ReadFromFile((char *)&maskIndex, sizeof(struct maskRecord), maskPos);
	//int width = maskIndex.bmiHeader.biWidth;
	int height = maskIndex.bmiHeader.biHeight;
	if (height < 0)
		height = -height;
	
	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, maskIndex.dataSize);
	pDataFile->ReadDeflateBlockFromFile(pBuffer, maskIndex.dataSize, maskIndex.compSize, maskIndex.fPosData);
	pData->SetMaskImage(&maskIndex.bfh, &maskIndex.bmiHeader, pBuffer);
}

vector <string> SPYDataFacade::GetWindowNames()
{
	vector <string> strKeys;

	strKeys.clear();

	map <string, SpyWindowData *>::iterator itWindows = mapWindowData.begin();
	while (itWindows != mapWindowData.end())
	{
		strKeys.push_back(itWindows->first);
		itWindows++;
	}

	return strKeys;
}

vector <string> SPYDataFacade::GetURLNames()
{
	vector <string> strKeys;

	strKeys.clear();

	map <string, SpyUrlData *>::iterator itURLS = mapUrlData.begin();
	while (itURLS != mapUrlData.end())
	{
		strKeys.push_back(itURLS->first);
		itURLS++;
	}

	return strKeys;
}


vector <string> SPYDataFacade::GetMaskNames()
{
	vector <string> strKeys;

	strKeys.clear();

	map <string, SpyMaskData *>::iterator itMaskData = mapMaskData.begin();
	while (itMaskData != mapMaskData.end())
	{
		strKeys.push_back(itMaskData->first);
		itMaskData++;
	}

	return strKeys;
}


SpyWindowData *SPYDataFacade::GetProcessWindow(string name)
{
	SpyWindowData *pWindow = NULL;
	map <string, SpyWindowData *>::iterator itvars = mapWindowData.begin();
	itvars = mapWindowData.find(name);
	if (itvars != mapWindowData.end())
	{
		pWindow = itvars->second;
	}

	return pWindow;
}


bool SPYDataFacade::IsProcessPresent(string key)
{
	bool flg = false;
	map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.find(key);
	if (itMapProcessData != mapProcessData.end())
	{
		flg = true;
	}
	return flg;
}

void SPYDataFacade::GetProcessList(list <wstring> *pList)
{
	map <string, SpyProcessData *>::iterator itMapProcessData = mapProcessData.begin();
	pList->clear();
	while (itMapProcessData != mapProcessData.end())
	{
		SpyProcessData *pProcessData = itMapProcessData->second;
		string str = pProcessData->GetSystemName();
		wstring strVal = s2ws(str);
		pList->push_back(strVal);
		itMapProcessData++;
	}
}

void SPYDataFacade::GetURLList(list <wstring> *pList)
{
	map <string, SpyUrlData *>::iterator itMapUrlData = mapUrlData.begin();
	pList->clear();
	while (itMapUrlData != mapUrlData.end())
	{
		SpyUrlData *pURLData = itMapUrlData->second;
		string str = pURLData->GetURL();
		wstring strVal = s2ws(str);
		pList->push_back(strVal);
		itMapUrlData++;
	}
}


void SPYDataFacade::UpdateProcessData(SpyProcessData *pProcessData)
{
	WriteProcessDataFile(pProcessData);

	string key = pProcessData->GetSystemName();
	if (!IsProcessPresent(key))
	{
		mapProcessData.insert(pair<string, SpyProcessData *>(key, pProcessData));
	}
}

void SPYDataFacade::UpdateURLData(SpyUrlData *pURLData)
{
	WriteURLDataFile(pURLData);

	string key = pURLData->GetURL();
	if (!GetUrlData(key))
	{
		mapUrlData.insert(pair<string, SpyUrlData *>(key, pURLData));
	}
}

SPYDataFacade::~SPYDataFacade()
{
	delete pDataFile;
	ClearProcessData();
	ClearWindowData();
	ClearControlsData();
	ClearMaskData();

	// Now delete the map objects

	map <string, SpyProcessData *>::iterator itProcess = mapProcessData.begin();
	while (itProcess != mapProcessData.end())
	{
		SpyProcessData *pProcess = itProcess->second;
		delete pProcess;
		itProcess++;
	}
	mapProcessData.clear();

	map <string, SpyWindowData *>::iterator itWindow = mapWindowData.begin();
	while (itWindow != mapWindowData.end())
	{
		SpyWindowData *pPtr = itWindow->second;
		delete pPtr;
		itWindow++;
	}
	mapWindowData.clear();

	map <DWORD, SpyControlData *>::iterator itControl = mapControlData.begin();
	while (itControl != mapControlData.end())
	{
		SpyControlData *pPtr = itControl->second;
		delete pPtr;
		itControl++;
	}
	mapControlData.clear();

	map <string, SpyMaskData *>::iterator itMask = mapMaskData.begin();
	while (itMask != mapMaskData.end())
	{
		SpyMaskData *pPtr = itMask->second;
		delete pPtr;
		itMask++;
	}
	mapMaskData.clear();

	map <string, SpyUrlData *>::iterator itUrlData = mapUrlData.begin();
	while (itUrlData != mapUrlData.end())
	{
		SpyUrlData *pPtr = itUrlData->second;
		delete pPtr;
		itUrlData++;
	}
	mapUrlData.clear();

}

void SPYDataFacade::AddWindowCapturedControl(SpyWindowData* pWindowData, CaptureElementData* pCaptureData)
{
	SpyControlData* pControlData = DBG_NEW SpyControlData;
	
	int i = mapControlData.size();
	time_t t = time(NULL);
	int id = (i * 100) + (t % 100);

	pControlData->SetAutoID(pCaptureData->GetAutomationID());
	pControlData->SetClassname(pCaptureData->GetControlClassName());
	pControlData->SetClickAnnotation(pCaptureData->GetAnnotation());
	pControlData->SetControlID(id);
	pControlData->SetControlUserName(pCaptureData->GetUserDefinedName());
	pControlData->SetID(id);
	pControlData->SetName(pCaptureData->GetControlName());
	RECT rect;

	pCaptureData->GetBoundingRect(&rect);
	pControlData->SetRect(&rect);
	pControlData->SetScreenControlType(pCaptureData->GetItemType());
	pControlData->SetType(pCaptureData->GetControlType());
	pControlData->SetValue("");

	struct controlRecord controlIndex;
	ZeroMemory(&controlIndex, sizeof(struct controlRecord));
	controlIndex.recType = 'C';
	string JSONStr = pControlData->GetJSonString();
	if (JSONStr.size() > 0)
	{
		controlIndex.dataSize = JSONStr.size() + 10;
		controlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char*)JSONStr.c_str(), controlIndex.dataSize, &controlIndex.compSize);
	}

	long long fPosControl = pDataFile->AppendToFile((char*)&controlIndex, sizeof(struct controlRecord));
	
	if (pWindowData->GetControlFPos() == NULL)
	{
		pWindowData->SetControlFPos(fPosControl);
		// now rewrite Window Data
		struct windowRecord wndIndex;
		ZeroMemory(&wndIndex, sizeof(struct windowRecord));
		//bool bNewRecord = false;
		long long fPos = pWindowData->GetFilePosition();
		pDataFile->ReadFromFile((char*)&wndIndex, sizeof(struct windowRecord), fPos);
		wndIndex.fPosControls = fPosControl;
		pDataFile->WriteToFile(fPos, (char*)&wndIndex, sizeof(struct windowRecord));
	}
	else
	{
		SpyControlData *pLastControl = GetLastControlRecord();
		long long fPos = pLastControl->GetFilePosition();
		pDataFile->ReadFromFile((char*)&controlIndex, sizeof(struct controlRecord), fPos);
		controlIndex.fPosNextControl = fPosControl;
		pDataFile->WriteToFile(fPos, (char*)&controlIndex, sizeof(struct controlRecord));
	}

	WriteCapturedControlInfo(&controlIndex, fPosControl, pCaptureData);
	
	AddControl(pControlData);
}

void SPYDataFacade::WriteCapturedControlInfo(struct controlRecord *controlIndex, long long fPosControl, CaptureElementData *pCaptureData)
{
	// Write the Parent Records
	pDataFile->ReadFromFile((char*)controlIndex, sizeof(struct controlRecord), fPosControl); // Read main file again
	list <ElementData*>* pParentList = pCaptureData->GetParentList();
	long long oldElementFPos = NULL;
	list <ElementData*>::iterator itList = pParentList->begin();

	while (itList != pParentList->end())
	{
		ElementData* pParentElement = *itList;
		struct controlElementRecord elementControlIndex;
		ZeroMemory(&elementControlIndex, sizeof(struct controlElementRecord));
		elementControlIndex.recType = 'L';
		SpyControlData controlElement;
		controlElement.SetAutoID(pParentElement->GetAutomationID());
		controlElement.SetClassname(pParentElement->GetControlClassName());
		controlElement.SetName(pParentElement->GetControlName());
		RECT rect;
		pParentElement->GetBoundingRect(&rect);
		controlElement.SetRect(&rect);
		controlElement.SetScreenControlType(pParentElement->GetItemType());
		controlElement.SetType(pParentElement->GetControlType());
		controlElement.SetValue("");

		string JSONStr = controlElement.GetJSonString();
		if (JSONStr.size() > 0)
		{
			elementControlIndex.dataSize = JSONStr.size() + 10;
			elementControlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char*)JSONStr.c_str(), elementControlIndex.dataSize, &elementControlIndex.compSize);
		}

		long long fPosElementControl = pDataFile->AppendToFile((char*)&elementControlIndex, sizeof(struct controlElementRecord));

		if (controlIndex->fPosParent == NULL)
		{
			controlIndex->fPosParent = fPosElementControl;
			pDataFile->WriteToFile(fPosControl, (char*)controlIndex, sizeof(struct controlRecord));
		}
		else
		{
			pDataFile->ReadFromFile((char*)&elementControlIndex, sizeof(struct controlElementRecord), oldElementFPos);
			elementControlIndex.fPosNextControl = fPosElementControl;
			pDataFile->WriteToFile(oldElementFPos, (char*)&elementControlIndex, sizeof(struct controlElementRecord));
		}
		oldElementFPos = fPosElementControl;
		itList++;
	}

	// Write the left sibling records
	list <ElementData*>* pLeftSiblingList = pCaptureData->GetLeftSiblingList();
	itList = pLeftSiblingList->begin();
	oldElementFPos = NULL;

	while (itList != pLeftSiblingList->end())
	{
		ElementData* pLeftElement = *itList;
		struct controlElementRecord elementControlIndex;
		ZeroMemory(&elementControlIndex, sizeof(struct controlElementRecord));
		elementControlIndex.recType = 'L';

		SpyControlData controlElement;
		controlElement.SetAutoID(pLeftElement->GetAutomationID());
		controlElement.SetClassname(pLeftElement->GetControlClassName());
		controlElement.SetName(pLeftElement->GetControlName());
		RECT rect;
		pLeftElement->GetBoundingRect(&rect);
		controlElement.SetRect(&rect);
		controlElement.SetScreenControlType(pLeftElement->GetItemType());
		controlElement.SetType(pLeftElement->GetControlType());
		controlElement.SetValue("");

		string JSONStr = controlElement.GetJSonString();
		if (JSONStr.size() > 0)
		{
			elementControlIndex.dataSize = JSONStr.size() + 10;
			elementControlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char*)JSONStr.c_str(), elementControlIndex.dataSize, &elementControlIndex.compSize);
		}

		long long fPosElementControl = pDataFile->AppendToFile((char*)&elementControlIndex, sizeof(struct controlElementRecord));

		if (controlIndex->fPosLeftSibling == NULL)
		{
			controlIndex->fPosLeftSibling = fPosElementControl;
			pDataFile->WriteToFile(fPosControl, (char*)controlIndex, sizeof(struct controlRecord));
		}
		else
		{
			pDataFile->ReadFromFile((char*)&elementControlIndex, sizeof(struct controlElementRecord), oldElementFPos);
			elementControlIndex.fPosNextControl = fPosElementControl;
			pDataFile->WriteToFile(oldElementFPos, (char*)&elementControlIndex, sizeof(struct controlElementRecord));
		}
		oldElementFPos = fPosElementControl;
		itList++;
	}

	// Write the right sibling records
	list <ElementData*>* pRightSiblingList = pCaptureData->GetRightSiblingList();
	itList = pRightSiblingList->begin();
	oldElementFPos = NULL;

	while (itList != pRightSiblingList->end())
	{
		ElementData* pRightElement = *itList;
		struct controlElementRecord elementControlIndex;
		ZeroMemory(&elementControlIndex, sizeof(struct controlElementRecord));
		elementControlIndex.recType = 'L';
		SpyControlData controlElement;
		controlElement.SetAutoID(pRightElement->GetAutomationID());
		controlElement.SetClassname(pRightElement->GetControlClassName());
		controlElement.SetName(pRightElement->GetControlName());
		RECT rect;
		pRightElement->GetBoundingRect(&rect);
		controlElement.SetRect(&rect);
		controlElement.SetScreenControlType(pRightElement->GetItemType());
		controlElement.SetType(pRightElement->GetControlType());
		controlElement.SetValue("");

		string JSONStr = controlElement.GetJSonString();
		if (JSONStr.size() > 0)
		{
			elementControlIndex.dataSize = JSONStr.size() + 10;
			elementControlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char*)JSONStr.c_str(), elementControlIndex.dataSize, &elementControlIndex.compSize);
		}

		long long fPosElementControl = pDataFile->AppendToFile((char*)&elementControlIndex, sizeof(struct controlElementRecord));

		if (controlIndex->fPosRightSibling == NULL)
		{
			controlIndex->fPosRightSibling = fPosElementControl;
			pDataFile->WriteToFile(fPosControl, (char*)controlIndex, sizeof(struct controlRecord));
		}
		else
		{
			pDataFile->ReadFromFile((char*)&elementControlIndex, sizeof(struct controlElementRecord), oldElementFPos);
			elementControlIndex.fPosNextControl = fPosElementControl;
			pDataFile->WriteToFile(oldElementFPos, (char*)&elementControlIndex, sizeof(struct controlElementRecord));
		}
		oldElementFPos = fPosElementControl;
		itList++;
	}
}

void SPYDataFacade::AddURLCapturedControl(SpyUrlData* pWindowData, CaptureElementData* pCaptureData)
{
	SpyControlData* pControlData = DBG_NEW SpyControlData;

	int i = mapControlData.size();
	time_t t = time(NULL);
	int id = (i * 100) + (t % 100);

	pControlData->SetAutoID(pCaptureData->GetAutomationID());
	pControlData->SetClassname(pCaptureData->GetControlClassName());
	pControlData->SetClickAnnotation(pCaptureData->GetAnnotation());
	pControlData->SetControlID(id);
	pControlData->SetControlUserName(pCaptureData->GetUserDefinedName());
	pControlData->SetID(id);
	pControlData->SetName(pCaptureData->GetControlName());
	RECT rect;

	pCaptureData->GetBoundingRect(&rect);
	pControlData->SetRect(&rect);
	pControlData->SetScreenControlType(pCaptureData->GetItemType());
	pControlData->SetType(pCaptureData->GetControlType());
	pControlData->SetValue("");

	struct controlRecord controlIndex;
	ZeroMemory(&controlIndex, sizeof(struct controlRecord));
	controlIndex.recType = 'C';
	string JSONStr = pControlData->GetJSonString();
	if (JSONStr.size() > 0)
	{
		controlIndex.dataSize = JSONStr.size() + 10;
		controlIndex.fPosData = pDataFile->AppendDeflateBlockToFile((char*)JSONStr.c_str(), controlIndex.dataSize, &controlIndex.compSize);
	}

	long long fPosControl = pDataFile->AppendToFile((char*)&controlIndex, sizeof(struct controlRecord));

	if (pWindowData->GetControlFPos() == NULL)
	{
		pWindowData->SetControlFPos(fPosControl);
		// now rewrite Window Data
		struct URLRecord wndIndex;
		ZeroMemory(&wndIndex, sizeof(struct URLRecord));
		//bool bNewRecord = false;
		long long fPos = pWindowData->GetFilePosition();
		pDataFile->ReadFromFile((char*)&wndIndex, sizeof(struct URLRecord), fPos);
		wndIndex.fPosControls = fPosControl;
		pDataFile->WriteToFile(fPos, (char*)&wndIndex, sizeof(struct URLRecord));
	}
	else
	{
		SpyControlData* pLastControl = GetLastControlRecord();
		long long fPos = pLastControl->GetFilePosition();
		pDataFile->ReadFromFile((char*)&controlIndex, sizeof(struct controlRecord), fPos);
		controlIndex.fPosNextControl = fPosControl;
		pDataFile->WriteToFile(fPos, (char*)&controlIndex, sizeof(struct controlRecord));
	}

	WriteCapturedControlInfo(&controlIndex, fPosControl, pCaptureData);
	AddControl(pControlData);
}

SpyUrlData* SPYDataFacade::GetMatchedURLData(string name)
{
	vector <string> urls = GetURLNames();
	vector <string>::iterator itURLs = urls.begin();
	while (itURLs != urls.end())
	{
		string prcURLName = *itURLs;
		SpyUrlData* pUrlData = GetUrlData(prcURLName);
		string prcURLRuleValue = pUrlData->GetURLRuleValue(); // This holds the result
		string prcURLTitle = pUrlData->GetURL();
		string prcURLRule = pUrlData->GetRule();
		if (prcURLRuleValue == "" && prcURLTitle != "" && prcURLRule != "")
		{
			regex rgx(prcURLRule);
			smatch matchValue;

			if (regex_search(prcURLTitle, matchValue, rgx))
			{
				prcURLRuleValue = matchValue[0];
			}
			pUrlData->SetURLRuleValue(prcURLRuleValue);
		}
		string strResult = "";
		if (prcURLRuleValue != "" && prcURLTitle != "" && prcURLRule != "")
		{
			regex rgx(prcURLRule);
			smatch matchValue;
			if (regex_search(name, matchValue, rgx))
			{
				strResult = matchValue[0];
			}
		}

		if (strResult == "" && prcURLRuleValue == "" && name == prcURLTitle)
			return pUrlData;
		if (strResult != "" && prcURLRuleValue != "")
			return pUrlData;
		itURLs++;
	}

	return NULL;
}

SpyWindowData* SPYDataFacade::GetMatchedWindowData(string name)
{
	vector <string> windows = GetWindowNames();
	vector <string>::iterator itWindows = windows.begin();
	while (itWindows != windows.end())
	{
		string prcWindowName = *itWindows;
		SpyWindowData* pWindowData = GetProcessWindow(prcWindowName);
		string prcWindowRuleValue = pWindowData->GetWindowRuleValue(); // This holds the result
		string prcWindowTitle = pWindowData->GetTitle();
		string prcWindowRule = pWindowData->GetMatchingRule();
		if (prcWindowRuleValue == "" && prcWindowTitle != "" && prcWindowRule != "")
		{
			regex rgx(prcWindowRule);
			smatch matchValue;
			if (regex_search(prcWindowTitle, matchValue, rgx))
			{
				prcWindowRuleValue = matchValue[0];
			}
			pWindowData->SetWindowRuleValue(prcWindowRuleValue);
		}
		string strResult = "";
		if (prcWindowRuleValue != "" && prcWindowName != "" && prcWindowRule != "")
		{
			regex rgx(prcWindowRule);
			smatch matchValue;
			if (regex_search(name, matchValue, rgx))
			{
				strResult = matchValue[0];
			}
		}

		if (strResult == "" && prcWindowRuleValue == "" && name == prcWindowTitle)
			return pWindowData;
		if (strResult != "" && prcWindowRuleValue != "")
			return pWindowData;
		itWindows++;
	}

	return NULL;
}

int SPYDataFacade::GetNumberOfProcesses()
{
	return mapProcessData.size();
}

int SPYDataFacade::GetNumberOfURLs()
{
	return mapUrlData.size();
}

vector <SpyControlData*> SPYDataFacade::GetControlRelatives(long long fPos)
{
	vector <SpyControlData*> controlList;
	char* pBuffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 204800L);

	controlList.clear();
	struct controlElementRecord elementControlIndex;
	ZeroMemory(&elementControlIndex, sizeof(struct controlElementRecord));

	while (fPos != NULL)
	{
		pDataFile->ReadFromFile((char*)&elementControlIndex, sizeof(struct controlElementRecord), fPos);

		long long fPosData = elementControlIndex.fPosData;
		pDataFile->ReadDeflateBlockFromFile(pBuffer, elementControlIndex.dataSize, elementControlIndex.compSize, fPosData);
		string strJSON = pBuffer;
		SpyControlData* pControlData = DBG_NEW SpyControlData();
		pControlData->SetData(strJSON);
		pControlData->SetFilePosition(fPosData);
		pControlData->SetFileNextControlPosition(elementControlIndex.fPosNextControl);
		controlList.push_back(pControlData);
		fPos = elementControlIndex.fPosNextControl;
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);

	return controlList;
}
