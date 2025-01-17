#pragma once

#include <vector>


//#include <d3d9.h>
//#include <d3dx9.h>

#include <mutex>
#include <vector>
#include "RecordBufferManager.h"

using namespace std;

class ScreenRecorder
{
	vector <struct ScreenData *> screenData;
	vector <HMONITOR> monitorHandles;

	RecorderDataFileManager *pDataRecorder;
	RecordBufferManager *pRecordBufferManager;
	bool bCaptureDirectX;
	int WriteIndex;
	int ReadIndex;
	HWND hWndPlayer;
	bool firstRecord;
	bool bActiveMonitorCapture;
	void WriteImageBMP(struct ScreenData *screen);
	POINT mousePosition;
	long long recDuration;
	// Direct X definitions
	//IDirect3D9*			g_pD3D = NULL;
	//void EnumerateDirectXDisplays();
	//std::vector <IDXGIAdapter*> EnumerateAdapters(void);
	//std::vector <IDXGIOutput*> EnumerateOutputs(IDXGIAdapter *pAdapter);
	//void InitializeDirectX();
	long CompareData(long *dest, long *screenLHS, long *screenRHS, int height, int width, long *numBlocks);
	int indexInterval;

	list <struct monitorDetails *> *pMonitorList;

public:
	void ScreenCapture(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor);	
	//void ScreenCaptureDirectX(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor);
	void ScreenCaptureGDI(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor);

	void CreateMonitorData(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor);

	void StoreLastImage(HMONITOR hMon);
	void GetScreenDataDetails();
	void SetDataManager(RecorderDataFileManager *pRecorder);
	void SetRecordBufferManager(RecordBufferManager *pRecManager);
	void SetIndexInterval(int iInterval);
	struct ScreenData *GetScreenData(HMONITOR hMon);
	void SetHWnd(HWND hWnd);
	void CaptureScreenEnum();
	void CaptureScreens(bool bFirst);
	void StoreLastScreenImage();
	void CaptureMonitorDetails();
	void WriteMonitorDetails();;
	void MonitorDataCapture(HMONITOR hMon, LPRECT lprcMonitor);
	HMONITOR GetMonitorNum(POINT pt);
	bool InitializeDirectXAdaptor(int adaptorNum, struct ScreenData *screen);
	void SetRecDuration(long long dur);
	void SetSelectedMonitorList(list <struct monitorDetails *> *pList);
	ScreenRecorder();

	~ScreenRecorder();
};

