#pragma once

#include <wx/wx.h>
#include "wx/dcbuffer.h"
#include "UIUtility.h"

class LEDDisplayPanel;
class AudioGauge;
class RecStatusPanel;

class RecorderDisplayPanel :
	public wxPanel
{
	wxWindow *pParent;
	LEDDisplayPanel *ledPanel;
	AudioGauge *audioGauge;
	bool bVideoFlg;
	bool bAudioFlg;
	bool bEventFlg;
	int  nFPS;
	bool bActiveMonitorFlg;
	std::list <struct monitorDetails *> *pMonitorList;
	RecStatusPanel *pRecState;
	wxStaticText *pShutdownStatus;
	int recMode;
	int nBlocksWritten;
	int nTotalBlocks;

public:
	RecorderDisplayPanel(wxWindow * parent, wxSize size);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void SetAudioValue(unsigned short audioValue);
	void SetRecTimeValue(ULONG recTime);
	void SetMonitorList(std::list <struct monitorDetails *> *pList);
	void SetVideoFlg(bool flg);
	void SetAudioFlg(bool flg);
	void SetEventFlg(bool flg);
	void SetActiveMonitorFlg(bool flg);
	void SetFPS(int fps);
	void SetRecorderMode(int mode);
	void SetRecorderWriteStatus(int numBlocksWritten, int totalBlocks, bool bFlg);
	DECLARE_EVENT_TABLE()
};


