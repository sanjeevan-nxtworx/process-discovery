#include "stdafx.h"
#include "RecorderDataFileManager.h"
#include "ScreenRecorder.h"
#include "MMHeader.h"
#include <shellscalingapi.h>
#include "configclass.h"
#include "utility.h"
#include "UIUtility.h"
#include "spdlog/spdlog.h"

static BOOL CALLBACK StoreLastImageMonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
static BOOL CALLBACK ScreenCaptureMonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
static BOOL CALLBACK MonitorDataCaptureMonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
static BOOL CALLBACK CreateMonitorDataEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);

ScreenRecorder::ScreenRecorder()
{
	WriteIndex = 0;
	ReadIndex = 0;
	firstRecord = true;
	pRecordBufferManager = NULL;
	pDataRecorder = NULL;
	ConfigClass *pConfigClass = GetConfigClass();
	bCaptureDirectX = false;
	mousePosition.x = 0;
	mousePosition.y = 0;
	hWndPlayer = NULL;
	bActiveMonitorCapture = true;
	if (pConfigClass != NULL)
	{
		bCaptureDirectX = pConfigClass->getCaptureModeDirectX();
		bActiveMonitorCapture = pConfigClass->getCaptureMonitorMode();
	}
	//if (bCaptureDirectX)
	//{
	//	InitializeDirectX();
	//}
	pMonitorList = NULL;
}

void ScreenRecorder::SetRecDuration(long long dur)
{
	recDuration = dur;
}

void ScreenRecorder::SetSelectedMonitorList(list <struct monitorDetails *> *pList)
{
	pMonitorList = pList;
}

//void ScreenRecorder::InitializeDirectX()
//{
//	//	EnumerateDirectXDisplays();
//	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
//	int nMon = g_pD3D->GetAdapterCount();
//	for (int i = 0; i < nMon; i++)
//	{
//		HMONITOR hMon = g_pD3D->GetAdapterMonitor(i);
//		monitorHandles.push_back(hMon);
//	}
//
//}

ScreenRecorder::~ScreenRecorder()
{
	for (std::vector<struct ScreenData *>::iterator it = screenData.begin(); it != screenData.end(); ++it)
	{
		struct ScreenData *screenPtr = (struct ScreenData *) *it;
		//if (screenPtr->g_pSurface)
		//{
		//	screenPtr->g_pSurface->Release();
		//	screenPtr->g_pSurface = NULL;
		//}

		//if (screenPtr->g_pd3dDevice)
		//{
		//	screenPtr->g_pd3dDevice->Release();
		//	screenPtr->g_pd3dDevice = NULL;
		//}
		if (screenPtr->lpScreen != NULL)
			HeapFree(GetProcessHeap(), 0, screenPtr->lpScreen);
		if (screenPtr->lpScreenNew != NULL)
			HeapFree(GetProcessHeap(), 0, screenPtr->lpScreenNew);
		delete screenPtr;
	}

	screenData.clear();

	//if (g_pD3D)
	//{
	//	g_pD3D->Release();
	//	g_pD3D = NULL;
	//}
}


void ScreenRecorder::CaptureMonitorDetails()
{
	HDC hDC = GetDC(NULL);
	EnumDisplayMonitors(hDC, 0, MonitorDataCaptureMonitorEnum, (LPARAM)this);
	DeleteDC(hDC);
}

void ScreenRecorder::StoreLastScreenImage()
{
	HDC hDC = GetDC(NULL);
	spdlog::info("Storing Last Screen Images");
	EnumDisplayMonitors(hDC, 0, StoreLastImageMonitorEnum, (LPARAM)this);
	DeleteDC(hDC);
}

static BOOL CALLBACK StoreLastImageMonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	UNREFERENCED_PARAMETER(hdc);
	UNREFERENCED_PARAMETER(lprcMonitor);

	ScreenRecorder* pThis = reinterpret_cast<ScreenRecorder *>(pData);

	pThis->StoreLastImage(hMon);
	return TRUE;
}

