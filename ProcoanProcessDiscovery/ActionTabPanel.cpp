#include "stdafx.h"
#include "CustomEvents.h"
#include "BPMNElement.h"
#include "ActionTabPanel.h"

wxDEFINE_EVENT(PROCOAN_UPDATE_TAB, wxCommandEvent);
BEGIN_EVENT_TABLE(ActionTabPanel, wxPanel)
END_EVENT_TABLE()



ActionTabPanel::ActionTabPanel(wxNotebook *parent, wxSize size) :
	wxPanel(parent, ID_ACTION_TAB, wxPoint(0, 0), wxSize(size), wxNO_BORDER)
{
	int columnPos = size.GetWidth() - 20;
	pParent = parent;
	pNameLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Name"), wxPoint(5, 1), wxSize(100, 20));
	pName2Label = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Branch Bottom"), wxPoint(5, 52), wxSize(100, 20));
	pNameText = DBG_NEW wxTextCtrl(this, EVENT_NAME_TEXT, wxEmptyString, wxPoint(5, 22), wxSize(columnPos / 2, 20));
	pName2Text = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(5, 72), wxSize(columnPos / 2, 20));
	pDescLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Description"), wxPoint(5, 42), wxSize(100, 20));
	pDescText = DBG_NEW wxTextCtrl(this, EVENT_NAME_DESC, wxEmptyString, wxPoint(5, 62), wxSize(columnPos / 2, 55), wxTE_MULTILINE);
	pName2Label->Hide();
	pName2Text->Hide();
	pGroupLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Group"), wxPoint(columnPos / 2 + 10, 5), wxSize(80, 20));
	pActivityGroup = DBG_NEW wxChoice(this, EVENT_ACTION_GROUP, wxPoint(columnPos / 2 + 100, 5), wxSize(columnPos / 2 - 120, 20));
	pSubGroupLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Sub-Group"), wxPoint(columnPos / 2 + 10, 30), wxSize(80, 20));
	pActivitySubGroup = DBG_NEW wxChoice(this, EVENT_ACTION_SUBGROUP, wxPoint(columnPos / 2 + 100, 30), wxSize(columnPos / 2 - 120, 20));
	pTypeLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Type"), wxPoint(columnPos / 2 + 10, 55), wxSize(80, 20));
	pActivityType = DBG_NEW wxChoice(this, EVENT_ACTION_TYPE, wxPoint(columnPos / 2 + 100, 55), wxSize(columnPos / 2 - 120, 20));
	pActionLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Action"), wxPoint(columnPos / 2 + 10, 80), wxSize(80, 20));
	pActivityAction = DBG_NEW wxChoice(this, EVENT_ACTION, wxPoint(columnPos / 2 + 100, 80), wxSize(columnPos / 2 - 120, 20));
	pSaveButton = DBG_NEW wxButton(this, EVENT_SAVE, wxString("&Save"), wxPoint(columnPos - 80, 105), wxSize(50, 20));
	pActivityGroup->Bind(wxEVT_CHOICE, &ActionTabPanel::OnActivityGroup, this, EVENT_ACTION_GROUP);
	pActivitySubGroup->Bind(wxEVT_CHOICE, &ActionTabPanel::OnActivitySubGroup, this, EVENT_ACTION_SUBGROUP);
	pSaveButton->Bind(wxEVT_BUTTON, &ActionTabPanel::OnActivitySave, this, EVENT_SAVE);
	bIsBranch = false;
	InitializeTabControls();
}

void ActionTabPanel::SetBranch(bool bValue)
{
	bIsBranch = bValue;
}

bool ActionTabPanel::GetBranch()
{
	return bIsBranch;
}

void ActionTabPanel::ResizePanel()
{
	wxSize size = GetSize();
	int columnPos = size.GetWidth() - 20;
	
	pNameLabel->SetPosition(wxPoint(5, 1));
	
	pNameText->SetPosition(wxPoint(5, 22));
	pNameText->SetSize(wxSize(columnPos / 2, 20));

	if (!bIsBranch)
	{
		pNameLabel->SetLabelText(wxT("Name"));
		pName2Label->Hide();
		pName2Text->Hide();
		pDescLabel->Hide();
		pDescLabel->SetPosition(wxPoint(5, 42));
		pDescText->SetPosition(wxPoint(5, 62));
		pDescText->SetSize(wxSize(columnPos / 2, 55));
	}
	else
	{
		pDescLabel->Hide();
		pNameLabel->SetLabelText(wxT("Branch Right"));
		pName2Label->SetPosition(wxPoint(5, 42));
		pName2Text->SetPosition(wxPoint(5, 62));
		pDescLabel->SetPosition(wxPoint(5, 82));
		pDescText->SetPosition(wxPoint(5, 102));
		pDescText->SetSize(wxSize(columnPos / 2, 55));
		pName2Label->Show();
		pName2Text->Show();
	}
	pDescLabel->Show();
	pGroupLabel->SetPosition(wxPoint(columnPos / 2 + 10, 5));
	pActivityGroup->SetPosition(wxPoint(columnPos / 2 + 100, 5));
	pActivityGroup->SetSize(wxSize(columnPos / 2 - 120, 20));

	pSubGroupLabel->SetPosition(wxPoint(columnPos / 2 + 10, 30));
	pActivitySubGroup->SetPosition(wxPoint(columnPos / 2 + 100, 30));
	pActivitySubGroup->SetSize(wxSize(columnPos / 2 - 120, 20));
	
	pTypeLabel->SetPosition(wxPoint(columnPos / 2 + 10, 55));
	pActivityType->SetPosition(wxPoint(columnPos / 2 + 100, 55));
	pActivityType->SetSize(wxSize(columnPos / 2 - 120, 20));

	pActionLabel->SetPosition(wxPoint(columnPos / 2 + 10, 80));
	pActivityAction->SetPosition(wxPoint(columnPos / 2 + 100, 80));
	pActivityAction->SetSize(wxSize(columnPos / 2 - 120, 20));
	pSaveButton->SetPosition(wxPoint(columnPos - 80, 105));

}

