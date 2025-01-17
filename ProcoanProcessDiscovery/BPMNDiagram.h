#pragma once

#include <vector>
#include <list>
#include <wx/wx.h>
using namespace std;

#define COLUMN_MARGIN 500
#define ROW_MARGIN 400

class BPMNElement;
class PlayerDataFileManager;
class BPMNSubProcess;
class BPMNDecision;
class LineConnectionInfo;

class BPMNDiagram
{
	vector <vector <struct cellDef *> *>diagram;
	vector <struct headerDef *> columnHeader;
	vector <struct headerDef *> rowHeader;
	vector <LineConnectionInfo *> notInPathList;
	vector <LineConnectionInfo *> inPathList;


	list <struct cellDef *> selectedList;
	struct cellDef *pLastSelected;
	BPMNElement *pSyncElement;

	int nCurColumn;
	int nCurRow;
	PlayerDataFileManager *pPlayerManager;
	void SelectLastSelected(struct cellDef *pSelected);
	void FindElementListRowAndColumn(list <BPMNElement *> *pElementList, int *row, int *col, int *maxCol);
	struct cellDef *SetElementsInDiagram(list <BPMNElement *> *pElementList, int *row, int *col, int *maxCol, struct cellDef *pCellPrev, list <cellDef *> *bBranchEndElements);
	void UpdateBranchCells(list <struct cellDef *> *pBranchElementList, int startCol);
	void UpdateBranchCellSizes(list <struct cellDef *> *pBranchElementList);
	void DrawConnectingLines(HDC hdc, LPRECT rectFrom, LPRECT rectTo, LPRECT wndRect);
	wxSize GetDiagramExtents(HDC hdc);
	void GetControlDimensions(list <BPMNElement *> *pElementList);
public:
	void SetSyncElement();
	void SetCurrentColumn(int nCol);
	void SetCurrentRow(int nRow);
	int GetCurrentColumn();
	int GetCurrentRow();
	bool IsSyncElementDisplayed();

	//void InsertNewColumn();

	bool MultiSelectBPMNElement(int xPos, int yPos);
	bool SelectBPMNElement(int xPos, int yPos, bool bTestFlg = true);
	bool IsSubProcessSelected(int xPos, int yPos);
	void AddColumn();
	void SetColumn(int colNum);
	void ResetDiagram();
	void AddRowToCurrentColumn(BPMNElement *pElement);
	int GetColumnRows(int colNum);
	int GetDrawingHeight();
	int GetDrawingWidth();
	void Clear();
	void FindDiagramDimensions(list <BPMNElement *> *pElementList);

	void DrawDiagram(HDC hdc, int width, int height, POINT offset);
	void DrawDiagramForPrint(HDC hdc, int width, int height, POINT offset);
	void CreateBPMNBitmap(vector<unsigned char>& buffer,  SIZE *szBmp);
	void GetEventBitmap(vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventID, vector <BPMNElement *> &pElementList);

	bool IsElementInWindow(BPMNElement *pElement, LPRECT pWindowRect, LPRECT pElementRect);
	bool IsElementPartialInWindow(BPMNElement *pElement, LPRECT pWindowRect, LPRECT pElementRect);
	void SetDrawPosition();

	struct cellDef *FindCell(int xPos, int yPos);

	struct cellDef *FindCellInDiagram(int col, int row);
	struct cellDef *FindEventInDiagram(ULONG nEventNum);

	void SetController(PlayerDataFileManager *pManager);
	bool SetAnnotationDetails(string strAnnotation, string strAnnotationNotes);
	bool SetSubProcessDetails(string strName, string strDescription);
	bool CheckIfSelectedInSequence();
	list <BPMNElement *>GetSelectedItems();
	void ClearSelectedList();
	void DeleteCell(struct cellDef *pCell);
	BPMNSubProcess *GetSubProcessSelected();
	struct cellDef *GetSubProcessToAdd(int direction);
	void SetSelectedCell(struct cellDef *pCell);
	bool SelectedListIsAtTop();
	bool SelectedListIsAtBottom();
	void GetSubProcessesInDiagram(list <BPMNElement *> *pSubProcesses);
	void SetCurrentSyncronizedElement(BPMNElement *pElement, bool bSegOn);
	BPMNElement *GetCurrentSyncronizedElement();
	bool IsItemSelected();
	bool IsInSubprocess();
	bool IsSubProcessAboveSelected();
	bool IsSubProcessBelowSelected();
	bool IsSubProcessSelected();
	bool IsSelectedInSubProcess();
	bool IsOnTopOfList();
	bool IsOnBottomOfList();
	bool IsOneItemSelected();
	bool IsBranchSelected();
	bool IsPlayerInPlayMode();
	BPMNDecision *GetSelectedBranch();
	int GetNoOfItems();
	void CreateLevelDiagram(list <BPMNElement *> *pElementList);
	BPMNDiagram();
	~BPMNDiagram();

};

