#include "stdafx.h"
#include "PicButtonControlPanel.h"
#include "UIUtility.h"
#include "CustomEvents.h"
#include "ProcoanButton.h"
#include "EditorButtons.h"

// Event table for MyFrame
BEGIN_EVENT_TABLE(PicButtonControlPanel, wxPanel)
EVT_COMMAND(EDITOR_FREW, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnFastRewind)
EVT_COMMAND(EDITOR_SREW, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnSlowRewind)
EVT_COMMAND(EDITOR_FFWD, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnFastForward)
EVT_COMMAND(EDITOR_SFWD, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnSlowForward)
EVT_COMMAND(EDITOR_STOP, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnStop)
EVT_COMMAND(EDITOR_PLAY, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnPlay)
EVT_COMMAND(EDITOR_ADD_ACTIVTY, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnAddActivity)
EVT_COMMAND(EDITOR_START_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnStartSegment)
EVT_COMMAND(EDITOR_END_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnEndSegment)
EVT_COMMAND(EDITOR_COPY_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnCopySegment)
EVT_COMMAND(EDITOR_CUT_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnCutSegment)
EVT_COMMAND(EDITOR_PASTE_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnPasteSegment)
EVT_COMMAND(EDITOR_CLEAR_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnClearSegment)
EVT_COMMAND(EDITOR_BRANCH_SEGMENT, PROCOAN_BUTTON_DOWN, PicButtonControlPanel::OnBranchSegment)

END_EVENT_TABLE()

wxPanel *pPicControlPanel = NULL;


