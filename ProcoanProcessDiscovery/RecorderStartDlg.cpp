#include "stdafx.h"
#include "RecorderStartDlg.h"
#include "recorderButtons.h"
#include "CustomEvents.h"
#include "Utility.h"
#include "ProcoanButton.h"
//// Event table for MyFrame

BEGIN_EVENT_TABLE(RecorderStartDlg, wxDialog)
EVT_COMMAND(RECORDER_SETUP_SELECT_DIR, PROCOAN_BUTTON_DOWN, RecorderStartDlg::FindDirectoryButtonClicked)
EVT_COMMAND(RECORDER_SETUP_START, PROCOAN_BUTTON_DOWN, RecorderStartDlg::OnStartRecording)
EVT_COMMAND(RECORDER_SETUP_SAVE, PROCOAN_BUTTON_DOWN, RecorderStartDlg::OnSaveRecording)
EVT_COMMAND(RECORDER_SETUP_CLOSE, PROCOAN_BUTTON_DOWN, RecorderStartDlg::OnCloseRecording)
END_EVENT_TABLE()


RecorderStartDlg::RecorderStartDlg(std::list<struct monitorDetails *> *pList, wxWindow * parent,  const wxString & title,
	const wxPoint & position, const wxSize & size)
	: wxDialog(NULL, -1, title, position, size)
{
	
	SetBackgroundColour(wxColour(RGB(0xcf, 0xee, 0xee)));
	fpsList.Add("30");
	fpsList.Add("20");
	fpsList.Add("15");
	fpsList.Add("10");
	fpsList.Add("5");
	fpsList.Add("2");
	fpsList.Add("1");

	wxString wxMonitorString;
	pMonitorList = pList;
	std::list <struct monitorDetails *>::iterator itMonitors = pMonitorList->begin();
	int iMonNum = 0;
	while (itMonitors != pMonitorList->end())
	{
		struct monitorDetails *pMonitor = (struct monitorDetails *)*itMonitors;
		wxString strMonitorName = pMonitor->monitorInfo.szDevice;
		if (strMonitorName == "\\")
			strMonitorName = wxString::Format("#%d", iMonNum + 1);
		iMonNum++;
		wxMonitorString = wxString::Format("%s - %dx%d", strMonitorName, pMonitor->monitorInfo.rcMonitor.right - pMonitor->monitorInfo.rcMonitor.left,
			pMonitor->monitorInfo.rcMonitor.bottom - pMonitor->monitorInfo.rcMonitor.top);
		monitorList.Add(wxMonitorString);
		itMonitors++;
	}

	lblDirectoryName = DBG_NEW wxStaticText(this, -1, "c:\\", wxPoint(150, 10), wxDefaultSize);
	//wxStaticText *lblFileName = new wxStaticText(this, -1, "Recording Filename:", wxPoint(10, 30), wxDefaultSize);
	txtFileName = DBG_NEW wxTextCtrl(this, -1, "", wxPoint(150, 30), wxSize(230, 20));

	btnFindDirectory = DBG_NEW ProcoanButton(
		this, RECORDER_SETUP_SELECT_DIR, wxPoint(390, 25), wxSize(30, 30));
	btnFindDirectory->SetNormalBitmap(folderOpenNormal_xpm);
	btnFindDirectory->SetPressedBitmap(folderOpenPressed_xpm);
	btnFindDirectory->SetDisabledBitmap(folderOpenNormal_xpm);
	btnFindDirectory->SetActiveBitmap(folderOpenMouseOver_xpm);


	chkRecordVideo = DBG_NEW wxCheckBox(this, RECORDER_VIDEO_CHECKBOX, _("Record Video"), wxPoint(10, 50), wxSize(100,20));
	
	//wxStaticText*lblFramesPerSec = DBG_NEW wxStaticText(this, -1, "Frames/Sec (fps)", wxPoint(40, 80), wxDefaultSize);
	cbFramesPerSec = DBG_NEW wxChoice(this, wxID_ANY, wxPoint(40, 100), wxDefaultSize, fpsList);
	chkActiveMonitorOnly = DBG_NEW wxCheckBox(this, wxID_ANY, _("Active Monitor Only"), wxPoint(40, 130), wxSize(130, 20));

	//wxStaticText*lblMonitorList = DBG_NEW wxStaticText(this, -1, "Select Monitors to Record", wxPoint(220, 80), wxDefaultSize);
	lbMonitorSelection = DBG_NEW wxCheckListBox(this, wxID_ANY, wxPoint(210, 100), wxSize(170, 70), monitorList);

	chkRecordAudio = DBG_NEW wxCheckBox(this, RECORDER_AUDIO_CHECKBOX, _("Record Audio"), wxPoint(10, 155), wxSize(100, 20));
	chkRecordEvents = DBG_NEW wxCheckBox(this, RECORDER_EVENTS_CHECKBOX, _("Record Events"), wxPoint(10, 190), wxSize(100, 20));
	
	Connect(RECORDER_VIDEO_CHECKBOX, wxEVT_CHECKBOX, wxCommandEventHandler(RecorderStartDlg::OnVideoCheck));
	Connect(RECORDER_AUDIO_CHECKBOX, wxEVT_CHECKBOX, wxCommandEventHandler(RecorderStartDlg::OnAudioCheck));
	Connect(RECORDER_EVENTS_CHECKBOX, wxEVT_CHECKBOX, wxCommandEventHandler(RecorderStartDlg::OnEventCheck));


	btnStartRecording = DBG_NEW ProcoanButton(
		this, RECORDER_SETUP_START, wxPoint(10, 230), wxSize(145, 30));
	btnStartRecording->SetNormalBitmap(recorderSetupStartNormal_xpm);
	btnStartRecording->SetPressedBitmap(recorderSetupStartPressed_xpm);
	btnStartRecording->SetDisabledBitmap(recorderSetupStartNormal_xpm);
	btnStartRecording->SetActiveBitmap(recorderSetupStartMouseOver_xpm);

	btnSaveOptions = DBG_NEW ProcoanButton(
		this, RECORDER_SETUP_SAVE, wxPoint(160, 230), wxSize(130, 30));
	btnSaveOptions->SetNormalBitmap(recorderSetupSaveNormal_xpm);
	btnSaveOptions->SetPressedBitmap(recorderSetupSavePressed_xpm);
	btnSaveOptions->SetDisabledBitmap(recorderSetupSaveNormal_xpm);
	btnSaveOptions->SetActiveBitmap(recorderSetupSaveMouseOver_xpm);

	btnCancel = DBG_NEW ProcoanButton(
		this, RECORDER_SETUP_CLOSE, wxPoint(295, 230), wxSize(120, 30));
	btnCancel->SetNormalBitmap(recorderSetupCloseNormal_xpm);
	btnCancel->SetPressedBitmap(recorderSetupClosePressed_xpm);
	btnCancel->SetDisabledBitmap(recorderSetupCloseNormal_xpm);
	btnCancel->SetActiveBitmap(recorderSetupCloseMouseOver_xpm);

	Center();
	parentWnd = parent;
	parentWnd->Hide();
//	btnFindDirectory->Bind(wxEVT_BUTTON, &RecorderStartDlg::FindDirectoryButtonClicked, this);
	string strPath = getcurrentpath() + "\\resources\\PecanConfig.xml";
	ConfigClass *pConfigClass = CreateConfigClass(wxString((char *)strPath.c_str()));
	if (pConfigClass != NULL)
	{
		SetVideoFlg(pConfigClass->getCaptureVideo());
		SetAudioFlg(pConfigClass->getCaptureAudio());
		SetEventFlg(pConfigClass->getCaptureEvents());
		SetActiveMonitorFlg(pConfigClass->getCaptureMonitorMode());
		SetFPS(pConfigClass->getCaptureFrameRate());
		vector <string> *pSelectedMonitors = pConfigClass->getMonitorList();
		vector <string>::iterator itSelectMonitor = pSelectedMonitors->begin();
		while (itSelectMonitor != pSelectedMonitors->end())
		{
			string strMonitorName = *itSelectMonitor;
			selectedMonitorList.push_back(strMonitorName.c_str());
			itSelectMonitor++;
		}
	}
	else
	{
		SetVideoFlg(true);
		SetAudioFlg(false);
		SetEventFlg(true);
		SetActiveMonitorFlg(true);
		SetFPS(10);
	}
	DeleteConfigClass();
	SetDirectory(wxString("c:\\Recorder\\"));
	SetFilename(wxString("Untitled.dat"));

}

