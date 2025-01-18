#include "stdafx.h"
#include "RecordingDescriptionDialog.h"
#include "ProcessSummaryPanel.h"
#include "json11.hpp"

using namespace json11;


RecordingDescriptionDialog::RecordingDescriptionDialog(string jSonProcessDesc, wxWindow * parent, wxWindowID id, const wxString & title,
const wxPoint & position, const wxSize & size)
	: wxDialog(NULL, -1, title, position, size)
{
	prMainNotebook = DBG_NEW wxNotebook(this, -1, wxPoint(0, 0), wxSize(size.GetWidth(), size.GetHeight()), wxSUNKEN_BORDER, "processInfo");
	prMainNotebook->SetBackgroundColour(wxColor(0x6d, 0x92, 0x9b));
	prSummaryPanel = DBG_NEW wxPanel(prMainNotebook, wxID_ANY, wxPoint(0,0), wxSize(size.GetWidth(), size.GetHeight()));
	wxStaticText *pStaticText = NULL;

	pStaticText = DBG_NEW wxStaticText(prSummaryPanel, wxID_ANY, wxString("Name"), wxPoint(5, 5), wxSize(100, 20));
	pProcessName = DBG_NEW wxTextCtrl(prSummaryPanel, wxID_ANY, wxEmptyString, wxPoint(120, 5), wxSize(350, 20));
	pStaticText = DBG_NEW wxStaticText(prSummaryPanel, wxID_ANY, wxString("Description"), wxPoint(5, 30), wxSize(100, 20));
	pProcessDescription = DBG_NEW wxTextCtrl(prSummaryPanel, wxID_ANY, wxEmptyString, wxPoint(5, 55), wxSize(465, 100), wxTE_MULTILINE);
	//pStaticText = DBG_NEW wxStaticText(prSummaryPanel, wxID_ANY, wxString("Sub-Process Name"), wxPoint(5, 165), wxSize(110, 20));
	//pSubProcessName = DBG_NEW wxTextCtrl(prSummaryPanel, wxID_ANY, wxEmptyString, wxPoint(120, 165), wxSize(350, 20));
	pStaticText = DBG_NEW wxStaticText(prSummaryPanel, wxID_ANY, wxString("Created By"), wxPoint(5, 190), wxSize(100, 20));
	pCreatedBy = DBG_NEW wxTextCtrl(prSummaryPanel, wxID_ANY, wxEmptyString, wxPoint(120, 190), wxSize(350, 20));
	pStaticText = DBG_NEW wxStaticText(prSummaryPanel, wxID_ANY, wxString("History"), wxPoint(5, 220), wxSize(150, 20));
	pHistory = DBG_NEW wxTextCtrl(prSummaryPanel, wxID_ANY, wxEmptyString, wxPoint(5, 250), wxSize(465, 100), wxTE_MULTILINE);
	pSave = DBG_NEW wxButton(prSummaryPanel, wxID_OK, wxString("Save"), wxPoint(5, 370), wxSize(100, 25));
	pCancel = DBG_NEW wxButton(prSummaryPanel, wxID_CANCEL, wxString("Cancel"), wxPoint(120, 370), wxSize(80, 25));
	pHistory->Enable(false);
	prSummaryPanel->SetBackgroundColour(wxColor(0xc1, 0xda, 0xd6));
	prMainNotebook->AddPage(prSummaryPanel, wxT("Recording Summary"), true, -1);
	Centre();
}


RecordingDescriptionDialog::~RecordingDescriptionDialog()
{
}


void RecordingDescriptionDialog::SetProcessData(string val)
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
		else if (key == "History")
		{
			pHistory->SetValue(valStr.c_str());
		}
		else if (key == "CreatedBy")
		{
			pCreatedBy->SetValue(valStr.c_str());
		}
		itProcessInfo++;
	}
}

string RecordingDescriptionDialog::GetProcessData()
{
	string strJson = "{ \"ProcessInfo\": {\"Name\": \"";
	strJson = strJson + GetProcessName() + "\",";
	strJson = strJson + "\"Description\": \"";
	strJson = strJson + GetProcessDescription() + "\",";
	strJson = strJson + "\"History\": \"";
	strJson = strJson + GetHistory() + "\",";
	strJson = strJson + "\"CreatedBy\": \"";
	strJson = strJson + GetCreatedBy() + "\"";
	strJson = strJson + "}";
	strJson = strJson + "}";

	return strJson;
}

string RecordingDescriptionDialog::GetProcessName()
{
	return string(pProcessName->GetValue().mb_str());
}

string RecordingDescriptionDialog::GetProcessDescription()
{
	return string(pProcessDescription->GetValue().mb_str());
}

string RecordingDescriptionDialog::GetHistory()
{
	return string(pHistory->GetValue().mb_str());;
}

string RecordingDescriptionDialog::GetCreatedBy()
{
	return string(pCreatedBy->GetValue().mb_str());
}
