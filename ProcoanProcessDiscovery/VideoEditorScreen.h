#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/event.h>
#include <wx/toolbar.h>
#include "SliderPanel.h"
#include "PicControlPanel.h"
#include "IPlayerWindow.h"
#include "IBPMNControl.h"
#include "MMHeader.h"
#include <wx/print.h>

class PictureDisplay;
class ProcoanButton;
class PicButtonControlPanel;
class BPMNControl;
class AudioPlayerBuffer;
class RecordBufferManager;
class BPMNControlPanel;
class ActionTabPanel;
class InfoTabPanel;
class MainToolBarPanel;
class ScenarioFacade;
class ScenarioDefintion;

class VideoEditorScreen : public wxFrame, public IPlayerWindow
{
	wxPrintDialogData g_printDialogData;

	wxWindow *m_pParent;
	wxMenuBar *m_pMenuBar;
	wxMenu *m_pFileMenu;
	wxMenu *m_pHelpMenu;
	wxMenu *m_pViewMenu;
	wxMenu *m_pToolBarPositionMenu;
	wxMenu *m_pVideoWindowPositionMenu;
	wxMenu *m_pDisplayTabWindowMenu;

	wxPoint mainVideoPanelPos;
	wxSize  mainVideoPanelSize;
	
	wxPoint bpmnPanelPos;
	wxSize bpmnPanelSize;

	wxPoint tabPanelPos;
	wxSize tabPanelSize;

	wxPanel *framePanel;
	// BPMN Panel Elements
	BPMNControlPanel *bpmnPanel;

	// Tab Panel Elements
	wxPanel *tabPanel;
	wxNotebook *editMainTab;
	InfoTabPanel *infoTab;
	ActionTabPanel* mainEditTab;
	//wxTextCtrl *pNameText;
	//wxTextCtrl *pDescText;
	//wxChoice *pActivityGroup;
	//wxChoice *pActivitySubGroup;
	//wxChoice *pActivityType;
	//wxChoice *pActivityAction;
	//wxButton *pSaveButton;

	// Video Panel Elements;
	wxPanel *videoPanel;
	wxNotebook *videoMainTab;
	wxPanel* mainTab;
	PictureDisplay* mainPicturePanel;
	PicControlPanel * picControlPanel;
	wxBitmapButton *recorderButton;

	PicButtonControlPanel* toolControlPanel;
	MainToolBarPanel *mainToolBarPanel;

	bool	bPlayerMode;
	ULONG	currentEventDisplayed;

	// IPlayerWindow Vars
	bool	bStartPlay;
	bool	bFirst;
	bool	bMainFileOpen;
	bool	bExitThreads;
	bool bActiveMonitorDisplay;
	bool	bShowSubTitle;
	IBPMNControl *pBPMNControl;
	PlayerDataFileManager *pDataRecorder;
	RecordBufferManager *pRecordBufferManager;
	int fps;
	int numMonitors;
	bool bVideoThreadWaiting;
	bool bDisplayThreadWaiting;
	//vector <struct ScreenData *> screenData;
	//struct MonitorScreenData *pMonitorDetails;
	BPMNElement *pCurrentEdited;
	BPMNElement *pCurrentEventDisplay;
	string	displayAnnotationString;
	string  displayAnnotationNotes;
	string  displayAnnotationBottomString;

	void ResizeWindow(wxSize szFrameSize);
	void ReArrangeForm();

	void CreateMainMenuBar();

	int toolBarPosition;
	bool bVideoPanelLeft;
	bool bDisplayTabWindow;

