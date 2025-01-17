#include "stdafx.h"
#include "SpyCaptureScreen.h"
#include "SPYEditorEnum.h"
#include "Utility.h"
#include "CaptureElementData.h"
#include "SpyWindowData.h"
#include "SpyControlData.h"
#include "SpyUrlData.h"
#include "SpyProcessData.h"
#include "SpyEditorScreen.h"


wxBEGIN_EVENT_TABLE(SpyCaptureScreen, wxFrame)
EVT_BUTTON(CAPTURE_DONE, SpyCaptureScreen::OnDone)
EVT_BUTTON(CAPTURE_SAVE, SpyCaptureScreen::OnSave)
EVT_TIMER(TIMER_ID, SpyCaptureScreen::OnTimer)
wxEND_EVENT_TABLE()


SpyCaptureScreen::SpyCaptureScreen(wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& title,
	const wxPoint& position, const wxSize& size) : m_timer(this, TIMER_ID)
	, wxFrame(NULL, -1, title, position, size, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP)
{
	m_pParent = parent;
	pDataFacade = NULL;
	bIsDone = false;
	wxPanel* framePanel;
	framePanel = DBG_NEW wxPanel(this, -1, wxPoint(0, 0), this->GetSize(), 0, "framePanel");

	DBG_NEW wxStaticText(framePanel, -1, _("Screen Name:"), wxPoint(10, 5), wxSize(80, 25));
	urlName = DBG_NEW wxTextCtrl(framePanel, CAPTURE_URLNAME, wxEmptyString, wxPoint(100, 5), wxSize(275, 25));

	DBG_NEW wxStaticText(framePanel, -1, _("Name:"), wxPoint(10, 35), wxSize(80, 25));
	controlName = DBG_NEW wxTextCtrl(framePanel, CAPTURE_CONTROLNAME, wxEmptyString, wxPoint(100, 35), wxSize(275, 25));

	DBG_NEW wxStaticText(framePanel, -1, _("Type:"), wxPoint(10, 65), wxSize(80, 25));
	controlType = DBG_NEW wxTextCtrl(framePanel, CAPTURE_CONTROLTYPE, wxEmptyString, wxPoint(100, 65), wxSize(275, 25));

	DBG_NEW wxStaticText(framePanel, -1, _("Annotation:"), wxPoint(10, 95), wxSize(80, 25));
	Annotation = DBG_NEW wxTextCtrl(framePanel, CAPTURE_CONTROLANNOTATION, wxEmptyString, wxPoint(100, 95), wxSize(275, 25));

	DBG_NEW wxStaticText(framePanel, -1, _("Control List:"), wxPoint(10, 125), wxSize(80, 25));
	controlLB = DBG_NEW wxListBox(framePanel, CAPTURE_CONTROLLIST, wxPoint(10, 155), wxSize(size.GetWidth() - 35, size.GetHeight() - 250));

	urlName->Enable(false);
	controlName->Enable(false);
	controlType->Enable(false);
	Annotation->Enable(false);
	controlLB->Enable(true);

	doneBtn = DBG_NEW wxButton(framePanel, CAPTURE_DONE, _("Done"), wxPoint(size.GetWidth() - 95, size.GetHeight() - 85), wxSize(70, 25));
	saveBtn = DBG_NEW wxButton(framePanel, CAPTURE_SAVE, _("Save"), wxPoint(size.GetWidth() - 195, size.GetHeight() - 85), wxSize(70, 25));
	pEventCapture = DBG_NEW EventCaptureURL();

	CreateRecorderWindow();
	GetRecorderWnd()->CreateRecorderWindow(this->GetHWND(), GetModuleHandle(NULL));
	GetRecorderWnd()->SetCallBack(pEventCapture);
	setKeyboardHook(GetRecorderWnd()->GetRecorderWindowHandle());
	setMouseHook(GetRecorderWnd()->GetRecorderWindowHandle());
	pEventCapture->CreateTimerThread();
	pEventCapture->CreateUIAThread();
	m_timer.Start(500);
}

