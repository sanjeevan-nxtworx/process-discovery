#pragma once
#include <wx/wx.h>
#include "wx/dcbuffer.h"

class AudioGauge :
	public wxPanel
{
	wxWindow *pParent;
	wxColor foreColor;
	wxColor backColor;
	wxColor LEDbackColor;
	unsigned short guageValue;

public:
	AudioGauge(wxWindow * parent, wxPoint pt, wxSize size);

	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void SetForeColor(wxColor wxforeColor);
	void SetBackColor(wxColor wxbackColor);
	void SetLEDBackColor(wxColor wxbackColor);
	//void SetAudioValue(unsigned short value);

	DECLARE_EVENT_TABLE()
};

