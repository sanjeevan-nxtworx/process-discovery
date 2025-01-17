#include "stdafx.h"
#include "VideoEditorScreen.h"
#include "EditorButtons.h"
#include "ProcoanButton.h"
#include "PicButtonControlPanel.h"
#include "CustomEvents.h"
#include "Utility.h"
#include "ExecutionManager.h"
#include "RecordBufferManager.h"
#include "PlayerDataFileManager.h"
#include "AudioPlayerBuffer.h"
#include "BPMNControlPanel.h"
#include "PictureDisplay.h"
#include <mutex>
#include "BPMNElement.h"
#include "ActionTabPanel.h"
#include "InfoTabPanel.h"
#include "ConfigClass.h"
#include "MainToolBarPanel.h"
#include "spdlog/spdlog.h"
#include "WordReport/SOPWordReport.h"
#include "ProcessDescriptionDialog.h"
#include "BPMNDiagram.h"
#include "SOPPrintReport.h"
#include "RecordingDescriptionDialog.h"
#include <wx/print.h>
#include "ManageScenarioDlg.h"
#include "ScenarioFacade.h"
// For compilers that support precompilation, includes "wx/wx.h".
#include "PDFDocument.h"

mutex lockPlayerWindow;
// Event table for MyFrame
wxBEGIN_EVENT_TABLE(VideoEditorScreen, wxFrame)
//EVT_SIZE(VideoEditorScreen::OnEditorResize)
EVT_MENU(wxID_OPEN, VideoEditorScreen::OnOpenFile)
EVT_MENU(wxID_TOOLBAR_BOTTOM, VideoEditorScreen::OnToolBarBottom)
EVT_MENU(wxID_TOOLBAR_TOP, VideoEditorScreen::OnToolBarTop)
EVT_MENU(wxID_TOOLBAR_LEFT, VideoEditorScreen::OnToolBarLeft)
EVT_MENU(wxID_TOOLBAR_RIGHT, VideoEditorScreen::OnToolBarRight)
EVT_MENU(wxID_VIDEOPOSITION_LEFT, VideoEditorScreen::OnVideoPaneLeft)
EVT_MENU(wxID_VIDEOPOSITION_RIGHT, VideoEditorScreen::OnVideoPaneRight)
EVT_MENU(wxID_DISPLAYTAB_ON, VideoEditorScreen::OnDisplayTabOn)
EVT_MENU(wxID_DISPLAYTAB_OFF, VideoEditorScreen::OnDisplayTabOff)
EVT_MENU(wxID_DISPLAY_ABOUT, VideoEditorScreen::OnAbout)
EVT_MENU(wxID_EXIT, VideoEditorScreen::OnExit)
EVT_MENU(IDM_OPEN_READONLY, VideoEditorScreen::OnOpenReadOnly)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_MOVEABSOLUTE, VideoEditorScreen::OnMoveAbsolute)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_HOVER, VideoEditorScreen::OnSliderHover)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_HOVER_CLEAR, VideoEditorScreen::OnSliderHoverClear)
EVT_COMMAND(PIC_SLIDER, PROCOAN_SLIDER_UPDATETIME, VideoEditorScreen::OnSliderUpdateTime)
EVT_COMMAND(EDITOR_FREW, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnFastRewind)
EVT_COMMAND(EDITOR_SREW, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnSlowRewind)
EVT_COMMAND(EDITOR_FFWD, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnFastForward)
EVT_COMMAND(EDITOR_SFWD, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnSlowForward)
EVT_COMMAND(EDITOR_STOP, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnStop)
EVT_COMMAND(EDITOR_PLAY, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnPlay)
EVT_COMMAND(EDITOR_ADD_ACTIVTY, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnAddActivity)
EVT_COMMAND(EDITOR_START_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnStartSegment)
EVT_COMMAND(EDITOR_END_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnEndSegment)
EVT_COMMAND(EDITOR_COPY_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnCopySegment)
EVT_COMMAND(EDITOR_CUT_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnCutSegment)
EVT_COMMAND(EDITOR_PASTE_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnPasteSegment)
EVT_COMMAND(EDITOR_CLEAR_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnClearSegment)
EVT_COMMAND(EDITOR_BRANCH_SEGMENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBranchSegment)
EVT_COMMAND(ID_ACTION_TAB, PROCOAN_UPDATE_TAB, VideoEditorScreen::OnUpdateTabFields)
EVT_CHOICE(EVENT_ACTION_GROUP, VideoEditorScreen::OnActivityGroup)
EVT_CHOICE(EVENT_ACTION_SUBGROUP, VideoEditorScreen::OnActivitySubGroup)
EVT_BUTTON(EVENT_SAVE, VideoEditorScreen::OnActivitySave)
EVT_CLOSE(VideoEditorScreen::OnClose)
EVT_NOTEBOOK_PAGE_CHANGED(ID_VIDEO_PANEL_NOTEBOOK, OnFileChanged)
EVT_COMMAND(ID_BPMN_CONTROL, PROCOAN_UPDATE_BPMNWINDOW, VideoEditorScreen::OnUpdateBPMNControl)
EVT_COMMAND(TOOLBAR_OPEN_PRIMARY, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnOpenFile)
EVT_COMMAND(TOOLBAR_OPEN_SECONDARY, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnOpenReadOnly)
EVT_COMMAND(TOOLBAR_CLOSE_FILE, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnFileClose)
EVT_COMMAND(TOOLBAR_SWITCH_PANEL, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnSwitchPanel)
EVT_COMMAND(TOOLBAR_SHOW_TAB, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnDisplayTab)
EVT_COMMAND(TOOLBAR_SHOW_LEFT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnToolBarLeft)
EVT_COMMAND(TOOLBAR_SHOW_TOP, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnToolBarTop)
EVT_COMMAND(TOOLBAR_SHOW_RIGHT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnToolBarRight)
EVT_COMMAND(TOOLBAR_SHOW_BOTTOM, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnToolBarBottom)
EVT_COMMAND(TOOLBAR_SHOW_SUBTITLE, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnShowSubTitle)
//EVT_COMMAND(TOOLBAR_REPORT_SOP, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnReportSOP)
EVT_COMMAND(TOOLBAR_REPORT_SOP, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnReportPDF)
EVT_COMMAND(TOOLBAR_MANAGE_SCENARIO, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnManageScenario)
EVT_COMMAND(TOOLBAR_RECORDING_INFO, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnRecordingInfo)
EVT_COMMAND(TOOLBAR_SHOW_MULTI_MONITOR, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnMultiMonitor)
EVT_COMMAND(TOOLBAR_SHOW_ACTIVE_MONITOR, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnActiveMonitor)
//EVT_COMMAND(TOOLBAR_REPORT_SOP, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnPreview)

EVT_COMMAND(TOOLBAR_BPMN_BRANCH_BOTTOM, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNBranchBottom)
EVT_COMMAND(TOOLBAR_BPMN_BRANCH_RIGHT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNBranchRight)
EVT_COMMAND(TOOLBAR_BPMN_CREATE_SUBPROCESS, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNCreateSubProcess)
EVT_COMMAND(TOOLBAR_BPMN_DELETE_BRANCH, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNDeleteBranch)
EVT_COMMAND(TOOLBAR_BPMN_DELETE_EVENT, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNDeleteEvent)
EVT_COMMAND(TOOLBAR_BPMN_UP_LEVEL, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNLevelUp)
EVT_COMMAND(TOOLBAR_BPMN_MOVE_PROCESS_BELOW, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNMoveProcessBelow)
EVT_COMMAND(TOOLBAR_BPMN_MOVE_SUBPROCESS_ABOVE, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNMoveSubProcessAbove)
EVT_COMMAND(TOOLBAR_BPMN_MOVE_SUBPROCESS_BELOW, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNMoveSubProcessBelow)
EVT_COMMAND(TOOLBAR_BPMN_SYNC_VIDEO, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnBPMNSyncVideo)
EVT_CHOICE(TOOLBAR_SCENARIO_CHOICE, VideoEditorScreen::OnScenarioChanged)
EVT_COMMAND(TOOLBAR_PLAY_SCENARIO, PROCOAN_BUTTON_DOWN, VideoEditorScreen::OnScenarioPlay)

wxEND_EVENT_TABLE()


const int MIN_TAB_WIDTH = 300;
const int MIN_BPMN_WIDTH = 400;
const int MIN_VIDEO_WIDTH = 400;
const int MIN_VIDEO_HEIGHT = 300;
const int MIN_BPMN_HEIGHT = 400;
const int MIN_TAB_HEIGHT = 200;

wxFrame *pVideoFrame = NULL;

VideoEditorScreen::VideoEditorScreen(wxWindow * parent, wxWindowID id, const wxString & title,
	const wxPoint & position, const wxSize & size, bool bFlg) 
	: wxFrame(NULL, -1, title, position, size, wxDEFAULT_FRAME_STYLE)
	
{
	displayAnnotationString = "";
	displayAnnotationNotes = "";
	displayAnnotationBottomString = "";
	m_pParent = parent;
	pVideoFrame = this;
	bPlayerMode = bFlg;
	bFirst = true;
	bMainFileOpen = false;
	bStartPlay = false;
	bExitThreads = false;
	bActiveMonitorDisplay = true;
	bShowSubTitle = true;
	currentEventDisplayed = 0;
	string strPath = getcurrentpath() + "\\resources\\PecanConfig.xml";
	ConfigClass *pConfigClass = CreateConfigClass(wxString(strPath.c_str()));

	if (pConfigClass != NULL)
	{
		bActiveMonitorDisplay = pConfigClass->getPlayDisplayActiveMonitorMode();
	}
	pBPMNControl = NULL;
	pDataRecorder = NULL;
	pRecordBufferManager = NULL;
	fps = 0;
	numMonitors = 0;
	//pMonitorDetails = NULL;
	bVideoThreadWaiting = true;
	bDisplayThreadWaiting = true;
	framePanel = NULL;
	toolBarPosition = 0; // In Bottom
	bVideoPanelLeft = true; // Video Pane on Left
	if (!bPlayerMode)
		bDisplayTabWindow = true; // Tab Window is displayed
	else
		bDisplayTabWindow = false;

	m_pMenuBar = NULL;
	m_pFileMenu = NULL;
	m_pHelpMenu = NULL;
	m_pViewMenu = NULL;
	m_pToolBarPositionMenu = NULL;
	m_pVideoWindowPositionMenu = NULL;
	m_pDisplayTabWindowMenu = NULL;
	pCurrentEdited = NULL;
	pCurrentEventDisplay = NULL;
	framePanel = NULL;
	// BPMN Panel Elements
	bpmnPanel = NULL;

	// Tab Panel Elements
	tabPanel = NULL;
	editMainTab = NULL;
	mainEditTab = NULL;

	// Video Panel Elements;
	videoPanel = NULL;
	videoMainTab = NULL;
	mainTab = NULL;
	mainPicturePanel = NULL;
	picControlPanel = NULL;
	recorderButton = NULL;

	toolControlPanel = NULL;

	//wxBitmapButton *btnRewind;
	CreateMainMenuBar();

	//bool bSwitched = false;

	WXWidget hWnd = this->GetHandle();
	HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO target;
	target.cbSize = sizeof(MONITORINFO);

	GetMonitorInfo(hMon, &target);
	wxSize frameSize;
	frameSize.SetWidth(target.rcWork.right - target.rcWork.left);
	frameSize.SetHeight(target.rcWork.bottom - target.rcWork.top);
	wxPoint pos = wxPoint(target.rcWork.left, target.rcWork.top);
	SetPosition(pos);
	SetSize(frameSize);
	mainToolBarPanel = DBG_NEW MainToolBarPanel(this, -1, wxPoint(0, 0), wxSize(frameSize.GetWidth(), 35), bPlayerMode);
	mainToolBarPanel->EnableShowActiveMonitor(false);
	mainToolBarPanel->EnableShowMultiMonitor(false);
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	mainToolBarPanel->SetScenarioName("Default");
	mainToolBarPanel->SetShowSubTitleTip(bShowSubTitle);
	framePanel = DBG_NEW wxPanel(this, -1, wxPoint(0, 35), wxSize(frameSize.GetWidth(), frameSize.GetHeight()-35), 0, "framePanel");
	framePanel->SetBackgroundColour(wxColor(0x0, 0, 0));
	ReArrangeForm();
	SetButtonStatus();
}