void RecorderStartDlg::UpdateDialog(bool flg)
{
	int fpsArr[] = { 30,20,15,10,5,2,1 };
	int sel = 0;
	int iMonNum = 0;
	std::list <struct monitorDetails *>::iterator itMonitors = pMonitorList->begin();

	if (flg)
	{
		bVideoFlg = chkRecordVideo->GetValue();
		bAudioFlg = chkRecordAudio->GetValue();
		bEventFlg = chkRecordEvents->GetValue();
		bActiveMonitorFlg = chkActiveMonitorOnly->GetValue();
		strDirectory = lblDirectoryName->GetLabelText();
		strFileName = txtFileName->GetValue();
		if (!strFileName.Contains("."))
			strFileName += ".dat";
		sel = cbFramesPerSec->GetSelection();
		nFPS = fpsArr[sel];
		while (itMonitors != pMonitorList->end())
		{
			struct monitorDetails *pMonitor = (struct monitorDetails *)*itMonitors;
			if (lbMonitorSelection->IsChecked(iMonNum))
			{
				pMonitor->bSelect = true;
			}
			else
			{
				pMonitor->bSelect = false;
			}
			iMonNum++;
			itMonitors++;
		}

	}
	else
	{
		chkRecordVideo->SetValue(bVideoFlg);
		chkRecordAudio->SetValue(bAudioFlg);
		chkRecordEvents->SetValue(bEventFlg);
		chkActiveMonitorOnly->SetValue(bActiveMonitorFlg);
		lblDirectoryName->SetLabelText(strDirectory);
		txtFileName->SetValue(strFileName);
		for (sel = 0; sel < 7; sel++)
		{
			if (nFPS == fpsArr[sel])
				break;
		}
		if (sel == 7)
			sel = 3;
		cbFramesPerSec->Select(sel);

		while (itMonitors != pMonitorList->end())
		{
			struct monitorDetails *pMonitor = (struct monitorDetails *)*itMonitors;
			string lbString = lbMonitorSelection->GetString(iMonNum);
			for (size_t index = 0; index < selectedMonitorList.size(); index++)
			{
				wxString strSelected = selectedMonitorList[index];
				string strComp = string(strSelected.mb_str());
				if (strComp == lbString)
					pMonitor->bSelect = true;
			}
			selectedMonitorList.clear();  // This has to be done only once at start
			if (pMonitor->bSelect)
			{
				lbMonitorSelection->Check(iMonNum, true);
			}
			iMonNum++;
			itMonitors++;
		}
	}
}
void RecorderStartDlg::SetVideoFlg(bool flg)
{
	bVideoFlg = flg;
}