PicButtonControlPanel::PicButtonControlPanel(wxPanel *parent, DWORD id, wxPoint pt, wxSize size, int buttonWidth, int buttonMargin, int buttonXPosDiff, int buttonYPosDiff, bool bFlg) :
	wxPanel(parent, -1, pt, size, wxNO_BORDER)
{
	bPlayerMode = bFlg;
	controlID = id;
	pParent = parent;
	btnRewind = DBG_NEW ProcoanButton(
			this, EDITOR_FREW, wxPoint(buttonMargin, buttonMargin),
			wxSize(buttonWidth, buttonWidth));
	SetButtonBitmaps(btnRewind, FRewNormal_xpm, FRewDisabled_xpm, FRewMouseOver_xpm, FRewPressed_xpm, buttonWidth);
	SetSmallButtonBitmaps(btnRewind, FRewSMNormal_xpm, FRewSMDisabled_xpm, FRewSMMouseOver_xpm, FRewSMPressed_xpm, 30);
	btnRewind->SetToolTip(wxT("Fast Rewind"));
	btnStepBack = DBG_NEW ProcoanButton(
			this, EDITOR_SREW,
			wxPoint(buttonMargin + (buttonXPosDiff * 1), buttonMargin + (buttonYPosDiff * 1)),
			wxSize(buttonWidth, buttonWidth));
	SetButtonBitmaps(btnStepBack, SRewNormal_xpm, SRewDisabled_xpm, SRewMouseOver_xpm, SRewPressed_xpm, buttonWidth);
	SetSmallButtonBitmaps(btnStepBack, SRewSMNormal_xpm, SRewSMDisabled_xpm, SRewSMMouseOver_xpm, SRewSMPressed_xpm, 30);
	btnStepBack->SetToolTip(wxT("Step Back"));

	btnStop = DBG_NEW ProcoanButton(
			this, EDITOR_STOP,
			wxPoint(buttonMargin + (buttonXPosDiff * 2), buttonMargin + (buttonYPosDiff * 2)),
			wxSize(buttonWidth, buttonWidth));
	SetButtonBitmaps(btnStop, stopNormal_xpm, stopDisabled_xpm, stopMouseOver_xpm, stopPressed_xpm, buttonWidth);
	SetSmallButtonBitmaps(btnStop, stopSMNormal_xpm, stopSMDisabled_xpm, stopSMMouseOver_xpm, stopSMPressed_xpm, 30);
	btnStop->SetToolTip(wxT("Stop"));

	btnPlay = DBG_NEW ProcoanButton(
			this, EDITOR_PLAY,
			wxPoint(buttonMargin + (buttonXPosDiff * 3), buttonMargin + (buttonYPosDiff * 3)),
			wxSize(buttonWidth, buttonWidth));
	SetButtonBitmaps(btnPlay, playNormal_xpm, playDisabled_xpm, playMouseOver_xpm, playPressed_xpm, buttonWidth);
	SetSmallButtonBitmaps(btnPlay, playSMNormal_xpm, playSMDisabled_xpm, playSMMouseOver_xpm, playSMPressed_xpm, 30);
	btnPlay->SetToolTip(wxT("Play"));

	btnStepFwd = DBG_NEW ProcoanButton(
			this, EDITOR_SFWD,
			wxPoint(buttonMargin + (buttonXPosDiff * 4), buttonMargin + (buttonYPosDiff * 4)),
			wxSize(buttonWidth, buttonWidth));
	SetButtonBitmaps(btnStepFwd, SFwdNormal_xpm, SFwdDisabled_xpm, SFwdMouseOver_xpm, SFwdPressed_xpm, buttonWidth);
	SetSmallButtonBitmaps(btnStepFwd, SFwdSMNormal_xpm, SFwdSMDisabled_xpm, SFwdSMMouseOver_xpm, SFwdSMPressed_xpm, 30);
	btnStepFwd->SetToolTip(wxT("Step Forward"));

	btnForward = DBG_NEW ProcoanButton(
			this, EDITOR_FFWD,
			wxPoint(buttonMargin + (buttonXPosDiff * 5), buttonMargin + (buttonYPosDiff * 5)),
			wxSize(buttonWidth, buttonWidth));
	SetButtonBitmaps(btnForward, FFwdNormal_xpm, FFwdDisabled_xpm, FFwdMouseOver_xpm, FFwdPressed_xpm, buttonWidth);
	SetSmallButtonBitmaps(btnForward, FFwdSMNormal_xpm, FFwdSMDisabled_xpm, FFwdSMMouseOver_xpm, FFwdSMPressed_xpm, 30);
	btnForward->SetToolTip(wxT("Fast Forward"));

	if (!bPlayerMode)
	{
		btnAddActivity = DBG_NEW ProcoanButton(
			this, EDITOR_ADD_ACTIVTY,
			wxPoint(buttonMargin + (buttonXPosDiff * 6), buttonMargin + (buttonYPosDiff * 6)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnAddActivity, addActivityNormal_xpm, addActivityDisabled_xpm, addActivityMouseOver_xpm, addActivityPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnAddActivity, addActivitySMNormal_xpm, addActivitySMDisabled_xpm, addActivitySMMouseOver_xpm, addActivitySMPressed_xpm, 30);
		btnAddActivity->SetToolTip(wxT("Add New Activity"));

		btnStartSegment = DBG_NEW ProcoanButton(
			this, EDITOR_START_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 7), buttonMargin + (buttonYPosDiff * 7)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnStartSegment, startSegmentNormal_xpm, startSegmentDisabled_xpm, startSegmentMouseOver_xpm, startSegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnStartSegment, startSegmentSMNormal_xpm, startSegmentSMDisabled_xpm, startSegmentSMMouseOver_xpm, startSegmentSMPressed_xpm, 30);
		btnStartSegment->SetToolTip(wxT("Start Segment"));

		btnEndSegment = DBG_NEW ProcoanButton(
			this, EDITOR_END_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 8), buttonMargin + (buttonYPosDiff * 8)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnEndSegment, endSegmentNormal_xpm, endSegmentDisabled_xpm, endSegmentMouseOver_xpm, endSegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnEndSegment, endSegmentSMNormal_xpm, endSegmentSMDisabled_xpm, endSegmentSMMouseOver_xpm, endSegmentSMPressed_xpm, 30);
		btnEndSegment->SetToolTip(wxT("End Segment"));

		btnClearSegment = DBG_NEW ProcoanButton(
			this, EDITOR_CLEAR_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 9), buttonMargin + (buttonYPosDiff * 9)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnClearSegment, clearSegmentNormal_xpm, clearSegmentDisabled_xpm, clearSegmentMouseOver_xpm, clearSegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnClearSegment, clearSegmentSMNormal_xpm, clearSegmentSMDisabled_xpm, clearSegmentSMMouseOver_xpm, clearSegmentSMPressed_xpm, 30);
		btnClearSegment->SetToolTip(wxT("Clear Segment"));

		btnCopySegment = DBG_NEW ProcoanButton(
			this, EDITOR_COPY_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 10), buttonMargin + (buttonYPosDiff * 10)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnCopySegment, copySegmentNormal_xpm, copySegmentDisabled_xpm, copySegmentMouseOver_xpm, copySegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnCopySegment, copySegmentSMNormal_xpm, copySegmentSMDisabled_xpm, copySegmentSMMouseOver_xpm, copySegmentSMPressed_xpm, 30);
		btnCopySegment->SetToolTip(wxT("Copy Segment"));

		btnCutSegment = DBG_NEW ProcoanButton(
			this, EDITOR_CUT_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 11), buttonMargin + (buttonYPosDiff * 11)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnCutSegment, cutSegmentNormal_xpm, cutSegmentDisabled_xpm, cutSegmentMouseOver_xpm, cutSegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnCutSegment, cutSegmentSMNormal_xpm, cutSegmentSMDisabled_xpm, cutSegmentSMMouseOver_xpm, cutSegmentSMPressed_xpm, 30);
		btnCutSegment->SetToolTip(wxT("Cut Segment"));

		btnPasteSegment = DBG_NEW ProcoanButton(
			this, EDITOR_PASTE_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 12), buttonMargin + (buttonYPosDiff * 12)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnPasteSegment, pasteSegmentNormal_xpm, pasteSegmentDisabled_xpm, pasteSegmentMouseOver_xpm, pasteSegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnPasteSegment, pasteSegmentSMNormal_xpm, pasteSegmentSMDisabled_xpm, pasteSegmentSMMouseOver_xpm, pasteSegmentSMPressed_xpm, 30);
		btnPasteSegment->SetToolTip(wxT("Paste Segment"));

		btnBranchSegment = DBG_NEW ProcoanButton(
			this, EDITOR_BRANCH_SEGMENT,
			wxPoint(buttonMargin + (buttonXPosDiff * 13), buttonMargin + (buttonYPosDiff * 13)),
			wxSize(buttonWidth, buttonWidth));
		SetButtonBitmaps(btnBranchSegment, branchSegmentNormal_xpm, branchSegmentDisabled_xpm, branchSegmentMouseOver_xpm, branchSegmentPressed_xpm, buttonWidth);
		SetSmallButtonBitmaps(btnBranchSegment, branchSegmentSMNormal_xpm, branchSegmentSMDisabled_xpm, branchSegmentSMMouseOver_xpm, branchSegmentSMPressed_xpm, 30);
		btnBranchSegment->SetToolTip(wxT("Create Branch Sement"));

	}
	else
	{
		btnAddActivity = NULL;
		btnStartSegment = NULL;
		btnEndSegment = NULL;
		btnClearSegment = NULL;
		btnCopySegment = NULL;
		btnCutSegment = NULL;
		btnPasteSegment = NULL;
		btnBranchSegment = NULL;
	}

	bMainFileOpenFlg = false;
	bNavigationOn = false;
	bPlayOn = false;
	bSegmentOn = false;
	bSegmentDone = false;
	bSegmentCopy = false;
	bCutSegment = false;
	bPrimaryFile = false;
	bSyncElementDisplayed = false;
	bSegCopiedOther = false;
	pPicControlPanel = this;
	bSegCopied = false;
}

