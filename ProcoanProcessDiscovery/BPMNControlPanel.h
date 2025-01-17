#pragma once
#include <wx/wx.h>
#include <list>
#include "IBPMNControl.h"
#include "HorizScrollBarPanel.h"
#include "VertScrollBarPanel.h"

using namespace std;

class BPMNElement;
class BPMNDiagram;
class BPMNDrawPanel;
class MainToolBarPanel;

class BPMNControlPanel : public wxPanel, public IBPMNControl
{
	wxPanel *pParentWnd;
	BPMNDrawPanel *bpmnDrawPanel;
	MainToolBarPanel *pToolBarPanel;

	HorizScrollBarPanel *pHorizScroll;
	VertScrollBarPanel *pVertScroll;

	list <BPMNElement *> *pMainList;
	PlayerDataFileManager *pPlayerManager;
	list <BPMNElement *> *pBPMNDiagram;
	BPMNDiagram *pDiagramMap;
	int drawingHeight;
	int drawingWidth;
	map <ULONG, struct eventListElement *> *pEventMap;
	int currentEventNumber;



private:
	list <BPMNElement *> *GetElementList(BPMNElement *pElement);
	void CreateNewDiagram(list <BPMNElement *> *pDiagram); 
	void ShowElement(ULONG nEventNum);
	void ShowElement();
	BPMNElement *InsertElementInList(ULONG nEventID, long long timestamp, list <BPMNElement *> *pElementList, BPMNElement *pElement, int type, bool insAbove);
	BPMNElement *InsertElementInList(ULONG nEventID, list <BPMNElement *> *pElementList, BPMNElement *pElement, int type, bool insAbove);
	void GetControlDimensions();
	void SetDrawingScrollBars();
	void AddNewEventRecord(struct eventListElement *pEventListElement);
	int GetDrawingHeight();
	int GetDrawingWidth();
	int GetHorizontalOffset();
	int GetVerticalOffset();
	void UpdateBPMNControlWindow();
	void DeleteSelectedBranch(BPMNElement *pBranch);
public:
	BPMNControlPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size);
	~BPMNControlPanel();

	void CreateSubProcess();
	BPMNElement *RemoveElementsFromList(list <BPMNElement *> *pElementList, list <BPMNElement *> selectedList);
	BPMNElement *DeleteElementsFromList(list <BPMNElement *> *pElementList, list <BPMNElement *> selectedList);

	void AddToSubProcessAbove();
	void AddToSubProcessBelow();
	void UpdateSourceAndDestinationParentRecords(BPMNElement *pSource, BPMNElement *pDest);
	void GoUpDiagramLevel();
	void MoveItemsLevelUp();
	void DeleteItems();
	void SynchronizeVideo();
	void BranchDelete();
	void BranchCopy();
	void BranchRight();
	void BranchBottom();
	void GetPointScrollOffset(POINT *ptOffset);
	BPMNDiagram *GetDiagram();
	// IBPM Control Interface Methods
	bool ShowEventInWindow(ULONG nEventNum);
	void SetCurrentSyncronizedElement(BPMNElement *pElement, bool bSegOn);
	void SetCurrentSyncronizedElement(ULONG nEventID, bool bSegOn);
	void ClearSegmentElements();
	void InsertItem(ULONG nEventID, long long timestamp);
	void SetAnnotationDetails(string strAnnotation, string strNotes);
	void SetSubProcessDetails(string strName, string strDescription);
	void SetController(PlayerDataFileManager *pManager);
	void SetMainList(list <BPMNElement *> *pElementList);
	void CreateAndDisplayDiagram(list <BPMNElement *> *pDiagram);
	void SetEventMap(map <ULONG, struct eventListElement *> *pMap);
	void GetPointOffset(POINT *ptOffset);
	bool IsSubProcessSelected(int xPos, int yPos);
	void ShowSubProcessMap();
	bool MultiSelectBPMNElement(int xPos, int yPos);
	bool SelectBPMNElement(int xPos, int yPos);
	void OnVerticalScroll(wxCommandEvent& event);;
	void OnHorizontalScroll(wxCommandEvent& event);
	void PositionControls();
	void RefreshDiagram();
	bool IsSyncElementDisplayed();
	void SetMainToolbarPanel(MainToolBarPanel *pPanel);
	void EnableCreateProcessButton(bool bFlg);
	void EnableMoveToSubProcessAboveButton(bool bFlg);
	void EnableMoveToSubProcessBelowButton(bool bFlg);
	void EnableMoveToProcessAboveButton(bool bFlg);
	void EnableGoUpLevelButton(bool bFlg);
	void EnableDeleteBranchButton(bool bFlg);
	void EnableDeleteEventButton(bool bFlg);
	void EnableSyncVideoButton(bool bFlg);
	void EnableBranchBottomButton(bool bFlg);
	void EnableBranchRightButton(bool bFlg);
	void UpdateBPMNWindow();
	void ResetDiagram();
	void SetCopySegmentFlag(bool bFlg);
	DECLARE_EVENT_TABLE();


};

