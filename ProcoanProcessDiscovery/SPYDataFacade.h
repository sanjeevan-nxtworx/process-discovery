#pragma once
#include "SpyHeader.h"
#include <string>
#include <map>
#include <list>
#include <vector>


class DataFile;
class SpyProcessData;
class SpyWindowData;
class SpyWindowLayout;
class SpyControlData;
class SpyMaskData;
class SpyUrlData;
class CaptureElementData;

using namespace std;

class SPYDataFacade
{
	DataFile *pDataFile;
	struct spyRoot rootIndex;
	map <string, SpyProcessData *> mapProcessData;
	map <string, SpyWindowData *> mapWindowData;
	map <DWORD, SpyControlData *> mapControlData;
	map <string, SpyMaskData *> mapMaskData;
	map <string, SpyUrlData *> mapUrlData;

	SpyProcessData *GetLastProcessRecord();
	SpyWindowData *GetLastWindowRecord();
	SpyControlData *GetLastControlRecord();
	SpyMaskData *GetLastMaskRecord();
	SpyUrlData *GetLastURLRecord();



public:
	void ClearProcessData();
	void ClearWindowData();
	void ClearControlsData();
	void ClearMaskData();
	SPYDataFacade();
	SPYDataFacade(char *filename);
	~SPYDataFacade();
	void ReadDataFile();
	SpyProcessData *GetProcessData(string key);
	SpyUrlData *GetUrlData(string key);
	void ReadURLMaskInfo(string key);
	void ReadWindowMaskInfo(string key);
	int GetNumberOfProcesses();
	int GetNumberOfURLs();
	void ReadProcessWindowsData(string key);
	void AddWindow(string name, SpyWindowData *window);
	void ReadWindowControlsData(string key);
	vector <string> GetMaskNames();
	void ReadURLControlsData(string key);
	void WriteWindowDataFile(SpyProcessData* pProcessData, SpyWindowData* pWindowData);
	SpyUrlData* GetMatchedURLData(string name);
	SpyWindowData* GetMatchedWindowData(string name);


	bool IsProcessPresent(string key);
	void UpdateURLData(SpyUrlData *pURLData);
	void UpdateProcessData(SpyProcessData *pProcessData);
	void GetProcessList(list <wstring> *pList);
	void GetURLList(list <wstring> *pList);
	void WriteProcessDataFile(SpyProcessData *pProcessData);
	void WriteURLDataFile(SpyUrlData *pUrlData);
	vector <string> GetWindowNames();
	vector <string> GetURLNames();
	void GetMaskImageData(SpyMaskData *pData);
	SpyWindowData *GetProcessWindow(string name);
	vector <SpyControlData *> GetControls();

	vector <SpyControlData*> GetControlRelatives(long long fPos);

	void AddControl(SpyControlData *pControlData);
	void AddWindowCapturedControl(SpyWindowData *pWindowData, CaptureElementData* pCaptureData);
	void AddURLCapturedControl(SpyUrlData* pWindowData, CaptureElementData* pCaptureData);
	void WriteCapturedControlInfo(struct controlRecord *controlIndex, long long fPosControl, CaptureElementData *pCaptureData);

	void AddURLMask(string name, SpyMaskData* pMaskData);
	void AddWindowMask(string name, SpyMaskData* pMaskData);
	SpyControlData *GetControl(DWORD id);
	DWORD GetNextControlID();
	void WriteControlData(SpyControlData *pControl);
	SpyMaskData *GetMaskData(string name);

};

