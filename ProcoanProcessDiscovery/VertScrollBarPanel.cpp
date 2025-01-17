#include "stdafx.h"
#include "VertScrollBarPanel.h"
#include "UIUtility.h"
#include "CustomEvents.h"

wxDEFINE_EVENT(PROCOAN_VSCROLL_MOVEABSOLUTE, wxCommandEvent);

BEGIN_EVENT_TABLE(VertScrollBarPanel, wxPanel)
// catch paint events

EVT_MOTION(VertScrollBarPanel::OnMouseMove)
EVT_LEFT_DOWN(VertScrollBarPanel::OnLeftDown)
EVT_LEFT_UP(VertScrollBarPanel::OnLeftUp)
EVT_PAINT(VertScrollBarPanel::paintEvent)

END_EVENT_TABLE()



VertScrollBarPanel::VertScrollBarPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size) :
	wxPanel(parent, id, pt, size, wxNO_BORDER)
{
	controlID = id;
	pParent = parent;
	backColor = wxColor(RGB(0x7f, 0x7f, 0x7f));
	knobColor = wxColor(RGB(0, 0, 0xff));
	segColor = wxColor(RGB(0x7f, 0, 0));
	maxVal = 100L;
	currVal = 0L;
	thumbSize = 5;
	savedPos = 0;
}

void VertScrollBarPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void VertScrollBarPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}


void VertScrollBarPanel::OnLeftDown(wxMouseEvent & event)
{
	wxSize winSize = this->GetSize();
	int currPos = (currVal * winSize.GetHeight()) / maxVal;

	int sliderTop = currPos;
	wxPoint pos = event.GetPosition();
	int rightPos = (thumbSize * thumbSize) / maxVal;
	if (wxRect(0, sliderTop, 10, rightPos).Contains(pos))
	{
		savedPos = event.GetY();
		CaptureMouse();
	}
}

void VertScrollBarPanel::OnLeftUp(wxMouseEvent &event)
{
	if (HasCapture())
	{
		ReleaseMouse();
	}
	else
	{
		wxPoint pt = event.GetPosition();
		wxSize winSize = this->GetSize();
		int rightPos = (thumbSize * thumbSize) / maxVal;
		int currPos = (currVal * winSize.GetHeight()) / maxVal;

		if (pt.y < currPos)
			currVal -= thumbSize;
		if (currVal < 0)
			currVal = 0;
		if (pt.y > (currPos + rightPos))
			currVal += thumbSize;
		if ((currVal + thumbSize) > maxVal)
			currVal = maxVal - thumbSize;
		//paintNow();
		Refresh();
	}
	wxCommandEvent customEvent(PROCOAN_VSCROLL_MOVEABSOLUTE, GetId());
	customEvent.SetEventObject(this);

	// Do send it
	ProcessWindowEvent(customEvent);
}

void VertScrollBarPanel::OnMouseMove(wxMouseEvent & event)
{
	wxPoint pt = event.GetPosition();
	wxSize winSize = this->GetSize();

	if (HasCapture())
	{
		int diff = pt.y - savedPos;
		savedPos = pt.y;
		currVal += (diff	* maxVal) / winSize.GetHeight();
		
		if (currVal < 0)
			currVal = 0;
		if ((currVal + thumbSize) > maxVal)
			currVal = maxVal - thumbSize;
		Refresh();
		//paintNow();
		wxCommandEvent customEvent(PROCOAN_VSCROLL_MOVEABSOLUTE, GetId());
		customEvent.SetEventObject(this);

		// Do send it
		ProcessWindowEvent(customEvent);
	}
}


void VertScrollBarPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();
	dc.SetBrush(wxBrush(backColor));
	dc.SetPen(wxPen(wxColor(RGB(0,0,0))));
	dc.DrawRectangle(wxPoint(0, 0), winSize);

	if (maxVal == 0)
		return;

	int currPos = (currVal * winSize.GetHeight()) / maxVal;
	int rightPos = (thumbSize * thumbSize) / maxVal;

	dc.SetBrush(wxBrush(knobColor));
	dc.SetPen(wxPen(knobColor));

	dc.DrawRoundedRectangle(wxPoint(1, currPos), wxSize(winSize.GetWidth()-2, rightPos), 2);
}

void VertScrollBarPanel::SetBackColor(wxColor color)
{
	backColor = color;
	//paintNow();
	Refresh();
}

wxColor VertScrollBarPanel::GetBackColor(void)
{
	return backColor;
}

void VertScrollBarPanel::SetKnobColor(wxColor color)
{
	knobColor = color;
	//paintNow();
	Refresh();
}

wxColor VertScrollBarPanel::GetKnobColor(void)
{
	return knobColor;
}

void VertScrollBarPanel::SetSegmentColor(wxColor color)
{
	segColor = color;
	//paintNow();
	Refresh();
}

wxColor VertScrollBarPanel::GetSegmentColor(void)
{
	return segColor;
}

void VertScrollBarPanel::SetMaxValue(ULONG value)
{
	maxVal = value;
	//paintNow();
	Refresh();
}

ULONG VertScrollBarPanel::GetMaxValue(void)
{
	return maxVal;
}

void VertScrollBarPanel::SetCurrentValue(ULONG value)
{
	currVal = value;
	//paintNow();
	Refresh();
}

ULONG VertScrollBarPanel::GetCurrentValue(void)
{
	return currVal;
}

void VertScrollBarPanel::SetThumbSize(ULONG value)
{
	thumbSize = value;
	//paintNow();
	Refresh();
}

ULONG VertScrollBarPanel::GetThumbSize(void)
{
	return thumbSize;
}

