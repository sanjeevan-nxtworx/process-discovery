#include "stdafx.h"
#include "PicControlPanel.h"
#include "UIUtility.h"
#include "CustomEvents.h"
#include "SliderPanel.h"
#include "Utility.h"
#include "TimeDisplay.h"

// Event table for MyFrame
BEGIN_EVENT_TABLE(PicControlPanel, wxPanel)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_MOVEABSOLUTE, PicControlPanel::OnMoveAbsolute)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_HOVER, PicControlPanel::OnSliderHover)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_HOVER_CLEAR, PicControlPanel::OnSliderHoverClear)
END_EVENT_TABLE()


PicControlPanel::PicControlPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size) :
	wxPanel(parent, -1, pt, size, wxNO_BORDER)
{
	controlID = id;
	pParent = parent;
	txtTime = DBG_NEW TimeDisplay(this, (ULONG)wxID_ANY, wxPoint(size.GetWidth() - 100, 0), wxSize(100, 20));
	slider = DBG_NEW SliderPanel(this, PIC_SLIDER, wxPoint(0, 0), wxSize(size.GetWidth() - 100, 20));
}

void PicControlPanel::ResizePanel()
{
	wxSize size = GetSize();
	txtTime->SetPosition(wxPoint(size.GetWidth() - 100, 2));
	txtTime->SetSize(wxSize(100, 20));
	//txtVideoTime->SetPosition(wxPoint(20, 0));
	slider->SetSize(wxSize(size.GetWidth() - 100, 20));
}


void PicControlPanel::OnMoveAbsolute(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

ULONG PicControlPanel::GetThumbTime()
{
	return slider->GetThumbTime();
}

void PicControlPanel::OnSliderHover(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void PicControlPanel::OnSliderHoverClear(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

int PicControlPanel::GetHoverXPos()
{
	return slider->GetHoverXPos();
}

void PicControlPanel::SetSliderPosition(long position)
{
	slider->SetCurrentValue(position);
}

bool PicControlPanel::GetSegInProgress()
{
	return slider->IsSegInProgress();
}

bool PicControlPanel::GetSegPresent()
{
	return slider->IsSegPresent();
}

ULONG PicControlPanel::GetSegStartPos()
{
	return slider->GetStartSegValue();
}

ULONG PicControlPanel::GetSegEndPos()
{
	return slider->GetEndSegValue();
}


void PicControlPanel::SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal)
{
	slider->SetSegmentDetails(bInProgress, bPresent, startVal, endVal);
}

long PicControlPanel::GetSliderPosition()
{
	return slider->GetCurrentValue();
}

long PicControlPanel::SetSliderSegmentStart()
{
	return slider->StartSegment();
}

void PicControlPanel::SetSliderMaxTime(ULONG time)
{
	slider->SetMaxValue(time);
}

void PicControlPanel::SetTimeValue()
{
	ULONG curr = slider->GetCurrentValue();
	ULONG max = slider->GetMaxValue();
	long temp;
	wxString strTime;
	int currhh, currmm, currss, maxhh, maxmm, maxss;

	currhh = curr / 3600000L;
	temp = curr % 3600000L;
	currmm = temp / 60000L;
	temp = temp % 60000L;
	currss = (int)(temp / 1000);

	maxhh = max / 3600000L;
	temp = max % 3600000L;
	maxmm = temp / 60000L;
	temp = temp % 60000L;
	maxss = (int)(temp / 1000);

	strTime = wxString::Format("%02d:%02d:%02d/%02d:%02d:%02d", currhh, currmm, currss, maxhh, maxmm, maxss);
	txtTime->SetTimeText(strTime);
}

void PicControlPanel::AddPlayTime(ULONG time)
{
	slider->AddPlayTime(time);
	SetTimeValue();
}

void PicControlPanel::ClearSliderSegment()
{
	return slider->ClearSegValue();
}

long PicControlPanel::SetSliderSegmentEnd()
{
	return slider->EndSegment();
}


