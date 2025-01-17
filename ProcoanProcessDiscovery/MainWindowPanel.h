#pragma once
#include <wx/wx.h>
class ProcoanButton;

class MainWindowPanel :
	public wxPanel
{
	wxFrame *pParent;

	ProcoanButton *recorderButton;
	ProcoanButton *editorButton;
	ProcoanButton *playerButton;
	ProcoanButton *spyButton;
	ProcoanButton *closeButton;
	ProcoanButton *iconButton;

public:
	MainWindowPanel(wxFrame * parent, wxSize size);
	~MainWindowPanel();

	void OnExit(wxCommandEvent& event);
	void OnRecord(wxCommandEvent& event);
	void OnEditor(wxCommandEvent& event);
	void OnSpy(wxCommandEvent& event);

	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);

	DECLARE_EVENT_TABLE()
};