void ActionTabPanel::OnActivityGroup(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void ActionTabPanel::OnActivitySubGroup(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}

void ActionTabPanel::OnActivitySave(wxCommandEvent& event)
{
	wxPostEvent(pParent, event);
}


void ActionTabPanel::SetAnnotation(string value)
{
	strAnnotation = value;
}

void ActionTabPanel::SetAnnotationBottom(string value)
{
	strAnnotationBottom = value;
}


void ActionTabPanel::SetNotes(string value)
{
	strNotes = value;
}

void ActionTabPanel::SetGroup(string value)
{
	strGroup = value;
}

void ActionTabPanel::SetSubGroup(string value)
{
	strSubGroup = value;
}

void ActionTabPanel::SetType(string value)
{
	strType = value;
}

void ActionTabPanel::SetAction(string value)
{
	strAction = value;
}

void ActionTabPanel::ShowTabData()
{
	int selected = 0;

	pNameText->SetValue(strAnnotation);
	pDescText->SetValue(strNotes);
	pName2Text->SetValue(strAnnotationBottom);
	selected = pActivityGroup->FindString(strGroup);
	if (selected != wxNOT_FOUND)
		pActivityGroup->SetSelection(selected);

	InitActivityGroup();
	selected = pActivitySubGroup->FindString(strSubGroup);
	if (selected != wxNOT_FOUND)
		pActivitySubGroup->SetSelection(selected);
	InitActivitySubGroup();
	selected = pActivityType->FindString(strType);
	if (selected != wxNOT_FOUND)
		pActivityType->SetSelection(selected);
	selected = pActivityAction->FindString(strAction);
	if (selected != wxNOT_FOUND)
		pActivityAction->SetSelection(selected);

}

void ActionTabPanel::InitializeTabControls()
{
	pNameText->SetValue(wxString(""));
	pDescText->SetValue(wxString(""));

	wxArrayString arrOptions;

	arrOptions.Clear();
	arrOptions.Add(wxString(""));
	arrOptions.Add(wxString("Application"));
	arrOptions.Add(wxString("Manual"));
	arrOptions.Add(wxString("Cognitive"));
	arrOptions.Add(wxString("Intermediate"));
	pActivityGroup->Clear();
	pActivityGroup->Append(arrOptions);
	pActivityGroup->SetSelection(0);

	arrOptions.Clear();
	arrOptions.Add(wxString(""));

	pActivitySubGroup->Clear();
	pActivitySubGroup->Append(arrOptions);
	pActivitySubGroup->SetSelection(0);

	pActivityType->Clear();
	pActivityType->Append(arrOptions);
	pActivityType->SetSelection(0);

	arrOptions.Clear();
	arrOptions.Add(wxString(""));
	arrOptions.Add(wxString("Retain"));
	arrOptions.Add(wxString("Postpone"));
	arrOptions.Add(wxString("Remove"));
	arrOptions.Add(wxString("Automate"));

	pActivityAction->Clear();
	pActivityAction->Append(arrOptions);
	pActivityAction->SetSelection(0);
}

void ActionTabPanel::UpdateTabFields()
{
	wxCommandEvent customEvent(PROCOAN_UPDATE_TAB, GetId());
	customEvent.SetEventObject(this);
	// Do send it
	//ProcessWindowEvent(customEvent);
	QueueEvent(customEvent.Clone());
}

void ActionTabPanel::InitActivityGroup()
{
	wxArrayString arrOptions;

	int numSelected = pActivityGroup->GetSelection();
	if (numSelected >= 0)
	{
		arrOptions.Clear();
		arrOptions.Add(wxString(""));
		wxString strOptionSelected = pActivityGroup->GetString(numSelected);
		if (strOptionSelected == wxString("Application"))
		{
			arrOptions.Add(wxString("Data"));
			arrOptions.Add(wxString("Form"));
			arrOptions.Add(wxString("Application"));
		}
		else if (strOptionSelected == wxString("Manual"))
		{
			arrOptions.Add(wxString("Scan"));
			arrOptions.Add(wxString("Print"));
			arrOptions.Add(wxString("Other"));
		}
		else if (strOptionSelected == wxString("Cognitive"))
		{
			arrOptions.Add(wxString("Rule Based"));
			arrOptions.Add(wxString("Ad-Hoc"));
		}
		else if (strOptionSelected == wxString("Intermediate"))
		{
			arrOptions.Add(wxString("Transaction"));
			arrOptions.Add(wxString("Process Initiation"));
			arrOptions.Add(wxString("Process Termination"));
		}
		pActivitySubGroup->Clear();
		pActivitySubGroup->Append(arrOptions);
		pActivitySubGroup->SetSelection(0);

		arrOptions.Clear();
		arrOptions.Add(wxString(""));
		pActivityType->SetSelection(0);
	}
}

string ActionTabPanel::GetActivityJSonString(BPMNElement *pCurrentEdited)
{
	string strJSon = "";
	if (pCurrentEdited != NULL)
	{
		strJSon = "{\n";
		BPMNElement *pElement = pCurrentEdited;
		if (pElement != NULL)
		{
			strJSon += "\"Annotation\": \"" + pNameText->GetValue() + "\"";
			strJSon += ",\"AnnotationBottom\": \"" + pName2Text->GetValue() + "\"";
			strJSon += ",\n\"Notes\": \"" + pDescText->GetValue() + "\"";
			int sel = pActivityGroup->GetCurrentSelection();
			if (sel >= 0)
			{
				strJSon += ",\n\"Group\": \"" + pActivityGroup->GetString(sel) + "\"";
			}
			sel = pActivitySubGroup->GetCurrentSelection();
			if (sel >= 0)
			{
				strJSon += ",\n\"SubGroup\": \"" + pActivitySubGroup->GetString(sel) + "\"";
			}
			sel = pActivityType->GetCurrentSelection();
			if (sel >= 0)
			{
				strJSon += ",\n\"Type\": \"" + pActivityType->GetString(sel) + "\"";
			}
			sel = pActivityAction->GetCurrentSelection();
			if (sel >= 0)
			{
				strJSon += ",\n\"Action\": \"" + pActivityAction->GetString(sel) + "\"";
			}
			strJSon += "\n}";
		}
	}
	return strJSon;
}

void ActionTabPanel::InitActivitySubGroup()
{
	wxArrayString arrOptions;

	int numSelected = pActivityGroup->GetSelection();
	if (numSelected >= 0)
	{
		arrOptions.Clear();
		arrOptions.Add(wxString(""));
		wxString strOptionSelected = pActivityGroup->GetString(numSelected);
		if (strOptionSelected == wxString("Application"))
		{
			numSelected = pActivitySubGroup->GetSelection();
			strOptionSelected = pActivitySubGroup->GetString(numSelected);
			if (strOptionSelected == wxString("Data"))
			{
				arrOptions.Add(wxString("Structured"));
				arrOptions.Add(wxString("Un-Structured"));
				arrOptions.Add(wxString("Image"));
			}
			else if (strOptionSelected == wxString("Form"))
			{
				arrOptions.Add(wxString("Open/Close"));
				arrOptions.Add(wxString("Save"));
				arrOptions.Add(wxString("Alert Closed"));
				arrOptions.Add(wxString("Print"));
			}
			else if (strOptionSelected == wxString("Application"))
			{
				arrOptions.Add(wxString("Open/Close"));
				arrOptions.Add(wxString("Menu Selection"));
				arrOptions.Add(wxString("Tab Selection"));
			}
		}
		else if (strOptionSelected == wxString("Cognitive"))
		{
			numSelected = pActivitySubGroup->GetSelection();
			strOptionSelected = pActivitySubGroup->GetString(numSelected);
			if (strOptionSelected == wxString("Rule Based"))
			{
				arrOptions.Add(wxString("Simple"));
				arrOptions.Add(wxString("Medium"));
				arrOptions.Add(wxString("Complex"));
				arrOptions.Add(wxString("Very-Complex"));
			}
			else if (strOptionSelected == wxString("Ad-Hoc"))
			{
				arrOptions.Add(wxString("Simple"));
				arrOptions.Add(wxString("Medium"));
				arrOptions.Add(wxString("Complex"));
				arrOptions.Add(wxString("Very-Complex"));
			}
		}
		else if (strOptionSelected == wxString("Intermediate"))
		{
			numSelected = pActivitySubGroup->GetSelection();
			strOptionSelected = pActivitySubGroup->GetString(numSelected);
			if (strOptionSelected == "Transaction" || strOptionSelected == "Process Initiation" || strOptionSelected == "Process Termination")
			{
				arrOptions.Add(wxString("Start"));
				arrOptions.Add(wxString("End"));
			}
		}
		else if (strOptionSelected == wxString("Manual"))
		{
			;
		}
		pActivityType->Clear();
		pActivityType->Append(arrOptions);
		pActivityType->SetSelection(0);
	}
}


ActionTabPanel::~ActionTabPanel()
{
}