	void GetPanelSizes();
	void CalculatePanelLandscapeDimensions();
	void CalculatePanelPortraitDimensions();
	//struct ScreenData *GetScreenData(int monNum);
	void SetButtonStatus();
	void UpdateTabFields();
	void InitializeAnnotationString(BPMNElement *pElement);
	string GetControlTypeName(int nType);

public:
	void OnOpenFile(wxCommandEvent &event);
	void OnOpenReadOnly(wxCommandEvent &event);
	void OnToolBarBottom(wxCommandEvent &event);
	void OnToolBarTop(wxCommandEvent &event);
	void OnToolBarLeft(wxCommandEvent &event);
	void OnToolBarRight(wxCommandEvent &event);
	void OnFileChanged(wxNotebookEvent &event);
	void OnFileClose(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnVideoPaneLeft(wxCommandEvent &event);
	void OnVideoPaneRight(wxCommandEvent &event);
	void OnSwitchPanel(wxCommandEvent &event);
	void OnAbout(wxCommandEvent& event);
	void OnDisplayTab(wxCommandEvent &event);
	void OnDisplayTabOn(wxCommandEvent &event);
	void OnDisplayTabOff(wxCommandEvent &event);
	void OnEditorResize(wxSizeEvent& event);
	void OnSliderHover(wxCommandEvent& event);
	void OnSliderHoverClear(wxCommandEvent& event);
	void OnSliderUpdateTime(wxCommandEvent& event);
	void OnMoveAbsolute(wxCommandEvent& event);
	void OnFastRewind(wxCommandEvent& event);
	void OnSlowRewind(wxCommandEvent& event);
	void OnFastForward(wxCommandEvent& event);
	void OnSlowForward(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void OnPlay(wxCommandEvent& event);
	void OnAddActivity(wxCommandEvent& event);
	void OnStartSegment(wxCommandEvent& event);
	void OnEndSegment(wxCommandEvent& event);
	void OnCopySegment(wxCommandEvent& event);
	void OnCutSegment(wxCommandEvent& event);
	void OnPasteSegment(wxCommandEvent& event);
	void OnClearSegment(wxCommandEvent& event);
	void OnBranchSegment(wxCommandEvent& event);
	void OnActivityGroup(wxCommandEvent& event);
	void OnActivitySubGroup(wxCommandEvent& event);
	void OnActivitySave(wxCommandEvent& event);
	void OnUpdateTabFields(wxCommandEvent& event);
	void OnUpdateBPMNControl(wxCommandEvent& event);
	
	void OnPreview(wxCommandEvent& event);
	void OnReportSOP(wxCommandEvent& event);
	void OnReportPDF(wxCommandEvent& event);
	void OnShowSubTitle(wxCommandEvent& event);
	void OnManageScenario(wxCommandEvent& event);
	void OnRecordingInfo(wxCommandEvent& event);
	void OnMultiMonitor(wxCommandEvent& event);
	void OnActiveMonitor(wxCommandEvent& event);

	void OnBPMNBranchBottom(wxCommandEvent& event);
	void OnBPMNBranchRight(wxCommandEvent& event);
	void OnBPMNCreateSubProcess(wxCommandEvent& event);
	void OnBPMNDeleteBranch(wxCommandEvent& event);
	void OnBPMNDeleteEvent(wxCommandEvent& event);
	void OnBPMNLevelUp(wxCommandEvent& event);
	void OnBPMNMoveProcessBelow(wxCommandEvent& event);
	void OnBPMNMoveSubProcessAbove(wxCommandEvent& event);
	void OnBPMNMoveSubProcessBelow(wxCommandEvent& event);
	void OnBPMNSyncVideo(wxCommandEvent& event);
	void OnScenarioChanged(wxCommandEvent& event);
	void OnScenarioPlay(wxCommandEvent& event);

	void SaveOrGetScenario(bool bSave, ScenarioDefintion *pScenario);
	void SaveScenarioDefinitions(ScenarioFacade *pScenarioFacade);
	ULONG GetThumbTime();
	// Interface methods
	void SetSliderPosition(long position);
	void StopPlay();
	void SetExitFlag(bool flg);
	int GetWindowWidth();
	int GetWindowHeight();
	bool GetActiveMonitorDisplayFlag();
	void SetDataManager(PlayerDataFileManager *pRecorder);
	long VideoSegmentStartTime();
	void ResetSegmentMarker();
	long VideoSegmentEndTime();
	void GetPointOffset(POINT *pPoint);
	void SetPlayerEndTime(ULONG endTime);
	void SetPlayerTimeWindow();
	void SetDiagram(list<BPMNElement *> *pDiagram);
	void SetEventMap(map<ULONG, struct eventListElement *> *pEventMap);
	void InsertEvent(ULONG nEventID, long long timestamp);
	bool IsVideoThreadWaiting();
	void SetStartFlag(bool bFlg);
	void StartPlay(long long fPos);
	void StartDisplay();
	long GetSliderPosition();
	void DisplayImage();
	void BPMNElementSelected(struct cellDef *pCell);
	void SetFramesPerSecond(int fpsCount);
	int GetFramesPerSecond();
	bool GetSegInProgress();
	bool GetSegPresent();
	ULONG GetSegStartPos();
	ULONG GetSegEndPos();
	void SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal);
	void SetTabFields(struct cellDef *pSelected);
	void SetTabFields(struct eventListElement *pEventListElement);


	//void GetMonitorDetails(int noMonitors, long long fPosMonitorDetails);
	void SetRecordBufferManager(RecordBufferManager *pRecBuffer);
	void OnClose(wxCloseEvent & event);
	VideoEditorScreen(wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & position, const wxSize & size, bool bFlg);
	void DrawVideoPanel(wxPanel *parent, int width, int height, int xPos, int yPos);
	void DrawBPMNPanel(wxPanel *parent, int width,int height,  int xPos, int yPos);
	void DrawEditTabPanel(wxPanel *parent, int width, int height, int xPos, int yPos);
	
	void OpenFileInPlayer(wxString fileName);
	~VideoEditorScreen();
private:
	wxDECLARE_EVENT_TABLE();
};

enum MenuControls {
	wxID_VIEW = wxID_HIGHEST + 1,
	wxID_TOOLBAR_BOTTOM,
	wxID_TOOLBAR_TOP,
	wxID_TOOLBAR_LEFT,
	wxID_TOOLBAR_RIGHT,
	wxID_VIDEOPOSITION_LEFT,
	wxID_VIDEOPOSITION_RIGHT,
	wxID_DISPLAYTAB_ON,
	wxID_DISPLAYTAB_OFF,
	wxID_DISPLAY_ABOUT
};