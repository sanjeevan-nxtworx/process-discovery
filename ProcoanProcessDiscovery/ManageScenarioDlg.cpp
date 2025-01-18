#include "stdafx.h"
#include "ManageScenarioDlg.h"
#include "ScenarioFacade.h"
#include "CustomEvents.h"
#include "Utility.h"
#include "ScenarioDefintion.h"

wxBEGIN_EVENT_TABLE(ManageScenarioDlg, wxDialog)
EVT_LISTBOX(SCENARIO_LISTBOX, ManageScenarioDlg::OnScenarioChanged)
EVT_BUTTON(SCENARIO_NEWBTN, ManageScenarioDlg::OnNewScenario)
EVT_BUTTON(SCENARIO_SAVEBTN, ManageScenarioDlg::OnSaveScenario)
EVT_BUTTON(SCENARIO_PLAYBTN, ManageScenarioDlg::OnPlayScenario)
EVT_BUTTON(SCENARIO_SELECTBTN, ManageScenarioDlg::OnSelectScenario)
EVT_BUTTON(SCENARIO_CANCELBTN, ManageScenarioDlg::OnCancelDlg)
EVT_BUTTON(SCENARIO_CLOSEBTN, ManageScenarioDlg::OnCloseDlg)
wxEND_EVENT_TABLE()

ManageScenarioDlg::ManageScenarioDlg(ScenarioFacade *pScenarios, wxWindow * parent, const wxString & title,
	const wxPoint & position, const wxSize & size)
	: wxDialog(NULL, -1, title, position, size)
{
	pScenarioFacade = pScenarios;
	strSelectedScenario = "";
	pParent = (VideoEditorScreen *)parent;
	wxStaticText *pStaticText = NULL;
	wxArrayString scenarioList;
	scenarioList.clear();
	scenarioList.Add("Default");
	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Scenario List"), wxPoint(20, 5), wxSize(100, 20));
	pScenarioLB = DBG_NEW wxListBox(this, SCENARIO_LISTBOX, wxPoint(20, 25), wxSize(350, 120), scenarioList);

	//pPlayButton = DBG_NEW wxButton(this, SCENARIO_PLAYBTN, "Select && Play", wxPoint(50, 160), wxSize(100, 20));
	//pSelectButton = DBG_NEW wxButton(this, SCENARIO_SELECTBTN, "Select", wxPoint(180, 160), wxSize(80, 20));
	pNewButton = DBG_NEW wxButton(this, SCENARIO_NEWBTN, "New", wxPoint(160, 160), wxSize(70, 20));

	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Name"), wxPoint(20, 200), wxSize(80, 20));
	pScenarioName = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(100, 200), wxSize(350, 20));

	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Description"), wxPoint(20, 230), wxSize(150, 20));
	pScenarioDesc = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(20, 255), wxSize(445, 100), wxTE_MULTILINE);

	pSaveButton = DBG_NEW wxButton(this, SCENARIO_SAVEBTN, "Save", wxPoint(120, 370), wxSize(70, 20));
	pCancelButton = DBG_NEW wxButton(this, SCENARIO_CANCELBTN, "Cancel", wxPoint(220, 370), wxSize(70, 20));
	pCloseButton = DBG_NEW wxButton(this, SCENARIO_CLOSEBTN, "Close", wxPoint(320, 370), wxSize(70, 20));

	UpdateScenarioListBox(pScenarios);

	Centre();
	pScenarioLB->Select(0);
	pScenarioName->SetValue("Default");
	pScenarioDesc->SetValue("Default Scenario");
	pScenarioName->Enable(false);
	pScenarioDesc->Enable(false);
	pSaveButton->Enable(false);
	pCancelButton->Enable(false);
}


ManageScenarioDlg::~ManageScenarioDlg()
{
}

string ManageScenarioDlg::GetSelectedString()
{
	return strSelectedScenario;
}

void ManageScenarioDlg::UpdateScenarioListBox(ScenarioFacade *pScenarios)
{

	int nSel = pScenarioLB->GetSelection();
	wxString selString = "";
	if (nSel != -1)
		selString = pScenarioLB->GetString(nSel);
	vector <string> scenarioNames = pScenarios->GetScenarioNames();
	pScenarioLB->Clear();
	wxArrayString scenarioList;
	scenarioList.clear();
	scenarioList.Add("Default");
	vector <string>::iterator itNames = scenarioNames.begin();
	while (itNames != scenarioNames.end())
	{
		scenarioList.Add(*itNames);
		itNames++;
	}
	pScenarioLB->Append(scenarioList);
	nSel = pScenarioLB->FindString(selString);
	pScenarioLB->SetSelection(nSel);
}


