#pragma once

#ifndef __PECAN_MAINFRAME_H 
// Make sure to only declare these classes once 
#define __PECAN_MAINFRAME_H 
#include "resource.h"
#include "CustomEvents.h"

class ProcoanButton;
class MainWindowPanel;

class PecanMainFrame : public wxFrame
{
	// It contains the window and all objects in it 
	wxPoint m_delta;
	MainWindowPanel *backPanel;

public:
	PecanMainFrame(int width);
	void OnExit(wxCommandEvent& event);
	void OnRecord(wxCommandEvent& event);
	void OnPlayer(wxCommandEvent& event);
	void OnEditor(wxCommandEvent& event);
	void OnSpy(wxCommandEvent& event);

	void OnMouseMove(wxMouseEvent & event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};

#endif

