#include "stdafx.h"
#include "MainWindowPanel.h"
#include "MainButtons.h"
#include "ProcoanButton.h"
#include "CustomEvents.h"
#include "UIUtility.h"
#include "Utility.h"

BEGIN_EVENT_TABLE(MainWindowPanel, wxPanel)
// some useful events

	EVT_MOTION(MainWindowPanel::mouseMoved)
	EVT_LEFT_DOWN(MainWindowPanel::mouseDown)
	EVT_LEFT_UP(MainWindowPanel::mouseReleased)
	EVT_COMMAND(BUTTON_CLOSE, PROCOAN_BUTTON_DOWN, MainWindowPanel::OnExit)
	EVT_COMMAND(BUTTON_RECORDER, PROCOAN_BUTTON_DOWN, MainWindowPanel::OnRecord)
	EVT_COMMAND(BUTTON_EDITOR, PROCOAN_BUTTON_DOWN, MainWindowPanel::OnEditor)
	EVT_COMMAND(BUTTON_SPY, PROCOAN_BUTTON_DOWN, MainWindowPanel::OnSpy)

/* EVT_KEY_DOWN(BasicDrawPane::keyPressed)
 EVT_KEY_UP(BasicDrawPane::keyReleased)
 EVT_MOUSEWHEEL(BasicDrawPane::mouseWheelMoved)
 EVT_RIGHT_DOWN(MainWindowPanel::rightClick)
 EVT_LEAVE_WINDOW(MainWindowPanel::mouseLeftWindow)
 */

 // catch paint events
	EVT_PAINT(MainWindowPanel::paintEvent)

	END_EVENT_TABLE()



MainWindowPanel::MainWindowPanel(wxFrame *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 0), wxSize(size), wxNO_BORDER)
{
	pParent = parent;

	recorderButton = DBG_NEW ProcoanButton(
		this, BUTTON_RECORDER, wxPoint(120, 10), wxSize(50, 50));
	recorderButton->SetActiveBitmap(recorderMouseOver_xpm);
	recorderButton->SetDisabledBitmap(recorderDisabled_xpm);
	recorderButton->SetNormalBitmap(recorderNormal_xpm);
	recorderButton->SetPressedBitmap(recorderPressed_xpm);
	recorderButton->SetToolTip(wxT("Record Process"));

	editorButton = DBG_NEW ProcoanButton(
		this, BUTTON_EDITOR, wxPoint(200, 10), wxSize(50, 50));
	editorButton->SetActiveBitmap(cutMouseOver_xpm);
	editorButton->SetDisabledBitmap(cutDisabled_xpm);
	editorButton->SetNormalBitmap(cutNormal_xpm);
	editorButton->SetPressedBitmap(cutPressed_xpm);
	editorButton->SetToolTip(wxT("Edit Recording"));

	playerButton = DBG_NEW ProcoanButton(
		this, BUTTON_PLAYER, wxPoint(280, 10), wxSize(50, 50));
	playerButton->SetActiveBitmap(playMouseOver_xpm);
	playerButton->SetDisabledBitmap(playDisabled_xpm);
	playerButton->SetNormalBitmap(playNormal_xpm);
	playerButton->SetPressedBitmap(playPressed_xpm);
	playerButton->SetToolTip(wxT("Play Recording"));

	spyButton = DBG_NEW ProcoanButton(
		this, BUTTON_SPY, wxPoint(360, 10), wxSize(50, 50));
	spyButton->SetActiveBitmap(spyMouseOver_xpm);
	spyButton->SetDisabledBitmap(spyDisabled_xpm);
	spyButton->SetNormalBitmap(spyNormal_xpm);
	spyButton->SetPressedBitmap(spyPressed_xpm);
	spyButton->SetToolTip(wxT("Spy Utility"));

	closeButton = DBG_NEW ProcoanButton(
		this, BUTTON_CLOSE, wxPoint(440, 10), wxSize(50, 50));
	closeButton->SetNormalBitmap(windowClose_xpm);

	iconButton = DBG_NEW ProcoanButton(
		this, -1, wxPoint(10, 10), wxSize(90, 55));
	iconButton->SetNormalBitmap(mainlogo_xpm);
}


MainWindowPanel::~MainWindowPanel()

{
	//delete recorderButton;
	//delete editorButton;
	//delete playerButton;
	//delete spyButton;
	//delete closeButton;
	//delete iconButton;
}




void MainWindowPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void MainWindowPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void MainWindowPanel::OnExit(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	DeleteConfigClass();
}

void MainWindowPanel::OnRecord(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void MainWindowPanel::OnEditor(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void MainWindowPanel::OnSpy(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void MainWindowPanel::mouseMoved(wxMouseEvent& event) 
{ 
	wxPostEvent(pParent, event); 
}
void MainWindowPanel::mouseDown(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void MainWindowPanel::mouseWheelMoved(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}

void MainWindowPanel::mouseReleased(wxMouseEvent& event)
{
	wxPostEvent(pParent, event);
}




void MainWindowPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();
	
	DrawBevel(dc, wxColor(0x7f, 0xcd, 0xcd), wxColor(0x7f, 0xcd, 0xcd), wxPoint(0,0), winSize, 4, true);
}