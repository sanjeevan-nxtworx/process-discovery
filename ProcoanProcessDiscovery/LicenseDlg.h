#pragma once
#include "wx/wx.h"

class LicenseDlg :
	public wxDialog
{
	wxString licString;
	wxTextCtrl *licTxt;

public:
	LicenseDlg(const wxString & title, const wxPoint & position, const wxSize & size);
	~LicenseDlg();
	wxString GetLicString();
};

