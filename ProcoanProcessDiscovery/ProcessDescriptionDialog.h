#pragma once
#include "wx/wx.h"
#include <wx/notebook.h>
#include <wx/event.h>

#include <string>
using namespace std;
class ProcessSummaryPanel;

class ProcessDescriptionDialog :
	public wxDialog
{
	wxNotebook *prMainNotebook;
	ProcessSummaryPanel *prSummaryPanel;
public:
	ProcessDescriptionDialog(string jSonProcessDesc,  const wxString & title,
		const wxPoint & position, const wxSize & size);
	~ProcessDescriptionDialog();
	string GetProcessData();
	void SetProcessData(string val);
};

