#include "stdafx.h"
#include "AudioGauge.h"
#include "UIUtility.h"
#include "wx/graphics.h"

BEGIN_EVENT_TABLE(AudioGauge, wxPanel)
// some useful events
// catch paint events
EVT_PAINT(AudioGauge::paintEvent)
END_EVENT_TABLE()

AudioGauge::AudioGauge(wxWindow *parent, wxPoint pt, wxSize size) :
	wxPanel(parent, -1, pt, size, wxNO_BORDER)
{
	pParent = parent;
	guageValue = 0;
}

void AudioGauge::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void AudioGauge::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void AudioGauge::render(wxDC&  dc)
{
	if (guageValue > 224)
	{
		dc.SetBrush(wxBrush(wxColor(0xc3, 0xc5, 0xc9)));
		dc.SetPen(wxPen(wxColor(0xc3, 0xc5, 0xc9)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 4, 50, 6));

	if (guageValue > 192)
	{
		dc.SetBrush(wxBrush(wxColor(0xb5, 0xbd, 0xc9)));
		dc.SetPen(wxPen(wxColor(0xb5, 0xbd, 0xc9)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 12, 50, 6));

	if (guageValue > 160)
	{
		dc.SetBrush(wxBrush(wxColor(0xad, 0xbb, 0xd1)));
		dc.SetPen(wxPen(wxColor(0xad, 0xbb, 0xd1)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 20, 50, 6));

	if (guageValue > 128)
	{
		dc.SetBrush(wxBrush(wxColor(0x75, 0x97, 0xce)));
		dc.SetPen(wxPen(wxColor(0x75, 0x97, 0xce)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 28, 50, 6));

	if (guageValue > 96)
	{
		dc.SetBrush(wxBrush(wxColor(0x4f, 0x7f, 0xcc)));
		dc.SetPen(wxPen(wxColor(0x4f, 0x7f, 0xcc)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 36, 50, 6));

	if (guageValue > 64)
	{
		dc.SetBrush(wxBrush(wxColor(0x3c, 0x75, 0xd1)));
		dc.SetPen(wxPen(wxColor(0x3c, 0x75, 0xd1)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 44, 50, 6));

	if (guageValue > 32)
	{
		dc.SetBrush(wxBrush(wxColor(0x2b, 0x6d, 0xd8)));
		dc.SetPen(wxPen(wxColor(0x2b, 0x6d, 0xd8)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 52, 50, 6));

	if (guageValue > 0)
	{
		dc.SetBrush(wxBrush(wxColor(0x6, 0x59, 0xdd)));
		dc.SetPen(wxPen(wxColor(0x6, 0x59, 0xdd)));
	}
	else
	{
		dc.SetBrush(wxBrush(LEDbackColor));
		dc.SetPen(wxPen(LEDbackColor));
	}
	dc.DrawRectangle(wxRect(2, 60, 50, 6));
}

void AudioGauge::SetForeColor(wxColor wxforeColor)
{
	foreColor = wxforeColor;
}

void AudioGauge::SetBackColor(wxColor wxbackColor)
{
	backColor = wxbackColor;
	this->SetBackgroundColour(backColor);
}

void AudioGauge::SetLEDBackColor(wxColor wxbackColor)
{
	LEDbackColor = wxbackColor;
}

//void AudioGauge::SetAudioValue(unsigned short value)
//{
//	guageValue = value;
//}