void ManageScenarioDlg::OnSelectScenario(wxCommandEvent& WXUNUSED(event))
{
	int nSel = pScenarioLB->GetSelection();
	wxString strScenario = pScenarioLB->GetString(nSel);
	strSelectedScenario = strScenario.char_str();
	EndDialog(SCENARIO_SELECTBTN);
}

void ManageScenarioDlg::OnPlayScenario(wxCommandEvent& WXUNUSED(event))
{
	int nSel = pScenarioLB->GetSelection();
	wxString strScenario = pScenarioLB->GetString(nSel);
	strSelectedScenario = strScenario.char_str();
	EndDialog(SCENARIO_PLAYBTN);
}

void ManageScenarioDlg::OnCancelDlg(wxCommandEvent& WXUNUSED(event))
{
	pScenarioLB->Select(0);
	pScenarioLB->Enable(true);
	pScenarioName->SetValue(wxT("Default"));
	pScenarioDesc->SetValue(wxT(""));
	pScenarioName->Enable(false);
	pScenarioDesc->Enable(false);
	//pPlayButton->Enable(true);
	//pSelectButton->Enable(true);
	pNewButton->Enable(true);
	pSaveButton->Enable(true);
	pCancelButton->Enable(false);
	pCloseButton->Enable(true);
}

void ManageScenarioDlg::OnCloseDlg(wxCommandEvent& WXUNUSED(event))
{
	strSelectedScenario = "";
	EndDialog(SCENARIO_CLOSEBTN);
}


void ManageScenarioDlg::OnScenarioChanged(wxCommandEvent& event)
{
	int nSel = event.GetSelection();
	wxString strScenario = pScenarioLB->GetString(nSel);
	if (nSel == 0)
	{
		pScenarioName->SetValue("Default");
		pScenarioDesc->SetValue("Default Scenario");
		pScenarioName->Enable(false);
		pScenarioDesc->Enable(false);
		pSaveButton->Enable(false);
	}
	else
	{
		string str = strScenario.char_str();
		ScenarioDefintion *pScenario = pScenarioFacade->GetScenario(str);
		pScenarioName->SetValue(wxString(pScenario->GetName()));
		pScenarioDesc->SetValue(wxString(pScenario->GetDesc()));
		pScenarioName->Enable(true);
		pScenarioDesc->Enable(true);
		pSaveButton->Enable(true);
		pCancelButton->Enable(true);
		pCloseButton->Enable(true);
	}
}

void ManageScenarioDlg::OnNewScenario(wxCommandEvent& WXUNUSED(event))
{
	pScenarioLB->Select(0);
	pScenarioLB->Enable(false);

	pScenarioName->SetValue(wxT(""));
	pScenarioDesc->SetValue(wxT(""));
	pScenarioName->Enable(true);
	pScenarioDesc->Enable(true);

	//pPlayButton->Enable(false);
	//pSelectButton->Enable(false);
	pNewButton->Enable(false);

	pSaveButton->Enable(true);
	pCancelButton->Enable(true);
	pCloseButton->Enable(true);
}

void ManageScenarioDlg::OnSaveScenario(wxCommandEvent& WXUNUSED(event))
{
	int nSel = pScenarioLB->GetSelection();

	if (nSel == 0)
	{
		// New Scenario Saved
		string name = pScenarioName->GetValue().char_str();
		string desc = pScenarioDesc->GetValue().char_str();
		ScenarioDefintion *pScenarioDefn = DBG_NEW ScenarioDefintion();
		pScenarioDefn->SetName(name);
		pScenarioDefn->SetDesc(desc);
		pParent->SaveOrGetScenario(true,pScenarioDefn);
		pScenarioFacade->AddScenario(pScenarioDefn);
	}
	else
	{
		string sceneName = pScenarioLB->GetString(nSel);
		string name = pScenarioName->GetValue().char_str();
		string desc = pScenarioDesc->GetValue().char_str();
		ScenarioDefintion *pScenarioDefn = pScenarioFacade->GetScenario(sceneName);
		pScenarioDefn->SetName(name);
		pScenarioDefn->SetDesc(desc);
	}

	pParent->SaveScenarioDefinitions(pScenarioFacade);
	UpdateScenarioListBox(pScenarioFacade);
	pScenarioLB->Enable(true);
	pScenarioName->SetValue(wxT(""));
	pScenarioDesc->SetValue(wxT(""));
	pScenarioName->Enable(true);
	pScenarioDesc->Enable(true);
	//pPlayButton->Enable(true);
	//pSelectButton->Enable(true);
	pNewButton->Enable(true);
	pSaveButton->Enable(true);
	pCancelButton->Enable(false);
	pCloseButton->Enable(true);
}
