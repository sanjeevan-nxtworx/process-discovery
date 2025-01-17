#include "stdafx.h"
#include "RecorderButtonBar.h"
#include "recorderButtons.h"
#include "ProcoanButton.h"
#include "CustomEvents.h"
#include "UIUtility.h"

BEGIN_EVENT_TABLE(RecorderButtonBar, wxPanel)
// some useful events

EVT_MOTION(RecorderButtonBar::mouseMoved)
EVT_LEFT_DOWN(RecorderButtonBar::mouseDown)
EVT_LEFT_UP(RecorderButtonBar::mouseReleased)
EVT_COMMAND(BUTTON_STOP, PROCOAN_BUTTON_DOWN, RecorderButtonBar::OnStop)
EVT_COMMAND(BUTTON_RECORD, PROCOAN_BUTTON_DOWN, RecorderButtonBar::OnRecord)
EVT_COMMAND(BUTTON_PAUSE, PROCOAN_BUTTON_DOWN, RecorderButtonBar::OnPause)

// catch paint events
EVT_PAINT(RecorderButtonBar::paintEvent)

END_EVENT_TABLE()



RecorderButtonBar::RecorderButtonBar(wxWindow *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 187), wxSize(size), wxNO_BORDER)
{
	pParent = parent;
	// The last parameter (0) - so we have 2D behaviour
	stopButton = DBG_NEW ProcoanButton(
		this, BUTTON_STOP, wxPoint(120, 3), wxSize(50, 50));
	stopButton->SetNormalBitmap(stopNormal_xpm);
	stopButton->SetPressedBitmap(stopPressed_xpm);
	stopButton->SetDisabledBitmap(stopDisabled_xpm);
	stopButton->SetActiveBitmap(stopMouseOver_xpm);
	stopButton->SetToolTip(wxT("Stop Recording"));

	pauseButton = DBG_NEW ProcoanButton(
		this, BUTTON_PAUSE, wxPoint(180, 3), wxSize(50, 50));
	pauseButton->SetNormalBitmap(pauseNormal_xpm);
	pauseButton->SetPressedBitmap(pausePressed_xpm);
	pauseButton->SetDisabledBitmap(pauseDisabled_xpm);
	pauseButton->SetActiveBitmap(pauseMouseOver_xpm);
	pauseButton->SetToolTip(wxT("Pause Recording"));

	recordButton = DBG_NEW ProcoanButton(
		this, BUTTON_RECORD, wxPoint(240, 3), wxSize(50, 50));
	recordButton->SetNormalBitmap(recordNormal_xpm);
	recordButton->SetPressedBitmap(recordPressed_xpm);
	recordButton->SetDisabledBitmap(recordDisabled_xpm);
	recordButton->SetActiveBitmap(recordMouseOver_xpm);
	recordButton->SetToolTip(wxT("Record"));

	stopButton->SetDisabled(true);
	pauseButton->SetDisabled(true);

}

void RecorderButtonBar::SetOnRecordButton()
{
	stopButton->SetDisabled(false);
	pauseButton->SetDisabled(false);
	recordButton->SetDisabled(true);
}

void RecorderButtonBar::SetOnPauseButton()
{
	recordButton->SetDisabled(false);
	stopButton->SetDisabled(false);
	pauseButton->SetDisabled(true);
}

void RecorderButtonBar::SetOnStopButton()
{
	recordButton->SetDisabled(true);
	pauseButton->SetDisabled(true);
	stopButton->SetDisabled(true);
}

void RecorderButtonBar::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void RecorderButtonBar::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void RecorderButtonBar::OnStop(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::OnPause(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::OnRecord(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::mouseMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::mouseDown(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::mouseWheelMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::mouseReleased(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderButtonBar::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();

	DrawBevel(dc, wxColor(0x7f, 0xcd, 0xcd), wxColor(0x7f, 0xcd, 0xcd), wxPoint(0, 0), winSize, 3, true);
}