bool RecorderStartDlg::GetVideoFlg(void)
{
	return bVideoFlg;
}
void RecorderStartDlg::SetAudioFlg(bool flg)
{
	bAudioFlg = flg;
}

bool RecorderStartDlg::GetAudioFlg(void)
{
	return bAudioFlg;
}

void RecorderStartDlg::SetEventFlg(bool flg)
{
	bEventFlg = flg;
}

bool RecorderStartDlg::GetEventFlg(void)
{
	return bEventFlg;
}

void RecorderStartDlg::SetActiveMonitorFlg(bool flg)
{
	bActiveMonitorFlg = flg;
}

bool RecorderStartDlg::GetActiveMonitorFlg(void)
{
	return bActiveMonitorFlg;
}

void RecorderStartDlg::SetFPS(int fps)
{
	nFPS = fps;
}

int RecorderStartDlg::GetFPS(void)
{
	return nFPS;
}

void RecorderStartDlg::SetDirectory(wxString dir)
{
	strDirectory = dir;
}

wxString RecorderStartDlg::GetDirectory(void)
{
	return strDirectory;
}

void RecorderStartDlg::SetFilename(wxString filename)
{
	strFileName = filename;
}

wxString RecorderStartDlg::GetFilename(void)
{
	return strFileName;
}

void RecorderStartDlg::OnVideoCheck(wxCommandEvent& WXUNUSED(evt))
{
	if (!chkRecordVideo->GetValue())
	{
		chkRecordAudio->SetValue(false);
		chkRecordAudio->Enable(false);
	}
	else
	{
		if (!chkRecordAudio->IsEnabled())
		{
			chkRecordAudio->Enable();
			chkRecordAudio->SetValue(true);
		}
	}
}

void RecorderStartDlg::OnStartRecording(wxCommandEvent& WXUNUSED(evt))
{
	wxString nameCheck = txtFileName->GetValue();
	if (nameCheck == "")
	{
		wxMessageBox(wxString("Recording Filename cannot be empty"), wxString("Filename Error"));
		return;
	}

	wxArrayInt checkArr;
	lbMonitorSelection->GetCheckedItems(checkArr);
	if (checkArr.Count() == 0)
	{
		wxMessageBox(wxString("Select Atleast 1 Monitor to Record"), wxString("Monitor Select"));
		return;
	}

	UpdateDialog(true);
	EndModal(wxID_OK);
}


