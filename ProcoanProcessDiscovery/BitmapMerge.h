#pragma once

class RecordBufferManager;

class BitmapMerge
{
	int nBitmaps;
	int *nWidths;
	int *nHeights;
	char **pBitBuffers;
	char **pResizeBuffers;
	ULONG *pResizeBufferLen;
	int *nResizeWidths;
	int *nResizeHeights;
	int *nThumbWidths;
	int *nThumbHeights;
	char **pThumbBuffers;
	ULONG *pThumbBufferLen;

	//char *pBMPBuffer;
	int nBMPWidth;
	int nBMPHeight;

	bool bActiveMonitorDisplay;
	int nActiveMonitorNum;
	int nResizeWindowHeight;
	int nResizeWindowWidth;

	RecordBufferManager *pRecBufferManager;

	int CalcBMPWidth();
	int CalcBMPHeight();
	int CalcActualWidth();
	int CalcActualHeight();
	int GetBitmapXOffset(int num);
	int GetBitmapYOffset(int num);
	int GetResizeBitmapXOffset(int num);
	int GetResizeBitmapYOffset(int num);
	int GetThumbBitmapXOffset(int num);
	int GetThumbBitmapYOffset(int num);
	int CalcThumbActualWidth();
	int CalcThumbActualHeight();


public:
	void SetDisplayMode(bool bActiveDisplayOn);
	void SetActiveMonitor(int monNum);
	void SetResizeWindowSize(int windowWidth, int windowHeight);
	void SetNewResizeWindowSize(int windowWidth, int windowHeight);
	bool GetDisplayMode();
	void InitResizeMerge();
	//void Init();
	int GetBMPWidth();
	int GetBMPHeight();
	//char *GetBMPBuffer();
	void SetRecordBufferManager(RecordBufferManager *pRecManager);
	BitmapMerge(int noBitmaps);
	~BitmapMerge();
	void SetSize(int bmpNo, int width, int height);
	void SetBuffer(int bmpNo, char *bitBuffer);
	void DrawCursorInBitmap(struct ResizeScreenBuffer *pResizeBuffer, int monNum, RECT r);
	void DrawRectangleInBitmap(struct ResizeScreenBuffer *pResizeBuffer, int monNum, RECT r, bool inPt);
	void DrawHorizLine(struct ResizeScreenBuffer *pResizeBuffer, int leftCol, int row, int rightCol, bool inPt);
	void DrawVertLine(struct ResizeScreenBuffer *pResizeBuffer, int col, int topRow, int botRow, bool inPt);
	//void MergeBitmaps(int monNum);
	void MergeResizeBitmaps(int monNum, struct ResizeScreenBuffer *pResizeScreenBuffer);
	void MergePrintImage(vector<unsigned char>& buffer, int monNum, ULONG& cx, ULONG& cy);
	void ResizePrintImage(vector<unsigned char>& buffer, ULONG& cx, ULONG& cy);


	void MergeThumbResizeBitmaps(int monNum, struct ResizeScreenBuffer *pResizeScreenBuffer, struct MonitorScreenData *pMonitorDetails);
};

