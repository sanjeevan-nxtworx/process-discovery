#include "stdafx.h"
#include "SliderPanel.h"
#include "UIUtility.h"
#include "CustomEvents.h"
#include <chrono>

wxDEFINE_EVENT(PROCOAN_SLIDER_MOVEABSOLUTE, wxCommandEvent);
wxDEFINE_EVENT(PROCOAN_SLIDER_HOVER, wxCommandEvent);
wxDEFINE_EVENT(PROCOAN_SLIDER_HOVER_CLEAR, wxCommandEvent);
wxDEFINE_EVENT(PROCOAN_SLIDER_UPDATETIME, wxCommandEvent);

BEGIN_EVENT_TABLE(SliderPanel, wxPanel)
// catch paint events

EVT_MOTION(SliderPanel::OnMouseMove)
EVT_LEFT_DOWN(SliderPanel::OnLeftDown)
EVT_LEFT_UP(SliderPanel::OnLeftUp)
EVT_PAINT(SliderPanel::paintEvent)
EVT_LEAVE_WINDOW(SliderPanel::OnLeaveWnd)
END_EVENT_TABLE()



SliderPanel::SliderPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size) :
	wxPanel(parent, id, pt, size, wxNO_BORDER)
{
	controlID = id;
	pParent = parent;
	backColor = wxColor(RGB(0x7f, 0x7f, 0x7f));
	sliderColor = wxColor(RGB(0, 0, 0x7f));
	knobColor = wxColor(RGB(0, 0, 0xff));
	segColor = wxColor(RGB(0x7f, 0, 0));
	maxVal = 0L;
	currVal = 0L;
	startSegVal = endSegVal = 0L;
	bSegInProgress = bSegPresent = false;
	bHoverOn = false;
	timer.Bind(wxEVT_TIMER, &SliderPanel::OnTimer, this);
	timer.Start(100);
	hoverXPos = 0;
	bIsHoverOn = false;
}

void SliderPanel::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	if (bHoverOn && maxVal != 0)
	{
		time_t newTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		if ((newTime - startTime) > 200000L)
		{
			startTime = 0;
			bHoverOn = false;
			bIsHoverOn = true;
			wxCommandEvent customEvent(PROCOAN_SLIDER_HOVER, GetId());
			customEvent.SetEventObject(this);
			// Do send it
			ProcessWindowEvent(customEvent);
		}
	}
}

void SliderPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void SliderPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}


void SliderPanel::OnLeftDown(wxMouseEvent & event)
{
	if (maxVal == 0)
		return;
	wxSize winSize = this->GetSize();
	ULONG currPos = (ULONG)(((long long)currVal * winSize.GetWidth()) / maxVal);
	
	int sliderTop = winSize.GetHeight() / 2 - 3;
	wxPoint pos = event.GetPosition();
	
	if (wxRect(currPos - 6, sliderTop, 12, 6).Contains(pos))
	{
		CaptureMouse();
	}
	wxCommandEvent customEvent(PROCOAN_SLIDER_HOVER_CLEAR, GetId());
	customEvent.SetEventObject(this);
	// Do send it
	ProcessWindowEvent(customEvent);


}

void SliderPanel::OnLeftUp(wxMouseEvent &event)
{
	if (maxVal == 0)
		return;
	if (HasCapture())
	{
		ReleaseMouse();
	}
	else
	{
		wxPoint pt = event.GetPosition();
		wxSize winSize = this->GetSize();
		currVal = (ULONG)((pt.x * (long long)maxVal) / winSize.GetWidth());
		//paintNow();
		Refresh();
	}
	wxCommandEvent customEvent(PROCOAN_SLIDER_MOVEABSOLUTE, GetId());
	customEvent.SetEventObject(this);

	// Do send it
	ProcessWindowEvent(customEvent);
}

void SliderPanel::AddPlayTime(ULONG time)
{
	static ULONG overflow;

	long long cValue = (((long long)currVal * 1000L) + overflow) + time;
	currVal = (long)(cValue / 1000L);
	if (currVal > maxVal)
		currVal = maxVal;
	if (bSegInProgress)
		endSegVal = currVal;
	overflow = (long)(cValue % 1000L);
	//paintNow();
	Refresh();
}

void SliderPanel::OnMouseMove(wxMouseEvent & event)
{
	if (maxVal == 0)
		return;
	wxPoint pt = event.GetPosition();
	wxSize winSize = this->GetSize();

	if (HasCapture())
	{
		if (pt.x < 0)
			pt.x = 0;
		currVal = (ULONG)((pt.x * (long long)maxVal) / winSize.GetWidth());
		if (currVal > maxVal)
			currVal = maxVal;
		paintNow();
		wxCommandEvent customEvent(PROCOAN_SLIDER_UPDATETIME, GetId());
		customEvent.SetEventObject(this);
		ProcessWindowEvent(customEvent);
		//Refresh();
	}
	else
	{
		startTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		hoverXPos = pt.x;
		bHoverOn = true;
		if (bIsHoverOn)
		{
			wxCommandEvent customEvent(PROCOAN_SLIDER_HOVER_CLEAR, GetId());
			customEvent.SetEventObject(this);
			// Do send it
			ProcessWindowEvent(customEvent);
		}
		bIsHoverOn = false;
	}
}

