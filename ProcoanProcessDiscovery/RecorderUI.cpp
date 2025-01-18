#include "stdafx.h"
#include "RecorderUI.h"
#include "CustomEvents.h"
#include "RecorderTitleBarPanel.h"
#include "RecorderInfoBarPanel.h"
#include "RecorderDisplayPanel.h"
#include "RecorderButtonBar.h"
#include "RecorderMinimizePanel.h"
#include "Utility.h"

#include <wx/gauge.h>

// Event table for MyFrame
BEGIN_EVENT_TABLE(RecorderUI, wxDialog)
EVT_COMMAND(BUTTON_CLOSE, PROCOAN_BUTTON_DOWN, RecorderUI::OnExit)
EVT_COMMAND(BUTTON_MINIMIZE, PROCOAN_BUTTON_DOWN, RecorderUI::OnMinimize)
EVT_COMMAND(BUTTON_MAXIMIZE, PROCOAN_BUTTON_DOWN, RecorderUI::OnMaximize)
EVT_COMMAND(BUTTON_STOP, PROCOAN_BUTTON_DOWN, RecorderUI::OnStop)
EVT_COMMAND(BUTTON_PAUSE, PROCOAN_BUTTON_DOWN, RecorderUI::OnPause)
EVT_COMMAND(BUTTON_RECORD, PROCOAN_BUTTON_DOWN, RecorderUI::OnRecord)
EVT_MOTION(RecorderUI::OnMouseMove)
EVT_LEFT_DOWN(RecorderUI::OnLeftDown)
EVT_LEFT_UP(RecorderUI::OnLeftUp)
EVT_PAINT(RecorderUI::paintEvent)
END_EVENT_TABLE()

RecorderUI::RecorderUI(wxWindow * parent, wxWindowID id, const wxString & title,
	const wxPoint & position, const wxSize & size) : wxDialog(parent, -1, title, position, size, 0)
{
	titlePanel = DBG_NEW RecorderTitleBarPanel(this,wxSize(size.GetWidth(), 36));
	infoBarPanel = DBG_NEW RecorderInfoBarPanel(this, wxSize(size.GetWidth(), 30));
	displayPanel = DBG_NEW RecorderDisplayPanel(this, wxSize(size.GetWidth(), 120));
	buttonPanel = DBG_NEW RecorderButtonBar(this, wxSize(size.GetWidth(), 55));
	minimizePanel = DBG_NEW RecorderMinimizePanel(this, wxSize(size.GetWidth(),150));
	minimizePanel->Hide();
	recorderMode = 0;
	//int buttonSize = 30;
	timer.Bind(wxEVT_TIMER, &RecorderUI::OnTimer, this);
	recTime = 0;
	audioValue = 0;
	parentWnd = parent;
	parent->Hide();
	monitorList.clear();
	CreateRecorderWindow();
	GetRecorderWnd()->CreateRecorderWindow(GetModuleHandle(NULL));
	CreateExecutionManager()->StartExecutionManager();
}

RecorderUI::~RecorderUI()
{
	DeleteRecorderWindow();
	DeleteExecutionManager();

	list<monitorDetails *>::iterator itList = monitorList.begin();
	while (itList != monitorList.end())
	{
		monitorDetails *pMonitorDetails = *itList;
		delete pMonitorDetails;
		itList++;
	}
	monitorList.clear();
}

void RecorderUI::UpdateUI()
{
	//displayPanel->paintNow();
	//infoBarPanel->paintNow();
	displayPanel->Refresh();
	infoBarPanel->Refresh();
}

void RecorderUI::SetMonitorList(std::list <struct monitorDetails *> *pList)
{
	std::list <struct monitorDetails *>::iterator itMon = monitorList.begin();
	if (itMon != monitorList.end())
	{
		struct monitorDetails *pOldMon = *itMon;
		delete pOldMon;
	}
	monitorList.clear();

	itMon = pList->begin();
	while (itMon != pList->end())
	{
		struct monitorDetails *pOldMon = *itMon;
		struct monitorDetails *pNewMon = DBG_NEW struct monitorDetails;
		CopyMemory(pNewMon, pOldMon, sizeof(struct monitorDetails));
		monitorList.push_back(pNewMon);
		itMon++;
	}

	displayPanel->SetMonitorList(&monitorList);
}
void RecorderUI::SetVideoFlg(bool flg)
{
	bVideoFlg = flg;
	displayPanel->SetVideoFlg(flg);
}

void RecorderUI::SetAudioFlg(bool flg)
{
	bAudioFlg = flg;
	displayPanel->SetAudioFlg(flg);
}

void RecorderUI::SetEventFlg(bool flg)
{
	bEventFlg = flg;
	displayPanel->SetEventFlg(flg);
}

void RecorderUI::SetActiveMonitorFlg(bool flg)
{
	bActiveMonitorFlg = flg;
	displayPanel->SetActiveMonitorFlg(flg);
}

void RecorderUI::SetFPS(int fps)
{
	nFPS = fps;
	displayPanel->SetFPS(fps);
}

void RecorderUI::SetDirectory(wxString dir)
{
	strDirectory = dir;
	infoBarPanel->SetDirectory(dir);

}

void RecorderUI::SetFilename(wxString filename)
{
	strFileName = filename;
	infoBarPanel->SetFilename(filename);
}


void RecorderUI::OnExit(wxCommandEvent& WXUNUSED(event))
{
	this->EndModal(0);
}

void RecorderUI::OnMinimize(wxCommandEvent& WXUNUSED(event))
{
	titlePanel->Hide();
	infoBarPanel->Hide();
	displayPanel->Hide();
	buttonPanel->Hide();
	wxSize size = GetSize();
	size.SetHeight(60);
	this->SetSize(size);
	minimizePanel->Show();
}