void ScreenRecorder::GetScreenDataDetails()
{
	HDC hDC = GetDC(NULL);
	EnumDisplayMonitors(hDC, 0, CreateMonitorDataEnum, (LPARAM)this);
	DeleteDC(hDC);
}

void ScreenRecorder::CaptureScreens(bool bFirst)
{

	spdlog::info("Capture all screens");

	// Get Mouse position;
	GetPhysicalCursorPos(&mousePosition);
	static HMONITOR hMonitorCaptured;
	static int indexNum = -1;
	static int iInterval = 0;

	for (std::vector<struct ScreenData *>::iterator it = screenData.begin(); it != screenData.end(); ++it)
	{
		struct ScreenData *screenPtr = (struct ScreenData *) *it;

		screenPtr->bCapture = false;
		screenPtr->bFullScreen = false;
		if (screenPtr->bScreenSelected && (bFirst  || !bActiveMonitorCapture || (bActiveMonitorCapture && PtInRect(&screenPtr->rcMonitor, mousePosition))))
		{
			indexNum++;
			if (indexNum == NUM_DATA_POINTS)
			{
				indexNum = 0;
			}
			if (indexNum == 0)
				bFirst = true;

			screenPtr->bCapture = true;
			if (bFirst || (bActiveMonitorCapture && hMonitorCaptured != screenPtr->hMonitor))
			{
				screenPtr->bFullScreen = true;
			}
			hMonitorCaptured = screenPtr->hMonitor;
		}
	}

	CaptureScreenEnum();
}

void ScreenRecorder::CaptureScreenEnum()
{
	HDC hDC = GetDC(NULL);
	EnumDisplayMonitors(hDC, 0, ScreenCaptureMonitorEnum, (LPARAM)this);
	DeleteDC(hDC);
}

static BOOL CALLBACK CreateMonitorDataEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	ScreenRecorder* pThis = reinterpret_cast<ScreenRecorder *>(pData);

	pThis->CreateMonitorData(hMon, hdc, lprcMonitor);
	return TRUE;
}

static BOOL CALLBACK ScreenCaptureMonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	ScreenRecorder* pThis = reinterpret_cast<ScreenRecorder *>(pData);
	spdlog::info("Capturing screen {}", (UINT)hMon);

	pThis->ScreenCapture(hMon, hdc, lprcMonitor);
	return TRUE;
}


void ScreenRecorder::WriteMonitorDetails()
{
	CaptureMonitorDetails();
}

struct ScreenData *ScreenRecorder::GetScreenData(HMONITOR hMon)
{
	struct ScreenData *screen = NULL;

	for (std::vector<struct ScreenData *>::iterator it = screenData.begin(); it != screenData.end(); ++it)
	{
		struct ScreenData *screenPtr = (struct ScreenData *) *it;
		if (screenPtr->hMonitor == hMon)
		{
			screen = screenPtr;
			break;
		}
	}
	return screen;
}


void ScreenRecorder::StoreLastImage(HMONITOR hMon)
{
	struct ScreenData *screen = NULL;
	screen = GetScreenData(hMon);
	if (!screen->bCapture)
		return;
	struct CompareBuffer *pCompBuffer = NULL;
	struct bufferRecord *pRecBuffer;

	pRecBuffer = (struct bufferRecord *)pRecordBufferManager->GetFreeScreenBuffer(screen);
	pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
	pCompBuffer->screenNum = hMon;
	pCompBuffer->numOfBlocks = 0;
	pCompBuffer->numBytes = 0L;

	pCompBuffer->numBytes = 0;
	pRecBuffer->timeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	//WriteImageBMP(screen);
	pRecBuffer->mouseX = (short)mousePosition.x;
	pRecBuffer->mouseY = (short)mousePosition.y;
	pRecBuffer->recDurationus = recDuration;
	// Now can send the buffer to Write thread
	pRecordBufferManager->WriteToFile(pRecBuffer);
}

