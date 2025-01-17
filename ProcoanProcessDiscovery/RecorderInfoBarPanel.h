#pragma once
#include <wx/wx.h>

class RecorderInfoBarPanel :
	public wxPanel
{
	wxWindow *pParent;
	wxString directory;
	wxString filename;

public:
	RecorderInfoBarPanel(wxWindow * parent, wxSize size);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void SetDirectory(wxString strDir);
	void SetFilename(wxString strFilename);

	DECLARE_EVENT_TABLE()
};



