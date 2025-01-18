#include "stdafx.h"
#include "MainToolBarPanel.h"
#include "ToolbarButtons.h"
#include "ProcoanButton.h"
#include "CustomEvents.h"

MainToolBarPanel::MainToolBarPanel(wxFrame *parent, DWORD id, wxPoint pt, wxSize size, bool bFlg) :
	wxPanel(parent, -1, pt, size, wxNO_BORDER)
{
	pParent = parent;
	controlID = id;

	pOpenPrimary = NULL;
	pOpenSecondary = NULL;
	pCloseFile = NULL;
	pSwitchPanel = NULL;
	pShowTab = NULL;
	pToolBarLeft = NULL;
	pToolBarTop = NULL;
	pToolBarRight = NULL;
	pToolBarBottom = NULL;
	pShowSubTitle = NULL;
	pShowDescription = NULL;
	pZoomDisplayForEvents = NULL;
	pRecorderDetails = NULL;
	pShowMultiMonitor = NULL;
	pShowActiveMonitor = NULL;
	pManageScenarios = NULL;
	pScenarioChoice = NULL;
	pPlayScenario = NULL;


	bMultiMonitors = false;
	bPlayerMode = bFlg;
	pOpenPrimary = DBG_NEW ProcoanButton(
			this, TOOLBAR_OPEN_PRIMARY, wxPoint(5, 2),
			wxSize(30, 30));
	SetButtonBitmaps(pOpenPrimary, openPrimaryNormal_xpm, openPrimaryDisabled_xpm, openPrimaryMouseOver_xpm, openPrimaryPressed_xpm);
	SetSmallButtonBitmaps(pOpenPrimary, openPrimaryNormal_xpm, openPrimaryDisabled_xpm, openPrimaryMouseOver_xpm, openPrimaryPressed_xpm);
	pOpenPrimary->SetToolTip(wxT("Open Primary File"));

	if (!bPlayerMode)
	{
		pOpenSecondary = DBG_NEW ProcoanButton(
			this, TOOLBAR_OPEN_SECONDARY, wxPoint(40, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pOpenSecondary, openSecondaryNormal_xpm, openSecondaryDisabled_xpm, openSecondaryMouseOver_xpm, openSecondaryPressed_xpm);
		SetSmallButtonBitmaps(pOpenSecondary, openSecondaryNormal_xpm, openSecondaryDisabled_xpm, openSecondaryMouseOver_xpm, openSecondaryPressed_xpm);
		pOpenSecondary->SetToolTip(wxT("Open Secondary File"));
	}

	pCloseFile = DBG_NEW ProcoanButton(
		this, TOOLBAR_CLOSE_FILE, wxPoint(75, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pCloseFile, closeFileNormal_xpm, closeFileDisabled_xpm, closeFileMouseOver_xpm, closeFilePressed_xpm);
	SetSmallButtonBitmaps(pCloseFile, closeFileNormal_xpm, closeFileDisabled_xpm, closeFileMouseOver_xpm, closeFilePressed_xpm);
	pCloseFile->SetToolTip(wxT("Close File"));


	if (!bPlayerMode)
	{
		pShowTab = DBG_NEW ProcoanButton(
			this, TOOLBAR_SHOW_TAB, wxPoint(110, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pShowTab, showTabNormal_xpm, showTabDisabled_xpm, showTabMouseOver_xpm, showTabPressed_xpm);
		SetSmallButtonBitmaps(pShowTab, showTabNormal_xpm, showTabDisabled_xpm, showTabMouseOver_xpm, showTabPressed_xpm);
		pShowTab->SetToolTip(wxT("Show Activity Tab"));
	}
	pToolBarLeft = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_LEFT, wxPoint(145, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pToolBarLeft, toolBarLeftNormal_xpm, toolBarLeftDisabled_xpm, toolBarLeftMouseOver_xpm, toolBarLeftPressed_xpm);
	SetSmallButtonBitmaps(pToolBarLeft, toolBarLeftNormal_xpm, toolBarLeftDisabled_xpm, toolBarLeftMouseOver_xpm, toolBarLeftPressed_xpm);
	pToolBarLeft->SetToolTip(wxT("Show Toolbar At Left"));

	pToolBarTop = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_TOP, wxPoint(180, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pToolBarTop, toolBarTopNormal_xpm, toolBarTopDisabled_xpm, toolBarTopMouseOver_xpm, toolBarTopPressed_xpm);
	SetSmallButtonBitmaps(pToolBarTop, toolBarTopNormal_xpm, toolBarTopDisabled_xpm, toolBarTopMouseOver_xpm, toolBarTopPressed_xpm);
	pToolBarTop->SetToolTip(wxT("Show Toolbar At Top"));

	pToolBarRight = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_RIGHT, wxPoint(215, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pToolBarRight, toolBarRightNormal_xpm, toolBarRightDisabled_xpm, toolBarRightMouseOver_xpm, toolBarRightPressed_xpm);
	SetSmallButtonBitmaps(pToolBarRight, toolBarRightNormal_xpm, toolBarRightDisabled_xpm, toolBarRightMouseOver_xpm, toolBarRightPressed_xpm);
	pToolBarRight->SetToolTip(wxT("Show Toolbar At Right"));

	pToolBarBottom = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_BOTTOM, wxPoint(250, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pToolBarBottom, toolBarBottomNormal_xpm, toolBarBottomDisabled_xpm, toolBarBottomMouseOver_xpm, toolBarBottomPressed_xpm);
	SetSmallButtonBitmaps(pToolBarBottom, toolBarBottomNormal_xpm, toolBarBottomDisabled_xpm, toolBarBottomMouseOver_xpm, toolBarBottomPressed_xpm);
	pToolBarBottom->SetToolTip(wxT("Show Toolbar At Bottom"));

	pShowSubTitle = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_SUBTITLE, wxPoint(285, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pShowSubTitle, showSubTitleNormal_xpm, showSubTitleDisabled_xpm, showSubTitleMouseOver_xpm, showSubTitlePressed_xpm);
	SetSmallButtonBitmaps(pShowSubTitle, showSubTitleNormal_xpm, showSubTitleDisabled_xpm, showSubTitleMouseOver_xpm, showSubTitlePressed_xpm);
	pShowSubTitle->SetToolTip(wxT("Show Activity Sub-Titles"));

	//pShowDescription = DBG_NEW ProcoanButton(
	//	this, TOOLBAR_SHOW_DESCRIPTION, wxPoint(320, 2),
	//	wxSize(30, 30));
	//SetButtonBitmaps(pShowDescription, showDescriptionNormal_xpm, showDescriptionDisabled_xpm, showDescriptionMouseOver_xpm, showDescriptionPressed_xpm);
	//SetSmallButtonBitmaps(pShowDescription, showDescriptionNormal_xpm, showDescriptionDisabled_xpm, showDescriptionMouseOver_xpm, showDescriptionPressed_xpm);
	//pShowDescription->SetToolTip(wxT("Show Activity Description"));

	//pZoomDisplayForEvents = DBG_NEW ProcoanButton(
	//	this, TOOLBAR_SHOW_ZOOM, wxPoint(355, 2),
	//	wxSize(30, 30));
	//SetButtonBitmaps(pZoomDisplayForEvents, zoomEventsNormal_xpm, zoomEventsDisabled_xpm, zoomEventsMouseOver_xpm, zoomEventsPressed_xpm);
	//SetSmallButtonBitmaps(pZoomDisplayForEvents, zoomEventsNormal_xpm, zoomEventsDisabled_xpm, zoomEventsMouseOver_xpm, zoomEventsPressed_xpm);
	//pZoomDisplayForEvents->SetToolTip(wxT("Zoom on Activity"));

	pSOPReport = DBG_NEW ProcoanButton(
		this, TOOLBAR_REPORT_SOP, wxPoint(390, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pSOPReport, reportNormal_xpm, reportDisabled_xpm, reportMouseOver_xpm, reportPressed_xpm);
	SetSmallButtonBitmaps(pSOPReport, reportNormal_xpm, reportDisabled_xpm, reportMouseOver_xpm, reportPressed_xpm);
	pSOPReport->SetToolTip(wxT("SOP Report"));


	pSwitchPanel = DBG_NEW ProcoanButton(
		this, TOOLBAR_SWITCH_PANEL, wxPoint(425, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pSwitchPanel, switchPanelNormal_xpm, switchPanelDisabled_xpm, switchPanelMouseOver_xpm, switchPanelPressed_xpm);
	SetSmallButtonBitmaps(pSwitchPanel, switchPanelNormal_xpm, switchPanelDisabled_xpm, switchPanelMouseOver_xpm, switchPanelPressed_xpm);
	pSwitchPanel->SetToolTip(wxT("Switch View Panel"));

	pManageScenarios = DBG_NEW ProcoanButton(
		this, TOOLBAR_MANAGE_SCENARIO, wxPoint(460, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pManageScenarios, manageScenarioNormal_xpm, manageScenarioDisabled_xpm, manageScenarioMouseOver_xpm, manageScenarioPressed_xpm);
	SetSmallButtonBitmaps(pManageScenarios, manageScenarioNormal_xpm, manageScenarioDisabled_xpm, manageScenarioMouseOver_xpm, manageScenarioPressed_xpm);
	pManageScenarios->SetToolTip(wxT("Manage Scenarios"));

	if (!bPlayerMode)
	{
		pRecorderDetails = DBG_NEW ProcoanButton(
			this, TOOLBAR_RECORDING_INFO, wxPoint(495, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pRecorderDetails, recInfoNormal_xpm, recInfoDisabled_xpm, recInfoMouseOver_xpm, recInfoPressed_xpm);
		SetSmallButtonBitmaps(pRecorderDetails, recInfoNormal_xpm, recInfoDisabled_xpm, recInfoMouseOver_xpm, recInfoPressed_xpm);
		pRecorderDetails->SetToolTip(wxT("Update Recording Info"));
	}

	pShowMultiMonitor = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_MULTI_MONITOR, wxPoint(530, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pShowMultiMonitor, multiMonitorNormal_xpm, multiMonitorDisabled_xpm, multiMonitorMouseOver_xpm, multiMonitorPressed_xpm);
	SetSmallButtonBitmaps(pShowMultiMonitor, multiMonitorNormal_xpm, multiMonitorDisabled_xpm, multiMonitorMouseOver_xpm, multiMonitorPressed_xpm);
	pShowMultiMonitor->SetToolTip(wxT("Show Multi-Monitor"));

	pShowActiveMonitor = DBG_NEW ProcoanButton(
		this, TOOLBAR_SHOW_ACTIVE_MONITOR, wxPoint(565, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pShowActiveMonitor, activeMonitorNormal_xpm, activeMonitorDisabled_xpm, activeMonitorMouseOver_xpm, activeMonitorPressed_xpm);
	SetSmallButtonBitmaps(pShowActiveMonitor, activeMonitorNormal_xpm, activeMonitorDisabled_xpm, activeMonitorMouseOver_xpm, activeMonitorPressed_xpm);
	pShowActiveMonitor->SetToolTip(wxT("Show Active Monitor"));
	wxArrayString scenarioChoice;
	scenarioChoice.clear();
	scenarioChoice.Add(wxT("Default"));
	pScenarioChoice = DBG_NEW wxChoice(this, TOOLBAR_SCENARIO_CHOICE, wxPoint(600, 7), wxSize(200, 30), scenarioChoice);
	
	pPlayScenario  = DBG_NEW ProcoanButton(
		this, TOOLBAR_PLAY_SCENARIO, wxPoint(805, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pPlayScenario, playScenarioNormal_xpm, playScenarioDisabled_xpm, playScenarioMouseOver_xpm, playScenarioPressed_xpm);
	SetSmallButtonBitmaps(pPlayScenario, playScenarioNormal_xpm, playScenarioDisabled_xpm, playScenarioMouseOver_xpm, playScenarioPressed_xpm);
	pPlayScenario->SetToolTip(wxT("Play Scenario"));



	// BPMN Control Toolbar
	pBranchBottom = DBG_NEW ProcoanButton(
		this, TOOLBAR_BPMN_BRANCH_BOTTOM, wxPoint(450, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pBranchBottom, branchBottomNormal_xpm, branchBottomDisabled_xpm, branchBottomMouseOver_xpm, branchBottomPressed_xpm);
	SetSmallButtonBitmaps(pBranchBottom, branchBottomNormal_xpm, branchBottomDisabled_xpm, branchBottomMouseOver_xpm, branchBottomPressed_xpm);
	pBranchBottom->SetToolTip(wxT("Branch Bottom Active"));

	pBranchRight = DBG_NEW ProcoanButton(
		this, TOOLBAR_BPMN_BRANCH_RIGHT, wxPoint(485, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pBranchRight, branchRightNormal_xpm, branchRightDisabled_xpm, branchRightMouseOver_xpm, branchRightPressed_xpm);
	SetSmallButtonBitmaps(pBranchRight, branchRightNormal_xpm, branchRightDisabled_xpm, branchRightMouseOver_xpm, branchRightPressed_xpm);
	pBranchRight->SetToolTip(wxT("Branch Right Active"));


	if (!bPlayerMode)
	{
		pCreateSubProcess = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_CREATE_SUBPROCESS, wxPoint(520, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pCreateSubProcess, createSubProcessNormal_xpm, createSubProcessDisabled_xpm, createSubProcessMouseOver_xpm, createSubProcessPressed_xpm);
		SetSmallButtonBitmaps(pCreateSubProcess, createSubProcessNormal_xpm, createSubProcessDisabled_xpm, createSubProcessMouseOver_xpm, createSubProcessPressed_xpm);
		pCreateSubProcess->SetToolTip(wxT("Create Sub-Process"));

		pDeleteBranch = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_DELETE_BRANCH, wxPoint(555, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pDeleteBranch, deleteBranchNormal_xpm, deleteBranchDisabled_xpm, deleteBranchMouseOver_xpm, deleteBranchPressed_xpm);
		SetSmallButtonBitmaps(pDeleteBranch, deleteBranchNormal_xpm, deleteBranchDisabled_xpm, deleteBranchMouseOver_xpm, deleteBranchPressed_xpm);
		pDeleteBranch->SetToolTip(wxT("Delete Branch"));

		pDeleteEvent = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_DELETE_EVENT, wxPoint(590, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pDeleteEvent, deleteEventNormal_xpm, deleteEventDisabled_xpm, deleteEventMouseOver_xpm, deleteEventPressed_xpm);
		SetSmallButtonBitmaps(pDeleteEvent, deleteEventNormal_xpm, deleteEventDisabled_xpm, deleteEventMouseOver_xpm, deleteEventPressed_xpm);
		pDeleteEvent->SetToolTip(wxT("Delete Activity"));

		pUpLevel = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_UP_LEVEL, wxPoint(625, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pUpLevel, goUpLevelNormal_xpm, goUpLevelDisabled_xpm, goUpLevelMouseOver_xpm, goUpLevelPressed_xpm);
		SetSmallButtonBitmaps(pUpLevel, goUpLevelNormal_xpm, goUpLevelDisabled_xpm, goUpLevelMouseOver_xpm, goUpLevelPressed_xpm);
		pUpLevel->SetToolTip(wxT("Move Level Up"));

		pMoveProcessBelow = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_MOVE_PROCESS_BELOW, wxPoint(660, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pMoveProcessBelow, moveProcessBelowNormal_xpm, moveProcessBelowDisabled_xpm, moveProcessBelowMouseOver_xpm, moveProcessBelowPressed_xpm);
		SetSmallButtonBitmaps(pMoveProcessBelow, moveProcessBelowNormal_xpm, moveProcessBelowDisabled_xpm, moveProcessBelowMouseOver_xpm, moveProcessBelowPressed_xpm);
		pMoveProcessBelow->SetToolTip(wxT("Move To Process Below"));

		pMoveSubProcessAbove = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_MOVE_SUBPROCESS_ABOVE, wxPoint(695, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pMoveSubProcessAbove, moveSubProcessAboveNormal_xpm, moveSubProcessAboveDisabled_xpm, moveSubProcessAboveMouseOver_xpm, moveSubProcessAbovePressed_xpm);
		SetSmallButtonBitmaps(pMoveSubProcessAbove, moveSubProcessAboveNormal_xpm, moveSubProcessAboveDisabled_xpm, moveSubProcessAboveMouseOver_xpm, moveSubProcessAbovePressed_xpm);
		pMoveSubProcessAbove->SetToolTip(wxT("Move To Sub-Process Above"));

		pMoveSubProcessBelow = DBG_NEW ProcoanButton(
			this, TOOLBAR_BPMN_MOVE_SUBPROCESS_BELOW, wxPoint(730, 2),
			wxSize(30, 30));
		SetButtonBitmaps(pMoveSubProcessBelow, moveSubProcessBelowNormal_xpm, moveSubProcessBelowDisabled_xpm, moveSubProcessBelowMouseOver_xpm, moveSubProcessBelowPressed_xpm);
		SetSmallButtonBitmaps(pMoveSubProcessBelow, moveSubProcessBelowNormal_xpm, moveSubProcessBelowDisabled_xpm, moveSubProcessBelowMouseOver_xpm, moveSubProcessBelowPressed_xpm);
		pMoveSubProcessBelow->SetToolTip(wxT("Move To Sub-Process Below"));
	}
	pSyncVideo = DBG_NEW ProcoanButton(
		this, TOOLBAR_BPMN_SYNC_VIDEO, wxPoint(765, 2),
		wxSize(30, 30));
	SetButtonBitmaps(pSyncVideo, syncVideoNormal_xpm, syncVideoDisabled_xpm, syncVideoMouseOver_xpm, syncVideoPressed_xpm);
	SetSmallButtonBitmaps(pSyncVideo, syncVideoNormal_xpm, syncVideoDisabled_xpm, syncVideoMouseOver_xpm, syncVideoPressed_xpm);
	pSyncVideo->SetToolTip(wxT("Sync Video"));



	EnableOpenPrimary(true);
	EnableOpenSecondary(false);
	EnableCloseFile(false);
	EnableSwitchPanel(false);
	EnableShowTab(false);
	EnableToolBarLeft(false);
	EnableToolBarTop(false);
	EnableToolBarRight(false);
	EnableToolBarBottom(false);
	EnableShowSubTitle(false);
	EnableShowDescription(false);
	EnableZoomOnEvents(false);
	EnableReportSOP(false);
	//EnableRecordingInfo(false);
	EnableManageScenario(false);
	EnableShowMultiMonitor(false);
	EnableShowActiveMonitor(false);

	// BPMN Control Toolbar
	EnableBranchBottom(false);
	EnableBranchRight(false);
	EnableCreateSubProcess(false);
	EnableDeleteBranch(false);
	EnableDeleteEvent(false);
	EnableUpLevel(false);
	EnableMoveProcessBelow(false);
	EnableMoveSubProcessAbove(false);
	EnableMoveSubProcessBelow(false);
	EnableSyncVideo(false);


}



MainToolBarPanel::~MainToolBarPanel()
{
}


string MainToolBarPanel::GetSelectedScenarioName()
{
	int nSel = pScenarioChoice->GetSelection();
	if (nSel != -1)
		return string(pScenarioChoice->GetString(nSel).mbc_str());
	return "";
}

void MainToolBarPanel::SetScenarioName(string value)
{
	int nSel = pScenarioChoice->FindString(value.c_str());
	pScenarioChoice->Select(nSel);
}

void MainToolBarPanel::SetShowSubTitleTip(bool bFlg)
{
	if (bFlg)
		pShowSubTitle->SetToolTip(wxT("Disable Activity Sub-Titles"));
	else
		pShowSubTitle->SetToolTip(wxT("Show Activity Sub-Titles"));
}

void MainToolBarPanel::SetScenarioNames(vector <string> &names)
{
	wxArrayString strNames;

	strNames.Add(wxT("Default"));
	vector <string>::iterator itNames = names.begin();
	while (itNames != names.end())
	{
		string name = *itNames;

		strNames.Add(name.c_str());
		itNames++;
	}

	pScenarioChoice->Clear();
	pScenarioChoice->Append(strNames);
}

void MainToolBarPanel::SetButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed)
{
	if (btnObj == NULL)
		return;

	wxImage Image;

	if (xpmNormal != NULL)
	{
		btnObj->SetNormalBitmap(xpmNormal);
	}

	if (xpmDisabled != NULL)
	{
		btnObj->SetDisabledBitmap(xpmDisabled);
	}

	if (xpmFocussed != NULL)
	{
		btnObj->SetActiveBitmap(xpmFocussed);
	}

	if (xpmPressed != NULL)
	{
		btnObj->SetPressedBitmap(xpmPressed);
	}

}

void MainToolBarPanel::PositionControls(int vPos, int bPos)
{
	int val = 0;
	pOpenPrimary->SetButtonPosition(wxPoint(vPos, 2));
	val += 32;
	if (!bPlayerMode)
	{
		pOpenSecondary->Show();
		pOpenSecondary->SetButtonPosition(wxPoint(vPos + val, 2));
		val += 32;
	}
	else
	{
		if (pOpenSecondary != NULL)
			pOpenSecondary->Hide();
	}
	pCloseFile->SetButtonPosition(wxPoint(vPos + val, 2));
	val += 32;
	if (!bPlayerMode)
	{
		pShowTab->Show();
		pShowTab->SetButtonPosition(wxPoint(vPos + val, 2));
		val += 32;
	}
	else
	{
		if (pShowTab != NULL)
			pShowTab->Hide();
	}

	pToolBarLeft->SetButtonPosition(wxPoint(vPos + val, 2));
	val += 32;
	pToolBarTop->SetButtonPosition(wxPoint(vPos + val, 2));
	val += 32;
	pToolBarRight->SetButtonPosition(wxPoint(vPos + val, 2));
	val += 32;
	pToolBarBottom->SetButtonPosition(wxPoint(vPos+ val, 2));
	val += 32;
	pShowSubTitle->SetButtonPosition(wxPoint(vPos + val, 2));
	val += 32;
	//pShowDescription->SetButtonPosition(wxPoint(vPos + val, 2));
	//val += 32;
	//pZoomDisplayForEvents->SetButtonPosition(wxPoint(vPos + val, 2));
	//val += 32;
	pSOPReport->SetButtonPosition(wxPoint(vPos + val, 2));
	//if (vPos == 5)
	////////{
	//	val += 32;
	//	pSwitchPanel->SetButtonPosition(wxPoint(vPos + val, 2));
	//	val += 32;
	//	pManageScenarios->SetButtonPosition(wxPoint(vPos + val, 2));
	//	val += 32;
	//	if (!bPlayerMode)
	//	{
	//		pRecorderDetails->Show();
	//		pRecorderDetails->SetButtonPosition(wxPoint(vPos + val, 2));
	//		val += 32;
	//	}
	//	else
	//	{
	//		if (pRecorderDetails != NULL)
	//			pRecorderDetails->Hide();

	//	}
	//	
	//	pShowMultiMonitor->Show();
	//	pShowActiveMonitor->Show();
	//	pShowMultiMonitor->SetButtonPosition(wxPoint(vPos + val, 2));
	//	val += 32;
	//	pShowActiveMonitor->SetButtonPosition(wxPoint(vPos + val, 2));
	//	val += 32;
	//	if (bMultiMonitors)
	//	{
	//		pShowMultiMonitor->Enable(true);
	//		pShowActiveMonitor->Enable(true);
	//	}
	//	else
	//	{
	//		pShowMultiMonitor->Enable(false);
	//		pShowActiveMonitor->Enable(false);
	//	}
	//	pScenarioChoice->SetPosition(wxPoint(vPos + val, 7));
	//	val += 205;
	//	pPlayScenario->SetButtonPosition(wxPoint(vPos + val, 2));
	//}
	val = 0;
	pBranchBottom->SetButtonPosition(wxPoint(bPos, 2));
	val += 32;
	pBranchRight->SetButtonPosition(wxPoint(bPos + val, 2));
	val += 32;
	if (!bPlayerMode)
	{
		pCreateSubProcess->SetButtonPosition(wxPoint(bPos + val, 2));
		val += 32;
		pDeleteBranch->SetButtonPosition(wxPoint(bPos + val, 2));
		val += 32;
		pDeleteEvent->SetButtonPosition(wxPoint(bPos + val, 2));
		val += 32;
		pUpLevel->SetButtonPosition(wxPoint(bPos + val, 2));
		val += 32;
		pMoveProcessBelow->SetButtonPosition(wxPoint(bPos + val, 2));;
		val += 32;
		pMoveSubProcessAbove->SetButtonPosition(wxPoint(bPos + val, 2));
		val += 32;
		pMoveSubProcessBelow->SetButtonPosition(wxPoint(bPos + val, 2));
		val += 32;
	}
	pSyncVideo->SetButtonPosition(wxPoint(bPos + val, 2));
}

void MainToolBarPanel::SetSmallButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed)
{
	if (btnObj == NULL)
		return;

	wxImage Image;

	if (xpmNormal != NULL)
	{
		btnObj->SetNormalSmallBitmap(xpmNormal);
	}

	if (xpmDisabled != NULL)
	{
		btnObj->SetDisabledSmallBitmap(xpmDisabled);
	}

	if (xpmFocussed != NULL)
	{
		btnObj->SetActiveSmallBitmap(xpmFocussed);
	}

	if (xpmPressed != NULL)
	{
		btnObj->SetPressedSmallBitmap(xpmPressed);
	}

}


void MainToolBarPanel::EnableOpenPrimary(bool bFlg)
{
	pOpenPrimary->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableOpenSecondary(bool bFlg)
{
	if (!bPlayerMode)
		pOpenSecondary->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableCloseFile(bool bFlg)
{
	pCloseFile->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableSwitchPanel(bool bFlg)
{
	pSwitchPanel->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableShowTab(bool bFlg)
{
	if (!bPlayerMode)
		pShowTab->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableToolBarLeft(bool bFlg)
{
	pToolBarLeft->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableToolBarTop(bool bFlg)
{
	pToolBarTop->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableToolBarRight(bool bFlg)
{
	pToolBarRight->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableToolBarBottom(bool bFlg)
{
	pToolBarBottom->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableShowSubTitle(bool bFlg)
{
	pShowSubTitle->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableShowDescription(bool bFlg)
{
	//pShowDescription->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableZoomOnEvents(bool bFlg)
{
	//pZoomDisplayForEvents->ShowEnable(bFlg);
	;
}

void MainToolBarPanel::EnableReportSOP(bool bFlg)
{
	pSOPReport->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableRecordingInfo(bool bFlg)
{
	pRecorderDetails->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableManageScenario(bool bFlg)
{
	pManageScenarios->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableShowMultiMonitor(bool bFlg)
{
	//if (pShowMultiMonitor != NULL)
		pShowMultiMonitor->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableShowActiveMonitor(bool bFlg)
{
	//if (pShowActiveMonitor != NULL)
		pShowActiveMonitor->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableScenarioChoice(bool bFlg)
{
	pScenarioChoice->Enable(bFlg);
}

void MainToolBarPanel::EnablePlayScenario(bool bFlg)
{
	pPlayScenario->ShowEnable(bFlg);
}

// BPMN Control Toolbar
void MainToolBarPanel::EnableBranchBottom(bool bFlg)
{
	pBranchBottom->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableBranchRight(bool bFlg)
{
	pBranchRight->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableCreateSubProcess(bool bFlg)
{
	if (!bPlayerMode)
		pCreateSubProcess->ShowEnable(bFlg);

}

void MainToolBarPanel::EnableDeleteBranch(bool bFlg)
{
	if (!bPlayerMode)
		pDeleteBranch->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableDeleteEvent(bool bFlg)
{
	if (!bPlayerMode)
		pDeleteEvent->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableUpLevel(bool bFlg)
{
	if (!bPlayerMode)
		pUpLevel->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableMoveProcessBelow(bool bFlg)
{
	if (!bPlayerMode)
		pMoveProcessBelow->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableMoveSubProcessAbove(bool bFlg)
{
	if (!bPlayerMode)
		pMoveSubProcessAbove->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableMoveSubProcessBelow(bool bFlg)
{
	if (!bPlayerMode)
		pMoveSubProcessBelow->ShowEnable(bFlg);
}

void MainToolBarPanel::EnableSyncVideo(bool bFlg)
{
	pSyncVideo->ShowEnable(bFlg);
}

void MainToolBarPanel::SetMultiMonitors(bool bFlg)
{
	bMultiMonitors = bFlg;
}

void MainToolBarPanel::SetPlayerMode( bool bFlg)
{
	bPlayerMode = bFlg;
}

void MainToolBarPanel::SetActiveMonitorMode(bool bFlg)
{
	bActiveMonitorMode = bFlg;
	EnableShowActiveMonitor(!bFlg);
	EnableShowMultiMonitor(bFlg);
}