void PicButtonControlPanel::Resize(int buttonWidth, int buttonMargin, int buttonXPosDiff, int buttonYPosDiff)
{
	btnRewind->SetPosition(wxPoint(buttonMargin, buttonMargin));
	btnRewind->SetSize(wxSize(buttonWidth, buttonWidth));

	btnStepBack->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 1), buttonMargin + (buttonYPosDiff * 1)));
	btnStepBack->SetSize(wxSize(buttonWidth, buttonWidth));

	btnStop->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 2), buttonMargin + (buttonYPosDiff * 2)));
	btnStop->SetSize(wxSize(buttonWidth, buttonWidth));

	btnPlay->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 3), buttonMargin + (buttonYPosDiff * 3)));
	btnPlay->SetSize(wxSize(buttonWidth, buttonWidth));

	btnStepFwd->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 4), buttonMargin + (buttonYPosDiff * 4)));
	btnStepFwd->SetSize(wxSize(buttonWidth, buttonWidth));

	btnForward->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 5), buttonMargin + (buttonYPosDiff * 5)));
	btnForward->SetSize(wxSize(buttonWidth, buttonWidth));

	if (!bPlayerMode)
	{
		btnAddActivity->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 6), buttonMargin + (buttonYPosDiff * 6)));
		btnAddActivity->SetSize(wxSize(buttonWidth, buttonWidth));

		btnStartSegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 7), buttonMargin + (buttonYPosDiff * 7)));
		btnStartSegment->SetSize(wxSize(buttonWidth, buttonWidth));

		btnEndSegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 8), buttonMargin + (buttonYPosDiff * 8)));
		btnEndSegment->SetSize(wxSize(buttonWidth, buttonWidth));

		btnClearSegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 9), buttonMargin + (buttonYPosDiff * 9)));
		btnClearSegment->SetSize(wxSize(buttonWidth, buttonWidth));

		btnCopySegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 10), buttonMargin + (buttonYPosDiff * 10)));
		btnCopySegment->SetSize(wxSize(buttonWidth, buttonWidth));

		btnCutSegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 11), buttonMargin + (buttonYPosDiff * 11)));
		btnCutSegment->SetSize(wxSize(buttonWidth, buttonWidth));

		btnPasteSegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 12), buttonMargin + (buttonYPosDiff * 12)));
		btnPasteSegment->SetSize(wxSize(buttonWidth, buttonWidth));

		btnBranchSegment->SetPosition(wxPoint(buttonMargin + (buttonXPosDiff * 13), buttonMargin + (buttonYPosDiff * 13)));
		btnBranchSegment->SetSize(wxSize(buttonWidth, buttonWidth));
	}
}

