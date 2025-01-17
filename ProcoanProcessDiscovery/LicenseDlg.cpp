#include "stdafx.h"
#include "LicenseDlg.h"


LicenseDlg::LicenseDlg(const wxString & title, const wxPoint & position, const wxSize & size)
	: wxDialog(NULL, -1, title, position, size)
{
	DBG_NEW wxStaticText(this, -1, "Enter License", wxPoint(5, 10), wxDefaultSize);
	licTxt = DBG_NEW wxTextCtrl(this, -1, wxString(""), wxPoint(5, 35), wxSize(420, 50), wxTE_MULTILINE);
	DBG_NEW wxButton(this, wxID_OK, wxString("Validate"), wxPoint(5, 100), wxSize(75, 20));
	DBG_NEW wxButton(this, wxID_CANCEL, wxString("Exit Application"), wxPoint(100, 100), wxSize(100, 20));
	Centre();
}

LicenseDlg::~LicenseDlg()
{
}

wxString LicenseDlg::GetLicString()
{
	licString = licTxt->GetValue();
	return licString;
}
