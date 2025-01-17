#pragma once
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/timer.h>
#include "EventCaptureURL.h"
#include <vector>
#include <map>
#include "SPYDataFacade.h"

using namespace std;


class SpyCaptureScreen : public wxFrame
{
	wxWindow* m_pParent;
	wxTextCtrl* urlName;
	wxTextCtrl* controlName;
	wxTextCtrl* controlType;
	wxTextCtrl* Annotation;
	wxListBox* controlLB;
	wxButton* doneBtn;
	wxButton* saveBtn;
	bool bIsDone;
	void OnDone(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	EventCaptureURL* pEventCapture;
	wxTimer m_timer;
	SPYDataFacade* pDataFacade;

public:
	SpyCaptureScreen(wxWindow* parent, wxWindowID id, const wxString& title,
		const wxPoint& position, const wxSize& size);
	~SpyCaptureScreen();
	void OnTimer(wxTimerEvent& event);
	void SetDataFacade(SPYDataFacade* pValue);
private:
	wxDECLARE_EVENT_TABLE();

};

