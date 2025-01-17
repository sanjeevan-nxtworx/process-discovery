#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <string>
using namespace std;

class BPMNElement;

class ActionTabPanel : public wxPanel
{
	wxNotebook *pParent;
	wxTextCtrl *pNameText;
	wxTextCtrl *pName2Text;
	wxTextCtrl *pDescText;
	wxChoice *pActivityGroup;
	wxChoice *pActivitySubGroup;
	wxChoice *pActivityType;
	wxChoice *pActivityAction;
	wxButton *pSaveButton;

	wxStaticText *pNameLabel;
	wxStaticText *pName2Label;
	wxStaticText *pDescLabel;
	wxStaticText *pGroupLabel;
	wxStaticText *pSubGroupLabel;
	wxStaticText *pTypeLabel;
	wxStaticText *pActionLabel;

	string strAnnotation;
	string strAnnotationBottom;
	string strNotes;
	string strGroup;
	string strSubGroup;
	string strType;
	string strAction;
	bool	bIsBranch;

public:
	ActionTabPanel(wxNotebook *parent, wxSize size);
	void InitializeTabControls();
	void InitActivityGroup();
	void InitActivitySubGroup();
	string GetActivityJSonString(BPMNElement *pCurrentEdited);
	void SetAnnotationBottom(string value);
	void SetAnnotation(string value);
	void SetNotes(string value);
	void SetGroup(string value);
	void SetSubGroup(string value);
	void SetType(string value);
	void SetAction(string value);
	void OnActivityGroup(wxCommandEvent& event);
	void OnActivitySubGroup(wxCommandEvent& event);
	void OnActivitySave(wxCommandEvent& event);
	void ShowTabData();
	void UpdateTabFields();
	void ResizePanel();
	void SetBranch(bool bValue);
	bool GetBranch();
	~ActionTabPanel();

	wxDECLARE_EVENT_TABLE();
};

