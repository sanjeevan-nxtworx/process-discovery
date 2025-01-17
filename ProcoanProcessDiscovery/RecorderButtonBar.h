#pragma once
#include <wx/wx.h>
class ProcoanButton;

class RecorderButtonBar :
	public wxPanel
{
	wxWindow *pParent;

	ProcoanButton *stopButton;
	ProcoanButton *pauseButton;
	ProcoanButton *recordButton;

public:
	RecorderButtonBar(wxWindow * parent, wxSize size);

	void OnStop(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void OnRecord(wxCommandEvent& event);
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

