#pragma once
#include "wx/wx.h"
#include <wx/notebook.h>
#include <wx/event.h>
#include <string>

using namespace std;

class ProcessSummaryPanel :
	public wxPanel
{
	wxTextCtrl *pProcessName;
	wxTextCtrl *pProcessDescription;
	wxTextCtrl *pSubProcessName;
	wxTextCtrl *pSubProcessDescription;
	wxTextCtrl *pCreatedBy;
	wxButton *pCancel;
	wxButton *pReport;

public:
	ProcessSummaryPanel(wxNotebook *parent, wxSize size);
	~ProcessSummaryPanel();

	string GetProcessName();
	string GetProcessDescription();
	string GetSubProcessName();
	string GetSubProcessDescription();
	string GetCreatedBy();
	void SetProcessData(string val);
};