void RecorderStartDlg::OnSaveRecording(wxCommandEvent& WXUNUSED(evt))
{
	UpdateDialog(true);

	string strPath = getcurrentpath() + "\\resources\\PecanConfig.xml";
	ConfigClass *pConfigClass = CreateConfigClass(wxString((char *)strPath.c_str()));


	string strFileNameXML = getcurrentpath() + "\\resources\\PecanConfig.xml";
	string strRenameFilename = getcurrentpath() + "\\resources\\PecanConfig.bak";

	remove(strRenameFilename.c_str());
	rename(strFileNameXML.c_str(), strRenameFilename.c_str());


	xml_document outputDoc;
	// add description node with text child
	xml_node configNode = outputDoc.append_child("config");
	xml_node generalNode = configNode.append_child("general");
	xml_node recorderNode = configNode.append_child("recorder");
	xml_node playerNode = configNode.append_child("player");

	xml_node audioCaptureNode = recorderNode.append_child("captureaudio");
	if (bAudioFlg)
		audioCaptureNode.append_child(pugi::node_pcdata).set_value("ON");
	else
		audioCaptureNode.append_child(pugi::node_pcdata).set_value("OFF");

	xml_node eventCaptureNode = recorderNode.append_child("captureevents");
	if (bEventFlg)
		eventCaptureNode.append_child(pugi::node_pcdata).set_value("ON");
	else
		eventCaptureNode.append_child(pugi::node_pcdata).set_value("OFF");

	xml_node activeMonitorNode = recorderNode.append_child("captureactivemonitors");
	if (bActiveMonitorFlg)
		activeMonitorNode.append_child(pugi::node_pcdata).set_value("ON");
	else
		activeMonitorNode.append_child(pugi::node_pcdata).set_value("OFF");

	xml_node videoCaptureNode = recorderNode.append_child("capturevideo");
	if (bVideoFlg)
		videoCaptureNode.append_child(pugi::node_pcdata).set_value("ON");
	else
		videoCaptureNode.append_child(pugi::node_pcdata).set_value("OFF");

	xml_node frameRateNode = recorderNode.append_child("framerate");
	char str[100];
	sprintf_s(str, "%d", nFPS);
	frameRateNode.append_child(pugi::node_pcdata).set_value(str);

	xml_node playerActiveMonitorNode = playerNode.append_child("playactivemonitors");
	if (pConfigClass->getPlayDisplayActiveMonitorMode())
		playerActiveMonitorNode.append_child(pugi::node_pcdata).set_value("ON");
	else
		playerActiveMonitorNode.append_child(pugi::node_pcdata).set_value("OFF");

	xml_node selectedMonitorsNode = recorderNode.append_child("selectedmonitors");
	list <struct monitorDetails *>::iterator itMonitors = pMonitorList->begin();
	int monNum = 0;
	while (itMonitors != pMonitorList->end())
	{
		struct monitorDetails *pMonitor = *itMonitors;
		if (pMonitor->bSelect)
		{
			xml_node monitorNameNode = selectedMonitorsNode.append_child("monitorname");
			string strVal = string(lbMonitorSelection->GetString(monNum).mb_str());
			monitorNameNode.append_child(pugi::node_pcdata).set_value(strVal.c_str());
		}

		itMonitors++;
		monNum++;
	}
	outputDoc.save_file(strFileNameXML.c_str());

	DeleteConfigClass();
}

void RecorderStartDlg::OnCloseRecording(wxCommandEvent& WXUNUSED(evt))
{
	EndModal(wxID_CANCEL);
}

void RecorderStartDlg::OnAudioCheck(wxCommandEvent& WXUNUSED(evt))
{
	
}

void RecorderStartDlg::OnEventCheck(wxCommandEvent& WXUNUSED(evt))
{
	
}


void RecorderStartDlg::SetMonitorList(std::list <struct monitorDetails *> *pList)
{
	pMonitorList = pList;
}

void RecorderStartDlg::FindDirectoryButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose Recording File Directory","", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_OK)
	{
		wxString path = dlg.GetPath() + "\\";
		lblDirectoryName->SetLabelText(path);
	}
}

