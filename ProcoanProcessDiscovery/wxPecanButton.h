#pragma once

#include <wx/wx.h>
#include <wx/event.h>
class wxPecanButton : public wxButton
{
	wxBitmap normalBitmap;
	wxBitmap disabledBitmap;
	wxBitmap pressedBitmap;
	wxBitmap focusBitmap;

	bool mouseInControl;
	bool pressedDown;


public:
	wxPecanButton(wxWindow *parent, wxWindowID id, const wxString &label = wxEmptyString, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0, const wxValidator &validator = wxDefaultValidator, const wxString &name = wxButtonNameStr);
	void SetBitmapCurrent(wxBitmap bmpValue);
	void SetBitmapDisabled(wxBitmap bmpValue);
	void SetBitmapPressed(wxBitmap bmpValue);
	void SetBitmapFocus(wxBitmap bmpValue);

	void paintEvent(wxPaintEvent & evt);
	void render(wxDC& dc);

	// some useful events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()
};

