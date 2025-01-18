#include "stdafx.h"
#include "ProcessDescriptionDialog.h"
#include "ProcessSummaryPanel.h"
#include "json11.hpp"
using namespace json11;

ProcessDescriptionDialog::ProcessDescriptionDialog(string jSonProcessDesc, const wxString & title,
const wxPoint & position, const wxSize & size)
	: wxDialog(NULL, -1, title, position, size)
{
	prMainNotebook = DBG_NEW wxNotebook(this, -1, wxPoint(0, 0), wxSize(size.GetWidth(), size.GetHeight()), wxSUNKEN_BORDER, "processInfo");
	prMainNotebook->SetBackgroundColour(wxColor(0x6d, 0x92, 0x9b));
	prSummaryPanel = DBG_NEW ProcessSummaryPanel(prMainNotebook, wxSize(size.GetWidth(), size.GetHeight()));
	prSummaryPanel->SetBackgroundColour(wxColor(0xc1, 0xda, 0xd6));
	prMainNotebook->AddPage(prSummaryPanel, wxT("Process Summary"), true, -1);
	Centre();
}

void ProcessDescriptionDialog::SetProcessData(string val)
{
	prSummaryPanel->SetProcessData(val);
}

string ProcessDescriptionDialog::GetProcessData()
{
	string strJson = "{ \"ProcessInfo\": {\"Name\": \"";
	strJson = strJson + prSummaryPanel->GetProcessName() + "\",";
	strJson = strJson + "\"Description\": \"";
	strJson = strJson + prSummaryPanel->GetProcessDescription() + "\",";
	strJson = strJson + "\"SPName\": \"";
	strJson = strJson + prSummaryPanel->GetSubProcessName() + "\",";
	strJson = strJson + "\"SPDescription\": \"";
	strJson = strJson + prSummaryPanel->GetSubProcessDescription() + "\",";
	strJson = strJson + "\"CreatedBy\": \"";
	strJson = strJson + prSummaryPanel->GetCreatedBy() + "\"";
	strJson = strJson + "}";
	strJson = strJson + "}";

	return strJson;
}

ProcessDescriptionDialog::~ProcessDescriptionDialog()
{
}
