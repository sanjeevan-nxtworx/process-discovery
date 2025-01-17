#include "stdafx.h"
#include "RecorderTitleBarPanel.h"
#include "recorderButtons.h"
#include "ProcoanButton.h"
#include "CustomEvents.h"
#include "UIUtility.h"

BEGIN_EVENT_TABLE(RecorderTitleBarPanel, wxPanel)
// some useful events

EVT_MOTION(RecorderTitleBarPanel::mouseMoved)
EVT_LEFT_DOWN(RecorderTitleBarPanel::mouseDown)
EVT_LEFT_UP(RecorderTitleBarPanel::mouseReleased)
EVT_COMMAND(BUTTON_CLOSE, PROCOAN_BUTTON_DOWN, RecorderTitleBarPanel::OnExit)
EVT_COMMAND(BUTTON_MINIMIZE, PROCOAN_BUTTON_DOWN, RecorderTitleBarPanel::OnMinimize)

 // catch paint events
	EVT_PAINT(RecorderTitleBarPanel::paintEvent)

	END_EVENT_TABLE()



RecorderTitleBarPanel::RecorderTitleBarPanel(wxWindow *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 0), wxSize(size), wxNO_BORDER)
{
	pParent = parent;
	// The last parameter (0) - so we have 2D behaviour
	procoanButton = DBG_NEW ProcoanButton(
		this, -1, wxPoint(3, 7), wxSize(30, 19));
	procoanButton->SetNormalBitmap(smalllogo_xpm);
	int width = this->GetSize().GetWidth();

	minimizeButton = DBG_NEW ProcoanButton(
		this, BUTTON_MINIMIZE, wxPoint(width - 70, 3), wxSize(30, 30));
	minimizeButton->SetNormalBitmap(windowMinimizeSmall_xpm);

	closeButton = DBG_NEW ProcoanButton(
		this, BUTTON_CLOSE, wxPoint(width-35, 3), wxSize(30, 30));

	closeButton->SetNormalBitmap(windowCloseSmall_xpm);
}

void RecorderTitleBarPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void RecorderTitleBarPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void RecorderTitleBarPanel::OnExit(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderTitleBarPanel::OnMinimize(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderTitleBarPanel::mouseMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}
void RecorderTitleBarPanel::mouseDown(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderTitleBarPanel::mouseWheelMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderTitleBarPanel::mouseReleased(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void RecorderTitleBarPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();

	DrawBevel(dc, wxColor(0x7f, 0xcd, 0xcd), wxColor(0x7f, 0xcd, 0xcd), wxPoint(0, 0), winSize, 2, true);
}