void VideoEditorScreen::SetButtonStatus()
{
	toolControlPanel->SetFileOpenFlg(bMainFileOpen);
	toolControlPanel->SetPrimaryFile(videoMainTab->GetSelection() == 0);
	toolControlPanel->SetSyncElementDisplayed(pBPMNControl->IsSyncElementDisplayed());
	toolControlPanel->SetButtonStatus();
}

void VideoEditorScreen::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(wxString::Format
	(
		"Welcome to %s!\n"
		"\n"
		"This is the minimal wxWidgets sample\n"
		"running under %s.",
		wxVERSION_STRING,
		wxGetOsDescription()
	),
		"About wxWidgets minimal sample",
		wxOK | wxICON_INFORMATION,
		this);
}

void VideoEditorScreen::OnScenarioChanged(wxCommandEvent& WXUNUSED(event))
{
	string scenarioName = mainToolBarPanel->GetSelectedScenarioName();
	pDataRecorder->SetScenarioName(scenarioName);
	pDataRecorder->SetScenario(scenarioName);
	mainToolBarPanel->EnableScenarioChoice(true);
	mainToolBarPanel->EnablePlayScenario(true);
}


void VideoEditorScreen::OnScenarioPlay(wxCommandEvent& WXUNUSED(event))
{
	string scenarioName = mainToolBarPanel->GetSelectedScenarioName();
	pDataRecorder->SetScenarioName(scenarioName);
	pDataRecorder->PlayScenario(scenarioName);
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	toolControlPanel->SetPlay(true);
	SetButtonStatus();
}


void VideoEditorScreen::OnEditorResize(wxSizeEvent& event)
{
	wxSize szFrameSize;   //size of the frame
	szFrameSize = event.GetSize();
	if (framePanel != NULL)
		ResizeWindow(szFrameSize);
	SetButtonStatus();
}