void RecorderUI::OnMaximize(wxCommandEvent& WXUNUSED(event))
{
	wxSize size = GetSize();
	size.SetHeight(241);
	this->SetSize(size);
	minimizePanel->Hide();
	titlePanel->Show();
	infoBarPanel->Show();
	displayPanel->Show();
	buttonPanel->Show();
	//displayPanel->paintNow();
	//infoBarPanel->paintNow();
	displayPanel->Refresh();
	infoBarPanel->Refresh();

}

void RecorderUI::OnLeftDown(wxMouseEvent & event)
{
	CaptureMouse();
	wxPoint pos = ClientToScreen(event.GetPosition());
	wxPoint origin = GetPosition();
	int dx = pos.x - origin.x;
	int dy = pos.y - origin.y;
	m_delta = wxPoint(dx, dy);
}

void RecorderUI::OnLeftUp(wxMouseEvent &WXUNUSED(event))
{
	if (HasCapture())
	{
		ReleaseMouse();
	}
}

void RecorderUI::OnStop(wxCommandEvent &WXUNUSED(event))
{
	if (recorderMode != 0)
	{
		audioValue = 0;
		recorderMode = 0;
		displayPanel->SetAudioValue(audioValue);
		GetExecutionManager()->StopRecorderCommand();
		minimizePanel->SetOnStopButton();
		buttonPanel->SetOnStopButton();
		Sleep(1000);
	}
}

void RecorderUI::OnPause(wxCommandEvent &WXUNUSED(event))
{
	if (recorderMode == 1)
	{
		newTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		msTime = msTime + (newTime - prevTime);
		recorderMode = 2;
		minimizePanel->SetOnPauseButton();
		buttonPanel->SetOnPauseButton();
		GetExecutionManager()->PauseRecorderCommand();
	}
}

void RecorderUI::OnRecord(wxCommandEvent &WXUNUSED(event))
{
	minimizePanel->SetOnRecordButton();
	buttonPanel->SetOnRecordButton();

	if (recorderMode == 2)
	{
		// Unpause Recorder
		recorderMode = 1;
		prevTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		GetExecutionManager()->RestartRecorderCommand();
	}
	
	if (recorderMode == 0)
	{
		prevTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		msTime = 0L;
		recorderMode = 1;
		// Start Recorder
		timer.Start(250);
		recorderMode = 1;
		recTime = 0L;
		audioValue = 0;
		displayPanel->SetAudioValue(audioValue);
		displayPanel->SetRecTimeValue(recTime);
		minimizePanel->SetRecTimeValue(recTime);
		wxString fileName = "";
		if (strDirectory.EndsWith("\\"))
			fileName = strDirectory + strFileName;
		else
			fileName = strDirectory + wxString("\\") + strFileName;

		GetExecutionManager()->SetSelectedMonitorList(&monitorList);
		GetExecutionManager()->SetEventRecording(bEventFlg);
		GetExecutionManager()->SetVideoRecording(bVideoFlg);
		GetExecutionManager()->SetAudioRecording(bAudioFlg);
		GetExecutionManager()->StartRecorderCommand(fileName.char_str());
	}
}

void RecorderUI::OnTimer(wxTimerEvent& WXUNUSED(evt))
{
	static int oldMode;
	struct RecorderStatusRecord recStatus;
	if (bVideoFlg || recorderMode == 0)
		GetExecutionManager()->GetRecorderStatus(&recStatus);
	else
	{
		if (recorderMode == 1)
			recStatus.currStatus = 1;
		if (recorderMode == 0)
			recStatus.currStatus = 3;
	}
	if (recStatus.currStatus != 4)
	{
		if (recorderMode != 2 && recorderMode != 0)
		{
			newTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			msTime = msTime + (newTime - prevTime);
			if (!bVideoFlg)
				recStatus.recordTime = msTime;
			prevTime = newTime;
		}
		recStatus.lockStruct.lock();
		if (recStatus.audioValues.size() > 0)
		{
			recStatus.audioValue = recStatus.audioValues.front();
			recStatus.audioValues.pop_front();
		}
		else
		{
			recStatus.audioValue = 0L;
		}
		displayPanel->SetAudioValue(recStatus.audioValue);
		displayPanel->SetRecTimeValue(recStatus.recordTime / 1000L);
		minimizePanel->SetRecTimeValue(recStatus.recordTime / 1000L);
		oldMode = recStatus.currStatus;
		displayPanel->SetRecorderMode(recStatus.currStatus);
		displayPanel->SetRecorderWriteStatus(recStatus.indexWritten, recStatus.numIndex, bVideoFlg);
		recStatus.lockStruct.unlock();
		if (oldMode != recStatus.currStatus)
		{
			//displayPanel->paintNow();
			displayPanel->Refresh();
		}
	}
	else
	{
		timer.Stop();
		EndModal(0);
	}
}

void RecorderUI::OnMouseMove(wxMouseEvent & event)
{
	wxPoint pt = event.GetPosition();

	if (event.Dragging() && event.LeftIsDown())
	{
		wxPoint pos = ClientToScreen(pt);
		Move(wxPoint(pos.x - m_delta.x, pos.y - m_delta.y));
	}
}

void RecorderUI::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}


void RecorderUI::render(wxDC&  dc)
{
//	titlePanel->paintNow();
//	infoBarPanel->paintNow();
//	displayPanel->paintNow();
//	buttonPanel->paintNow();
//	minimizePanel->paintNow();
	titlePanel->Refresh();
	infoBarPanel->Refresh();
	displayPanel->Refresh();
	buttonPanel->Refresh();
	minimizePanel->Refresh();
}

