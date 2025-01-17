#include "stdafx.h"
#include "ProcessSummaryPanel.h"
#include "json11.hpp"
using namespace json11;

ProcessSummaryPanel::ProcessSummaryPanel(wxNotebook *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 0), wxSize(size), wxNO_BORDER)
{
	wxStaticText *pStaticText = NULL;

	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Name"), wxPoint(5, 5), wxSize(100, 20));
	pProcessName = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(120, 5), wxSize(350, 20));
	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Description"), wxPoint(5, 30), wxSize(100, 20));
	pProcessDescription = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(5, 55), wxSize(465, 100), wxTE_MULTILINE);
	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Sub-Process Name"), wxPoint(5, 165), wxSize(110, 20));
	pSubProcessName = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(120, 165), wxSize(350, 20));
	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Sub-Process Description"), wxPoint(5, 190), wxSize(150, 20));
	pSubProcessDescription = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(5, 215), wxSize(465, 100), wxTE_MULTILINE);
	pStaticText = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Created By"), wxPoint(5, 325), wxSize(100, 20));
	pCreatedBy = DBG_NEW wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(120, 325), wxSize(350, 20));
	pReport = DBG_NEW wxButton(this, wxID_OK, wxString("Generate Report"), wxPoint(5, 370), wxSize(100, 25));
	pCancel = DBG_NEW wxButton(this, wxID_CANCEL, wxString("Cancel"), wxPoint(120, 370), wxSize(80, 25));
}


ProcessSummaryPanel::~ProcessSummaryPanel()
{

}

void ProcessSummaryPanel::SetProcessData(string val)
{
	string err;
	Json jSonElement;
	map <string, Json> processInfo;

	jSonElement = Json::parse(val, err);
	processInfo = jSonElement["ProcessInfo"].object_items();
	map<string, Json>::iterator itProcessInfo = processInfo.begin();
	while (itProcessInfo != processInfo.end())
	{
		string key = itProcessInfo->first;
		string valStr = itProcessInfo->second.string_value();
		if (key == "Name")
		{
			pProcessName->SetValue(valStr.c_str());
		}
		else if (key == "Description")
		{
			pProcessDescription->SetValue(valStr.c_str());
		}
		else if (key == "SPName")
		{
			pSubProcessName->SetValue(valStr.c_str());
		}
		else if (key == "SPDescription")
		{
			pSubProcessDescription->SetValue(valStr.c_str());
		}
		else if (key == "CreatedBy")
		{
			pCreatedBy->SetValue(valStr.c_str());
		}
		itProcessInfo++;
	}
}

string ProcessSummaryPanel::GetProcessName()
{
	return pProcessName->GetValue();
}

string ProcessSummaryPanel::GetProcessDescription()
{
	return pProcessDescription->GetValue();
}

string ProcessSummaryPanel::GetSubProcessName()
{
	return pSubProcessName->GetValue();
}

string ProcessSummaryPanel::GetSubProcessDescription()
{
	return pSubProcessDescription->GetValue();
}

string ProcessSummaryPanel::GetCreatedBy()
{
	return pCreatedBy->GetValue();
}
