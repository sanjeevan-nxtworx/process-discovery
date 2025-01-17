#include "stdafx.h"
#include "CustomEvents.h"
#include "InfoTabPanel.h"


InfoTabPanel::InfoTabPanel(wxNotebook *parent, wxSize size) :
	wxPanel(parent, ID_INFO_TAB, wxPoint(0, 0), wxSize(size), wxNO_BORDER)
{
	//int columnPos = size.GetWidth();
	pParent = parent;
	pProcessNameLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Process Name: "), wxPoint(5, 5), wxSize(100, 20));
	pWindowNameLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Window Name: "), wxPoint(5, 25), wxSize(100, 20));
	pWindowPositionLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Window Position: "), wxPoint(5, 45), wxSize(100, 20));
	pWindowSizeLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Size: "), wxPoint(300, 45), wxSize(100, 20));
	pControlTypeNameLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Control Type: "), wxPoint(5, 65), wxSize(100, 20));
	pControlPositionLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Control Position: "), wxPoint(5, 85), wxSize(100, 20));
	pControlSizeLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Size: "), wxPoint(300, 85), wxSize(100, 20));
	pDocumentNameLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Document Name: "), wxPoint(5, 105), wxSize(100, 20));
	pActivityDurationLabel = DBG_NEW wxStaticText(this, wxID_ANY, wxString("Activity Duration: "), wxPoint(5, 125), wxSize(100, 20));

	pProcessName = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 5), wxSize(300, 20));
	pWindowName = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 25), wxSize(300, 20));
	pWindowPosition = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 45), wxSize(100, 20));
	pWindowSize = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(400, 45), wxSize(100, 20));
	pControlTypeName = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 65), wxSize(200, 20));
	pControlPosition = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 85), wxSize(100, 20));
	pControlSize = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(400, 85), wxSize(100, 20));
	pDocumentName = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 105), wxSize(300, 20));
	pActivityDuration = DBG_NEW wxStaticText(this, wxID_ANY, wxString(""), wxPoint(120, 125), wxSize(300, 20));
	duration = 0.0f;
}


void InfoTabPanel::SetProcessName(string value)
{
	strProcessName = value;
}

void InfoTabPanel::SetWindowName(string value)
{
	strWindowName = value;
}

void InfoTabPanel::SetWindowPosition(string value)
{
	strWindowPosition = value;
}

void InfoTabPanel::SetWindowSize(string value)
{
	strWindowSize = value;
}

void InfoTabPanel::SetControlType(string value)
{
	strControlType = value;
}

void InfoTabPanel::SetControlPosition(string value)
{
	strControlPosition = value;
}

void InfoTabPanel::SetControlSize(string value)
{
	strControlSize = value;
}

void InfoTabPanel::SetDocumentName(string value)
{
	strDocumentName = value;
}

void InfoTabPanel::SetActivityDuration(double value)
{
	duration = value;
}


void InfoTabPanel::ShowTabData()
{
	pProcessName->SetLabelText(strProcessName);
	pWindowName->SetLabelText(strWindowName);
	pWindowPosition->SetLabelText(strWindowPosition);
	pWindowSize->SetLabelText(strWindowSize);
	pControlTypeName->SetLabelText(strControlType);
	pControlPosition->SetLabelText(strControlPosition);
	pControlSize->SetLabelText(strControlSize);
	pDocumentName->SetLabelText(strDocumentName);
	wxString strVal = wxString::Format("%7.2f secs", duration);
	pActivityDuration->SetLabelText(strVal);
}

InfoTabPanel::~InfoTabPanel()
{
}
