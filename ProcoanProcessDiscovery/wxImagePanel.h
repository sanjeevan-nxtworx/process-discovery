#pragma once
#include <wx/wx.h>
#include <wx/sizer.h>
#include "wxImagePanel.h"

class wxImagePanel : public wxPanel
{
	wxBitmap image;

public:
	wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format, wxPoint pt, wxSize sZ);

	void paintEvent(wxPaintEvent & evt);
	void paintNow();

	void render(wxDC& dc);

	// some useful events
	/*
	 void mouseMoved(wxMouseEvent& event);
	 void mouseDown(wxMouseEvent& event);
	 void mouseWheelMoved(wxMouseEvent& event);
	 void mouseReleased(wxMouseEvent& event);
	 void rightClick(wxMouseEvent& event);
	 void mouseLeftWindow(wxMouseEvent& event);
	 void keyPressed(wxKeyEvent& event);
	 void keyReleased(wxKeyEvent& event);
	 */

	DECLARE_EVENT_TABLE()
};