void SpyCaptureScreen::SetDataFacade(SPYDataFacade* pValue)
{
	pDataFacade = pValue;
}


void SpyCaptureScreen::OnTimer(wxTimerEvent& event)
{
	if (pEventCapture->IsNewEventCaptured())
	{
		pEventCapture->StartEventProcessing();
		CaptureElementData* pCapturedElement = pEventCapture->GetCapturedEvent();
		if (pCapturedElement != NULL)
		{
			string processName = pCapturedElement->GetProcessName();
			int captureType = pCapturedElement->GetCaptureType();
			vector <SpyControlData*> controls;
			controls.clear();
			string windowName = "";
			windowName = pCapturedElement->GetWindowName();
			if (captureType == 1)
			{
				// This is a desktop capture
				SpyProcessData* pProcessData = pDataFacade->GetProcessData(processName);
				if (pProcessData != NULL)
				{
					pDataFacade->ReadProcessWindowsData(processName);
					SpyWindowData* pWindowData = pDataFacade->GetMatchedWindowData(windowName);

					// New get the controls of the window, and check if control user name is already set,
					// if set display message that it is duplicate, else display on the dialog for changes
					if (pWindowData != NULL)
					{
						pDataFacade->ReadWindowControlsData(pWindowData->GetTitle());

						// Show all control names in list box
						controls = pDataFacade->GetControls();
					}
				}
			}
			else if (captureType == 2)
			{
				// This is a URL Capture
				SpyUrlData* pURLData = pDataFacade->GetMatchedURLData(windowName);
				// New get the controls of the window, and check if control user name is already set,
				// if set display message that it is duplicate, else display on the dialog for changes
				if (pURLData != NULL)
				{
					pDataFacade->ReadURLControlsData(pURLData->GetURL());
					controls = pDataFacade->GetControls();
				}
			}

			// First update the listbox with the controls data

			// Also find if the control exists with the same user name
			// if exists then display an error message and continue
			// Show URL/Window Name, Name, Type, Annotation as blank
			vector <SpyControlData*>::iterator itControls = controls.begin();
			bool bFound = false;
			controlLB->Clear();
			while (itControls != controls.end())
			{
				SpyControlData* pControlData = *itControls;

				string controlNameLocal = pControlData->GetControlUserName();
				controlLB->AppendString(controlNameLocal);
				if (controlNameLocal == pCapturedElement->GetControlName())
					bFound = true;
				itControls++;
			}
			urlName->SetValue(windowName);
			controlName->SetValue(pCapturedElement->GetControlName());
			controlType->SetValue(pCapturedElement->GetControlTypeName());
			Annotation->SetValue("");
			controlName->Enable(true);
			Annotation->Enable(true);
			if (bFound)
			{
				wxMessageBox(wxT("Control already configured. Please use unique name"));
			}
		}
		pEventCapture->EndEventProcessing();

	}
}

SpyCaptureScreen::~SpyCaptureScreen()
{
	if (!bIsDone)
	{
		pEventCapture->QuitCapture();
		clearMouseHook(GetRecorderWnd()->GetRecorderWindowHandle());
		clearKeyboardHook(GetRecorderWnd()->GetRecorderWindowHandle());
		GetRecorderWnd()->SetCallBack(NULL);
		pEventCapture->KillTimerThread();
		pEventCapture->KillUIAThread();
		SpyEditorScreen* pParent = (SpyEditorScreen*)m_pParent;
		pParent->UpdateListBoxes();
		bIsDone = true;
	}
	m_pParent->Show();
	delete pEventCapture;
}

void SpyCaptureScreen::OnDone(wxCommandEvent& WXUNUSED(event))
{
	pEventCapture->QuitCapture();
	clearMouseHook(GetRecorderWnd()->GetRecorderWindowHandle());
	clearKeyboardHook(GetRecorderWnd()->GetRecorderWindowHandle());
	GetRecorderWnd()->SetCallBack(NULL);
	pEventCapture->KillTimerThread();
	pEventCapture->KillUIAThread();
	SpyEditorScreen* pParent = (SpyEditorScreen*)m_pParent;
	pParent->UpdateListBoxes();
	bIsDone = true;
	Close(TRUE); // Tells the OS to quit running this process
}