void ScreenRecorder::CreateMonitorData(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor)
{
	// remove warnings
	hdc = hdc;
	lprcMonitor = lprcMonitor;

	struct ScreenData *screen = NULL;

	screen = DBG_NEW struct ScreenData;
	ZeroMemory(screen, sizeof(struct ScreenData));
	MONITORINFO monitorInfo;
	screen->bFullScreen = false;
	screen->bCapture = false;
	screen->hMonitor = hMon;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMon, &monitorInfo);
	int width, height;
	screen->bScreenSelected = false;
	if (pMonitorList != NULL)
	{
		list <struct monitorDetails *>::iterator itSelectedMonitor = pMonitorList->begin();
		while (itSelectedMonitor != pMonitorList->end())
		{
			struct monitorDetails *pMonitor = *itSelectedMonitor;
			if (pMonitor->hMon == hMon)
			{
				screen->bScreenSelected = pMonitor->bSelect;
			}
			itSelectedMonitor++;
		}
	}

	//UINT dpiX, dpiY;
	//GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
	//screen->rcMonitor.left = MulDiv(monitorInfo.rcMonitor.left, dpiX, 96);
	//screen->rcMonitor.right = MulDiv(monitorInfo.rcMonitor.right, dpiX, 96);
	//screen->rcMonitor.top = MulDiv(monitorInfo.rcMonitor.top, dpiY, 96);
	//screen->rcMonitor.bottom = MulDiv(monitorInfo.rcMonitor.bottom, dpiY, 96);
	screen->rcMonitor = monitorInfo.rcMonitor;
	width = screen->rcMonitor.right - screen->rcMonitor.left;
	height = screen->rcMonitor.bottom - screen->rcMonitor.top;

	ULONG dwBmpSize = (DWORD)width * (DWORD)height * 4L;
	screen->lpScreen = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBmpSize);
	screen->lpScreenNew = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBmpSize);
	//if (bCaptureDirectX)
	//{
	//	// Add DirectX Screen Initialization out here
	//	int adaptorNum = D3DADAPTER_DEFAULT;
	//	for (unsigned i = 0; i < monitorHandles.size(); i++)
	//	{
	//		if (hMon == monitorHandles[i])
	//		{
	//			adaptorNum = i;
	//			break;
	//		}
	//	}
	//	if (!InitializeDirectXAdaptor(adaptorNum, screen))
	//	{
	//		return;
	//	}
	//}
	screenData.push_back(screen);
}

void ScreenRecorder::ScreenCapture(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor)
{
	//if (bCaptureDirectX)
	//	ScreenCaptureDirectX(hMon, hdc, lprcMonitor);
	//else
	ScreenCaptureGDI(hMon, hdc, lprcMonitor);
}

//void ScreenRecorder::ScreenCaptureDirectX(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor)
//{
//	spdlog::info("DirectX Screen Capture");
//
//	struct ScreenData *screen = NULL;
//	screen = GetScreenData(hMon);
//	if (!screen->bCapture)
//		return;
//	int bpp = 32;
////**************************************************** Direct X Capture
//	time_t startTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
//
//	D3DLOCKED_RECT lockedRect;
//	ZeroMemory(&lockedRect, sizeof(D3DLOCKED_RECT));
//
//	HRESULT hr;
//	hr = screen->g_pd3dDevice->GetFrontBufferData(0, screen->g_pSurface);
//	if (hr == D3DERR_DRIVERINTERNALERROR)
//		return;
//	if (hr == D3DERR_DEVICELOST)
//		return;
//	if (hr == D3DERR_INVALIDCALL)
//		return;
//	if (FAILED(hr))
//		return;
//	hr = screen->g_pSurface->LockRect(&lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
//	if (FAILED(hr))
//		return;
//	BYTE *screenBuffer = NULL;
//	struct CompareBuffer *pCompBuffer = NULL;
//
//	screenBuffer = screen->lpScreen;
//	struct bufferRecord *pRecBuffer;
//
//	pRecBuffer = (struct bufferRecord *)pRecordBufferManager->GetFreeScreenBuffer(screen);
//	pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
//	pCompBuffer->screenNum = hMon;
//	pCompBuffer->numOfBlocks = 0;
//	pCompBuffer->numBytes = 0L;
//
//	BYTE *compBufferFullScreen = (BYTE *)&pCompBuffer->blockData;
//	int screenWidth, screenHeight;
//
//	screenWidth = screen->rcMonitor.right - screen->rcMonitor.left; // screen->ddm.Width;
//	screenHeight = screen->rcMonitor.bottom - screen->rcMonitor.top; // screen->ddm.Height;
//	long blockCopy = 0;
//
//	char *src = (char *)lockedRect.pBits;
//	if (screen->bFullScreen)
//	{
//		pCompBuffer->numOfBlocks = -1;
//		pCompBuffer->numBytes = PackData((long *)screenBuffer, (long *)src, screenHeight, screenWidth, (long *)compBufferFullScreen);
//	}
//	else
//	{
//		PackData((long *)src, (long *)src, screenHeight, screenWidth);
//		pCompBuffer->numBytes = CompareData((long *)compBufferFullScreen, (long *)screenBuffer, (long *)src, screenHeight, screenWidth, &pCompBuffer->numOfBlocks);
//	}
//
//	pRecBuffer->timeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
//
//	hr = screen->g_pSurface->UnlockRect();
////*********************************************************************END DIRECT X Capture ************/
//	pRecBuffer->mouseX = (short)mousePosition.x;
//	pRecBuffer->mouseY = (short)mousePosition.y;
//	pRecBuffer->recDurationus = recDuration;
//
//	// Now can send the buffer to Write thread
//	pRecordBufferManager->WriteToFile(pRecBuffer);
//}

