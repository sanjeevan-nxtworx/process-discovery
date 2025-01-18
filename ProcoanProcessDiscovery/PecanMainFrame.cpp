#include "stdafx.h"
#include <wx/wxprec.h>
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "PecanMainFrame.h"
#include "RecorderStartDlg.h"
#include "VideoEditorScreen.h"
#include "SpyEditorScreen.h"
#include "RecorderUI.h"
#include "MainWindowPanel.h"
#include "UIUtility.h"
#include "Utility.h"
#include <list>

// Event table for MyFrame
BEGIN_EVENT_TABLE(PecanMainFrame, wxFrame)
EVT_COMMAND(BUTTON_CLOSE, PROCOAN_BUTTON_DOWN, PecanMainFrame::OnExit)
EVT_COMMAND(BUTTON_RECORDER, PROCOAN_BUTTON_DOWN, PecanMainFrame::OnRecord)
EVT_COMMAND(BUTTON_PLAYER, PROCOAN_BUTTON_DOWN, PecanMainFrame::OnPlayer)
EVT_COMMAND(BUTTON_EDITOR, PROCOAN_BUTTON_DOWN, PecanMainFrame::OnEditor)
EVT_COMMAND(BUTTON_SPY, PROCOAN_BUTTON_DOWN, PecanMainFrame::OnSpy)
EVT_MOTION(PecanMainFrame::OnMouseMove)
EVT_LEFT_DOWN(PecanMainFrame::OnLeftDown)
EVT_LEFT_UP(PecanMainFrame::OnLeftUp)
END_EVENT_TABLE()

HANDLE hSemaphore = NULL;

PecanMainFrame::PecanMainFrame(int width)
	: wxFrame((wxFrame *)NULL, -1, _(""), wxPoint(width / 2 - 200, 0), wxSize(500, 75), wxNO_BORDER,
		_(""))
{
	backPanel = DBG_NEW MainWindowPanel((wxFrame *)this, this->GetSize());
	hSemaphore = CreateSemaphore(NULL, 0, 1, L"NxtWorxSemaphore");
}

void PecanMainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	CloseHandle(hSemaphore);
	Close(TRUE); // Tells the OS to quit running this process
}

void PecanMainFrame::OnRecord(wxCommandEvent& WXUNUSED(event))
{
	std::list <struct monitorDetails *> monitorList;

	GetScreenDataDetails(&monitorList);

	RecorderStartDlg startDialog(&monitorList, this, _("Recorder Settings"),
		wxPoint(80, 100), wxSize(440, 310));
	startDialog.UpdateDialog(false);
	if (startDialog.ShowModal() == wxID_OK)
	{
		wxString strDirectory = startDialog.GetDirectory();
		wxString strFileName = startDialog.GetFilename().BeforeFirst(wxUniChar('.'));

		strFileName = strFileName + "Rec.log";
		wxString fullfileName = "";
		if (strDirectory.EndsWith("\\"))
			fullfileName = strDirectory + strFileName;
		else
			fullfileName = strDirectory + wxString("\\") + strFileName;

		CreateSPDLogFile(fullfileName.char_str());
		RecorderUI recorderUI(this, -1, _(""), wxPoint(100, 100), wxSize(460, 241));
		recorderUI.SetVideoFlg(startDialog.GetVideoFlg());
		recorderUI.SetAudioFlg(startDialog.GetAudioFlg());
		recorderUI.SetEventFlg(startDialog.GetEventFlg());
		recorderUI.SetActiveMonitorFlg(startDialog.GetActiveMonitorFlg());
		recorderUI.SetFPS(startDialog.GetFPS());
		recorderUI.SetDirectory(startDialog.GetDirectory());
		recorderUI.SetFilename(startDialog.GetFilename());
		recorderUI.SetMonitorList(&monitorList);
		recorderUI.UpdateUI();
		recorderUI.ShowModal();
		DeleteSPDLogFile();
	}

	std::list <struct monitorDetails *>::iterator itMonitorList = monitorList.begin();
	
	while (itMonitorList != monitorList.end())
	{
		struct monitorDetails *pPtr = *itMonitorList;
		delete pPtr;
		itMonitorList++;
	}
	monitorList.clear();
	this->Show();
//	startDialog.Destroy();
}

void PecanMainFrame::OnEditor(wxCommandEvent& WXUNUSED(event))
{
	VideoEditorScreen *editorDlg = DBG_NEW VideoEditorScreen(this,  _("Edit Recording"),
		wxPoint(100, 100), wxSize(400, 200), false);
	this->Hide();
	editorDlg->Show();
}

void PecanMainFrame::OnPlayer(wxCommandEvent& WXUNUSED(event))
{
	VideoEditorScreen *playerDlg = DBG_NEW VideoEditorScreen(this,  _("Play Recording"),
		wxPoint(100, 100), wxSize(400, 200), true);
	playerDlg->Show();
}


void PecanMainFrame::OnSpy(wxCommandEvent& WXUNUSED(event))
{
	SpyEditorScreen *editorDlg = DBG_NEW SpyEditorScreen(this, wxID_ANY, _("Edit Spy"),
		wxPoint(100, 100), wxSize(450, 850));
	this->Hide();
	editorDlg->Show();
}


void PecanMainFrame::OnLeftDown(wxMouseEvent & event)
{
	CaptureMouse();
	wxPoint pos = ClientToScreen(event.GetPosition());
	wxPoint origin = GetPosition();
	int dx = pos.x - origin.x;
	int dy = pos.y - origin.y;
	m_delta = wxPoint(dx, dy);
}

void PecanMainFrame::OnLeftUp(wxMouseEvent &) 
{
	if (HasCapture())
	{
		ReleaseMouse();
	}
}

void PecanMainFrame::OnMouseMove(wxMouseEvent & event)
{
	wxPoint pt = event.GetPosition();

	if (event.Dragging() && event.LeftIsDown())
	{
		wxPoint pos = ClientToScreen(pt);
		Move(wxPoint(pos.x - m_delta.x, pos.y - m_delta.y));
	}
}



