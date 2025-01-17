#include "stdafx.h"
#include "RecorderMinimizePanel.h"
#include "recorderButtons.h"
#include "ProcoanButton.h"
#include "CustomEvents.h"
#include "UIUtility.h"
#include "LEDDisplayPanel.h"

BEGIN_EVENT_TABLE(RecorderMinimizePanel, wxPanel)
// some useful events

EVT_MOTION(RecorderMinimizePanel::mouseMoved)
EVT_LEFT_DOWN(RecorderMinimizePanel::mouseDown)
EVT_LEFT_UP(RecorderMinimizePanel::mouseReleased)
EVT_COMMAND(BUTTON_STOP, PROCOAN_BUTTON_DOWN, RecorderMinimizePanel::OnStop)
EVT_COMMAND(BUTTON_RECORD, PROCOAN_BUTTON_DOWN, RecorderMinimizePanel::OnRecord)
EVT_COMMAND(BUTTON_PAUSE, PROCOAN_BUTTON_DOWN, RecorderMinimizePanel::OnPause)
EVT_COMMAND(BUTTON_CLOSE, PROCOAN_BUTTON_DOWN, RecorderMinimizePanel::OnClose)
EVT_COMMAND(BUTTON_MAXIMIZE, PROCOAN_BUTTON_DOWN, RecorderMinimizePanel::OnMaximize)

// catch paint events
EVT_PAINT(RecorderMinimizePanel::paintEvent)

END_EVENT_TABLE()



RecorderMinimizePanel::RecorderMinimizePanel(wxWindow *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 0), wxSize(size), wxNO_BORDER)
{
	pParent = parent;
	int width = size.GetWidth();
	// The last parameter (0) - so we have 2D behaviour
	stopButton = DBG_NEW ProcoanButton(
		this, BUTTON_STOP, wxPoint(90, 5), wxSize(50, 50));
	stopButton->SetNormalBitmap(stopNormal_xpm);
	stopButton->SetPressedBitmap(stopPressed_xpm);
	stopButton->SetDisabledBitmap(stopDisabled_xpm);
	stopButton->SetActiveBitmap(stopMouseOver_xpm);
	stopButton->SetToolTip(wxT("Stop Recording"));

	pauseButton = DBG_NEW ProcoanButton(
		this, BUTTON_PAUSE, wxPoint(150, 5), wxSize(50, 50));
	pauseButton->SetNormalBitmap(pauseNormal_xpm);
	pauseButton->SetPressedBitmap(pausePressed_xpm);
	pauseButton->SetDisabledBitmap(pauseDisabled_xpm);
	pauseButton->SetActiveBitmap(pauseMouseOver_xpm);
	pauseButton->SetToolTip(wxT("Pause Recording"));

	recordButton = DBG_NEW ProcoanButton(
		this, BUTTON_RECORD, wxPoint(210, 5), wxSize(50, 50));
	recordButton->SetNormalBitmap(recordNormal_xpm);
	recordButton->SetPressedBitmap(recordPressed_xpm);
	recordButton->SetDisabledBitmap(recordDisabled_xpm);
	recordButton->SetActiveBitmap(recordMouseOver_xpm);
	recordButton->SetToolTip(wxT("Record"));

	ledPanel = DBG_NEW LEDDisplayPanel(this, wxPoint(270, 15), wxSize(110, 30));
	ledPanel->SetThickness(1);
	ledPanel->SetGap(1);
	ledPanel->SetHeight(5);
	ledPanel->SetText(wxString("00:00:00"));
	ledPanel->SetForeColor(wxColor(0xff, 0xff, 0xff));
	ledPanel->SetBackColor(wxColor(0x29, 0x37, 0x49));
	//ledPanel->paintNow();
	ledPanel->Refresh();


	iconButton = DBG_NEW ProcoanButton(
		this, -1, wxPoint(3, 7), wxSize(30, 19));
	iconButton->SetNormalBitmap(smalllogo_xpm);

	maximizeButton = DBG_NEW ProcoanButton(
		this, BUTTON_MAXIMIZE, wxPoint(width - 70, 10), wxSize(30, 30));
	maximizeButton->SetNormalBitmap(windowMaximizeSmall_xpm);

	closeButton = DBG_NEW ProcoanButton(
		this, BUTTON_CLOSE, wxPoint(width - 35, 10), wxSize(30, 30));

	closeButton->SetNormalBitmap(windowCloseSmall_xpm);

	stopButton->SetDisabled(true);
	pauseButton->SetDisabled(true);
}

void RecorderMinimizePanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void RecorderMinimizePanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void RecorderMinimizePanel::SetRecTimeValue(ULONG recTime)
{
	wxString strTimeVal;
	long milli = recTime;
	long hr = milli / 3600000;
	milli = milli - 3600000 * hr;
	//60000 milliseconds in a minute
	long min = milli / 60000;
	milli = milli - 60000 * min;

	//1000 milliseconds in a second
	long sec = milli / 1000;
	milli = milli - 1000 * sec;

	strTimeVal = strTimeVal.Format("%02d:%02d:%02d", (int)hr, (int)min, (int)sec);
	ledPanel->SetText(strTimeVal);
	//ledPanel->paintNow();
	ledPanel->Refresh();
}

void RecorderMinimizePanel::OnStop(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::OnPause(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::OnRecord(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::SetOnRecordButton()
{
	stopButton->SetDisabled(false);
	pauseButton->SetDisabled(false);
	recordButton->SetDisabled(true);
}

void RecorderMinimizePanel::SetOnPauseButton()
{
	recordButton->SetDisabled(false);
	stopButton->SetDisabled(false);
	pauseButton->SetDisabled(true);
}

void RecorderMinimizePanel::SetOnStopButton()
{
	recordButton->SetDisabled(true);
	pauseButton->SetDisabled(true);
	stopButton->SetDisabled(true);
}

void RecorderMinimizePanel::OnMaximize(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::OnClose(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}
void RecorderMinimizePanel::mouseMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}
void RecorderMinimizePanel::mouseDown(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::mouseWheelMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::mouseReleased(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderMinimizePanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();

	DrawBevel(dc, wxColor(0x7f, 0xcd, 0xcd), wxColor(0x7f, 0xcd, 0xcd), wxPoint(0, 0), winSize, 3, true);
}