void ScreenRecorder::ScreenCaptureGDI(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor)
{
	UNREFERENCED_PARAMETER(lprcMonitor);
	UNREFERENCED_PARAMETER(hdc);

	spdlog::info("GDI Screen Capture");
	struct ScreenData *screen = NULL;
	screen = GetScreenData(hMon);
	if (!screen->bCapture)
		return;
	//int bpp = 32;
	/********************************BIT BLT Capture  *************************/
	//time_t startTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	time_t startSecTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	hdcWindow = hdc;
	int screenWidth, screenHeight;
	screenWidth = screen->rcMonitor.right - screen->rcMonitor.left; // screen->ddm.Width;
	screenHeight = screen->rcMonitor.bottom - screen->rcMonitor.top; // screen->ddm.Height;

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow, screenWidth, screenHeight);
	// Select the compatible bitmap into the compatible memory DC.
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC,
		0, 0,
		screenWidth, screenHeight,
		hdcWindow,
		screen->rcMonitor.left, screen->rcMonitor.top,
		SRCCOPY | CAPTUREBLT))
	{
		;
	}
	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = -bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcMemDC, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		screen->lpScreenNew,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	time_t endSecTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	//time_t spanCaptureTime = endSecTime - startSecTime;
	startSecTime = endSecTime;


	BYTE *screenBuffer = NULL;
	struct CompareBuffer *pCompBuffer = NULL;
	screenBuffer = screen->lpScreen;
	struct bufferRecord *pRecBuffer;

	pRecBuffer = (struct bufferRecord *)pRecordBufferManager->GetFreeScreenBuffer(screen);
	pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
	pCompBuffer->screenNum = hMon;
	pCompBuffer->numOfBlocks = 0;
	pCompBuffer->numBytes = 0L;
	
	BYTE *compBufferFullScreen = (BYTE *)&pCompBuffer->blockData;
	long packSize = 0L;
	long newPackSize = 0L;
	// Screen data is in new screen buffer
	char *src = (char *)screen->lpScreenNew;
	static int packCnt = 0;
	if (screen->bFullScreen)
	{
		//pRecBuffer->buffType = 'I';
		// If full screen to be stored then copy to the screen buffer in screen structure and to compBufferFullScreen to store in the file
		pCompBuffer->numOfBlocks = -1;
		packSize = PackData((long *)screenBuffer, (long *)src, screenHeight, screenWidth, (long *)compBufferFullScreen);
	}
	else
	{
		//pRecBuffer->buffType = 1;
		// Pack the source data before comparing
		newPackSize = PackData((long *)src, (long *)src, screenHeight, screenWidth);
		packSize = CompareData((long *)compBufferFullScreen, (long *)screenBuffer, (long *)src, screenHeight, screenWidth, &pCompBuffer->numOfBlocks);
		CopyMemory(screenBuffer, src, newPackSize);
	}
	pCompBuffer->numBytes = packSize;
	pRecBuffer->timeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	SelectObject(hdcMemDC, hOldBitmap);
	DeleteObject(hbmScreen);
	DeleteDC(hdcMemDC);
	endSecTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	//time_t spanCompareTime = endSecTime - startSecTime;
	startSecTime = endSecTime;

	//WriteImageBMP(screen);
	//time_t endTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	pRecBuffer->mouseX = (short)mousePosition.x;
	pRecBuffer->mouseY = (short)mousePosition.y;
	pRecBuffer->recDurationus = recDuration;
	// Now can send the buffer to Write thread
	pRecordBufferManager->WriteToFile(pRecBuffer);
}

