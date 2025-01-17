#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include "wxImagePanel.h"
#include <wx/timer.h>
#include "UIUtility.h"
#include <chrono>

class RecorderTitleBarPanel;
class RecorderInfoBarPanel;
class RecorderDisplayPanel;
class RecorderButtonBar;
class RecorderMinimizePanel;

class RecorderUI: public wxDialog
{

	RecorderTitleBarPanel *titlePanel;
	RecorderInfoBarPanel *infoBarPanel;
	RecorderDisplayPanel *displayPanel;
	RecorderButtonBar *buttonPanel;
	RecorderMinimizePanel *minimizePanel;
	time_t prevTime, newTime;
	long long  msTime;

	wxTimer timer;
	int recorderMode;

	wxPoint m_delta;
	wxWindow *parentWnd;
	ULONG recTime;
	unsigned short audioValue;


	wxString strDirectory;
	wxString strFileName;
	bool bVideoFlg;
	bool bAudioFlg;
	bool bEventFlg;
	int  nFPS;
	bool bActiveMonitorFlg;
	std::list <struct monitorDetails *> monitorList;

public:
	RecorderUI(wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & position, const wxSize & size);
	~RecorderUI();
	void OnExit(wxCommandEvent& event);
	void OnMaximize(wxCommandEvent& event);
	void OnMinimize(wxCommandEvent& event);
	void OnMouseMove(wxMouseEvent & event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & WXUNUSED(event));
	void OnStop(wxCommandEvent & event);
	void OnPause(wxCommandEvent & event);
	void OnRecord(wxCommandEvent & event);
	void OnTimer(wxTimerEvent& event);
	void SetMonitorList(std::list <struct monitorDetails *> *pList);
	void SetVideoFlg(bool flg);
	void SetAudioFlg(bool flg);
	void SetEventFlg(bool flg);
	void SetActiveMonitorFlg(bool flg);
	void SetFPS(int fps);
	void SetDirectory(wxString dir);
	void SetFilename(wxString filename);
	void UpdateUI();
	void paintEvent(wxPaintEvent &evt);
	void render(wxDC & dc);
	DECLARE_EVENT_TABLE()
};

