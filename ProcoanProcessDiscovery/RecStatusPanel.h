#pragma once
#include <wx/wx.h>

class RecStatusPanel :
	public wxPanel
{
	wxWindow *pParent;
	wxColor foreColor;
	wxColor backColor;
	wxString textVal;

public:
	RecStatusPanel(wxWindow * parent, wxPoint pt, wxSize size);

	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void SetForeColor(wxColor wxforeColor);
	void SetBackColor(wxColor wxbackColor);
	void SetValue(wxString str);
	DECLARE_EVENT_TABLE()
};