void ScreenRecorder::WriteImageBMP(struct ScreenData *screen)
{
	static int numImageBmp = 0;
	BITMAPINFO bmi;

	int width, height = 0;

	width = screen->rcMonitor.right - screen->rcMonitor.left;
	height = screen->rcMonitor.bottom - screen->rcMonitor.top;
	int bpp = 32;

	/* Create a DIB and select it into the dest_hdc */
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bpp;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	// Part 2
	int headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		(bpp <= 8 ? (1 << bpp) : 0) * sizeof(RGBQUAD) /* palette size */;
	int frameSize = width * height * 4;

	BITMAPFILEHEADER bfh;
	int file_size = headerSize + frameSize;

	bfh.bfType = 0x4d42; /* "BM" in little-endian */
	bfh.bfSize = file_size;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = headerSize;

	// Part 3

	char fileName[100];

	sprintf_s(fileName, "c:\\Recorder\\ScreenBitmaps\\Screen-%06d.bmp", numImageBmp);
	wchar_t wcstring[100];
	numImageBmp++;

	// Convert char* string to a wchar_t* string.  
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, strlen(fileName) + 1, fileName, _TRUNCATE);

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFile(wcstring,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	//DWORD dwSizeofDIB = file_size;

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)screen->lpScreen, frameSize, &dwBytesWritten, NULL);
	//Close the handle for the file that was created
	CloseHandle(hFile);
}

//bool ScreenRecorder::InitializeDirectXAdaptor(int adaptorNum, struct ScreenData *screen)
//{
//	D3DPRESENT_PARAMETERS	d3dpp;
//
//	if (FAILED(g_pD3D->GetAdapterDisplayMode(adaptorNum, &screen->ddm)))
//	{
//		return false;
//	}
//	int width, height;
//	width = screen->rcMonitor.right - screen->rcMonitor.left;
//	height = screen->rcMonitor.bottom - screen->rcMonitor.top;
////	width = screen->ddm.Width;
////	height = screen->ddm.Height;
//
//	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
//	d3dpp.Windowed = TRUE;
//	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
//	d3dpp.BackBufferFormat = screen->ddm.Format;
//	d3dpp.BackBufferHeight = height;
//	d3dpp.BackBufferWidth = width;
//	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
//	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
//	d3dpp.hDeviceWindow = GetDesktopWindow();
//	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
//	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//
//	DWORD dwBmpSize = height * width * 4;
//	screen->lpScreen = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBmpSize);
//	if (screen->lpScreen == NULL)
//		return false;
//
//	HRESULT hr = g_pD3D->CreateDevice(adaptorNum, D3DDEVTYPE_HAL, GetDesktopWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &screen->g_pd3dDevice);
//	if (hr == D3DERR_DEVICELOST)
//		return false;
//	if (hr == D3DERR_INVALIDCALL)
//		return false;
//	if (hr == D3DERR_NOTAVAILABLE)
//		return false;
//	if (hr == D3DERR_OUTOFVIDEOMEMORY)
//		return false;
//
//	if (FAILED(hr))
//	{
//		return false;
//	}
//
//	if (FAILED(screen->g_pd3dDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &screen->g_pSurface, NULL)))
//	{
//		return false;
//	}
//
//	return true;
//
//}

