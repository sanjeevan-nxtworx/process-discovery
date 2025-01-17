#pragma once
#include <wx/wx.h>

class ProcoanButton;


class PicButtonControlPanel :
	public wxPanel
{
	wxPanel *pParent;
	ULONG controlID;
	ProcoanButton *btnRewind;
	ProcoanButton *btnStepBack;
	ProcoanButton *btnStop;
	ProcoanButton *btnPlay;
	ProcoanButton *btnStepFwd;
	ProcoanButton *btnForward;
	ProcoanButton *btnAddActivity;
	ProcoanButton *btnStartSegment;
	ProcoanButton *btnEndSegment;
	ProcoanButton *btnClearSegment;
	ProcoanButton *btnCopySegment;
	ProcoanButton *btnCutSegment;
	ProcoanButton *btnPasteSegment;
	ProcoanButton *btnBranchSegment;

	bool	bMainFileOpenFlg;
	bool	bNavigationOn;
	bool	bPlayOn;
	bool	bSegmentOn;
	bool	bSegmentDone;
	bool	bSegmentCopy;
	bool	bCutSegment;
	bool	bPrimaryFile;
	bool	bPlayerMode;
	bool	bSegCopiedOther;
	bool	bSyncElementDisplayed;
	bool	bSegCopied;
public:
	PicButtonControlPanel(wxPanel * parent, DWORD id, wxPoint pt, wxSize size,
		int buttonWidth, int buttonMargin, int buttonXPosDiff, int buttonYPosDiff, bool bFlg);
	void SetFileOpenFlg(bool bFlg);
	void SetPrimaryFile(bool bFlg);
	void SetButtonStatus();
	void SetButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed, int buttonWidth);
	void SetSmallButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed, int buttonWidth);

	void Resize(int buttonWidth, int buttonMargin, int buttonXPosDiff, int buttonYPosDiff);
	void OnFastRewind(wxCommandEvent& event);
	void OnSlowRewind(wxCommandEvent& event);
	void OnFastForward(wxCommandEvent& event);
	void OnSlowForward(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void SetStop();
	void SetPlay(bool bFlg);

	void OnPlay(wxCommandEvent& event);
	void OnAddActivity(wxCommandEvent& event);
	void OnStartSegment(wxCommandEvent& event);
	void OnEndSegment(wxCommandEvent& event);
	void OnCopySegment(wxCommandEvent& event);
	void OnCutSegment(wxCommandEvent& event);
	void OnPasteSegment(wxCommandEvent& event);
	void OnClearSegment(wxCommandEvent& event);
	void OnBranchSegment(wxCommandEvent& event);
	void SetShowSmall(bool flg);
	void SetSyncElementDisplayed(bool flg);
	void ClearSegment();
	void ClearSegmentCopiedFlg();
	void SetSegmentCopiedFlg();
	void SetSegSelectedInOtherContexts(bool bFlg, bool bSegDoneFlg, bool bSegOnFlg, bool bSegCopyFlg);

	DECLARE_EVENT_TABLE()
};



