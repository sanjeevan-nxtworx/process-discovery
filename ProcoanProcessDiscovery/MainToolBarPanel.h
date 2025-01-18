#pragma once
#include <wx/wx.h>
#include <string>
#include <vector>
using namespace std;

class ProcoanButton;

class MainToolBarPanel
	: public wxPanel
{
	wxFrame *pParent;
	ULONG controlID;

	ProcoanButton *pOpenPrimary;
	ProcoanButton *pOpenSecondary;
	ProcoanButton *pCloseFile;
	ProcoanButton *pSwitchPanel;
	ProcoanButton *pShowTab;
	ProcoanButton *pToolBarLeft;
	ProcoanButton *pToolBarTop;
	ProcoanButton *pToolBarRight;
	ProcoanButton *pToolBarBottom;
	ProcoanButton *pShowSubTitle;
	ProcoanButton *pShowDescription;
	ProcoanButton *pZoomDisplayForEvents;
	ProcoanButton *pRecorderDetails;
	ProcoanButton *pShowMultiMonitor;
	ProcoanButton *pShowActiveMonitor;
	ProcoanButton *pManageScenarios;
	wxChoice	  *pScenarioChoice;
	ProcoanButton *pPlayScenario;

	// BPMN Control Toolbar
	ProcoanButton *pBranchBottom;
	ProcoanButton *pBranchRight;
	ProcoanButton *pCreateSubProcess;
	ProcoanButton *pDeleteBranch;
	ProcoanButton *pDeleteEvent;
	ProcoanButton *pUpLevel;
	ProcoanButton *pMoveProcessBelow;
	ProcoanButton *pMoveSubProcessAbove;
	ProcoanButton *pMoveSubProcessBelow;
	ProcoanButton *pSyncVideo;
	ProcoanButton *pSOPReport;

	bool bPlayerMode;
	bool bMultiMonitors;
	bool bActiveMonitorMode;

public:
	MainToolBarPanel(wxFrame *parent, DWORD id, wxPoint pt, wxSize size, bool bFlg);
	~MainToolBarPanel();
	void SetButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed);
	void SetSmallButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed);

	void EnableOpenPrimary(bool bFlg);
	void EnableOpenSecondary(bool bFlg);
	void EnableCloseFile(bool bFlg);
	void EnableSwitchPanel(bool bFlg);
	void EnableShowTab(bool bFlg);
	void EnableToolBarLeft(bool bFlg);
	void EnableToolBarTop(bool bFlg);
	void EnableToolBarRight(bool bFlg);
	void EnableToolBarBottom(bool bFlg);
	void EnableShowSubTitle(bool bFlg);
	void EnableShowDescription(bool bFlg);
	void EnableZoomOnEvents(bool bFlg);
	void EnableReportSOP(bool bFlg);
	void EnableRecordingInfo(bool bFlg);
	void EnableManageScenario(bool bFlg);
	void EnableShowMultiMonitor(bool bFlg);
	void EnableShowActiveMonitor(bool bFlg);
	void EnableScenarioChoice(bool bFlg);
	void EnablePlayScenario(bool bFlg);


	// BPMN Control Toolbar
	void EnableBranchBottom(bool bFlg);
	void EnableBranchRight(bool bFlg);
	void EnableCreateSubProcess(bool bFlg);
	void EnableDeleteBranch(bool bFlg);
	void EnableDeleteEvent(bool bFlg);
	void EnableUpLevel(bool bFlg);
	void EnableMoveProcessBelow(bool bFlg);
	void EnableMoveSubProcessAbove(bool bFlg);
	void EnableMoveSubProcessBelow(bool bFlg);
	void EnableSyncVideo(bool bFlg);
	void SetMultiMonitors(bool bFlg);
	void SetPlayerMode(bool bFlg);
	void PositionControls(int vPos, int bPos);
	void SetActiveMonitorMode(bool bFlg);
	void SetScenarioNames(vector <string> &nameArray);
	string GetSelectedScenarioName();
	void SetScenarioName(string value);
	void SetShowSubTitleTip(bool bFlg);
};

