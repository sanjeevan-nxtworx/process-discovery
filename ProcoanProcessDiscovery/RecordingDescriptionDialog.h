#pragma once
#include "wx/wx.h"
#include <wx/notebook.h>
#include <wx/event.h>
#include <string>

using namespace std;


class RecordingDescriptionDialog :
	public wxDialog
{

	wxTextCtrl *pProcessName;
	wxTextCtrl *pProcessDescription;
	wxTextCtrl *pHistory;
	wxTextCtrl *pCreatedBy;
	wxButton *pCancel;
	wxButton *pSave;

	wxNotebook *prMainNotebook;
	wxPanel *prSummaryPanel;

	string GetProcessName();
	string GetProcessDescription();
	string GetHistory();
	string GetCreatedBy();

public:
	RecordingDescriptionDialog(string jSonProcessDesc, wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & position, const wxSize & size);
	~RecordingDescriptionDialog();
	string GetProcessData();
	void SetProcessData(string val);
};

