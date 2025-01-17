#pragma once
#include "wx/wx.h"
#include <wx/notebook.h>
#include <wx/event.h>
#include <string>
#include "VideoEditorScreen.h"

using namespace std;
class ScenarioFacade;

class ManageScenarioDlg :
	public wxDialog
{
	ScenarioFacade *pScenarioFacade;
	VideoEditorScreen *pParent;
	wxListBox *pScenarioLB;
	wxButton *pPlayButton;
	wxButton *pSelectButton;
	wxButton *pNewButton;
	wxTextCtrl *pScenarioName;
	wxTextCtrl *pScenarioDesc;
	wxButton *pSaveButton;
	wxButton *pCancelButton;
	wxButton *pCloseButton;
	string   strSelectedScenario;

	void OnScenarioChanged(wxCommandEvent& event);
	void OnNewScenario(wxCommandEvent& event);
	void OnSaveScenario(wxCommandEvent& event);
	void UpdateScenarioListBox(ScenarioFacade *pScenarios);
	void OnSelectScenario(wxCommandEvent& event);
	void OnPlayScenario(wxCommandEvent& event);
	void OnCancelDlg(wxCommandEvent& event);
	void OnCloseDlg(wxCommandEvent& event);

public:
	ManageScenarioDlg(ScenarioFacade *pScenarios, wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & position, const wxSize & size);
	~ManageScenarioDlg();
	string GetSelectedString();
private:
	wxDECLARE_EVENT_TABLE();
};