void SpyCaptureScreen::OnSave(wxCommandEvent& WXUNUSED(event))
{
	// Add Process, URL, Window and Control as required
	CaptureElementData* pCapturedElement = pEventCapture->GetCapturedEvent();
	if (pCapturedElement != NULL)
	{
		string processName = pCapturedElement->GetProcessName();
		int captureType = pCapturedElement->GetCaptureType();
		
		wxString wControlName = controlName->GetValue();
		string controlName = wControlName.mb_str();
		if (controlName == "")
		{
			wxMessageBox("ERROR: Name is empty");
			return;
		}
		wxString wAnnotation = Annotation->GetValue();
		string annotation = wAnnotation.mb_str();

		//CONTROLTYPEID controlType = pCapturedElement->GetControlType();
		pCapturedElement->SetAnnotation(annotation);
		pCapturedElement->SetUserDefinedName(controlName);
		pDataFacade->ClearWindowData();
		pDataFacade->ClearControlsData();

		if (captureType == 1)
		{
			// Window
			string windowName = "";
			SpyWindowData* pWindowData = NULL;
			windowName = pCapturedElement->GetWindowName();
			SpyProcessData* pProcessData = pDataFacade->GetProcessData(processName);
			if (pProcessData == NULL)
			{
				pProcessData = DBG_NEW SpyProcessData;
				pProcessData->SetSystemName(processName);
				pProcessData->SetType("Application");
				pProcessData->SetPlatform("Desktop");
				int i = pDataFacade->GetNumberOfProcesses();
				time_t t = time(NULL);
				int id = (i * 100) + (t % 100);
				pProcessData->SetID(id);
				pDataFacade->UpdateProcessData(pProcessData);

				pWindowData = DBG_NEW SpyWindowData();
				pWindowData->SetTitle(windowName);
				pWindowData->SetWindowType("Main Window");
				pDataFacade->WriteWindowDataFile(pProcessData, pWindowData);
				pDataFacade->AddWindow(windowName, pWindowData);
			}
			else
			{
				pDataFacade->ReadProcessWindowsData(processName);
				pWindowData = pDataFacade->GetMatchedWindowData(windowName);
				if (pWindowData == NULL)
				{
					pWindowData = DBG_NEW SpyWindowData();
					pWindowData->SetTitle(windowName);
					pWindowData->SetWindowType("Main Window");
					pDataFacade->WriteWindowDataFile(pProcessData, pWindowData);
					pDataFacade->AddWindow(windowName, pWindowData);
				}
			}
			pDataFacade->ReadWindowControlsData(pWindowData->GetTitle());

			pDataFacade->AddWindowCapturedControl(pWindowData, pCapturedElement);
		}
		else
		{
			string urlNameLocal = "";
			urlNameLocal = pCapturedElement->GetWindowName();
			SpyUrlData* pUrlData = pDataFacade->GetMatchedURLData(urlNameLocal);
			if (pUrlData == NULL)
			{
				int i = pDataFacade->GetNumberOfURLs();
				time_t t = time(NULL);
				int id = (i * 100) + (t % 100);
				pUrlData = DBG_NEW SpyUrlData;
				pUrlData->SetURL(urlNameLocal);
				pUrlData->SetID(id);
				pDataFacade->UpdateURLData(pUrlData);
				pDataFacade->ClearWindowData();
				pDataFacade->ClearControlsData();
			}
			pDataFacade->ReadURLControlsData(pUrlData->GetURL());
			pDataFacade->AddURLCapturedControl(pUrlData, pCapturedElement);
		}
		pEventCapture->ClearCapturedEvent();
	}
	// Here display the process / window and URL list boxes

	controlName->Enable(false);
	Annotation->Enable(false);
}