void PicButtonControlPanel::SetFileOpenFlg(bool bFlg)
{
	bMainFileOpenFlg = bFlg;
}

void PicButtonControlPanel::SetPrimaryFile(bool bFlg)
{
	bPrimaryFile = bFlg;
}

void PicButtonControlPanel::SetSegSelectedInOtherContexts(bool bFlg, bool bSegDoneFlg, bool bSegOnFlg, bool bSegCopyFlg)
{
	bSegCopiedOther = bFlg;
	bSegmentDone = bSegDoneFlg;
	bSegmentOn = bSegOnFlg;
	bSegmentCopy = bSegCopyFlg;
}

void PicButtonControlPanel::SetSegmentCopiedFlg()
{
	bSegCopied = true;
}

void PicButtonControlPanel::ClearSegmentCopiedFlg()
{
	bSegCopied = false;
}


void PicButtonControlPanel::SetSyncElementDisplayed(bool bFlg)
{
	bSyncElementDisplayed = bFlg;
}

void PicButtonControlPanel::SetButtonStatus()
{
	if (!bMainFileOpenFlg)
	{
		btnRewind->SetDisabled(true);
		btnStepBack->SetDisabled(true);
		btnStop->SetDisabled(true);
		btnPlay->SetDisabled(true);
		btnStepFwd->SetDisabled(true);
		btnForward->SetDisabled(true);
		if (!bPlayerMode)
		{
			btnStartSegment->SetDisabled(true);
			btnEndSegment->SetDisabled(true);
			btnClearSegment->SetDisabled(true);
			btnCopySegment->SetDisabled(true);
			btnAddActivity->SetDisabled(true);
			btnCutSegment->SetDisabled(true);
			btnPasteSegment->SetDisabled(true);
			btnBranchSegment->SetDisabled(true);
		}
		return;
	}

	if (bNavigationOn || bPlayOn)
	{
		btnRewind->SetDisabled(false);
		btnStepBack->SetDisabled(false);
		btnStop->SetDisabled(false);
		btnPlay->SetDisabled(true);
		btnStepFwd->SetDisabled(false);
		btnForward->SetDisabled(false);
		if (!bPlayerMode)
		{
			btnStartSegment->SetDisabled(true);
			btnEndSegment->SetDisabled(true);
			btnClearSegment->SetDisabled(true);
			btnCopySegment->SetDisabled(true);
			btnAddActivity->SetDisabled(true);
			btnCutSegment->SetDisabled(true);
			btnPasteSegment->SetDisabled(true);
			btnBranchSegment->SetDisabled(true);
		}
		return;
	}

	if (!bPlayOn)
	{
		btnRewind->SetDisabled(false);
		btnStepBack->SetDisabled(false);
		btnStop->SetDisabled(true);
		btnPlay->SetDisabled(false);
		btnStepFwd->SetDisabled(false);
		btnForward->SetDisabled(false);
		if (!bPlayerMode)
		{
			btnStartSegment->SetDisabled(true);
			btnEndSegment->SetDisabled(true);
			btnClearSegment->SetDisabled(true);
			btnCopySegment->SetDisabled(true);
			btnCutSegment->SetDisabled(true);
			btnPasteSegment->SetDisabled(true);
			btnBranchSegment->SetDisabled(true);

			btnAddActivity->SetDisabled(false);

			if (!bPrimaryFile)
			{
				btnAddActivity->SetDisabled(true);
			}
			if (bSegCopied && bSegmentDone)
				btnBranchSegment->SetDisabled(false);
			btnStartSegment->SetDisabled(false);
			if (bSyncElementDisplayed && bPrimaryFile)
			{
				btnAddActivity->SetDisabled(false);
			}

			if (bSegmentOn && !bSegmentDone)
			{
				btnEndSegment->SetDisabled(false);
			}
			else if (bSegmentDone && !bSegmentCopy)
			{
				btnClearSegment->SetDisabled(false);
				btnCopySegment->SetDisabled(false);
				if (bPrimaryFile)
				{
					btnCutSegment->SetDisabled(false);
				}
			}
			else if (bSegmentCopy)
			{
				if (!bSegCopiedOther)
					btnClearSegment->SetDisabled(false);
				btnCopySegment->SetDisabled(true);
				if (bPrimaryFile)
				{
					if (!bCutSegment && !bSegCopiedOther)
						btnCutSegment->SetDisabled(false);
					btnPasteSegment->SetDisabled(false);
				}
			}
		}
	}
}


