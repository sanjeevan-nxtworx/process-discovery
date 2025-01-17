#pragma once
#include <wx/wx.h>
#include "wx/dcbuffer.h"

class LEDDisplayPanel :
	public wxPanel
{
	wxWindow *pParent;
	int thickness;
	int height;
	int gap;
	wxColor foreColor;
	wxColor backColor;
	wxString textValue;

public:
	LEDDisplayPanel(wxWindow * parent, wxPoint pt, wxSize size);

	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void SetHeight(int nHeight);
	void SetThickness(int nThickness);
	void SetGap(int nGap);
	void SetForeColor(wxColor wxforeColor);
	void SetBackColor(wxColor wxbackColor);
	void SetText(wxString value);

	DECLARE_EVENT_TABLE()
};

