
#include "stdafx.h"
#include <wx/wxprec.h>
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/splash.h"
#include "ProcoanProcessDiscovery.h"
#include "PecanMainFrame.h"
#include "LicenseDlg.h"
#include "licenceUtil.h"
#include "Utility.h"
#include "../ProcoanProcessDiscovery/Images/main/AppIcon.xpm"
#include "RecorderDataFileManager.h"

IMPLEMENT_APP(MainApp) // A macro that tells wxWidgets to create an instance of our application 

bool MainApp::OnInit()
{
	wxInitAllImageHandlers();
	wxBitmap bitmap;
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	string strPath = getcurrentpath() + "resources\\splash.bmp";
	if (bitmap.LoadFile((char *)strPath.c_str(), wxBITMAP_TYPE_BMP))
	{
		wxSplashScreen* splash = DBG_NEW wxSplashScreen(bitmap,
			wxSPLASH_CENTRE_ON_SCREEN,
			6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
			wxBORDER_SIMPLE | wxSTAY_ON_TOP);
		Sleep(3000);
		delete splash;
	}
	//wxYield();
	bool bLicenseChk = true;
	//if (!ValidateRegistryLicense())
	//{
	//	LicenseDlg licDlg("Enter License Key", wxPoint(400, 300), wxSize(450, 170));
	//	if (licDlg.ShowModal() == wxID_OK)
	//	{
	//		 Get License Key
	//		wxString wlicKey = licDlg.GetLicString();
	//		string licKey = wlicKey.char_str();
	//		if (!ValidateMainLicense(licKey))
	//			bLicenseChk = false;
	//	}
	//	else
	//		bLicenseChk = false;
	//}

	//RecorderDataFileManager *recManager = new RecorderDataFileManager("test.dat");
	//CreateSPDLogFile("Test.log");
	//recManager->SaveRecordingFile("C:\\Recorder\\Test_011119_02.dat");
	
	if (bLicenseChk)
	{
		// Create an instance of our frame, or window 
		wxSystemSettings wx;
		int w = wx.GetMetric(wxSYS_SCREEN_X);

		PecanMainFrame *MainWin = DBG_NEW PecanMainFrame(w);
		MainWin->SetIcon(appIcon_xpm);
		MainWin->Show(true); // show the window 
		SetTopWindow(MainWin); // and finally, set it as the main window 
		return true;
	}
	else
		return false;
}

