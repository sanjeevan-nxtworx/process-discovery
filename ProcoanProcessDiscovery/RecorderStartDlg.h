#pragma once

#include <wx/wx.h>
#include "UIUtility.h"

class ProcoanButton;

class RecorderStartDlg : public wxDialog
{
	wxBitmap bmpFindDirectory;
	wxArrayString fpsList;
	wxArrayString monitorList;
	wxArrayString selectedMonitorList;

	wxStaticText* lblDirectoryName;

	wxTextCtrl *txtFileName;
	ProcoanButton *btnFindDirectory;
	wxCheckBox* chkRecordVideo;
	wxCheckBox* chkDirectXCapture;
	wxCheckBox* chkActiveMonitorOnly;
	wxCheckBox* chkRecordAudio;
	wxCheckBox* chkRecordEvents;

	wxChoice*	cbFramesPerSec;
	wxCheckListBox* lbMonitorSelection;

	ProcoanButton* btnStartRecording;
	ProcoanButton* btnSaveOptions;
	ProcoanButton* btnCancel;
	std::list <struct monitorDetails *> *pMonitorList;
	wxWindow *parentWnd;

	bool bVideoFlg;
	bool bAudioFlg;
	bool bEventFlg;
	int  nFPS;
	bool bActiveMonitorFlg;
	wxString strDirectory;
	wxString strFileName;

	void OnVideoCheck(wxCommandEvent& evt);
	void OnAudioCheck(wxCommandEvent& evt);
	void OnEventCheck(wxCommandEvent& evt);
	void OnOK(wxCommandEvent&evt);

public:
	RecorderStartDlg(std::list <struct monitorDetails *> *pList, wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & position, const wxSize & size);
	void FindDirectoryButtonClicked(wxCommandEvent& event);
	void SetMonitorList(std::list <struct monitorDetails *> *pList);
	void SetVideoFlg(bool flg);
	bool GetVideoFlg(void);
	void SetAudioFlg(bool flg);
	bool GetAudioFlg(void);
	void SetEventFlg(bool flg);
	bool GetEventFlg(void);
	void SetActiveMonitorFlg(bool flg);
	bool GetActiveMonitorFlg(void);
	void UpdateDialog(bool flg);
	void SetFPS(int fps);
	int GetFPS(void);
	void SetDirectory(wxString dir);
	wxString GetDirectory(void);
	void SetFilename(wxString filename);
	wxString GetFilename(void);
	void OnStartRecording(wxCommandEvent& evt);
	void OnSaveRecording(wxCommandEvent& evt);
	void OnCloseRecording(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE()
};