void PicButtonControlPanel::SetButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed, int buttonWidth)
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

void PicButtonControlPanel::SetSmallButtonBitmaps(ProcoanButton *btnObj, const char *const *xpmNormal, const char *const *xpmDisabled, const char *const *xpmFocussed, const char *const *xpmPressed, int buttonWidth)
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

void PicButtonControlPanel::OnFastRewind(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bNavigationOn = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnSlowRewind(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bNavigationOn = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnFastForward(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bNavigationOn = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnSlowForward(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bNavigationOn = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnStop(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bNavigationOn = false;
	bPlayOn = false;
	//SetButtonStatu();
}

void PicButtonControlPanel::SetStop()
{
	bNavigationOn = false;
	bPlayOn = false;
}

void PicButtonControlPanel::SetPlay(bool bFlg)
{
	bPlayOn = bFlg;
}


void PicButtonControlPanel::OnPlay(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bPlayOn = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnAddActivity(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	//SetButtonStatu();
}

void PicButtonControlPanel::OnStartSegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentOn = true;
	bSegmentDone = false;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnEndSegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentDone = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnCopySegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentCopy = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnCutSegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentCopy = true;
	bCutSegment = true;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnPasteSegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentOn = bSegmentDone = bSegmentCopy = bCutSegment = false;
	//SetButtonStatu();
}

void PicButtonControlPanel::OnClearSegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentOn = bSegmentDone = bSegmentCopy = bCutSegment = false;
	//SetButtonStatu();
}

void PicButtonControlPanel::ClearSegment()
{
	bSegCopied = bSegmentCopy;
	bSegmentDone = bSegmentCopy = bCutSegment = false;
}

void PicButtonControlPanel::SetShowSmall(bool flg)
{

	btnRewind->SetShowSmall(flg);
	btnStepBack->SetShowSmall(flg);
	btnStop->SetShowSmall(flg);
	btnPlay->SetShowSmall(flg);
	btnStepFwd->SetShowSmall(flg);
	btnForward->SetShowSmall(flg);
	if (!bPlayerMode)
	{
		btnAddActivity->SetShowSmall(flg);
		btnStartSegment->SetShowSmall(flg);
		btnEndSegment->SetShowSmall(flg);
		btnClearSegment->SetShowSmall(flg);
		btnCopySegment->SetShowSmall(flg);
		btnCutSegment->SetShowSmall(flg);
		btnPasteSegment->SetShowSmall(flg);
		btnBranchSegment->SetShowSmall(flg);
	}
}

void PicButtonControlPanel::OnBranchSegment(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
	bSegmentOn = bSegmentDone = bSegmentCopy = bCutSegment = false;
	//SetButtonStatu();
}



