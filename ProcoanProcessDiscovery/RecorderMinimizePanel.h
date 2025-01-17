#pragma once
#include <wx/wx.h>
class ProcoanButton;
class LEDDisplayPanel;

class RecorderMinimizePanel :
	public wxPanel
{
	wxWindow *pParent;

	ProcoanButton *stopButton;
	ProcoanButton *pauseButton;
	ProcoanButton *recordButton;
	ProcoanButton *iconButton;
	ProcoanButton *maximizeButton;
	ProcoanButton *closeButton;
	LEDDisplayPanel *ledPanel;
public:
	RecorderMinimizePanel(wxWindow * parent, wxSize size);
	void SetRecTimeValue(ULONG recTime);
	void OnStop(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void OnRecord(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnMaximize(wxCommandEvent& event);

	void SetOnRecordButton();
	void SetOnPauseButton();
	void SetOnStopButton();

	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);

	DECLARE_EVENT_TABLE()
};


