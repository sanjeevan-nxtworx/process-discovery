#pragma once
#include <wx/wx.h>
class TimeDisplay: public wxWindow
{
	wxWindow *pParent;
	wxString timeText;

public:
	TimeDisplay(wxWindow * parent, ULONG id, wxPoint pt, wxSize size);
	void paintEvent(wxPaintEvent &evt);
	void SetTimeText(wxString timeVal);
	void paintNow();
	void render(wxDC & dc);
	DECLARE_EVENT_TABLE()
};