void ScreenRecorder::MonitorDataCapture(HMONITOR hMon, LPRECT lprcMonitor)
{
	pDataRecorder->WriteMonitorData(hMon, lprcMonitor);
}


static BOOL CALLBACK MonitorDataCaptureMonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	UNREFERENCED_PARAMETER(hdc);
	ScreenRecorder* pThis = reinterpret_cast<ScreenRecorder *>(pData);
	pThis->MonitorDataCapture(hMon, lprcMonitor);
	return TRUE;
}

HMONITOR ScreenRecorder::GetMonitorNum(POINT pt)
{
	HMONITOR hMon = NULL;

	for (std::vector<struct ScreenData *>::iterator it = screenData.begin(); it != screenData.end(); ++it)
	{
		struct ScreenData *screenPtr = (struct ScreenData *) *it;
		if (PtInRect(&screenPtr->rcMonitor, pt))
		{
			hMon = screenPtr->hMonitor;
			break;
		}
	}
	return hMon;
}

void ScreenRecorder::SetDataManager(RecorderDataFileManager *pDataFileManager)
{
	pDataRecorder = pDataFileManager;
}

void ScreenRecorder::SetRecordBufferManager(RecordBufferManager *pRecManager)
{
	pRecordBufferManager = pRecManager;
}

void ScreenRecorder::SetIndexInterval(int iInterval)
{
	indexInterval = iInterval;
}

void ScreenRecorder::SetHWnd(HWND hWnd)
{
	hWndPlayer = hWnd;
}


long ScreenRecorder::CompareData(long *dest, long *screenLHS, long *screenRHS, int height, int width, long *numBlocks)
{
	*numBlocks = 0;
	long size = 0L;
	int nRow, nCol;
	long leftPx = 0L;
	long rightPx = 0L;
	int gap = 0;
	int num = 0;
	bool bStoreMode = false;
	struct blockDetails *pBlockDetails = NULL;

	for (nRow = 0; nRow < height; nRow++)
	{
		gap = num = 0;
		bStoreMode = false;
		for (nCol = 0; nCol < width; )
		{
			leftPx = *screenLHS;
			rightPx = *screenRHS;
			// do a preliminary check on the data and go down only if mismatch
			if (gap == 0)
			{
				if (leftPx == rightPx) // if both are the same
				{
					nCol += (leftPx) & 0xffL;
					screenLHS++;
					screenRHS++;
					bStoreMode = false;
					continue;
				}
			}
			while (gap <= 0)
			{
				leftPx = *screenLHS;
				screenLHS++;
				num = leftPx & 0xffL;
				gap += num;
			}

			screenRHS++;
			num = rightPx & 0xffL;
			gap -= num;

			if (gap == 0)
			{
				if (leftPx != rightPx)
				{
					if (!bStoreMode)
					{
						bStoreMode = true;
						pBlockDetails = (struct blockDetails *)dest;
						pBlockDetails->lineNum = nRow;
						pBlockDetails->structPos = nCol;
						pBlockDetails->numPixels = 0;
						dest = (long *)((char *)pBlockDetails + sizeof(struct blockDetails));
						size += sizeof(struct blockDetails);
						*numBlocks = *numBlocks + 1;
					}
					pBlockDetails->numPixels += 1;
					*dest = rightPx;
					dest++;
					size += 4L;
				}
				else if (bStoreMode)
				{
					bStoreMode = false;
				}
			}
			else
			{
				if (!bStoreMode)
				{
					bStoreMode = true;
					pBlockDetails = (struct blockDetails *)dest;
					pBlockDetails->lineNum = nRow;
					pBlockDetails->structPos = nCol;
					pBlockDetails->numPixels = 0;
					*numBlocks = *numBlocks + 1;
					size += sizeof(struct blockDetails);
					dest = (long *)((char *)pBlockDetails + sizeof(struct blockDetails));
				}
				pBlockDetails->numPixels += 1;
				*dest = rightPx;
				dest++;
				size += 4L;
			}
			nCol += num;
		}
	}

	return size;
}



