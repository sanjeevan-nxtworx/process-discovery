#pragma once
#include <wx/wx.h>
class ProcoanButton;

class RecorderTitleBarPanel :
	public wxPanel
{
	wxWindow *pParent;

	ProcoanButton *procoanButton;
	ProcoanButton *minimizeButton;
	ProcoanButton *closeButton;

public:
	RecorderTitleBarPanel(wxWindow * parent, wxSize size);

	void OnExit(wxCommandEvent& event);
	void OnMinimize(wxCommandEvent& event);

	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);

	DECLARE_EVENT_TABLE()
};