void SliderPanel::OnLeaveWnd(wxMouseEvent & WXUNUSED(event))
{
	if (maxVal == 0)
		return;
	bHoverOn = false;
	startTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	wxCommandEvent customEvent(PROCOAN_SLIDER_HOVER_CLEAR, GetId());
	customEvent.SetEventObject(this);
	// Do send it
	ProcessWindowEvent(customEvent);
}

void SliderPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();
	int sliderTop = winSize.GetHeight() / 2 - 3;
	dc.SetBrush(wxBrush(backColor));
	dc.SetPen(wxPen(backColor));
	dc.DrawRectangle(wxPoint(0, 0), winSize);

	dc.SetBrush(wxBrush(sliderColor));
	dc.SetPen(wxPen(sliderColor));
	dc.DrawRectangle(wxPoint(0, sliderTop), wxSize(winSize.GetWidth(), 6));

	if (bSegInProgress || bSegPresent)
	{
		ULONG startSegPos, endSegPos;

		startSegPos = (ULONG)(((long long)startSegVal * winSize.GetWidth()) / maxVal);
		endSegPos = (ULONG)((long long)endSegVal * winSize.GetWidth()) / maxVal;

		dc.SetBrush(wxBrush(segColor));
		dc.SetPen(wxPen(segColor));
		if (startSegPos > endSegPos)
		{
			dc.DrawRectangle(wxPoint((int)endSegPos, sliderTop), wxSize((int)(startSegPos - endSegPos), 6));
		}
		else
		{
			dc.DrawRectangle(wxPoint((int)startSegPos, sliderTop), wxSize((int)(endSegPos - startSegPos), 6));
		}
	}

	int currPos = 0;
	if (maxVal > 0)
		currPos = (ULONG)(((long long)currVal * winSize.GetWidth()) / maxVal);
	dc.SetBrush(wxBrush(knobColor));
	dc.SetPen(wxPen(knobColor));
	dc.DrawCircle(wxPoint(currPos, sliderTop + 3), 12);
}

void SliderPanel::SetBackColor(wxColor color)
{
	backColor = color;
	//paintNow();
	Refresh();
}

wxColor SliderPanel::GetBackColor(void)
{
	return backColor;
}

void SliderPanel::SetSliderColor(wxColor color)
{
	sliderColor = color;
	//paintNow();
	Refresh();
}

wxColor SliderPanel::GetSliderColor(void)
{
	return sliderColor;
}

void SliderPanel::SetKnobColor(wxColor color)
{
	knobColor = color;
	//paintNow();
	Refresh();
}

wxColor SliderPanel::GetKnobColor(void)
{
	return knobColor;
}

void SliderPanel::SetSegmentColor(wxColor color)
{
	segColor = color;
	//paintNow();
	Refresh();
}

wxColor SliderPanel::GetSegmentColor(void)
{
	return segColor;
}

void SliderPanel::SetMaxValue(ULONG value)
{
	maxVal = value;
	if (currVal > maxVal)
		currVal = maxVal;
	//paintNow();
	Refresh();
}

ULONG SliderPanel::GetMaxValue(void)
{
	return maxVal;
}

bool SliderPanel::IsSegInProgress()
{
	return bSegInProgress;
}

bool SliderPanel::IsSegPresent()
{
	return bSegPresent;
}

void SliderPanel::SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal)
{
	bSegInProgress = bInProgress;
	bSegPresent = bPresent;
	startSegVal = startVal;
	endSegVal = endVal;
	//paintNow();
	Refresh();
}

void SliderPanel::SetCurrentValue(ULONG value)
{
	currVal = value;
	if (bSegInProgress)
		endSegVal = currVal;
	//paintNow();
	Refresh();
}

ULONG SliderPanel::GetCurrentValue(void)
{
	return currVal;
}

void SliderPanel::SetStartSegValue(ULONG value)
{
	if (value == 0)
		value = 1;
	startSegVal = value;
	bSegInProgress = true;
	bSegPresent = false;
	//paintNow();
	Refresh();
}

ULONG SliderPanel::GetThumbTime()
{
	wxSize winSize = this->GetSize();
	ULONG posHover = (ULONG)((hoverXPos * (long long)maxVal) / winSize.GetWidth());
	return posHover;
}

int SliderPanel::GetHoverXPos()
{
	return hoverXPos;
}

ULONG SliderPanel::StartSegment()
{
	startSegVal = currVal;
	bSegInProgress = true;
	bSegPresent = false;
	return startSegVal;
}

ULONG SliderPanel::EndSegment()
{
	endSegVal = currVal;
	bSegInProgress = false;
	bSegPresent = true;
	return endSegVal;
}

ULONG SliderPanel::GetStartSegValue(void)
{
	return startSegVal;
}

void SliderPanel::SetEndSegValue(ULONG value)
{
	endSegVal = value;
	bSegInProgress = false;
	bSegPresent = true;
}

ULONG SliderPanel::GetEndSegValue(void)
{
	return endSegVal;
}

void SliderPanel::ClearSegValue()
{
	startSegVal = endSegVal = 0;
	bSegPresent = false;
	bSegInProgress = false;
	//paintNow();
	Refresh();
}