void VideoEditorScreen::CreateMainMenuBar()
{
	m_pMenuBar = DBG_NEW wxMenuBar();

	// File Menu
	m_pFileMenu = DBG_NEW wxMenu();
	m_pFileMenu->Append(wxID_OPEN, _T("&Open Main File"), _("Open Main File"));
	if (!bPlayerMode)
		m_pFileMenu->Append(IDM_OPEN_READONLY, _T("Open &Readonly File"), _T("Open Readonly File"));
	m_pFileMenu->Append(wxID_SAVE, _T("&Save"), _("Close"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(wxID_EXIT, _T("&Quit"));
	m_pMenuBar->Append(m_pFileMenu, _T("&File"));

	// About menu
	m_pHelpMenu = DBG_NEW wxMenu();
	m_pHelpMenu->Append(wxID_DISPLAY_ABOUT, "&About\tF1", "Show about dialog");
	m_pMenuBar->Append(m_pHelpMenu, _T("&Help"));

	// View Menu
	m_pToolBarPositionMenu = DBG_NEW wxMenu();
	m_pToolBarPositionMenu->Append(wxID_TOOLBAR_BOTTOM, _T("Toolbar At Bottom"));
	m_pToolBarPositionMenu->Append(wxID_TOOLBAR_TOP, _T("Toolbar At Top"));
	m_pToolBarPositionMenu->Append(wxID_TOOLBAR_LEFT, _T("Toolbar At Left"));
	m_pToolBarPositionMenu->Append(wxID_TOOLBAR_RIGHT, _T("Toolbar At Right"));

	m_pVideoWindowPositionMenu = DBG_NEW wxMenu();
	m_pVideoWindowPositionMenu->Append(wxID_VIDEOPOSITION_LEFT, _T("Video Panel At Left"));
	m_pVideoWindowPositionMenu->Append(wxID_VIDEOPOSITION_RIGHT, _T("Video Panel At Right"));

	if (!bPlayerMode)
	{
		m_pDisplayTabWindowMenu = DBG_NEW wxMenu();
		m_pDisplayTabWindowMenu->Append(wxID_DISPLAYTAB_ON, _T("Show TAB Window"));
		m_pDisplayTabWindowMenu->Append(wxID_DISPLAYTAB_OFF, _T("Hide TAB Window"));
	}
	m_pViewMenu = DBG_NEW wxMenu();
	m_pViewMenu->AppendSubMenu(m_pToolBarPositionMenu, _("ToolBar"));
	m_pViewMenu->AppendSubMenu(m_pVideoWindowPositionMenu, _("Video Position"));
	if (!bPlayerMode)
		m_pViewMenu->AppendSubMenu(m_pDisplayTabWindowMenu, _("Tab Window Display"));

	m_pMenuBar->Append(m_pViewMenu, _T("View"));
	SetMenuBar(m_pMenuBar);
}

void VideoEditorScreen::OnOpenFile(wxCommandEvent &WXUNUSED(event))
{
	bFirst = true;
	
	wxFileDialog
		openFileDialog(this, _("Open Recording file"), "", "",
			"Recording File (*.dat)|*.dat", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	wxString wFileName = openFileDialog.GetPath();
	wxString wFile = openFileDialog.GetFilename();
	videoMainTab->SetPageText(0, wFile);
	bMainFileOpen = true;
	OpenFileInPlayer(wFileName);
	SetButtonStatus();
	mainToolBarPanel->EnableOpenPrimary(false);
	mainToolBarPanel->EnableOpenSecondary(true);
	mainToolBarPanel->EnableCloseFile(true);
	mainToolBarPanel->EnableSwitchPanel(true);
	mainToolBarPanel->EnableShowTab(true);
	mainToolBarPanel->EnableToolBarLeft(true);
	mainToolBarPanel->EnableToolBarTop(true);
	mainToolBarPanel->EnableToolBarRight(true);
	mainToolBarPanel->EnableToolBarBottom(true);
	mainToolBarPanel->EnableShowSubTitle(true);
	mainToolBarPanel->EnableShowDescription(false);
	mainToolBarPanel->EnableZoomOnEvents(false);
	mainToolBarPanel->EnableReportSOP(true);
//	mainToolBarPanel->EnableRecordingInfo(true);
	mainToolBarPanel->EnableManageScenario(true);

	// BPMN Control Toolbar
	mainToolBarPanel->EnableBranchBottom(false);
	mainToolBarPanel->EnableBranchRight(false);
	mainToolBarPanel->EnableCreateSubProcess(false);
	mainToolBarPanel->EnableDeleteBranch(false);
	mainToolBarPanel->EnableDeleteEvent(false);
	mainToolBarPanel->EnableUpLevel(false);
	mainToolBarPanel->EnableMoveProcessBelow(false);
	mainToolBarPanel->EnableMoveSubProcessAbove(false);
	mainToolBarPanel->EnableMoveSubProcessBelow(false);
	mainToolBarPanel->EnableSyncVideo(false);
	mainToolBarPanel->Update();
	
	SetButtonStatus();

}


void VideoEditorScreen::OnFileChanged(wxNotebookEvent &event)
{
	bFirst = true;
	int indexOld = event.GetOldSelection();
	int indexNew = event.GetSelection();

	if (indexOld == -1)
		return;
	pDataRecorder->SetRecorderStop();
	toolControlPanel->SetStop();
	if (!bPlayerMode)
		mainEditTab->Enable();
	pRecordBufferManager->ClearPlayBuffer();

	pDataRecorder->SwitchFileContext(indexOld, indexNew);
	if (indexNew > indexOld)
	{
		mainEditTab->Disable();
	}
	// if this is the primary display and segment is selected in secondary display
	// then the paste and branch should be enabled
	if (pDataRecorder->IsSegmentSelectedInOtherContexts(pDataRecorder->GetContextFileNumber()))
	{
		toolControlPanel->SetSegSelectedInOtherContexts(true, false, pDataRecorder->GetContextSegOn(), pDataRecorder->IsSegmentCopiedInAnyContext());
	}
	else if (pDataRecorder->IsSegmentSelectedInSameContext(pDataRecorder->GetContextFileNumber()))
	{
		toolControlPanel->SetSegSelectedInOtherContexts(false, true, pDataRecorder->GetContextSegOn(), pDataRecorder->IsSegmentCopiedInAnyContext());
	}
	else
	{
		bpmnPanel->ClearSegmentElements();
		toolControlPanel->SetSegSelectedInOtherContexts(false, false, pDataRecorder->GetContextSegOn(), pDataRecorder->IsSegmentCopiedInAnyContext());
	}

	if (indexNew > 0)
		pDataRecorder->SetPlayerMode(true);
	else
		pDataRecorder->SetPlayerMode(false);
	if (pDataRecorder->GetMaxMonitors() > 1)
	{
		mainToolBarPanel->EnableShowActiveMonitor(true);
		mainToolBarPanel->EnableShowMultiMonitor(true);
		mainToolBarPanel->SetMultiMonitors(true);
		mainToolBarPanel->SetActiveMonitorMode(bActiveMonitorDisplay);
	}
	else
	{
		mainToolBarPanel->SetMultiMonitors(false);
		mainToolBarPanel->EnableShowActiveMonitor(false);
		mainToolBarPanel->EnableShowMultiMonitor(false);
	}

	vector <string> scenarios = pDataRecorder->GetScenarioNames();
	mainToolBarPanel->SetScenarioNames(scenarios);
	string name = pDataRecorder->GetScenarioName();
	mainToolBarPanel->SetScenarioName(name);
	mainToolBarPanel->EnableScenarioChoice(true);
	mainToolBarPanel->EnablePlayScenario(true);
	mainToolBarPanel->SetShowSubTitleTip(bShowSubTitle);
	mainPicturePanel->SetShowSubTitle(bShowSubTitle);
	pDataRecorder->MoveAbsolute();
	SetButtonStatus();
}


void VideoEditorScreen::OnFileClose(wxCommandEvent &WXUNUSED(event))
{
	int pageNum = videoMainTab->GetSelection();

	if (pageNum == 0)
	{
		int answer = wxMessageBox("Quit program?", "Confirm",
		                          wxYES_NO, this);
		if (answer == wxNO)
			return;
	}

	pDataRecorder->SetRecorderStop();
	pRecordBufferManager->ClearPlayBuffer();
}

void VideoEditorScreen::OnOpenReadOnly(wxCommandEvent &WXUNUSED(event))
{
	bFirst = true;
	wxFileDialog
		openFileDialog(this, _("Open Read Only Recording file"), "", "",
			"Recording File (*.dat)|*.dat", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	wxString wFileName = openFileDialog.GetPath();
	wxString wFile = openFileDialog.GetFilename();

	pDataRecorder->SetRecorderStop();
	pRecordBufferManager->ClearPlayBuffer();

	wxSize mainTabSize = videoMainTab->GetSize();
	videoMainTab->AddPage(mainTab, wFile, true, -1);
	pDataRecorder->AddNewFileContext(wFileName.char_str());
	if (pDataRecorder->IsSegmentSelectedInOtherContexts(pDataRecorder->GetContextFileNumber()))
	{
		toolControlPanel->SetSegSelectedInOtherContexts(true, false, pDataRecorder->GetContextSegOn(), pDataRecorder->IsSegmentCopiedInAnyContext());
	}
	else if (pDataRecorder->IsSegmentSelectedInSameContext(pDataRecorder->GetContextFileNumber()))
	{
		toolControlPanel->SetSegSelectedInOtherContexts(false, true, pDataRecorder->GetContextSegOn(), pDataRecorder->IsSegmentCopiedInAnyContext());
	}
	else
	{
		toolControlPanel->SetSegSelectedInOtherContexts(false, false, pDataRecorder->GetContextSegOn(), pDataRecorder->IsSegmentCopiedInAnyContext());
	}

	vector <string> scenarios = pDataRecorder->GetScenarioNames();
	mainToolBarPanel->SetScenarioNames(scenarios);

	pDataRecorder->SetPlayerMode(true);

	if (pDataRecorder->GetMaxMonitors() > 1)
	{
		mainToolBarPanel->EnableShowActiveMonitor(true);
		mainToolBarPanel->EnableShowMultiMonitor(true);
		mainToolBarPanel->SetMultiMonitors(true);
		mainToolBarPanel->SetActiveMonitorMode(bActiveMonitorDisplay);
	}
	else
	{
		mainToolBarPanel->SetMultiMonitors(false);
		mainToolBarPanel->EnableShowActiveMonitor(false);
		mainToolBarPanel->EnableShowMultiMonitor(false);
	}
	mainToolBarPanel->EnableScenarioChoice(true);
	mainToolBarPanel->EnablePlayScenario(true);
	mainToolBarPanel->SetScenarioName("Default");
	mainToolBarPanel->SetShowSubTitleTip(bShowSubTitle);
	mainPicturePanel->SetShowSubTitle(bShowSubTitle);
	pDataRecorder->MoveAbsolute();
	SetButtonStatus();
}

void VideoEditorScreen::OpenFileInPlayer(wxString fileName)
{
	if (GetExecutionManager() == NULL)
		CreateExecutionManager()->StartExecutionManager();

	wxString strFileName = fileName.BeforeFirst(wxUniChar('.'));

	strFileName = strFileName + "Player.log";
	CreateSPDLogFile(strFileName.char_str());

	GetExecutionManager()->StartPlayerCommand(fileName.char_str(), (IPlayerWindow *)this);
	if (pDataRecorder->GetMaxMonitors() > 1)
	{
		mainToolBarPanel->EnableShowActiveMonitor(true);
		mainToolBarPanel->EnableShowMultiMonitor(true);
		mainToolBarPanel->SetMultiMonitors(true);
		mainToolBarPanel->SetActiveMonitorMode(bActiveMonitorDisplay);
	}
	else
	{
		mainToolBarPanel->SetMultiMonitors(false);
		mainToolBarPanel->EnableShowActiveMonitor(false);
		mainToolBarPanel->EnableShowMultiMonitor(false);
	}
	vector <string> scenarios = pDataRecorder->GetScenarioNames();
	mainToolBarPanel->SetScenarioNames(scenarios);
	mainToolBarPanel->SetScenarioName("Default");
	mainToolBarPanel->EnableScenarioChoice(true);
	mainToolBarPanel->EnablePlayScenario(true);
	mainToolBarPanel->SetShowSubTitleTip(bShowSubTitle);
	mainPicturePanel->SetShowSubTitle(bShowSubTitle);

	SetButtonStatus();
}

void VideoEditorScreen::OnToolBarBottom(wxCommandEvent &WXUNUSED(event))
{
	toolBarPosition = 0; // In Bottom
	if (bVideoPanelLeft)
		toolControlPanel->SetShowSmall(false);
	else
		toolControlPanel->SetShowSmall(true);
	mainToolBarPanel->EnableToolBarLeft(true);
	mainToolBarPanel->EnableToolBarTop(true);
	mainToolBarPanel->EnableToolBarRight(true);
	mainToolBarPanel->EnableToolBarBottom(false);
	mainToolBarPanel->Update();

	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnToolBarTop(wxCommandEvent &WXUNUSED(event))
{
	toolBarPosition = 1; // In Top
	if (bVideoPanelLeft)
		toolControlPanel->SetShowSmall(false);
	else
		toolControlPanel->SetShowSmall(true);
	mainToolBarPanel->EnableToolBarLeft(true);
	mainToolBarPanel->EnableToolBarTop(false);
	mainToolBarPanel->EnableToolBarRight(true);
	mainToolBarPanel->EnableToolBarBottom(true);
	mainToolBarPanel->Update();

	toolControlPanel->SetShowSmall(false);
	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnToolBarLeft(wxCommandEvent &WXUNUSED(event))
{
	toolBarPosition = 2; // In Left
	toolControlPanel->SetShowSmall(true);
	mainToolBarPanel->EnableToolBarLeft(false);
	mainToolBarPanel->EnableToolBarTop(true);
	mainToolBarPanel->EnableToolBarRight(true);
	mainToolBarPanel->EnableToolBarBottom(true);
	mainToolBarPanel->Update();
	ReArrangeForm();
	SetButtonStatus();
}

void VideoEditorScreen::OnManageScenario(wxCommandEvent& WXUNUSED(event))
{
	bool bRecorderPlay = pDataRecorder->GetRecorderStatus();
	if (bRecorderPlay)
		pDataRecorder->SetRecorderStop();
	string strValue = pDataRecorder->GetScenarioJSON();
	ScenarioFacade scenarios(strValue);
	ManageScenarioDlg prDialog(&scenarios, this, -1, _("Manage Scenario"), wxPoint(80, 100), wxSize(500, 450));
	int retValue = prDialog.ShowModal();

	if (retValue == SCENARIO_PLAYBTN || retValue == SCENARIO_SELECTBTN)
	{
		string selString = prDialog.GetSelectedString();
		if (selString == "Default")
		{
			pDataRecorder->SaveOrGetScenario(false, NULL);
		}
		else
		{
			ScenarioDefintion *pScenarioDefinition = scenarios.GetScenario(selString);
			pDataRecorder->SaveOrGetScenario(false, pScenarioDefinition);
		}
		if (retValue == SCENARIO_PLAYBTN)
		{
			pDataRecorder->SetPlayerToStart();
		}
		pDataRecorder->ResetPlayerScreen();
	}

	if (bRecorderPlay || retValue == SCENARIO_PLAYBTN)
	{
		toolControlPanel->SetPlay(true);
		pDataRecorder->SetRecorderPlay();
		SetButtonStatus();
	}

}

void VideoEditorScreen::SaveScenarioDefinitions(ScenarioFacade *pScenarioFacade)
{
	vector <string> names = pScenarioFacade->GetScenarioNames();
	mainToolBarPanel->SetScenarioNames(names);
	string strValue = pScenarioFacade->GetScenarioJSON();
	pDataRecorder->SetScenarioJSON(strValue);
}

void VideoEditorScreen::SaveOrGetScenario(bool bSave, ScenarioDefintion *pScenario)
{
	pDataRecorder->SaveOrGetScenario(bSave, pScenario);
}

void VideoEditorScreen::OnRecordingInfo(wxCommandEvent& WXUNUSED(event))
{
	string strValue = pDataRecorder->ReadProcessValue();

	bool bRecorderPlay = pDataRecorder->GetRecorderStatus();
	if (bRecorderPlay)
		pDataRecorder->SetRecorderStop();

	RecordingDescriptionDialog prDialog("", this, -1, _("Recording Information"), wxPoint(80, 100), wxSize(500, 500));
	prDialog.SetProcessData(strValue);
	if (prDialog.ShowModal() == wxID_OK)
	{
		// Stop Video Player
		strValue = prDialog.GetProcessData();
		strValue = EscapeJSonString(strValue);
		pDataRecorder->WriteProcessValue(strValue);
	}

	if (bRecorderPlay)
		pDataRecorder->SetRecorderPlay();

}

void VideoEditorScreen::OnMultiMonitor(wxCommandEvent& WXUNUSED(event))
{
	bool bRecorderPlay = pDataRecorder->GetRecorderStatus();
	if (bRecorderPlay)
		pDataRecorder->SetRecorderStop();
	pRecordBufferManager->ClearPlayBuffer();
	bActiveMonitorDisplay = false;
	mainToolBarPanel->SetActiveMonitorMode(bActiveMonitorDisplay);
	pDataRecorder->SetPlayerBitmap(bActiveMonitorDisplay);
	pDataRecorder->MoveAbsolute();
	if (bRecorderPlay)
		pDataRecorder->SetRecorderPlay();
}

void VideoEditorScreen::OnActiveMonitor(wxCommandEvent& WXUNUSED(event))
{
	bool bRecorderPlay = pDataRecorder->GetRecorderStatus();
	if (bRecorderPlay)
		pDataRecorder->SetRecorderStop();
	pRecordBufferManager->ClearPlayBuffer();
	bActiveMonitorDisplay = true;
	mainToolBarPanel->SetActiveMonitorMode(bActiveMonitorDisplay);
	pDataRecorder->SetPlayerBitmap(bActiveMonitorDisplay);
	pDataRecorder->MoveAbsolute();
	if (bRecorderPlay)
		pDataRecorder->SetRecorderPlay();
}

void VideoEditorScreen::OnPreview(wxCommandEvent& WXUNUSED(event))
{
	wxPrintPreview *preview = new wxPrintPreview(
		new SOPPrintReport, new SOPPrintReport,
		&g_printDialogData);
	if (!preview->Ok())
	{
		delete preview;
		wxMessageBox(wxT("There was a problem previewing.\nPerhaps your	current printer is not set correctly ? "),
			wxT("Previewing"), wxOK);
		return;
	}
	wxPreviewFrame *frame = new wxPreviewFrame(preview, this,
		wxT("Demo Print Preview"));
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show(true);
}

void VideoEditorScreen::OnToolBarRight(wxCommandEvent &WXUNUSED(event))
{
	toolBarPosition = 3; // In Right
	mainToolBarPanel->EnableToolBarLeft(true);
	mainToolBarPanel->EnableToolBarTop(true);
	mainToolBarPanel->EnableToolBarRight(false);
	mainToolBarPanel->EnableToolBarBottom(true);
	mainToolBarPanel->Update();

	toolControlPanel->SetShowSmall(true);
	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnVideoPaneLeft(wxCommandEvent &WXUNUSED(event))
{
	bVideoPanelLeft = true; // Video Pane on Left
	if (bVideoPanelLeft)
		toolControlPanel->SetShowSmall(false);
	else
		toolControlPanel->SetShowSmall(true);
	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnSwitchPanel(wxCommandEvent &WXUNUSED(event))
{
	if (bVideoPanelLeft)
		bVideoPanelLeft = false;
	else
		bVideoPanelLeft = true;
	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnVideoPaneRight(wxCommandEvent &WXUNUSED(event))
{
	bVideoPanelLeft = false; // Video Pane on Right
	if (bVideoPanelLeft)
		toolControlPanel->SetShowSmall(false);
	else
		toolControlPanel->SetShowSmall(true);
	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnDisplayTab(wxCommandEvent &WXUNUSED(event))
{
	if (bDisplayTabWindow)
		bDisplayTabWindow = false; // Tab Window is displayed
	else
		bDisplayTabWindow = true; // Tab Window is displayed

	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::OnDisplayTabOn(wxCommandEvent &WXUNUSED(event))
{
	bDisplayTabWindow = true; // Tab Window is displayed
	ReArrangeForm();
	SetButtonStatus();

}


void VideoEditorScreen::OnDisplayTabOff(wxCommandEvent &WXUNUSED(event))
{
	bDisplayTabWindow = false; // Tab Window is hidden
	ReArrangeForm();
	SetButtonStatus();

}

void VideoEditorScreen::ReArrangeForm()
{
	bool bIsPlaying = false;
	
	if (pDataRecorder)
	{
		bIsPlaying = pDataRecorder->IsPlaying();
		pDataRecorder->SetRecorderStop();
	}
	GetPanelSizes();
	DrawVideoPanel(framePanel, mainVideoPanelSize.GetWidth(), mainVideoPanelSize.GetHeight(), mainVideoPanelPos.x, mainVideoPanelPos.y);
	DrawBPMNPanel(framePanel, bpmnPanelSize.GetWidth(), bpmnPanelSize.GetHeight(), bpmnPanelPos.x, bpmnPanelPos.y);
	if (!bPlayerMode || !pDataRecorder)
		DrawEditTabPanel(framePanel, tabPanelSize.GetWidth(), tabPanelSize.GetHeight(), tabPanelPos.x, tabPanelPos.y);
	if (pDataRecorder)
	{
		pRecordBufferManager->ClearPlayBuffer();
		pRecordBufferManager->CreateFreeResizeScreenBufferQueue(this->GetWindowWidth(), this->GetWindowHeight(), 30);
		pDataRecorder->CreateScreenResizeBuffer(this->GetWindowWidth(), this->GetWindowHeight());
		pDataRecorder->MoveAbsolute();
		if (bIsPlaying)
			pDataRecorder->SetRecorderPlay();
	}
	Maximize();
}

void VideoEditorScreen::GetPanelSizes()
{
	WXWidget hWnd = this->GetHandle();
	HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO target;
	target.cbSize = sizeof(MONITORINFO);

	GetMonitorInfo(hMon, &target);
	wxSize frameSize;
	frameSize.SetWidth(target.rcWork.right - target.rcWork.left);
	frameSize.SetHeight(target.rcWork.bottom - target.rcWork.top - 35); // 35 is the size of the main toolbar window

	//frameSize = GetSize();
	int frameWidth = frameSize.GetWidth();
	int frameHeight = frameSize.GetHeight();
	if (bMainFileOpen)
	{
		spdlog::info("Screen Work Size {} {}", frameWidth, frameHeight);
	}

	if (frameWidth > frameHeight)
	{
		// Landscape layout
		CalculatePanelLandscapeDimensions();
	}
	else
	{
		// FrameHeight = 300 for Video Panel, 200 for Tab Window and 300 for BPMN window
		// Portrait Dimensions
		CalculatePanelPortraitDimensions();
	}
	bpmnPanelSize.SetWidth(GetClientSize().GetWidth() - mainVideoPanelSize.GetWidth());

}	

void VideoEditorScreen::CalculatePanelLandscapeDimensions()
{

	wxSize frameSize = GetClientSize();
	int frameHeight = frameSize.GetHeight()-35; // 35 is the size of the main toolbar window.
	int frameWidth = frameSize.GetWidth();  

	if (bMainFileOpen)
	{
		spdlog::info("Screen Client Size {} {}", frameWidth, frameHeight);
	}

	if (bDisplayTabWindow)
	{
		tabPanelSize.SetHeight(200);
	}
	else
	{
		tabPanelSize.SetHeight(0);
	}

	if (bVideoPanelLeft)
	{
		bpmnPanelSize.SetWidth(450);
		mainVideoPanelSize.SetWidth(frameWidth - bpmnPanelSize.GetWidth());
		mainVideoPanelPos.x = 0;
		bpmnPanelPos.x = mainVideoPanelSize.GetWidth() + 1;
		tabPanelPos.x = 0;
	}
	else
	{
		mainVideoPanelSize.SetWidth(450);
		bpmnPanelSize.SetWidth(frameWidth - mainVideoPanelSize.GetWidth());
		tabPanelPos.x = mainVideoPanelPos.x = bpmnPanelSize.GetWidth();
		bpmnPanelPos.x = 0;
	}

	mainVideoPanelPos.y = 0;
	bpmnPanelPos.y = 0;

	bpmnPanelSize.SetHeight(frameHeight);
	mainVideoPanelSize.SetHeight(frameHeight - tabPanelSize.GetHeight());
	if (bDisplayTabWindow)
	{
		tabPanelSize.SetWidth(mainVideoPanelSize.GetWidth());
		tabPanelPos.y = mainVideoPanelSize.GetHeight();
	}
}

void VideoEditorScreen::CalculatePanelPortraitDimensions()
{
	wxSize frameSize = GetClientSize();
	int frameHeight = frameSize.GetHeight()-30;
	int frameWidth = frameSize.GetWidth();

	if (bDisplayTabWindow)
	{
		tabPanelSize.SetHeight(200);
		tabPanelSize.SetWidth(300);
	}
	else
	{
		tabPanelSize.SetHeight(0);
		tabPanelSize.SetWidth(0);
	}


	if (frameWidth <= (400 + tabPanelSize.GetWidth()))
	{
		if (bVideoPanelLeft)
		{
			bpmnPanelSize.SetWidth(400);
			mainVideoPanelSize.SetWidth(bpmnPanelSize.GetWidth() + tabPanelSize.GetWidth());
		}
		else
		{
			mainVideoPanelSize.SetWidth(400);
			bpmnPanelSize.SetWidth(mainVideoPanelSize.GetWidth() + tabPanelSize.GetWidth());
		}
	}
	else
	{
		if (bVideoPanelLeft)
		{
			bpmnPanelSize.SetWidth(frameWidth-tabPanelSize.GetWidth());
			mainVideoPanelSize.SetWidth(frameWidth);
		}
		else
		{
			mainVideoPanelSize.SetWidth(frameWidth - tabPanelSize.GetWidth());
			bpmnPanelSize.SetWidth(frameWidth);
		}
	}

	int videoHeight = (mainVideoPanelSize.GetWidth() * 100) / 170;
	mainVideoPanelSize.SetHeight(videoHeight);
	bpmnPanelSize.SetHeight(frameHeight);

	if (bDisplayTabWindow && !bPlayerMode)
	{
		tabPanelPos.x = 0;
		tabPanelPos.y = mainVideoPanelSize.GetHeight() + 1;
	}
	else
	{
		tabPanelPos.x = tabPanelPos.y = 0;
	}

	if (bVideoPanelLeft)
	{
		mainVideoPanelPos.x = 0;
		mainVideoPanelPos.y = 0;
		bpmnPanelPos.x = tabPanelSize.GetWidth() + 1;
		bpmnPanelPos.y = mainVideoPanelSize.GetHeight() + 1;
	}
	else
	{
		bpmnPanelPos.x = 0;
		bpmnPanelPos.y = 0;
		mainVideoPanelPos.x = tabPanelSize.GetWidth() + 1;
		mainVideoPanelPos.y = bpmnPanelSize.GetHeight() + 1;
	}
}

void VideoEditorScreen::ResizeWindow(wxSize szFrameSize)
{
	ReArrangeForm();
}

void VideoEditorScreen::DrawBPMNPanel(wxPanel *parent, int panelWidth, int panelHeight, int xPos, int yPos)
{
	//panelHeight -= 30;
	if (bpmnPanel == NULL)
	{
		bpmnPanel = DBG_NEW BPMNControlPanel(parent, ID_BPMN_CONTROL, wxPoint(xPos, yPos), wxSize(panelWidth, panelHeight));
		bpmnPanel->SetMainToolbarPanel(mainToolBarPanel);
		pBPMNControl = (IBPMNControl *)bpmnPanel;
	}
	else
	{
		bpmnPanel->Hide();
		bpmnPanel->SetPosition(wxPoint(xPos, yPos));
		bpmnPanel->SetSize(wxSize(panelWidth, panelHeight));
		bpmnPanel->PositionControls();
	}
	if (bMainFileOpen)
	{
		spdlog::info("BPMN Control {} {} {} {}", xPos, yPos, panelWidth, panelHeight);
	}

	bpmnPanel->SetBackgroundColour(wxColor(0x7f, 0x7f, 0x7f));
	if (bVideoPanelLeft)
		mainToolBarPanel->PositionControls(5, xPos);
	else
		mainToolBarPanel->PositionControls(panelWidth, xPos);
	bpmnPanel->Show();
}


void VideoEditorScreen::DrawEditTabPanel(wxPanel *parent, int panelWidth,int panelHeight, int xPos, int yPos)
{
	//panelHeight -= 30;
	if (!bDisplayTabWindow || bPlayerMode)
	{
		if (tabPanel != NULL)
		{
			tabPanel->Hide();
		}
		return;
	}
	if (tabPanel == NULL)
	{
		tabPanel = DBG_NEW wxPanel(parent, -1, wxPoint(xPos, yPos), wxSize(panelWidth, panelHeight), 0, "TabPanel");
	}
	else
	{
		tabPanel->Hide();
		tabPanel->SetPosition(wxPoint(xPos, yPos));
		tabPanel->SetSize(wxSize(panelWidth, panelHeight));
	}
	if (bMainFileOpen)
	{
		spdlog::info("Tab Panel {} {} {} {}", xPos, yPos, panelWidth, panelHeight);
	}

	tabPanel->SetBackgroundColour(wxColor(0x7f, 0x7f, 0x7f));

	int height = panelHeight;
	int width = panelWidth;

	if (editMainTab == NULL)
	{
		editMainTab = DBG_NEW wxNotebook(tabPanel, -1, wxPoint(0, 0), wxSize(width, height), wxSUNKEN_BORDER, "editMainTab");
		editMainTab->SetBackgroundColour(wxColor(0x6d, 0x92, 0x9b));
		mainEditTab = DBG_NEW ActionTabPanel(editMainTab, wxSize(width, height));
		mainEditTab->SetBackgroundColour(wxColor(0xc1, 0xda, 0xd6));
		editMainTab->AddPage(mainEditTab, wxT("Main"), true, -1);
		infoTab = DBG_NEW InfoTabPanel(editMainTab, wxSize(width, height));
		infoTab->SetBackgroundColour(wxColor(0xc1, 0xda, 0xd6));
		editMainTab->AddPage(infoTab, wxT("Info"), false, -1);
		mainEditTab->Disable();
	}
	else
	{
		editMainTab->SetSize(wxSize(width, height));
		mainEditTab->SetSize(wxSize(width, height));
		mainEditTab->ResizePanel();
	}
	if (bMainFileOpen)
	{
		spdlog::info("Edit Main Tab {} {} {} {}", xPos, yPos, width, height);
	}
	//// Within Notebook
	tabPanel->Show();
}

void VideoEditorScreen::OnActivityGroup(wxCommandEvent& WXUNUSED(event))
{
	mainEditTab->InitActivityGroup();
	SetButtonStatus();
}


void VideoEditorScreen::OnActivitySave(wxCommandEvent& WXUNUSED(event))
{
	if (pCurrentEdited != NULL)
	{
		string strJSon = mainEditTab->GetActivityJSonString(pCurrentEdited);
		pCurrentEdited->SetNotes(strJSon);
		pDataRecorder->SaveEventDetails(pCurrentEdited->GetEventIndex());
		bpmnPanel->RefreshDiagram();

	}
	SetButtonStatus();

}

void VideoEditorScreen::OnActivitySubGroup(wxCommandEvent& WXUNUSED(event))
{
	mainEditTab->InitActivitySubGroup();
	SetButtonStatus();

}


void VideoEditorScreen::DrawVideoPanel(wxPanel *parent, int panelWidth, int panelHeight, int xPos, int yPos)
{
	if (videoPanel == NULL)
		videoPanel = DBG_NEW wxPanel(parent, -1, wxPoint(xPos, yPos), wxSize(panelWidth, panelHeight), wxSUNKEN_BORDER, _("VideoPanel"));
	else
	{
		videoPanel->Hide();
		videoPanel->SetPosition(wxPoint(xPos, yPos));
		videoPanel->SetSize(wxSize(panelWidth, panelHeight));
	}
	if (bMainFileOpen)
	{
		spdlog::info("Video Panel {} {} {} {}", xPos, yPos, panelWidth, panelHeight);
	}

	videoPanel->SetBackgroundColour(wxColor(0x9f, 0x9f, 0x9f));

	wxPoint toolBarPos;
	wxSize toolBarSize;
	wxPoint mainTabPos;
	wxSize mainTabSize;

	// Calculate for toolbar
	const int NUM_BUTTONS = 14;
	const int BUTTON_GAP = 2;
	const int TOOLBAR_MARGIN = 10;

	int buttonWidth = 0;
	int buttonMargin = BUTTON_GAP;
	int toolBarHeight = 0;
	int buttonXPosDiff = 0;
	int buttonYPosDiff = 0;
	if (toolBarPosition == 0 || toolBarPosition == 1)
	{
		buttonWidth = (panelWidth - (BUTTON_GAP * NUM_BUTTONS)) / NUM_BUTTONS;
		if (buttonWidth > 50)
		{
			buttonWidth = 50;
			buttonXPosDiff = buttonWidth + buttonMargin; //((panelWidth - (buttonWidth * NUM_BUTTONS)) / NUM_BUTTONS);
		}
		else
			buttonXPosDiff = buttonWidth + buttonMargin;
		toolBarHeight = buttonWidth;
		buttonYPosDiff = 0;
	}
	else
	{
		toolBarHeight = panelHeight;
		buttonWidth = (panelHeight - (BUTTON_GAP * NUM_BUTTONS)) / NUM_BUTTONS;
		if (buttonWidth > 30)
		{
			buttonWidth = 30;
			buttonYPosDiff = buttonWidth + buttonMargin; // (panelHeight - (buttonWidth * NUM_BUTTONS)) / NUM_BUTTONS;
		}
		else
			buttonYPosDiff = buttonWidth + buttonMargin;
		buttonXPosDiff = 0;
	}

	switch (toolBarPosition)
	{
	case 0:  // Bottom
		mainTabPos.x = 0;
		mainTabPos.y = 0;
		mainTabSize.SetWidth(panelWidth);
		mainTabSize.SetHeight(panelHeight - toolBarHeight - 30); // the Tool bar height is Width + 10 + 20 for the controlBar i.e. Slider
		toolBarPos.x = mainTabPos.x;
		toolBarPos.y = mainTabSize.GetHeight() + 20;
		toolBarSize.SetWidth(panelWidth);
		toolBarSize.SetHeight(toolBarHeight);
		break;
		
	case 1:	// Top
		toolBarPos.x = 0;
		toolBarPos.y = 0;
		toolBarSize.SetWidth(panelWidth);
		toolBarSize.SetHeight(toolBarHeight);
		mainTabPos.x = 0;
		mainTabPos.y = toolBarPos.y + toolBarSize.GetHeight();
		mainTabSize.SetWidth(panelWidth);
		mainTabSize.SetHeight(panelHeight - toolBarSize.GetHeight() - 30); // the Tool bar height is Width + 10 + 20 for the controlBar i.e. Slider
		break;

	case 2:  //Left
		toolBarPos.x = 0;
		toolBarPos.y = 0;
		toolBarSize.SetWidth(buttonWidth + 10);
		toolBarSize.SetHeight(toolBarHeight);
		mainTabPos.x = toolBarSize.GetWidth();
		mainTabPos.y = 0;
		mainTabSize.SetWidth(panelWidth - toolBarSize.GetWidth());
		mainTabSize.SetHeight(panelHeight - 30); // 20 for the control bar
		break;

	case 3:  // Right
		mainTabPos.x = 0;
		mainTabPos.y = 0;
		mainTabSize.SetWidth(panelWidth - buttonWidth-10);
		mainTabSize.SetHeight(panelHeight - 30); // 10 + 20 for the control bar
		toolBarPos.x = mainTabSize.GetWidth();
		toolBarPos.y = 0;
		toolBarSize.SetWidth(buttonWidth + 10);
		toolBarSize.SetHeight(toolBarHeight);
		break;


	default:
		break;
	}

	if (videoMainTab == NULL)
		videoMainTab = DBG_NEW wxNotebook(videoPanel, ID_VIDEO_PANEL_NOTEBOOK, mainTabPos, mainTabSize , wxSUNKEN_BORDER, "videoMainTab");
	else
	{
		videoMainTab->SetPosition(mainTabPos);
		videoMainTab->SetSize(mainTabSize);
	}
	if (bMainFileOpen)
	{
		spdlog::info("Video Main Tab {} {} {} {}", mainTabPos.x, mainTabPos.y, mainTabSize.GetWidth(), mainTabSize.GetHeight());
	}
	videoMainTab->SetBackgroundColour(wxColor(0x6d, 0x92, 0x9b));
	//// Within Notebook
		if (mainTab == NULL)
		{
			mainTab = DBG_NEW wxPanel(videoMainTab, wxID_ANY, wxPoint(0, 25), wxSize(mainTabSize.GetWidth(), mainTabSize.GetHeight()-30));
			videoMainTab->AddPage(mainTab, wxT("Main Tab"), true, -1);
		}
		else
		{
			mainTab->SetPosition(wxPoint(0, 25));
			mainTab->SetSize(wxSize(mainTabSize.GetWidth(), mainTabSize.GetHeight()-30));
		}
		mainTab->SetBackgroundColour(wxColor(0x7f, 0x7f, 0x7f));

		if (bMainFileOpen)
		{
			spdlog::info("Video Main Tab Panel {} {} {} {}", mainTabPos.x, mainTabPos.y + 25, mainTabSize.GetWidth(), mainTabSize.GetHeight() - 25);
		}

		if (mainPicturePanel == NULL)
			mainPicturePanel = DBG_NEW PictureDisplay(mainTab, wxID_ANY, wxPoint(0,0), wxSize(mainTab->GetSize().GetWidth()-2, mainTab->GetSize().GetHeight()-2));
		else
		{
			mainPicturePanel->SetPosition(wxPoint(0, 0));
			mainPicturePanel->SetSize(wxSize(mainTab->GetSize().GetWidth() - 2, mainTab->GetSize().GetHeight() - 2));
		}
		mainPicturePanel->SetBackgroundColour(wxColor(0x0, 0x0, 0x0));
		if (bMainFileOpen)
		{
			spdlog::info("Picture Panel {} {} {} {}", 0,0, mainTab->GetSize().GetWidth() - 10, mainTab->GetSize().GetHeight() - 10);
		}

	if (picControlPanel == NULL)
	{
		picControlPanel = DBG_NEW PicControlPanel(videoPanel, -1, wxPoint(videoMainTab->GetPosition().x, videoMainTab->GetPosition().y + videoMainTab->GetSize().GetHeight()), wxSize(videoMainTab->GetSize().GetWidth(), 20));
		picControlPanel->SetBackgroundColour(wxColor(0x53, 0x9c, 0xff));
	}
	else
	{
		picControlPanel->SetPosition(wxPoint(videoMainTab->GetPosition().x, videoMainTab->GetPosition().y + videoMainTab->GetSize().GetHeight()));
		picControlPanel->SetSize(wxSize(videoMainTab->GetSize().GetWidth(), 20));
		picControlPanel->ResizePanel();
	}

	if (bMainFileOpen)
	{
		spdlog::info("Pic Control Panel {} {} {} {}", mainTabPos.x, mainTabPos.y + mainTabSize.GetHeight()-20, mainTabSize.GetWidth(), 20);
	}

	if (toolControlPanel == NULL)
	{
		toolControlPanel = DBG_NEW PicButtonControlPanel(videoPanel, -1, wxPoint(toolBarPos.x, toolBarPos.y),
			wxSize(toolBarSize.GetWidth(), toolBarSize.GetHeight()), buttonWidth, buttonMargin, buttonXPosDiff, buttonYPosDiff,  bPlayerMode);
		toolControlPanel->SetBackgroundColour(wxColor(0xcc, 0xcc, 0x99));
	}
	else
	{
		toolControlPanel->SetPosition(wxPoint(toolBarPos.x, toolBarPos.y));
		toolControlPanel->SetSize(wxSize(toolBarSize.GetWidth(), toolBarSize.GetHeight()));
		toolControlPanel->Resize(buttonWidth, buttonMargin, buttonXPosDiff, buttonYPosDiff);
	}
	if (bMainFileOpen)
	{
		spdlog::info("Tool Control {} {} {} {}", toolBarPos.x, toolBarPos.y, toolBarSize.GetWidth(), toolBarSize.GetHeight());
	}
	videoPanel->Show();

}

VideoEditorScreen::~VideoEditorScreen()
{
	DeleteConfigClass();
	DeleteSPDLogFile();
	m_pParent->Show();
}


void VideoEditorScreen::OnMoveAbsolute(wxCommandEvent& WXUNUSED(event))
{
	pDataRecorder->MoveAbsolute();
	SetButtonStatus();

}

void VideoEditorScreen::OnSliderHover(wxCommandEvent& WXUNUSED(event))
{
	pDataRecorder->ShowThumb();
	if (!pDataRecorder->IsVideoImageMoving())
	{
		struct ResizeScreenBuffer **bmpImage = mainPicturePanel->LockBitmap();
		bmpImage = mainPicturePanel->GetThumbBuffer();
		*bmpImage = pRecordBufferManager->GetThumbResizeBuffer();
		if (*bmpImage != NULL)
		{
			int xPos = picControlPanel->GetHoverXPos();
			mainPicturePanel->SetThumbXPos(xPos);
		}
		mainPicturePanel->UnlockBitmap();
		//mainPicturePanel->paintNow();
		mainPicturePanel->Refresh();
	}
	SetButtonStatus();

}

void VideoEditorScreen::OnSliderUpdateTime(wxCommandEvent& WXUNUSED(event))
{
	picControlPanel->SetTimeValue();
}

void VideoEditorScreen::OnSliderHoverClear(wxCommandEvent& WXUNUSED(event))
{
	struct ResizeScreenBuffer **bmpImage = mainPicturePanel->LockBitmap();
	bmpImage = mainPicturePanel->GetThumbBuffer();
	*bmpImage = NULL;
	pRecordBufferManager->ClearThumbResizeBuffer();
	mainPicturePanel->UnlockBitmap();
	if (!pDataRecorder->IsVideoImageMoving())
	{
		//mainPicturePanel->paintNow();
		mainPicturePanel->Refresh();
	}
	//SetButtonStatus();

}

void VideoEditorScreen::OnFastRewind(wxCommandEvent& WXUNUSED(event))
{
	if (!bPlayerMode)
		mainEditTab->Disable();
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	pDataRecorder->RewindVideo();
	SetButtonStatus();

}

void VideoEditorScreen::OnSlowRewind(wxCommandEvent& WXUNUSED(event))
{
	if (!bPlayerMode)
		mainEditTab->Disable();
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	pDataRecorder->ReverseStepVideo();
	SetButtonStatus();

}

void VideoEditorScreen::OnFastForward(wxCommandEvent& WXUNUSED(event))
{
	if (!bPlayerMode)
		mainEditTab->Disable();
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	pDataRecorder->FastForwardVideo();
	SetButtonStatus();

}

void VideoEditorScreen::OnSlowForward(wxCommandEvent& WXUNUSED(event))
{
	if (!bPlayerMode)
		mainEditTab->Disable();
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	pDataRecorder->ForwardStepVideo();
	SetButtonStatus();

}

void VideoEditorScreen::OnStop(wxCommandEvent& WXUNUSED(event))
{
	if (!bPlayerMode)
		mainEditTab->Enable();
	pDataRecorder->SetRecorderStop();
	mainToolBarPanel->EnableScenarioChoice(true);
	mainToolBarPanel->EnablePlayScenario(true);
	SetButtonStatus();

}

void VideoEditorScreen::OnPlay(wxCommandEvent& WXUNUSED(event))
{
	if (!bPlayerMode)
		mainEditTab->Disable();
	
	pDataRecorder->SetRecorderPlay();
	mainToolBarPanel->EnableScenarioChoice(false);
	mainToolBarPanel->EnablePlayScenario(false);
	SetButtonStatus();

}

void VideoEditorScreen::OnAddActivity(wxCommandEvent& WXUNUSED(event))
{
	pDataRecorder->InsertEvent();
	pDataRecorder->InitializeVideoPath();
	SetButtonStatus();

}

void VideoEditorScreen::OnStartSegment(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->ClearSegmentElements();
	toolControlPanel->ClearSegment();
	pDataRecorder->StartSegment();
	bpmnPanel->SetCopySegmentFlag(false);
	SetButtonStatus();

}

void VideoEditorScreen::OnEndSegment(wxCommandEvent& WXUNUSED(event))
{
	pDataRecorder->EndSegment();
	SetButtonStatus();

}

void VideoEditorScreen::OnCopySegment(wxCommandEvent& WXUNUSED(event))
{
	toolControlPanel->ClearSegmentCopiedFlg();
	pDataRecorder->CopySegment();
	bpmnPanel->SetCopySegmentFlag(true);
	SetButtonStatus();

}

void VideoEditorScreen::OnCutSegment(wxCommandEvent& WXUNUSED(event))
{
	toolControlPanel->ClearSegmentCopiedFlg();
	pDataRecorder->CutSegment();
	bpmnPanel->SetCopySegmentFlag(true);
	SetButtonStatus();
}

ULONG VideoEditorScreen::GetThumbTime()
{
	return picControlPanel->GetThumbTime();
}


void VideoEditorScreen::OnPasteSegment(wxCommandEvent& WXUNUSED(event))
{
	toolControlPanel->ClearSegmentCopiedFlg();
	pDataRecorder->PasteSegment();
	bpmnPanel->SetCopySegmentFlag(false);
	bpmnPanel->ClearSegmentElements();
	SetButtonStatus();

}

void VideoEditorScreen::OnClearSegment(wxCommandEvent& WXUNUSED(event))
{
	toolControlPanel->ClearSegmentCopiedFlg();
	bpmnPanel->ClearSegmentElements();
	pDataRecorder->ClearSegment();
	bpmnPanel->SetCopySegmentFlag(false);
	SetButtonStatus();
}

void VideoEditorScreen::OnBranchSegment(wxCommandEvent& WXUNUSED(event))
{
	toolControlPanel->ClearSegmentCopiedFlg();
	pDataRecorder->CreateBranch();
	bpmnPanel->SetCopySegmentFlag(false);
	pDataRecorder->InitializeVideoPath();
	SetButtonStatus();
	bpmnPanel->ClearSegmentElements();
}

void VideoEditorScreen::OnClose(wxCloseEvent & event)
{
	ExecutionManager *pExecManager = GetExecutionManager();
	if (pExecManager != NULL)
	{
		if (pDataRecorder)
			pDataRecorder->SetRecorderStop();
		pExecManager->CloseAllWindows();
		DeleteExecutionManager();
	}
	pExecManager = NULL;
	event.Skip();
//	SetButtonStatus();

}

void VideoEditorScreen::OnExit(wxCommandEvent & event)
{
//	SetButtonStatus();
	Close();
}

void VideoEditorScreen::OnBPMNBranchBottom(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->BranchBottom();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNBranchRight(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->BranchRight();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNCreateSubProcess(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->CreateSubProcess();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNDeleteBranch(wxCommandEvent& WXUNUSED(event))
{
	pDataRecorder->InitializeVideoPath();
	SetButtonStatus();
}

void VideoEditorScreen::OnBPMNDeleteEvent(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->DeleteItems();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNLevelUp(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->GoUpDiagramLevel();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNMoveProcessBelow(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->GoUpDiagramLevel();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNMoveSubProcessAbove(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->AddToSubProcessAbove();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNMoveSubProcessBelow(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->AddToSubProcessBelow();
	SetButtonStatus();

}

void VideoEditorScreen::OnBPMNSyncVideo(wxCommandEvent& WXUNUSED(event))
{
	bpmnPanel->SynchronizeVideo();
	SetButtonStatus();

}


// Interface method implementation

void VideoEditorScreen::SetSliderPosition(long position)
{
	picControlPanel->SetSliderPosition(position);
}

long VideoEditorScreen::GetSliderPosition()
{
	return picControlPanel->GetSliderPosition();

}

long VideoEditorScreen::VideoSegmentStartTime()
{
	return picControlPanel->SetSliderSegmentStart();
}

long VideoEditorScreen::VideoSegmentEndTime()
{
	return picControlPanel->SetSliderSegmentEnd();
}

void VideoEditorScreen::ResetSegmentMarker()
{
	picControlPanel->ClearSliderSegment();
}

void VideoEditorScreen::SetPlayerEndTime(ULONG endTime)
{
	picControlPanel->SetSliderMaxTime(endTime);
}

void VideoEditorScreen::SetPlayerTimeWindow()
{
	if (!bFirst)
		picControlPanel->SetTimeValue();
	bFirst = false;
}

void VideoEditorScreen::StopPlay()
{
	lockPlayerWindow.lock();
	bStartPlay = false;
	lockPlayerWindow.unlock();


}
void VideoEditorScreen::SetExitFlag(bool flg)
{
	lockPlayerWindow.lock();
	bExitThreads = flg;
	lockPlayerWindow.unlock();
}

void VideoEditorScreen::SetStartFlag(bool bFlg)
{
	lockPlayerWindow.lock();
	bStartPlay = bFlg;
	lockPlayerWindow.unlock();
}

void VideoEditorScreen::StartPlay(long long fPos)
{
	int lastMon = 0;

	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	LARGE_INTEGER liDueTime;
	__int64         qwDueTime;

	while (TRUE)
	{
		lockPlayerWindow.lock();
		bool bPlay = bStartPlay;
		bool bPlayerStatus = pDataRecorder->GetRecorderStatus();
		bool bQuit = bExitThreads;
		if (bQuit || !bPlay || !bPlayerStatus)
			bVideoThreadWaiting = true;
		else
			bVideoThreadWaiting = false;
		lockPlayerWindow.unlock();
		if (bQuit)
			break;
		if (!bPlay || !bPlayerStatus)
		{
			qwDueTime = -100000LL; // wait for 10 ms
			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);

			if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
			{
				spdlog::info("Failed creating timer");
			}
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			{
				spdlog::info("Failed waiting for Timer");
			}
			CancelWaitableTimer(hTimer);
			continue;
		}
		if (pRecordBufferManager->IsFreeResizeScreenBufferAvailable())
		{
			spdlog::info("Video Buffer Available");
			struct bufferRecord *pBuffer = pRecordBufferManager->GetWriteBuffer();
			if (pBuffer == NULL)
			{
				qwDueTime = -20000LL; // wait for 2 ms
				// Copy the relative time into a LARGE_INTEGER.
				liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
				liDueTime.HighPart = (LONG)(qwDueTime >> 32);

				if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
				{
					spdlog::info("Failed creating timer");
				}
				if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
				{
					spdlog::info("Failed waiting for Timer");
				}
				CancelWaitableTimer(hTimer);
				continue;
			}
			USHORT *pFileContextNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2);
			USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));

			int currFileNum = *pFileContextNumPosition;
			int currChannelNum = *pFileContextChannelNumPosition;
			lastMon = pDataRecorder->ProcessVideoRecord(pBuffer, currFileNum, currChannelNum, lastMon);
			pRecordBufferManager->FreePlayerBuffer(pBuffer);
			spdlog::info("Video Buffer Returned");

		}
		else
		{
			spdlog::info("Video Buffer Not Available");
			qwDueTime = -50000LL; // wait for 5 ms
			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);

			if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
			{
				spdlog::info("Failed creating timer");
			}
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			{
				spdlog::info("Failed waiting for Timer");
			}
			CancelWaitableTimer(hTimer);
		}
	}
	CloseHandle(hTimer);
}


int VideoEditorScreen::GetWindowWidth()
{
	return mainPicturePanel->GetSize().GetWidth();
}

int VideoEditorScreen::GetWindowHeight()
{
	return mainPicturePanel->GetSize().GetHeight();
}

bool VideoEditorScreen::GetActiveMonitorDisplayFlag()
{
	return bActiveMonitorDisplay;
}

void VideoEditorScreen::SetDataManager(PlayerDataFileManager *pRecorder)
{
	pDataRecorder = pRecorder;
	pDataRecorder->SetPlayerMode(bPlayerMode);
	//*pTabControlRecorder->SetController(pRecorder);
	pBPMNControl->SetController(pRecorder);
	
}

void VideoEditorScreen::GetPointOffset(POINT *pPoint)
{
	if (pBPMNControl != NULL)
		pBPMNControl->GetPointOffset(pPoint);
}

void VideoEditorScreen::SetDiagram(list<BPMNElement *> *pDiagram)
{
	pBPMNControl->SetMainList(pDiagram);
	pBPMNControl->CreateAndDisplayDiagram(pDiagram);
}

void VideoEditorScreen::SetEventMap(map<ULONG, struct eventListElement *> *pEventMap)
{
	pBPMNControl->SetEventMap(pEventMap);
}

void VideoEditorScreen::SetTabFields(struct eventListElement *pEventListElement)
{
	if (pEventListElement != NULL)
	{
		pCurrentEventDisplay = pCurrentEdited = pEventListElement->pElement;
		
		if (!bPlayerMode)
		{
			InitializeAnnotationString(pCurrentEdited);
			mainEditTab->UpdateTabFields();
		}
		//else
		//	UpdateTabFields();
	}
}

void VideoEditorScreen::SetTabFields(struct cellDef *pSelected)
{

	if (pSelected != NULL && !bPlayerMode)
	{
		pCurrentEdited = pSelected->bpmnElement;
		mainEditTab->UpdateTabFields();
	}
}

void VideoEditorScreen::OnUpdateTabFields(wxCommandEvent& WXUNUSED(event))
{
	UpdateTabFields();
	SetButtonStatus();
}

void VideoEditorScreen::OnUpdateBPMNControl(wxCommandEvent& WXUNUSED(event))
{
	pBPMNControl->UpdateBPMNWindow();
	SetButtonStatus();
}

void VideoEditorScreen::OnShowSubTitle(wxCommandEvent& WXUNUSED(event))
{
	if (bShowSubTitle)
		bShowSubTitle = false;
	else
		bShowSubTitle = true;

	mainToolBarPanel->SetShowSubTitleTip(bShowSubTitle);
	mainPicturePanel->SetShowSubTitle(bShowSubTitle);
}

void VideoEditorScreen::OnReportPDF(wxCommandEvent& WXUNUSED(event))
{
	PDFDocument *pDocument = new PDFDocument;

	pDocument->SetPlayerManager(pDataRecorder);
	pDocument->GenerateReport();
}


void VideoEditorScreen::OnReportSOP(wxCommandEvent& WXUNUSED(event))
{
	string strValue = pDataRecorder->ReadProcessValue();

	ProcessDescriptionDialog prDialog("", this, -1, _("Process Information"), wxPoint(80, 100), wxSize(600, 500));
	prDialog.SetProcessData(strValue);
	if (prDialog.ShowModal() == wxID_OK)
	{
		// Stop Video Player
		bool bRecorderPlay = pDataRecorder->GetRecorderStatus();

		if (bRecorderPlay)
			pDataRecorder->SetRecorderStop();
		
		strValue = prDialog.GetProcessData();

		string err;
		Json jSonElement;
		map <string, Json> processInfo;

		jSonElement = Json::parse(strValue, err);
		processInfo = jSonElement["ProcessInfo"].object_items();
		string strNameVal = "";
		map<string, Json>::iterator itProcess = processInfo.find("Name");
		if (itProcess != processInfo.end())
		{
			strNameVal = (itProcess->second).string_value();
			strNameVal += "_SOPReport";
		}
		else
		{
			strNameVal = "SOPReport";
		}

		strNameVal += ".docx";

		string strFileName = "C:\\Recorder\\" + strNameVal;

		SOPWordReport sopReport((char *)strFileName.c_str());
		sopReport.SetProcessValue(strValue);
		sopReport.SetPlayerManager(pDataRecorder);
		pDataRecorder->WriteProcessValue(strValue);
	
		list <BPMNElement *> *mainDiagram = pDataRecorder->GetBPMNDiagram();
		sopReport.SetDiagram(mainDiagram);
		sopReport.CreateSOPReport();
		bpmnPanel->ResetDiagram();
		if (bRecorderPlay)
			pDataRecorder->SetRecorderPlay();
	}
}

void VideoEditorScreen::InitializeAnnotationString(BPMNElement *pElement)
{
	if (pElement == NULL)
	{
		displayAnnotationString = "";
		displayAnnotationNotes = "";
	}
	else
	{
		string strAnnotation = pElement->GetKeyValueAsString("Annotation");
		string strAnnotationBottom = pElement->GetKeyValueAsString("AnnotationBottom");
		string strNotes = pElement->GetKeyValueAsString("Notes");
		displayAnnotationString = strAnnotation;
		displayAnnotationNotes = strNotes;
		displayAnnotationBottomString = strAnnotationBottom;
	}
}

void VideoEditorScreen::UpdateTabFields()
{
	if (pCurrentEdited == NULL)
		return;
	BPMNElement *pElement = pCurrentEdited;
	if (pElement != NULL)
	{
		string strAnnotation = pElement->GetKeyValueAsString("Annotation");
		string strAnnotationBottom = pElement->GetKeyValueAsString("AnnotationBottom");
		string strNotes = pElement->GetKeyValueAsString("Notes");
		displayAnnotationString = strAnnotation;
		displayAnnotationNotes = strNotes;
		displayAnnotationBottomString = strAnnotationBottom;
		mainEditTab->SetBranch(false);
		if (pElement->GetType() == 4)
		{
			mainEditTab->SetBranch(true);
		}

		if (!bPlayerMode)
		{
			string strGroup = pElement->GetKeyValueAsString("Group");
			string strSubGroup = pElement->GetKeyValueAsString("SubGroup");
			string strType = pElement->GetKeyValueAsString("Type");
			string strAction = pElement->GetKeyValueAsString("Action");
			string strProcessName = pElement->GetKeyValueAsString("PName");
			string strWindowName = pElement->GetKeyValueAsString("WName");
			int controlType = pElement->GetKeyValueAsInt("CType");
			string strControlType = GetControlTypeName(controlType);
			RECT r;
			string strDocumentName = pElement->GetKeyValueAsString("DocName");
			double duration = (double)pElement->GetKeyValueAsInt("Duration");
			duration = duration / 1000000.0f;
			pElement->GetKeyValueAsRECT("WRect", &r);
			string strWindowPosition;
			char str[100];
			sprintf_s(str, "%d, %d", r.left, r.top);
			strWindowPosition = str;
			sprintf_s(str, "%d, %d", r.right - r.left, r.bottom - r.top);
			string strWindowSize = str;

			RECT *pRect = pElement->GetBoundingRect();
			sprintf_s(str, "%d, %d", pRect->left, pRect->top);

			string strControlPosition = str;
			sprintf_s(str, "%d, %d", pRect->right - pRect->left, pRect->bottom - pRect->top);
			string strControlSize = str;


			mainEditTab->SetAnnotation(strAnnotation);
			mainEditTab->SetAnnotationBottom(strAnnotationBottom);
			mainEditTab->SetNotes(strNotes);
			mainEditTab->SetGroup(strGroup);
			mainEditTab->SetSubGroup(strSubGroup);
			mainEditTab->SetType(strType);
			mainEditTab->SetAction(strAction);
			infoTab->SetProcessName(strProcessName);
			infoTab->SetWindowName(strWindowName);
			infoTab->SetWindowPosition(strWindowPosition);
			infoTab->SetWindowSize(strWindowSize);
			infoTab->SetControlType(strControlType);
			infoTab->SetControlPosition(strControlPosition);
			infoTab->SetControlSize(strControlSize);
			infoTab->SetDocumentName(strDocumentName);
			infoTab->SetActivityDuration(duration);
			mainEditTab->ResizePanel();
			mainEditTab->ShowTabData();
			infoTab->ShowTabData();
			if (!pDataRecorder->GetRecorderStatus()) // If recorder not playing
				mainEditTab->Enable();
		}
	}
}

string VideoEditorScreen::GetControlTypeName(int nControlType)
{
	string strName = "";

	switch (nControlType)
	{
	case UIA_ButtonControlTypeId:
		strName = "Button";
		break;
	case UIA_CalendarControlTypeId:
		strName = "Calendar";
		break;
	case UIA_CheckBoxControlTypeId:
		strName = "CheckBox";
		break;
	case UIA_ComboBoxControlTypeId: 
		strName = "ComboBox";
		break;
	case UIA_EditControlTypeId:
		strName = "Edit";
		break;
	case UIA_HyperlinkControlTypeId:
		strName = "Hyperlink";
		break;
	case UIA_ImageControlTypeId: 
		strName = "Image";
		break;
	case UIA_ListItemControlTypeId:
		strName = "ListItem";
		break;
	case UIA_ListControlTypeId:
		strName = "List";
		break;
	case UIA_MenuControlTypeId:
		strName = "Menu";
		break;
	case UIA_MenuBarControlTypeId:
		strName = "MenuBar";
		break;
	case UIA_MenuItemControlTypeId:
		strName = "MenuItem";
		break;
	case UIA_ProgressBarControlTypeId:
		strName = "ProgressBar";
		break;
	case UIA_RadioButtonControlTypeId:
		strName = "RadioButton";
		break;
	case UIA_ScrollBarControlTypeId:
		strName = "ScrollBar";
		break;
	case UIA_SliderControlTypeId:
		strName = "Slider";
		break;
	case UIA_SpinnerControlTypeId:
		strName = "Spinner";
		break;
	case UIA_StatusBarControlTypeId:
		strName = "StatusBar";
		break;
	case UIA_TabControlTypeId:
		strName = "Tab";
		break;
	case UIA_TabItemControlTypeId:
		strName = "TabItem";
		break;
	case UIA_TextControlTypeId:
		strName = "Text";
		break;
	case UIA_ToolBarControlTypeId:
		strName = "ToolBar";
		break;
	case UIA_ToolTipControlTypeId:
		strName = "ToolTip";
		break;
	case UIA_TreeControlTypeId:
		strName = "Tree";
		break;
	case UIA_TreeItemControlTypeId:
		strName = "TreeItem";
		break;
	case UIA_CustomControlTypeId:
		strName = "Custom";
		break;
	case UIA_GroupControlTypeId:
		strName = "Group";
		break;
	case UIA_ThumbControlTypeId:
		strName = "Thumb";
		break;
	case UIA_DataGridControlTypeId:
		strName = "DataGrid";
		break;
	case UIA_DataItemControlTypeId:
		strName = "DataItem";
		break;
	case UIA_DocumentControlTypeId:
		strName = "Document";
		break;
	case UIA_SplitButtonControlTypeId:
		strName = "SplitButton";
		break;
	case UIA_WindowControlTypeId:
		strName = "Windows";
		break;
	case UIA_PaneControlTypeId:
		strName = "Pane";
		break;
	case UIA_HeaderControlTypeId:
		strName = "Header";
		break;
	case UIA_HeaderItemControlTypeId:
		strName = "HeaderItem";
		break;
	case UIA_TableControlTypeId:
		strName = "Table";
		break;
	case UIA_TitleBarControlTypeId:
		strName = "TitleBar";
		break;
	case UIA_SeparatorControlTypeId:
		strName = "Separator";
		break;
	case UIA_SemanticZoomControlTypeId:
		strName = "SemanticZoom";
		break;
	case UIA_AppBarControlTypeId:
		strName = "AppBar";
		break;
	default:
		break;
	}

	return strName;
}


bool VideoEditorScreen::IsVideoThreadWaiting()
{
	lockPlayerWindow.lock();
	bool bVideoThread = bVideoThreadWaiting;
	bool bDisplayThread = bDisplayThreadWaiting;
	lockPlayerWindow.unlock();
	if (bVideoThread && bDisplayThread)
		return true;
	else
		return false;

}

void VideoEditorScreen::InsertEvent(ULONG nEventID, long long timestamp)
{
	if (pBPMNControl != NULL)
		pBPMNControl->InsertItem(nEventID, timestamp);
}

void VideoEditorScreen::StartDisplay()
{
	time_t t1Display, t2Display;
	//	time_t t1DataFile, t2DataFile;
	long long overFlow = 0L;
	
	ULONG newEventIndexNumber = 0;
	bool bFirstScreen = true;
	ULONG eventIndexNumber = 0;

	t1Display = t2Display = -1L;

	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	LARGE_INTEGER liDueTime;
	__int64         qwDueTime;


	while (TRUE)
	{
		lockPlayerWindow.lock();
		bool bPlay = bStartPlay;
		bool bQuit = bExitThreads;
		bool bPlayerStatus = pDataRecorder->GetRecorderStatus();
		if (bQuit || !bPlay || !bPlayerStatus)
			bDisplayThreadWaiting = true;
		else
			bDisplayThreadWaiting = false;
		lockPlayerWindow.unlock();
		if (bQuit)
			break;
		if (!bPlay || !bPlayerStatus)
		{
			qwDueTime = -100000LL; // wait for 10 ms
			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);

			if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
			{
				spdlog::info("Failed creating Audio Timer");
			}
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			{
				spdlog::info("Failed waiting for Audio Timer");
			}
			CancelWaitableTimer(hTimer);
			t1Display = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			continue;
		}
		struct ResizeScreenBuffer *pResizeBuffer = pRecordBufferManager->GetResizeBuffer();
		if (pResizeBuffer == NULL)
		{
			pDataRecorder->CheckIfStopPlayer();
			continue;
		}
		spdlog::info("Display Screen Start");
		struct ResizeScreenBuffer **bmpImage = mainPicturePanel->LockBitmap();
		*bmpImage = pResizeBuffer;
		bmpImage = mainPicturePanel->GetThumbBuffer();
		*bmpImage = pRecordBufferManager->GetThumbResizeBuffer();
		if (*bmpImage != NULL)
		{
			int xPos = picControlPanel->GetHoverXPos();
			mainPicturePanel->SetThumbXPos(xPos);
		}
		//mainPicturePanel->SetAnnotation(displayAnnotationString);
		//mainPicturePanel->SetNotes(displayAnnotationNotes);
		//mainPicturePanel->UnlockBitmap();
		//mainPicturePanel->paintNow();
		//lastfPosVideoPlayed = pResizeBuffer->fPosIndexHeader;
		//lastVideoIndexNum = pResizeBuffer->indexNum;

		pDataRecorder->SetCurrentPositionMarker(pResizeBuffer->fPosIndexHeader, pResizeBuffer->indexNum, pResizeBuffer->fileNumber, pResizeBuffer->channelNumber);
		if (bQuit)
			break;
		if (pDataRecorder->IsEventDataPresent())
		{
			newEventIndexNumber = pDataRecorder->FindEventIndex(pResizeBuffer->fPosIndexHeader, pResizeBuffer->indexNum);
			lockPlayerWindow.lock();
			eventIndexNumber = currentEventDisplayed;
			lockPlayerWindow.unlock();
			if (newEventIndexNumber != eventIndexNumber)
			{
				lockPlayerWindow.lock();
				currentEventDisplayed = newEventIndexNumber;
				lockPlayerWindow.unlock();
				bool isExist = pBPMNControl->ShowEventInWindow(newEventIndexNumber);
				pDataRecorder->SetCurrentEventDisplay(newEventIndexNumber);
				if (!isExist)
				{
					InitializeAnnotationString(NULL);
					pBPMNControl->SetCurrentSyncronizedElement((BPMNElement *)NULL, pDataRecorder->IsSegmentOn());
				}
				else
				{
					pBPMNControl->SetCurrentSyncronizedElement(newEventIndexNumber, pDataRecorder->IsSegmentOn());
				}
			}
		}
		mainPicturePanel->SetAnnotation(displayAnnotationString);
		mainPicturePanel->SetNotes(displayAnnotationNotes);
		mainPicturePanel->UnlockBitmap();
		spdlog::info("Displaying Screen");
		mainPicturePanel->paintNow();
		spdlog::info("Displaying Screen Done");
		//mainPicturePanel->Refresh();
		//long sleepTime = 0;
		if (bFirstScreen)
		{
			//			t1DataFile = pResizeBuffer->t1FileStart;
			bFirstScreen = false;
			t1Display = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		else
		{
			//			t2DataFile = pResizeBuffer->t1FileStart;
			long long timeIntData = pResizeBuffer->recDurationus;			//t2DataFile - t1DataFile;
			char str[100];
			sprintf_s(str, "RecDuration: %lld", timeIntData);
			spdlog::info(str);
			picControlPanel->AddPlayTime((ULONG)timeIntData);
			//SetPlayerTimeWindow();
			t2Display = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			long long timeIntDisplay = t2Display - t1Display + overFlow;
			if (timeIntData > timeIntDisplay)
			{
				spdlog::info("Start Display Wait");
				DWORD delayTime = (DWORD)(timeIntData - timeIntDisplay);

				qwDueTime = -10LL * delayTime; // wait for 10 ms
				// Copy the relative time into a LARGE_INTEGER.
				liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
				liDueTime.HighPart = (LONG)(qwDueTime >> 32);

				if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
				{
					spdlog::info("Failed creating Audio Timer");
				}
				if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
				{
					spdlog::info("Failed waiting for Audio Timer");
				}
				CancelWaitableTimer(hTimer);

				//sleepTime = delayTime / 1000;
				//Sleep((DWORD)sleepTime);
			}
			else
			{
				// This is just to prevent situations when the Video runs too fase;
				spdlog::info("Start Display No Wait");
				overFlow = timeIntDisplay - timeIntData;
				if (overFlow > 10000L)
				{
					overFlow = 0;
				}
			}

			t1Display = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		pRecordBufferManager->FreeResizeBuffer(pResizeBuffer);
	}
	CloseHandle(hTimer);

}

void VideoEditorScreen::DisplayImage()
{
	ULONG oldEventIndex;
	ULONG nEventIndex = 0L;

	struct ResizeScreenBuffer *pResizeBuffer = pRecordBufferManager->GetResizeBuffer();
	if (pResizeBuffer == NULL)
	{
		pDataRecorder->CheckIfStopPlayer();
		return;
	}

	struct ResizeScreenBuffer **bmpImage = mainPicturePanel->LockBitmap();
	*bmpImage = pResizeBuffer;
	pDataRecorder->SetCurrentPositionMarker(pResizeBuffer->fPosIndexHeader, pResizeBuffer->indexNum, pResizeBuffer->fileNumber, pResizeBuffer->channelNumber);

	if (pDataRecorder->IsEventDataPresent())
	{
		nEventIndex = pDataRecorder->FindEventIndex(pResizeBuffer->fPosIndexHeader, pResizeBuffer->indexNum);
		lockPlayerWindow.lock();
		oldEventIndex = currentEventDisplayed;
		lockPlayerWindow.unlock();

		if (nEventIndex != oldEventIndex)
		{
			lockPlayerWindow.lock();
			currentEventDisplayed = nEventIndex;
			lockPlayerWindow.unlock();
			bool isExist = pBPMNControl->ShowEventInWindow(nEventIndex);
			//pDataRecorder->SetCurrentEventDisplay(nEventIndex);
			if (!isExist)
			{
				InitializeAnnotationString(NULL);
				pBPMNControl->SetCurrentSyncronizedElement((BPMNElement *)NULL, pDataRecorder->IsSegmentOn());
			}
			else
			{
				pBPMNControl->SetCurrentSyncronizedElement(nEventIndex, pDataRecorder->IsSegmentOn());
			}
		}
	}
	mainPicturePanel->SetAnnotation(displayAnnotationString);
	mainPicturePanel->SetNotes(displayAnnotationNotes);
	mainPicturePanel->UnlockBitmap();
	mainPicturePanel->paintNow();
	//mainPicturePanel->Refresh();
	pRecordBufferManager->FreeResizeBuffer(pResizeBuffer);
}

void VideoEditorScreen::BPMNElementSelected(struct cellDef *pCell)
{
	if (pCell->bpmnElement == NULL)
		return;
	//string str = pCell->bpmnElement->GetAnnotation();
	//pTabControlRecorder->SetAnnotation(str);
	
	//str = pCell->bpmnElement->GetNotes();
	//pTabControlRecorder->SetAnnotationNotes(str);
}

void VideoEditorScreen::SetFramesPerSecond(int fpsCount)
{
	fps = fpsCount;
}

int VideoEditorScreen::GetFramesPerSecond()
{
	return fps;
}

bool VideoEditorScreen::GetSegInProgress()
{
	return picControlPanel->GetSegInProgress();
}

bool VideoEditorScreen::GetSegPresent()
{
	return picControlPanel->GetSegPresent();
}

ULONG VideoEditorScreen::GetSegStartPos()
{
	return picControlPanel->GetSegStartPos();
}

ULONG VideoEditorScreen::GetSegEndPos()
{
	return picControlPanel->GetSegEndPos();
}


void VideoEditorScreen::SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal)
{
	picControlPanel->SetSegmentDetails(bInProgress, bPresent, startVal, endVal);
}

//struct ScreenData *VideoEditorScreen::GetScreenData(int monNum)
//{
//	if ((int)screenData.size() > monNum)
//	{
//		return screenData[monNum];
//	}
//	return NULL;
//}
//
//void VideoEditorScreen::GetMonitorDetails(int noMonitors, long long fPosMonitorDetails)
//{
//	pMonitorDetails = new struct MonitorScreenData[noMonitors];
//	ZeroMemory((char *)pMonitorDetails, sizeof(struct MonitorScreenData) * noMonitors);
//
//	numMonitors = noMonitors;
//	for (int i = 0; i < noMonitors; i++)
//	{
//		if (i == 0)
//			pDataRecorder->ReadFromFile((char *)&pMonitorDetails[i].MonitorDetails, sizeof(struct MonitorHdr), fPosMonitorDetails);
//		else
//			pDataRecorder->ReadFromFile((char *)&pMonitorDetails[i].MonitorDetails, sizeof(struct MonitorHdr), -1L);
//
//		struct ScreenData *screen = GetScreenData((int)pMonitorDetails[i].MonitorDetails.MonitorNumber);
//
//		if (screen == NULL)
//		{
//			screen = new struct ScreenData;
//			ZeroMemory(screen, sizeof(struct ScreenData));
//
//			screen->hMonitor = pMonitorDetails[i].MonitorDetails.MonitorNumber;
//			screen->g_pd3dDevice = NULL;
//			screen->g_pSurface = NULL;
//			screen->bCapture = false;
//			screen->bFullScreen = false;
//			screen->rcMonitor.left = pMonitorDetails[i].MonitorDetails.left;
//			screen->rcMonitor.right = pMonitorDetails[i].MonitorDetails.right;
//			screen->rcMonitor.top = pMonitorDetails[i].MonitorDetails.top;
//			screen->rcMonitor.bottom = pMonitorDetails[i].MonitorDetails.bottom;
//
//			int width = pMonitorDetails[i].MonitorDetails.right - pMonitorDetails[i].MonitorDetails.left;
//			int height = pMonitorDetails[i].MonitorDetails.bottom - pMonitorDetails[i].MonitorDetails.top;
//			screen->bmInfo.biBitCount = 32;
//			screen->bmInfo.biHeight = height;
//			screen->bmInfo.biWidth = width;
//
//			screen->bmInfo.biSize = sizeof(BITMAPINFOHEADER);
//			screen->bmInfo.biPlanes = 1;
//			screen->bmInfo.biCompression = BI_RGB;
//			screen->bmInfo.biSizeImage = 0;
//			screen->bmInfo.biXPelsPerMeter = 0;
//			screen->bmInfo.biYPelsPerMeter = 0;
//			screen->bmInfo.biClrUsed = 0;
//			screen->bmInfo.biClrImportant = 0;
//			screenData.push_back(screen);
//			struct MonitorScreenBuffer *pMonitorScreenBuffer = new struct MonitorScreenBuffer;
//			pMonitorScreenBuffer->sizeOfBuffer = 4L * height * width;
//			pMonitorScreenBuffer->pBuffer = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMonitorScreenBuffer->sizeOfBuffer);
//			pMonitorDetails[i].pMonitorBuffer = pMonitorScreenBuffer;
//
//		}
//
//	}
//}

void VideoEditorScreen::SetRecordBufferManager(RecordBufferManager *pRecBuffer)
{
	pRecordBufferManager = pRecBuffer;
}

