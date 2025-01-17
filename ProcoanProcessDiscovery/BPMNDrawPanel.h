#pragma once

#include <wx/wx.h>
#include "UIUtility.h"
#include <list>
using namespace std;

class BPMNElement;
class BPMNDiagram;
class PlayerDataFileManager;
class BPMNControlPanel;

class BPMNDrawPanel : public wxPanel
{
	list <BPMNElement *> *pBPMNDiagram;
	BPMNDiagram *pDiagramMap;
	PlayerDataFileManager *pPlayerManager;
	BPMNControlPanel *pControlPanel;
	bool bCopySegmentFlg;

public:
	BPMNDrawPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size);
	~BPMNDrawPanel();

	void GetControlDimensions();

	void SetDiagram(list <BPMNElement *> *pDiagram);
	void SetDiagramMap(BPMNDiagram *pMap);
	void SetController(PlayerDataFileManager *pManager);
	void SetControlPanel(BPMNControlPanel *pBPMNPanel);
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
	bool IsBranchRight();

	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void OnMouseMove(wxMouseEvent & event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & WXUNUSED(event));
	void OnRightDown(wxMouseEvent & event);
	void OnPopupClick(wxCommandEvent &evt);
	void OnLeftDblClick(wxMouseEvent & event);
	void OnRightDblClick(wxMouseEvent &event);
	void UpdateButtons();
	void SetCopySegmentFlag(bool bFlg);
	DECLARE_EVENT_TABLE();
};

