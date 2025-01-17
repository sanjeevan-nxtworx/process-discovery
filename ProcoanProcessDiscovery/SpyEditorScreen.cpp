#include "stdafx.h"
#include "SpyCaptureScreen.h"
#include "SPYEditorEnum.h"
#include "SpyEditorScreen.h"
#include <wtsapi32.h>
#include <map>
#include <list>
#include "Utility.h"
#include "SpyHeader.h"
#include "SPYDataFacade.h"
#include "SpyProcessData.h"
#include "ProcessWindowData.h"
#include "WindowControlData.h"
#include "SpyControlData.h"
#include "SpyWindowData.h"
#include "MaskImagePanel.h"
#include <wx/filename.h>
#include <wx/busyinfo.h>
#include "SpyMaskData.h"
#include "SpyUrlData.h"
#include "./WordReport/lodepng.h"
#include <regex>
#include <wx/display.h>


using namespace std;
// Event table for MyFrame
wxBEGIN_EVENT_TABLE(SpyEditorScreen, wxFrame)
EVT_BUTTON(BTN_SAVE, SpyEditorScreen::OnSave)
EVT_BUTTON(BTN_SAVE_PROCESS, SpyEditorScreen::OnSaveProcess)
EVT_BUTTON(URL_SAVE, SpyEditorScreen::OnSaveURL)
EVT_BUTTON(BTN_SAVE_WINDOW, SpyEditorScreen::OnSaveWindow)
EVT_BUTTON(BTN_CONTROL_SAVE, SpyEditorScreen::OnSaveControls)
EVT_BUTTON(BTN_URL_CONTROL_SAVE, SpyEditorScreen::OnURLSaveControls)
EVT_BUTTON(BTN_MASK_SAVE, SpyEditorScreen::OnSaveMask)
EVT_BUTTON(BTN_UPDATE_WINDOW, SpyEditorScreen::OnProcessWindowButton)
EVT_BUTTON(BTN_WINDOW_CONTROLS, SpyEditorScreen::OnWindowControls)
EVT_BUTTON(URL_CONTROL, SpyEditorScreen::OnURLControls)
EVT_BUTTON(BTN_MASK_WINDOW, SpyEditorScreen::OnMaskWindow)
EVT_BUTTON(BTN_MASK_URL_WINDOW, SpyEditorScreen::OnURLMaskWindow)
EVT_BUTTON(BTN_UPDATE_PROCESS_WINDOW, SpyEditorScreen::OnUpdateProcessWindows)
EVT_BUTTON(BTN_UPDATE_URLS, SpyEditorScreen::OnUpdateURLs)
EVT_BUTTON(BTN_CAPTURE, SpyEditorScreen::OnCaptureControls)
//EVT_BUTTON(BTN_UPDATE_PROCESS, SpyEditorScreen::OnUpdateProcess)

EVT_LISTBOX(WINDOW_LIST, SpyEditorScreen::OnWindowListSelChanged)
EVT_LISTBOX(LIST_PROCESSES, SpyEditorScreen::OnProcessListSelChanged)
EVT_LISTBOX(URL_CONTROL_LIST, SpyEditorScreen::OnURLControlSelChanged)
EVT_LISTBOX(WINDOW_CONTROL_LIST, SpyEditorScreen::OnWindowControlSelChanged)
EVT_LISTBOX(LIST_URLS, SpyEditorScreen::OnURLListSelChanged)
EVT_BUTTON(BTN_CONTROL_BACKTO_URL, SpyEditorScreen::OnGotoURL)
EVT_BUTTON(BTN_BACKTO_PROCESS, SpyEditorScreen::OnGotoProcessWindows)
EVT_BUTTON(BTN_CONTROL_BACKTO_WINDOW, SpyEditorScreen::OnControlGotoWindows)
EVT_BUTTON(BTN_CONTROL_BACKTO_PROCESS, SpyEditorScreen::OnControlGotoProcess)
EVT_BUTTON(BTN_MASK_BACKTO_CONTROLS, SpyEditorScreen::OnMaskGotoControls)
EVT_BUTTON(BTN_MASK_BACKTO_WINDOW, SpyEditorScreen::OnMaskGotoWindows)
EVT_BUTTON(BTN_MASK_BACKTO_PROCESS, SpyEditorScreen::OnMaskGotoProcess)
EVT_BUTTON(BTN_OPEN_SCREEN, SpyEditorScreen::OnOpenScreenFile)
EVT_BUTTON(BTN_SAVE_MASK, SpyEditorScreen::OnSaveMaskImage)
EVT_BUTTON(BTN_SAVE_IMAGE1, SpyEditorScreen::OnSaveMatchImage1)
EVT_BUTTON(BTN_SAVE_IMAGE2, SpyEditorScreen::OnSaveMatchImage2)
EVT_BUTTON(BTN_SAVE_IMAGE3, SpyEditorScreen::OnSaveMatchImage3)
EVT_BUTTON(BTN_SAVE_IMAGE4, SpyEditorScreen::OnSaveMatchImage4)
EVT_BUTTON(BTN_SAVE_IMAGE5, SpyEditorScreen::OnSaveMatchImage5)
EVT_BUTTON(BTN_SAVE_IMAGE6, SpyEditorScreen::OnSaveMatchImage6)
EVT_BUTTON(BTN_NEW_MASK, SpyEditorScreen::OnNewMask)
EVT_BUTTON(URL_EXECUTE_BUTTON, SpyEditorScreen::OnExecuteURLRule)
EVT_BUTTON(BTN_WINDOW_EXECUTE, SpyEditorScreen::OnExecuteWindowRule)
EVT_CHOICE(CMB_MASK_LIST, SpyEditorScreen::OnMaskSelChanged)
//EVT_CHECKBOX(URL_ISHOME, SpyEditorScreen::OnIsHomeChanged)
wxEND_EVENT_TABLE()

IUIAutomation *g_pUIA = NULL;


DWORD WINAPI GetURLThreadFunction(LPVOID lpParameter);
DWORD WINAPI GetChromeURLThreadFunction(LPVOID lpParameter);
DWORD WINAPI GetWindowNameThreadFunction(LPVOID lpParameter);
DWORD WINAPI GetWindowControlsThreadFunction(LPVOID lpParameter);
DWORD WINAPI GetEdgeURLControlsThreadFunction(LPVOID lpParameter);
DWORD WINAPI GetChromeURLControlsThreadFunction(LPVOID lpParameter);

void GetWindowNamesInThread(struct threaddata *pThreadData);
void GetWindowControlsInThread(struct threaddata *pThreadData);
void GetURLControlsInThread(struct threaddata *pThreadData);
//void GetChromeURLControlsInThread(struct threaddata *pThreadData);

SpyEditorScreen::SpyEditorScreen(wxWindow * parent, wxWindowID WXUNUSED(id), const wxString & title,
	const wxPoint & position, const wxSize & size)
	: wxFrame(NULL, -1, title, position, size, wxDEFAULT_FRAME_STYLE)
{
	initSize = size;
	posPoint = position;
	m_pParent = parent;
	oldProcessKey = "";
	oldProcessName = "";
	oldWindowName = "";
	// Window Tab 
	winRulesTxt = NULL;
	winRuleExecBtn = NULL;
	winRulesValueTxt = NULL;
	windowTitleTxt = NULL;
	windowNameTxt = NULL;
	windowDescTxt = NULL;
	windowTypeCmb = NULL;
	winControlButton = NULL;
	isURLMask = false;

	processListCmb = NULL;
	windowUpdateBtn = NULL;
	urlLB = NULL;
	urlHomePage = NULL;
	//urlIgnoreEvents = NULL;
	//urlIgnoreVideo = NULL;
	urlTextTxt = NULL;
	urlRuleTxt = NULL;
	urlResultTxt = NULL;
	urlNameTxt = NULL;
	urlDescTxt = NULL;
	urlAppNameTxt = NULL;
	urlAppDescTxt = NULL;
	urlControlButton = NULL;
	urlExecuteButton = NULL;
	urlMaskInfoButton = NULL;

	controlProcessListCmb = NULL;
	controlUpdateBtn = NULL;
	ctrlUserNameTxt = NULL;
	controlTypeTxt = NULL;
	ctrlUserNameTxt = NULL;
	ctrlClickAnnotationRuleTxt = NULL;
	ctrlTypeListCmb = NULL;
	screenImage = NULL;
	InitializeScreen();

	pDataFacade = DBG_NEW SPYDataFacade();
	pDataFacade->ReadDataFile();
	list <wstring> processList;
	processList.clear();
	pDataFacade->GetProcessList(&processList);
	list <wstring>::iterator itProcessList = processList.begin();
	while (itProcessList != processList.end())
	{
		wstring str = *itProcessList;
		prc_processLB->Append(str);
		itProcessList++;
	}

	list <wstring> urlList;
	urlList.clear();
	pDataFacade->GetURLList(&urlList);
	list <wstring>::iterator itURLList = urlList.begin();
	while (itURLList != urlList.end())
	{
		wstring str = *itURLList;
		urlLB->Append(str);
		itURLList++;
	}
	
}

SpyEditorScreen::~SpyEditorScreen()
{
	delete pDataFacade;
	m_pParent->Show();
}


void SpyEditorScreen::InitializeScreen()
{
	wxPanel *framePanel;
	wxSize winSize;

	winSize.SetHeight(this->GetSize().GetHeight()-100);
	winSize.SetWidth(this->GetSize().GetWidth() - 20);
	
	framePanel = DBG_NEW wxPanel(this, -1, wxPoint(0, 0), winSize, 0, "framePanel");
	framePanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));

	wxSize mainTabSize = framePanel->GetSize();

	mainTabSize.SetHeight(mainTabSize.GetHeight());
	wxFont myFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	tabSize = mainTabSize;
	urlCaptureBtn = DBG_NEW wxButton(framePanel, BTN_CAPTURE, _("Screen Capture"), wxPoint(mainTabSize.GetWidth()/2 - 100, 10), wxSize(200, 40));
	urlCaptureBtn->SetFont(myFont);
	urlCaptureBtn->SetBackgroundColour(wxColor(0xe6, 0xe6, 0x00));

	spyMainTab = DBG_NEW wxSimplebook(framePanel, -1, wxPoint(0, 60), mainTabSize, wxSUNKEN_BORDER, "spyMainTab");
	Maximize(true);
	wxSize newMainTabSize = this->GetSize();
	spyMainTab->SetSize(newMainTabSize);
	spyMaskTab = CreateMaskTab();

	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);


	spyProcessNoteBook = CreateMainProcessTab();
	spyWindowTab = CreateWindowTab();
	spyControlTab = CreateControlTab();
	spyUrlTab = CreateURLControlTab();
	DisableWindowTabControls();
	urlCaptureBtn->Enable(true);

	spyMainTab->ShowNewPage(spyProcessNoteBook);


}

wxNotebook *SpyEditorScreen::CreateMainProcessTab()
{
	spyProcessNoteBook = DBG_NEW wxNotebook(spyMainTab, -1, wxPoint(0, 0), spyMainTab->GetSize(), 0, "choice Panel");
	spyProcessNoteBook->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyProcessTab = CreateProcessTab();
	spyWebPageTab = CreateWebPageTab();
	spyProcessNoteBook->SetSelection(0);

	return spyProcessNoteBook;
}


wxWindow *SpyEditorScreen::CreateMaskTab()
{
	/****************************************************************************************************
	Creating controls for Mask Tab
	*****************************************************************************************************/
	int width;
	wxSize size;
	int yPos, xPos;


	wxFont myFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxFont myTextFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	wxPanel *maskPanel = NULL;
	maskPanel = DBG_NEW wxPanel(spyMainTab, -1, wxPoint(0, 0), spyMainTab->GetSize(), 0, "maskPanel");
	maskPanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyMainTab->AddPage(maskPanel, wxT("Mask"), true, -1);
	size = spyMainTab->GetSize();
	width = (size.GetWidth() - 50) / 2;

	processLbl1 = DBG_NEW wxStaticText(maskPanel, -1, _("Process:"), wxPoint(10, 5), wxSize(50, 25));
	processLbl1->SetFont(myFont);
	processLbl2 = DBG_NEW wxTextCtrl(maskPanel, -1, _("Process Full Name"), wxPoint(10, 35), wxSize(250, 50), wxTE_MULTILINE);

	windowLbl1 = DBG_NEW wxStaticText(maskPanel, -1, _("Windows:"), wxPoint(10, 90), wxSize(50, 25));
	windowLbl1->SetFont(myFont);
	windowLbl2 = DBG_NEW wxTextCtrl(maskPanel, -1, _("Window Full Name"), wxPoint(10, 120), wxSize(250, 50), wxTE_MULTILINE);

	processLbl2->Enable(false);
	windowLbl2->Enable(false);

	wxStaticText *maskListLbl = DBG_NEW wxStaticText(maskPanel, -1, _("Masks:"), wxPoint(10, 175), wxSize(50, 25));
	maskListLbl->SetFont(myFont);
	masksCB = DBG_NEW wxChoice(maskPanel, CMB_MASK_LIST, wxPoint(60, 175), wxSize(200, 150));

	newMaskBtn = DBG_NEW wxButton(maskPanel, BTN_NEW_MASK, _("New Mask"), wxPoint(20, 280), wxSize(75, 25));
	newMaskBtn->SetFont(myFont);

	wxStaticText *nameLbl1 = DBG_NEW wxStaticText(maskPanel, -1, _("Name:"), wxPoint(10, 330), wxSize(50, 25));
	nameLbl1->SetFont(myFont);
	maskNameTxt = DBG_NEW wxTextCtrl(maskPanel, -1, wxEmptyString, wxPoint(80, 330), wxSize(150, 25));

	wxArrayString maskTypes;
	maskTypes.Add(_("Solid"));
	maskTypes.Add(_("Blur"));

	wxStaticText *typeLbl1 = DBG_NEW wxStaticText(maskPanel, -1, _("Type:"), wxPoint(10, 360), wxSize(50, 25));
	typeLbl1->SetFont(myFont);
	maskTypeCmb = DBG_NEW wxChoice(maskPanel, -1, wxPoint(80, 360), wxSize(150, 25), maskTypes);

	wxStaticText *areaLbl1 = DBG_NEW wxStaticText(maskPanel, -1, _("Area:"), wxPoint(10, 390), wxSize(50, 25));
	areaLbl1->SetFont(myFont);
	areaLbl2 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 390), wxSize(150, 25));
	areaLbl2->SetFont(myTextFont);
	saveAreaBtn = DBG_NEW wxButton(maskPanel, BTN_SAVE_MASK, _("S"), wxPoint(240, 390), wxSize(20, 20));
	saveAreaBtn->SetFont(myFont);

	wxStaticText *matchLbl1 = DBG_NEW wxStaticText(maskPanel, -1, _("Match 1:"), wxPoint(10, 420), wxSize(50, 25));
	matchLbl1->SetFont(myFont);
	matchDesc1 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 420), wxSize(150, 25));
	matchDesc1->SetFont(myTextFont);
	saveMatchBtn1 = DBG_NEW wxButton(maskPanel, BTN_SAVE_IMAGE1, _("S"), wxPoint(240, 420), wxSize(20, 20));
	saveMatchBtn1->SetFont(myFont);

	wxStaticText *matchLbl2 = DBG_NEW wxStaticText(maskPanel, -1, _("Match 2:"), wxPoint(10, 450), wxSize(50, 25));
	matchLbl2->SetFont(myFont);
	matchDesc2 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 450), wxSize(150, 25));
	matchDesc2->SetFont(myTextFont);
	saveMatchBtn2 = DBG_NEW wxButton(maskPanel, BTN_SAVE_IMAGE2, _("S"), wxPoint(240, 450), wxSize(20, 20));
	saveMatchBtn2->SetFont(myFont);

	wxStaticText *matchLbl3 = DBG_NEW wxStaticText(maskPanel, -1, _("Match 3:"), wxPoint(10, 480), wxSize(50, 25));
	matchLbl3->SetFont(myFont);
	matchDesc3 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 480), wxSize(150, 25));
	matchDesc3->SetFont(myTextFont);
	saveMatchBtn3 = DBG_NEW wxButton(maskPanel, BTN_SAVE_IMAGE3, _("S"), wxPoint(240, 480), wxSize(20, 20));
	saveMatchBtn3->SetFont(myFont);

	wxStaticText *matchLbl4 = DBG_NEW wxStaticText(maskPanel, -1, _("Match 4:"), wxPoint(10, 510), wxSize(50, 25));
	matchLbl4->SetFont(myFont);
	matchDesc4 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 510), wxSize(150, 25));
	matchDesc4->SetFont(myTextFont);
	saveMatchBtn4 = DBG_NEW wxButton(maskPanel, BTN_SAVE_IMAGE4, _("S"), wxPoint(240, 510), wxSize(20, 20));
	saveMatchBtn4->SetFont(myFont);

	wxStaticText *matchLbl5 = DBG_NEW wxStaticText(maskPanel, -1, _("Match 5:"), wxPoint(10, 540), wxSize(50, 25));
	matchLbl5->SetFont(myFont);
	matchDesc5 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 540), wxSize(150, 25));
	matchDesc5->SetFont(myTextFont);
	saveMatchBtn5 = DBG_NEW wxButton(maskPanel, BTN_SAVE_IMAGE5, _("S"), wxPoint(240, 540), wxSize(20, 20));
	saveMatchBtn5->SetFont(myFont);

	wxStaticText *matchLbl6 = DBG_NEW wxStaticText(maskPanel, -1, _("Match 6:"), wxPoint(10, 570), wxSize(50, 25));
	matchLbl6->SetFont(myFont);
	matchDesc6 = DBG_NEW wxStaticText(maskPanel, -1, _(""), wxPoint(80, 570), wxSize(150, 25));
	matchDesc6->SetFont(myTextFont);
	saveMatchBtn6 = DBG_NEW wxButton(maskPanel, BTN_SAVE_IMAGE6, _("S"), wxPoint(240, 570), wxSize(20, 20));
	saveMatchBtn6->SetFont(myFont);

	size = spyMainTab->GetSize();
	width = size.GetWidth() - 320;
	yPos = 10;
	xPos = 300;

	DBG_NEW wxStaticBox(maskPanel, wxID_STATIC, _("Screen Image"), wxPoint(xPos, yPos), wxSize(width, spyMainTab->GetSize().GetHeight() - 150));
	imagePanel = DBG_NEW MaskImagePanel(maskPanel, wxPoint(xPos + 20, yPos + 20), wxSize(width - 40, spyMainTab->GetSize().GetHeight() - 190));
	imagePanel->SetBackgroundColour(wxColor(0xff, 0xff, 0xff));

	//screenImage = DBG_NEW wxImage();
	//bool flg = screenImage->LoadFile(wxT("C:\\Recorder\\ScreenImage.png"), wxBITMAP_TYPE_PNG);
	//imagePanel->SetImage(screenImage);
	//delete screenImage;
	
	maskBackToProcessButton = DBG_NEW wxButton(maskPanel, BTN_MASK_BACKTO_PROCESS, _("Back to Process"), wxPoint(xPos, maskPanel->GetSize().GetHeight() - 120), wxSize(120, 25));
	maskBackToWindowButton = DBG_NEW wxButton(maskPanel, BTN_MASK_BACKTO_WINDOW, _("Back To Window"), wxPoint(xPos + 135, maskPanel->GetSize().GetHeight() - 120), wxSize(120, 25));
	maskBackToControlButton = DBG_NEW wxButton(maskPanel, BTN_MASK_BACKTO_CONTROLS, _("Back To Controls"), wxPoint(xPos + 270, maskPanel->GetSize().GetHeight() - 120), wxSize(120, 25));
	//openFileBtn = DBG_NEW wxButton(maskPanel, BTN_OPEN_SCREEN, _("Open File"), wxPoint(xPos + 400, maskPanel->GetSize().GetHeight() - 120), wxSize(100, 25));
	saveMaskBtn = DBG_NEW wxButton(maskPanel, BTN_MASK_SAVE, _("Save"), wxPoint(xPos + 400, maskPanel->GetSize().GetHeight() - 120), wxSize(100, 25));

	//openFileBtn->Enable(false);
	areaLbl2->Enable(false);
	matchDesc1->Enable(false);
	matchDesc2->Enable(false);
	matchDesc3->Enable(false);
	matchDesc4->Enable(false);
	matchDesc5->Enable(false);
	matchDesc6->Enable(false);
	saveMaskBtn->Enable(false);
	
	maskNameTxt->Enable(false);
	maskTypeCmb->Enable(false);
	saveAreaBtn->Enable(false);
	saveMatchBtn1->Enable(false);
	saveMatchBtn2->Enable(false);
	saveMatchBtn3->Enable(false);
	saveMatchBtn4->Enable(false);
	saveMatchBtn5->Enable(false);
	saveMatchBtn6->Enable(false);


	return maskPanel;
}


void SpyEditorScreen::OnOpenScreenFile(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog
		openFileDialog(this, _("Open Screen Bitmap File"), "", "",
			"BMP files (*.bmp)|*.bmp|PNG file (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	wxString wFileName = openFileDialog.GetPath();
	wxString wFile = openFileDialog.GetFilename();
	
	wxFileName wFullName(wFileName);
	wxString wExt = wFullName.GetExt().MakeLower();


	//if (screenImage != NULL)
	//	delete screenImage;
	//screenImage = DBG_NEW wxImage();
	if (wExt == "bmp")
	{
		HANDLE hFile = CreateFile(wFileName.ToStdWstring().c_str(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);

		BITMAPFILEHEADER bfh;
		BITMAPINFO bmi;
		DWORD dwBytesRead = 0;

		ReadFile(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
		ReadFile(hFile, (LPSTR)&bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
		char *maskBuffer = imagePanel->SaveHeaderAndGetBuffer(&bfh, &bmi.bmiHeader);
		ReadFile(hFile, (LPSTR)maskBuffer, imagePanel->GetBitImageSize(), &dwBytesRead, NULL);
		if (bmi.bmiHeader.biBitCount == 24)
			imagePanel->ConvertImageTo32();


		CloseHandle(hFile);
	}
	else if (wExt == "png")
	{
		unsigned error;
		unsigned char* image = 0;
		unsigned width, height;

		error = lodepng_decode32_file(&image, &width, &height, wFileName.c_str());

		int bpp = 32;
		BITMAPFILEHEADER bfh;
		BITMAPINFO bmi;

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -((int)height);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = bpp;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;

		int headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			(bpp <= 8 ? (1 << bpp) : 0) * sizeof(RGBQUAD) /* palette size */;
		size_t frameSize = width * height * 4;

		int file_size = headerSize + frameSize;

		bfh.bfType = 0x4d42; /* "BM" in little-endian */
		bfh.bfSize = file_size;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = headerSize;

		char *maskBuffer = imagePanel->SaveHeaderAndGetBuffer(&bfh, &bmi.bmiHeader);
		CopyMemory(maskBuffer, image, frameSize);
		free(image);
	}
	

	//bool flg = false;
	//if (wExt == "bmp")
	//	flg = screenImage->LoadFile(wFileName, wxBITMAP_TYPE_BMP);
	//else if (wExt == "jpg")
	//	flg = screenImage->LoadFile(wFileName, wxBITMAP_TYPE_JPEG);
	//else if (wExt == "png")
	//	flg = screenImage->LoadFile(wFileName, wxBITMAP_TYPE_PNG);
	//if (flg)
	imagePanel->SetImage();

	saveMaskBtn->Enable(true);
}


void SpyEditorScreen::OnSaveMaskImage(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	areaLbl2->SetLabelText(str);
	imagePanel->SetMaskImageRect(rectVal);
	saveMaskBtn->Enable(true);
}

void SpyEditorScreen::OnSaveMatchImage1(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc1->SetLabelText(str);
	imagePanel->SetMatchImage1Rect(rectVal);
}

void SpyEditorScreen::OnSaveMatchImage2(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc2->SetLabelText(str);
	imagePanel->SetMatchImage2Rect(rectVal);
	saveMaskBtn->Enable(true);
}

void SpyEditorScreen::OnSaveMatchImage3(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc3->SetLabelText(str);
	imagePanel->SetMatchImage3Rect(rectVal);
	saveMaskBtn->Enable(true);
}

void SpyEditorScreen::OnSaveMatchImage4(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc4->SetLabelText(str);
	imagePanel->SetMatchImage4Rect(rectVal);
	saveMaskBtn->Enable(true);
}

void SpyEditorScreen::OnSaveMatchImage5(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc5->SetLabelText(str);
	imagePanel->SetMatchImage5Rect(rectVal);
	saveMaskBtn->Enable(true);
}

void SpyEditorScreen::OnSaveMatchImage6(wxCommandEvent& WXUNUSED(event))
{
	RECT rectVal = imagePanel->GetCurrentRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc6->SetLabelText(str);
	imagePanel->SetMatchImage6Rect(rectVal);
	saveMaskBtn->Enable(true);
}

void SpyEditorScreen::OnSaveMask(wxCommandEvent& WXUNUSED(event))
{
	wxString strMaskName = maskNameTxt->GetValue();
	if (strMaskName == "")
		return;

	wxString strWindowName = windowLbl2->GetLabelText();

	string maskName = strMaskName.mb_str();
	SpyMaskData *pMaskData = pDataFacade->GetMaskData(maskName);
	if (pMaskData == NULL)
	{
		pMaskData = DBG_NEW  SpyMaskData();
		masksCB->AppendString(maskName);
	}

	int nSel = maskTypeCmb->GetSelection();
	wxString strMaskType = maskTypeCmb->GetString(nSel);
	string maskType = strMaskType.mb_str();
	pMaskData->SetMaskName(maskName);
	pMaskData->SetMaskType(maskType);
	pMaskData->SetImageRect(imagePanel->GetImageRect());
	pMaskData->SetImage1Rect(imagePanel->GetMatchImage1Rect());
	pMaskData->SetImage2Rect(imagePanel->GetMatchImage2Rect());
	pMaskData->SetImage3Rect(imagePanel->GetMatchImage3Rect());
	pMaskData->SetImage4Rect(imagePanel->GetMatchImage4Rect());
	pMaskData->SetImage5Rect(imagePanel->GetMatchImage5Rect());
	pMaskData->SetImage6Rect(imagePanel->GetMatchImage6Rect());
	pMaskData->SetMaskRect(imagePanel->GetMaskImageRect());
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bmiHeader;

	char *buffer = imagePanel->GetImage(&bfh, &bmiHeader);

	pMaskData->SetMaskImage(&bfh, &bmiHeader, buffer);

	// Here we should pass the window or url name.
	string windowName = strWindowName.mb_str();
	pDataFacade->AddWindowMask(windowName, pMaskData);

}

void SpyEditorScreen::InitMaskScreen()
{
	maskNameTxt->SetValue(wxT(""));
	maskTypeCmb->Select(0);
	maskNameTxt->Enable(true);
	maskTypeCmb->Enable(true);
	saveAreaBtn->Enable(true);
	saveMatchBtn1->Enable(true);
	saveMatchBtn2->Enable(true);
	saveMatchBtn3->Enable(true);
	saveMatchBtn4->Enable(true);
	saveMatchBtn5->Enable(true);
	saveMatchBtn6->Enable(true);
	//openFileBtn->Enable(true);
	areaLbl2->Enable(true);
	matchDesc1->Enable(true);
	matchDesc2->Enable(true);
	matchDesc3->Enable(true);
	matchDesc4->Enable(true);
	matchDesc5->Enable(true);
	matchDesc6->Enable(true);

	areaLbl2->SetLabelText(wxT(""));
	matchDesc1->SetLabelText(wxT(""));
	matchDesc2->SetLabelText(wxT(""));
	matchDesc3->SetLabelText(wxT(""));
	matchDesc4->SetLabelText(wxT(""));
	matchDesc5->SetLabelText(wxT(""));
	matchDesc6->SetLabelText(wxT(""));
	imagePanel->ClearImage();
	
}

void SpyEditorScreen::OnNewMask(wxCommandEvent& event)
{
	InitMaskScreen();
	OnOpenScreenFile(event);
}

void SpyEditorScreen::OnMaskSelChanged(wxCommandEvent& WXUNUSED(event))
{
	string maskName = "";
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bmiHeader;

	InitMaskScreen();

	int nSel = masksCB->GetSelection();
	maskName = string(masksCB->GetString(nSel).mb_str());
	SpyMaskData *pMaskData = pDataFacade->GetMaskData(maskName);
	pDataFacade->GetMaskImageData(pMaskData);

	char *src = pMaskData->GetMaskImage(&bfh, &bmiHeader);
	char *dest = imagePanel->SaveHeaderAndGetBuffer(&bfh, &bmiHeader);

	int width = bmiHeader.biWidth;
	int height = bmiHeader.biHeight;
	if (height < 0)
		height = -height;
	size_t sizeBuffer = width * height * 4L;
	CopyMemory(dest, src, sizeBuffer);
	imagePanel->SetImage();
	imagePanel->SetMatchImage1Rect(pMaskData->GetImage1Rect());
	imagePanel->SetMatchImage2Rect(pMaskData->GetImage2Rect());
	imagePanel->SetMatchImage3Rect(pMaskData->GetImage3Rect());
	imagePanel->SetMatchImage4Rect(pMaskData->GetImage4Rect());
	imagePanel->SetMatchImage5Rect(pMaskData->GetImage5Rect());
	imagePanel->SetMatchImage6Rect(pMaskData->GetImage6Rect());
	imagePanel->SetMaskImageRect(pMaskData->GetMaskRect());
	

	RECT rectVal = pMaskData->GetMaskRect();
	wxString str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	areaLbl2->SetLabelText(str);

	rectVal = pMaskData->GetImage1Rect();
	str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc1->SetLabelText(str);

	rectVal = pMaskData->GetImage2Rect();
	str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);
	if (str == "{0,0,0,0}")
		str = "";
	matchDesc2->SetLabelText(str);

	rectVal = pMaskData->GetImage3Rect();
	str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);

	if (str == "{0,0,0,0}")
		str = "";
	matchDesc3->SetLabelText(str);

	rectVal = pMaskData->GetImage4Rect();
	str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);

	if (str == "{0,0,0,0}")
		str = "";
	matchDesc4->SetLabelText(str);

	rectVal = pMaskData->GetImage5Rect();
	str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);

	if (str == "{0,0,0,0}")
		str = "";
	matchDesc5->SetLabelText(str);

	rectVal = pMaskData->GetImage6Rect();
	str = wxString::Format("{%d,%d,%d,%d}", rectVal.left, rectVal.top, rectVal.right, rectVal.bottom);

	if (str == "{0,0,0,0}")
		str = "";
	matchDesc6->SetLabelText(str);

	maskNameTxt->SetValue(maskName);
	nSel = maskTypeCmb->FindString(pMaskData->GetMaskType());
	maskTypeCmb->Select(nSel);
}

wxWindow *SpyEditorScreen::CreateControlTab()
{
	/****************************************************************************************************
	Creating controls for Control Tab
	*****************************************************************************************************/
	int width;
	wxSize size;
	int yPos, xPos;

	wxArrayString processList;
	processList.Add(_("Process 1"));
	processList.Add(_("Process 2"));
	processList.Add(_("Process 3"));

	wxArrayString windowList;
	windowList.Add(_("Window 1"));
	windowList.Add(_("Window 2"));
	windowList.Add(_("Window 3"));


	wxFont myFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxPanel *controlPanel = NULL;
	controlPanel = DBG_NEW wxPanel(spyMainTab, -1, wxPoint(0, 0), spyMainTab->GetSize(), 0, "controlPanel");
	controlPanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyMainTab->AddPage(controlPanel, wxT("Controls"), true, -1);
	size = spyMainTab->GetSize();
	width = (size.GetWidth() - 50) / 2;
	wxStaticText *processLbl = DBG_NEW wxStaticText(controlPanel, -1, _("Process"), wxPoint(10, 5), wxSize(50, 25));
	processLbl->SetFont(myFont);
	controlProcessListCmb = DBG_NEW wxChoice(controlPanel, -1, wxPoint(10, 30), wxSize(400, 25), processList);

	wxStaticText *windowLbl = DBG_NEW wxStaticText(controlPanel, -1, _("Windows"), wxPoint(10, 65), wxSize(50, 25));
	windowLbl->SetFont(myFont);
	controlWindowListCmb = DBG_NEW wxChoice(controlPanel, -1, wxPoint(10, 95), wxSize(400, 25), windowList);

	wxStaticText *controlsLbl = DBG_NEW wxStaticText(controlPanel, -1, _("Controls"), wxPoint(10, 130), wxSize(50, 25));
	controlsLbl->SetFont(myFont);

	windowControlList = DBG_NEW wxListBox(controlPanel, WINDOW_CONTROL_LIST, wxPoint(10, 160), wxSize(400,200));

	width = 300;
	yPos = 370;
	xPos = 10;
	wxStaticBox *controlStaticBox = DBG_NEW wxStaticBox(controlPanel, wxID_STATIC, _("Control Details"), wxPoint(5, yPos), wxSize(410, 175));
	wxStaticText *controlNameLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Name"), wxPoint(xPos, yPos + 30), wxSize(85, 25));
	wxStaticText *controlTypeLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Type"), wxPoint(xPos, yPos + 65), wxSize(85, 25));
	wxStaticText *ctrlClickAnnotationRuleLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Annotation"), wxPoint(xPos, yPos + 100), wxSize(50, 25));
	wxStaticText *cntrlControlTypeLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Enter Type"), wxPoint(xPos, yPos + 135), wxSize(100, 25));
	controlStaticBox->SetFont(myFont);
	controlNameLbl->SetFont(myFont);
	controlTypeLbl->SetFont(myFont);
	ctrlClickAnnotationRuleLbl->SetFont(myFont);
	cntrlControlTypeLbl->SetFont(myFont);

	xPos += 100;

	ctrlUserNameTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 30), wxSize(width, 25));
	controlTypeTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 65), wxSize(150, 25));
	ctrlClickAnnotationRuleTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 100), wxSize(width, 25));

	wxArrayString controlTypeList;
	controlTypeList.Add(_("Control"));
	controlTypeList.Add(_("Row Header"));
	controlTypeList.Add(_("Column Header"));
	
	ctrlTypeListCmb = DBG_NEW wxChoice(controlPanel, -1, wxPoint(xPos, yPos + 135), wxSize(150, 25), controlTypeList);

	controlTypeTxt->Enable(false);
	ctrlUserNameTxt->Enable(false);
	ctrlClickAnnotationRuleTxt->Enable(false);
	ctrlTypeListCmb->Enable(false);
	ctrlUserNameTxt->Enable(false);

	controlBackToProcessButton = DBG_NEW wxButton(controlPanel, BTN_CONTROL_BACKTO_PROCESS, _("Back to Process"), wxPoint(10, yPos + 180), wxSize(100, 25));
	controlBackToWindowButton = DBG_NEW wxButton(controlPanel, BTN_CONTROL_BACKTO_WINDOW, _("Back To Window"), wxPoint(120, yPos+180), wxSize(100, 25));
	controlMaskInfoButton = DBG_NEW wxButton(controlPanel, BTN_MASK_WINDOW , _("Mask Info"), wxPoint(230, yPos+180), wxSize(100, 25));
	saveControlButton = DBG_NEW wxButton(controlPanel, BTN_CONTROL_SAVE, _("Save"), wxPoint(340, yPos+180), wxSize(80, 25));

	controlMaskInfoButton->Enable(true);
	saveControlButton->Enable(false);
	return controlPanel;
}

wxWindow *SpyEditorScreen::CreateURLControlTab()
{
	/****************************************************************************************************
	Creating controls for Control Tab
	*****************************************************************************************************/
	int width;
	wxSize size;
	int yPos, xPos;

	wxArrayString UrlList;
	UrlList.Add(_("URL 1"));

	wxFont myFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxPanel *controlPanel = NULL;
	controlPanel = DBG_NEW wxPanel(spyMainTab, -1, wxPoint(0, 0), spyMainTab->GetSize(), 0, "controlPanel");
	controlPanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyMainTab->AddPage(controlPanel, wxT("URL Controls"), true, -1);
	size = spyMainTab->GetSize();
	width = (size.GetWidth() - 50) / 2;

	wxStaticText *windowLbl = DBG_NEW wxStaticText(controlPanel, -1, _("URL"), wxPoint(10, 35), wxSize(50, 25));
	windowLbl->SetFont(myFont);
	urlControlListCmb = DBG_NEW wxChoice(controlPanel, -1, wxPoint(10, 65), wxSize(400, 25), UrlList);

	wxStaticText *controlsLbl = DBG_NEW wxStaticText(controlPanel, -1, _("Controls"), wxPoint(10, 100), wxSize(50, 25));
	controlsLbl->SetFont(myFont);

	urlControlLB = DBG_NEW wxListBox(controlPanel, URL_CONTROL_LIST, wxPoint(10, 135), wxSize(400, 200));

	width = 300;
	yPos = 350;
	xPos = 15;
	wxStaticBox *controlStaticBox = DBG_NEW wxStaticBox(controlPanel, wxID_STATIC, _("Control Details"), wxPoint(5, yPos), wxSize(410, 180));
	wxStaticText *controlTypeLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Type"), wxPoint(xPos, yPos +35), wxSize(85, 25));
	wxStaticText *ctrlUserNameLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Name"), wxPoint(xPos, yPos + 70), wxSize(50, 25));
	wxStaticText *ctrlClickAnnotationRuleLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("Annotation"), wxPoint(xPos, yPos + 105), wxSize(50, 25));
	wxStaticText *ctrlXPathLbl = DBG_NEW wxStaticText(controlPanel, wxID_STATIC, _("XPath"), wxPoint(xPos, yPos + 140), wxSize(50, 25));

	controlStaticBox->SetFont(myFont);
	controlTypeLbl->SetFont(myFont);
	ctrlClickAnnotationRuleLbl->SetFont(myFont);
	ctrlUserNameLbl->SetFont(myFont);
	ctrlXPathLbl->SetFont(myFont);

	xPos += 100;
	width = 280;
	urlControlTypeTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos+35), wxSize(width, 25));
	urlCtrlUserNameTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 70), wxSize(width, 25));
	urlCtrlClickAnnotationRuleTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 105), wxSize(width, 25));
	urlCtrlXPathTxt = DBG_NEW wxTextCtrl(controlPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 140), wxSize(width, 25));
	urlControlTypeTxt->Enable(false);
	urlCtrlClickAnnotationRuleTxt->Enable(false);
	urlCtrlXPathTxt->Enable(false);
	urlCtrlUserNameTxt->Enable(false);

	urlControlBackToURLButton = DBG_NEW wxButton(controlPanel, BTN_CONTROL_BACKTO_URL, _("Back to URL"), wxPoint(145, yPos+190), wxSize(100, 25));
	urlControlMaskInfoButton = DBG_NEW wxButton(controlPanel, BTN_MASK_URL_WINDOW, _("Mask Info"), wxPoint(255, yPos + 190), wxSize(100, 25));
	urlSaveControlButton = DBG_NEW wxButton(controlPanel, BTN_URL_CONTROL_SAVE, _("Save"), wxPoint(365, yPos + 190), wxSize(50, 25));

	urlControlMaskInfoButton->Enable(true);
	urlSaveControlButton->Enable(false);

	return controlPanel;
}


void SpyEditorScreen::OnWindowControlSelChanged(wxCommandEvent& event)
{
	int index = event.GetSelection();

	vector <SpyControlData*> controlList = pDataFacade->GetControls();

	if (index <= controlList.size())
	{
		SpyControlData* pControl = controlList[index];

		ctrlUserNameTxt->SetValue(pControl->GetControlUserName());
		controlTypeTxt->SetValue(pControl->GetControlTypeName());
		ctrlClickAnnotationRuleTxt->SetValue(pControl->GetClickAnnotation());
		int selected = 0;
		selected = ctrlTypeListCmb->FindString(wxString(pControl->GetScreenControlType()));
		ctrlTypeListCmb->SetSelection(-1);
		if (selected != wxNOT_FOUND)
		{
			ctrlTypeListCmb->SetSelection(selected);
		}
		ctrlTypeListCmb->Enable(true);

		controlTypeTxt->Enable(false);
		ctrlClickAnnotationRuleTxt->Enable(true);
		ctrlUserNameTxt->Enable(true);
		saveControlButton->Enable(true);
	}
}


void SpyEditorScreen::OnURLControlSelChanged(wxCommandEvent& event)
{
	int index = event.GetSelection();

	vector <SpyControlData*> controlList = pDataFacade->GetControls();

	if (index <= controlList.size())
	{
		SpyControlData* pControl = controlList[index];
		urlCtrlUserNameTxt->SetValue(pControl->GetControlUserName());
		urlControlTypeTxt->SetValue(pControl->GetControlTypeName());
		urlCtrlClickAnnotationRuleTxt->SetValue(pControl->GetClickAnnotation());
		urlCtrlXPathTxt->SetValue(pControl->GetEnterAnnotation());
		urlControlTypeTxt->Enable(false);
		urlCtrlClickAnnotationRuleTxt->Enable(true);
		urlCtrlXPathTxt->Enable(true);
		urlCtrlUserNameTxt->Enable(true);
		urlSaveControlButton->Enable(true);
	}
}

wxWindow *SpyEditorScreen::CreateWebPageTab()
{
	/****************************************************************************************************
	Creating controls for Window Tab
	*****************************************************************************************************/
	int width;
	wxSize size;
	int xPos;

	wxArrayString urlList;
	urlList.clear();


	wxFont myFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxPanel *webPagePanel = NULL;
	webPagePanel = DBG_NEW wxPanel(spyProcessNoteBook, -1, wxPoint(0, 0), spyProcessNoteBook->GetSize(), 0, "webPagePanel");
	webPagePanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyProcessNoteBook->AddPage(webPagePanel, wxT("Web Page"), true, -1);
	size = spyMainTab->GetSize();
	width = (size.GetWidth() - 50) / 2;
	wxStaticText *urlLbl = DBG_NEW wxStaticText(webPagePanel, -1, _("URLs"), wxPoint(10, 10), wxSize(50, 25));
	urlLbl->SetFont(myFont);
	urlLB = DBG_NEW wxListBox(webPagePanel, LIST_URLS, wxPoint(10, 35), wxSize(400, 200));

	wxStaticBox *urlStaticBox = DBG_NEW wxStaticBox(webPagePanel, wxID_STATIC, _("URL Details"), wxPoint(5, 250), wxSize(410, 425));
	wxStaticText *urlTextLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("URL"), wxPoint(15, 280), wxSize(50, 25));
	wxStaticText *urlRuleLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("Rule"), wxPoint(15,315), wxSize(50, 25));
	wxStaticText *urlResulLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("Result"), wxPoint(15, 385), wxSize(50, 25));
	wxStaticText *urlNameLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("Name"), wxPoint(15, 420), wxSize(50, 25));
	wxStaticText *urlDescLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("Description"), wxPoint(15, 455), wxSize(50, 25));
	wxStaticText *urlAppNameLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("App. Name"), wxPoint(15, 560), wxSize(50, 25));
	wxStaticText *urlAppDescLbl = DBG_NEW wxStaticText(webPagePanel, wxID_STATIC, _("Description"), wxPoint(15, 595), wxSize(50, 25));

	urlStaticBox->SetFont(myFont);
	urlTextLbl->SetFont(myFont);
	urlRuleLbl->SetFont(myFont);
	urlResulLbl->SetFont(myFont);
	urlNameLbl->SetFont(myFont);
	urlDescLbl->SetFont(myFont);
	urlAppNameLbl->SetFont(myFont);
	urlAppDescLbl->SetFont(myFont);


	xPos = 95;
	width = 300;

	urlTextTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_TEXT, wxEmptyString, wxPoint(xPos, 280), wxSize(width, 25));
	urlRuleTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_RULE, wxEmptyString, wxPoint(xPos, 315), wxSize(width, 25));
	urlExecuteButton = DBG_NEW wxButton(webPagePanel, URL_EXECUTE_BUTTON, _("Execute"), wxPoint(xPos, 350), wxSize(100,25));
	urlResultTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_RESULT, wxEmptyString, wxPoint(xPos, 385), wxSize(width, 25));
	urlNameTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_NAME, wxEmptyString, wxPoint(xPos, 420), wxSize(width, 25));
	urlDescTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_DESC, wxEmptyString, wxPoint(xPos, 455), wxSize(width, 70));
	urlHomePage = DBG_NEW wxCheckBox(webPagePanel, URL_ISHOME, _("Is Home Page?"), wxPoint(xPos, 535), wxDefaultSize);
	urlAppNameTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_APPNAME, wxEmptyString, wxPoint(xPos, 560), wxSize(width, 25));
	urlAppDescTxt = DBG_NEW wxTextCtrl(webPagePanel, URL_APPDESC, wxEmptyString, wxPoint(xPos, 595), wxSize(width, 70));

	urlSaveButton = DBG_NEW wxButton(webPagePanel, URL_SAVE, _("Save"), wxPoint(365, 680), wxSize(50, 25));
	urlControlButton = DBG_NEW wxButton(webPagePanel, URL_CONTROL , _("Control Details"), wxPoint(255, 680), wxSize(100, 25));

	urlTextTxt->Enable(false);
	urlRuleTxt->Enable(false);
	urlExecuteButton->Enable(false);
	urlResultTxt->Enable(false);
	urlNameTxt->Enable(false);
	urlDescTxt->Enable(false);
	urlAppNameTxt->Enable(false);
	urlAppDescTxt->Enable(false);
	urlHomePage->Enable(false);
	urlControlButton->Enable(false);
	urlSaveButton->Enable(false);


	return webPagePanel;
}

wxWindow *SpyEditorScreen::CreateWindowTab()
{
	/****************************************************************************************************
	Creating controls for Window Tab
	*****************************************************************************************************/
	int width;
	wxSize size;
	int yPos, xPos;

	wxArrayString processList;
	processList.Add(_("Process 1"));
	processList.Add(_("Process 2"));
	processList.Add(_("Process 3"));

	wxFont myFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxPanel *windowPanel = NULL;
	windowPanel = DBG_NEW wxPanel(spyMainTab, -1, wxPoint(0, 0), spyMainTab->GetSize(), 0, "windowPanel");
	windowPanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyMainTab->AddPage(windowPanel, wxT("Windows"), true, -1);
	size = spyMainTab->GetSize();
	width = (size.GetWidth() - 50) / 2;
	wxStaticText *processLbl = DBG_NEW wxStaticText(windowPanel, -1, _("Process"), wxPoint(10, 5), wxSize(50, 25));
	processLbl->SetFont(myFont);
	processListCmb = DBG_NEW wxChoice(windowPanel, WINDOW_PROCESS_LIST, wxPoint(10, 30), wxSize(400, 25), processList);

	wxStaticText *windowLbl = DBG_NEW wxStaticText(windowPanel, -1, _("Windows"), wxPoint(10, 75), wxSize(50, 25));
	windowLbl->SetFont(myFont);

	windowLB = DBG_NEW wxListBox(windowPanel, WINDOW_LIST, wxPoint(10, 110), wxSize(400, 200));

	yPos = 330;
	width = 300;
	xPos = 15;

	wxStaticBox* windowStaticBox = DBG_NEW wxStaticBox(windowPanel, wxID_STATIC, _("Window Details"), wxPoint(5, yPos), wxSize(410, 370));
	yPos += 30;
	wxStaticText *winRuleLbl = DBG_NEW wxStaticText(windowPanel, wxID_STATIC, _("Rule"), wxPoint(xPos,yPos), wxSize(50, 25));
	wxStaticText *winRuleValueLbl = DBG_NEW wxStaticText(windowPanel, wxID_STATIC, _("Result"), wxPoint(xPos, yPos + 70), wxSize(50, 25));
	winRuleLbl->SetFont(myFont);
	winRuleValueLbl->SetFont(myFont);

	winRulesTxt = DBG_NEW wxTextCtrl(windowPanel, -1, wxEmptyString, wxPoint(xPos + 90, yPos), wxSize(width, 25));
	winRuleExecBtn = DBG_NEW wxButton(windowPanel, BTN_WINDOW_EXECUTE, _("Execute"), wxPoint(xPos + 90, yPos + 35), wxSize(50, 25));
	winRulesValueTxt = DBG_NEW wxTextCtrl(windowPanel, -1, wxEmptyString, wxPoint(xPos + 90, yPos + 70), wxSize(width, 25));

	wxStaticText *windowTitleLbl = DBG_NEW wxStaticText(windowPanel, wxID_STATIC, _("Title"), wxPoint(xPos, yPos + 105), wxSize(85, 25));
	wxStaticText *windowNameLbl = DBG_NEW wxStaticText(windowPanel, wxID_STATIC, _("Name"), wxPoint(xPos, yPos + 135), wxSize(85, 25));
	wxStaticText *windowDescLbl = DBG_NEW wxStaticText(windowPanel, wxID_STATIC, _("Description"), wxPoint(xPos, yPos + 165), wxSize(85, 25));
	wxStaticText *windowTypeLbl = DBG_NEW wxStaticText(windowPanel, wxID_STATIC, _("Type"), wxPoint(xPos, yPos + 270), wxSize(85, 25));

	windowTitleLbl->SetFont(myFont);
	windowStaticBox->SetFont(myFont);
	windowNameLbl->SetFont(myFont);
	windowDescLbl->SetFont(myFont);
	windowTypeLbl->SetFont(myFont);

	xPos += 90;

	windowTitleTxt = DBG_NEW wxTextCtrl(windowPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 105), wxSize(width, 25));
	windowNameTxt = DBG_NEW wxTextCtrl(windowPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 135), wxSize(width, 25));
	windowDescTxt = DBG_NEW wxTextCtrl(windowPanel, -1, wxEmptyString, wxPoint(xPos, yPos + 165), wxSize(width, 95));

	wxArrayString windowTypes;
	windowTypes.Add(_("Main Window"));
	windowTypes.Add(_("Child Window"));
	windowTypes.Add(_("Container Window"));
	windowTypeCmb = DBG_NEW wxChoice(windowPanel, -1, wxPoint(xPos, yPos + 270), wxDefaultSize, windowTypes);
	winHomePage = DBG_NEW wxCheckBox(windowPanel, WIN_ISMAIN, _("Is Main Window?"), wxPoint(xPos, yPos + 305), wxDefaultSize);


	winControlButton = DBG_NEW wxButton(windowPanel, BTN_WINDOW_CONTROLS, _("Control Details"), wxPoint(255, yPos + 345), wxSize(100, 25));
	saveWindowButton = DBG_NEW wxButton(windowPanel, BTN_SAVE_WINDOW, _("Save"), wxPoint(365, yPos + 345), wxSize(50, 25));
	winBackToWindowButton = DBG_NEW wxButton(windowPanel, BTN_BACKTO_PROCESS, _("Back to Process"), wxPoint(145, yPos+ 345), wxSize(100, 25));

	return windowPanel;
}

void SpyEditorScreen::DisableWindowTabControls()
{
	winRulesTxt->Enable(false);
	winRuleExecBtn->Enable(true);
	winRulesValueTxt->Enable(false);
	windowTitleTxt->Enable(false);
	windowNameTxt->Enable(false);
	windowDescTxt->Enable(false);
	windowTypeCmb->Enable(false);

	winControlButton->Enable(false);
	saveWindowButton->Enable(false);
}

void SpyEditorScreen::EnableWindowTabControls()
{
	winRulesTxt->Enable(true);
	winRuleExecBtn->Enable(true);
	windowTitleTxt->Enable(true);
	windowNameTxt->Enable(true);
	windowDescTxt->Enable(true);
	windowTypeCmb->Enable(true);
	winControlButton->Enable(true);
	saveWindowButton->Enable(true);
}


wxWindow *SpyEditorScreen::CreateProcessTab()
{
	
	wxFont myFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxPanel *processPanel;
	processPanel = DBG_NEW wxPanel(spyProcessNoteBook, -1, wxPoint(0, 0), spyProcessNoteBook->GetSize(), 0, "processPanel");
	processPanel->SetBackgroundColour(wxColor(0xe6, 0xe6, 0xfa));
	spyProcessNoteBook->AddPage(processPanel, wxT("Process Page"), true, -1);
	
	/****************************************************************************************************
	Creating controls for Process Tab
	*****************************************************************************************************/

	wxSize size = spyMainTab->GetSize();
	int width = (size.GetWidth() - 50) / 2;

	wxStaticText *processLbl = DBG_NEW wxStaticText(processPanel, -1, _("Processes"), wxPoint(10, 10), wxSize(50, 25));
	processLbl->SetFont(myFont);
	prc_processLB = DBG_NEW wxListBox(processPanel, LIST_PROCESSES, wxPoint(10, 45), wxSize(400, 200));

	int yPos = 290;
	int xPos = 15;
	wxStaticBox *processStaticBox = DBG_NEW wxStaticBox(processPanel, wxID_STATIC, _("Process Details"), wxPoint(5, 260), wxSize(410, 310));
	wxStaticText *processValuelbl = DBG_NEW wxStaticText(processPanel, wxID_STATIC, _("System Name"), wxPoint(xPos, yPos), wxSize(85, 25));
	wxStaticText *processNameLbl = DBG_NEW wxStaticText(processPanel, wxID_STATIC, _("Name"), wxPoint(xPos, yPos + 35), wxSize(85, 25));
	wxStaticText *processDescLbl = DBG_NEW wxStaticText(processPanel, wxID_STATIC, _("Description"), wxPoint(xPos, yPos + 70), wxSize(85, 25));
	wxStaticText *processTypeLbl = DBG_NEW wxStaticText(processPanel, wxID_STATIC, _("Type"), wxPoint(xPos, yPos + 210), wxSize(85, 25));
	wxStaticText *processPlatformLbl = DBG_NEW wxStaticText(processPanel, wxID_STATIC, _("Platform"), wxPoint(xPos, yPos + 245), wxSize(85, 25));

	processStaticBox->SetFont(myFont);
	processValuelbl->SetFont(myFont);
	processNameLbl->SetFont(myFont);
	processDescLbl->SetFont(myFont);
	processTypeLbl->SetFont(myFont);
	processPlatformLbl->SetFont(myFont);

	xPos = 110;
	width = 290;
	processValueTxt = DBG_NEW wxTextCtrl(processPanel, PROCESS_VALUE_NAME, wxEmptyString, wxPoint(xPos, yPos), wxSize(width, 25));
	processNameTxt = DBG_NEW wxTextCtrl(processPanel, PROCESS_NAME, wxEmptyString, wxPoint(xPos, yPos + 35), wxSize(width, 25));
	processDescTxt = DBG_NEW wxTextCtrl(processPanel, PROCESS_DESC, wxEmptyString, wxPoint(xPos, yPos + 70), wxSize(width, 130));

	wxArrayString processTypes;
	processTypes.Add(_("Utility"));
	processTypes.Add(_("Application"));

	wxArrayString platformTypes;
	platformTypes.Add(_("Web"));
	platformTypes.Add(_("Desktop"));
	platformTypes.Add(_("Mainframe"));
	platformTypes.Add(_("Cloud"));
	platformTypes.Add(_("Citrix"));


	processTypeCmb = DBG_NEW wxChoice(processPanel, PROCESS_TYPE, wxPoint(xPos, yPos + 210), wxDefaultSize, processTypes);
	processPlatformCmb = DBG_NEW wxChoice(processPanel, PROCESS_PLATFORM, wxPoint(xPos, yPos + 245), wxDefaultSize, platformTypes);
	windowButton = DBG_NEW wxButton(processPanel, BTN_UPDATE_WINDOW, _("Get Windows"), wxPoint(255, yPos + 290), wxSize(100,30));
	saveProcessButton = DBG_NEW wxButton(processPanel, BTN_SAVE_PROCESS, _("Save"), wxPoint(365, yPos + 290), wxSize(50,30));

	processValueTxt->Enable(false);
	processNameTxt->Enable(false);
	processDescTxt->Enable(false);
	processTypeCmb->Enable(false);
	processPlatformCmb->Enable(false);
	windowButton->Enable(false);
	saveProcessButton->Enable(false);
	return processPanel;
}


void SpyEditorScreen::OnIsHomeChanged(wxCommandEvent& event)
{
	if (event.IsChecked())
	{
		urlAppNameTxt->Enable(true);
		urlAppDescTxt->Enable(true);
	}
	else
	{
		urlAppNameTxt->SetValue(wxT(""));
		urlAppDescTxt->SetValue(wxT(""));
		urlAppNameTxt->Enable(false);
		urlAppDescTxt->Enable(false);
	}

}

SpyUrlData *SpyEditorScreen::GetMatchedURLData(string strURLValue)
{
	SpyUrlData *pUrlData = NULL;

	list <wstring> urlNames;
	pDataFacade->GetURLList(&urlNames);
	list <wstring>::iterator itURLNames = urlNames.begin();
	while (itURLNames != urlNames.end())
	{
		wstring str = *itURLNames;
		string savedName = ws2s(str);

		pUrlData = pDataFacade->GetUrlData(savedName);
		if (pUrlData != NULL)
		{
			string url = pUrlData->GetURL();
			string rule = pUrlData->GetRule();
			if (rule != "")
			{
				regex rgx(rule);
				smatch matchValue;

				if (regex_search(url, matchValue, rgx))
				{
					string strResult = matchValue[0];
					pUrlData->SetURLRuleValue(strResult);
				}
			}
			else
				pUrlData->SetURLRuleValue(url);
		}

		itURLNames++;
	}

	bool bFound = false;
	pUrlData = NULL;
	itURLNames = urlNames.begin();
	while (itURLNames != urlNames.end())
	{
		wstring str = *itURLNames;
		string savedName = ws2s(str);
		pUrlData = pDataFacade->GetUrlData(savedName);

		if (pUrlData != NULL)
		{
			string ruleValue = pUrlData->GetURLRuleValue();
			string rule = pUrlData->GetRule();
			if (rule != "")
			{
				regex rgx(rule);
				smatch matchValue;

				if (regex_search(strURLValue, matchValue, rgx))
				{
					string strResult = matchValue[0];
//					if (strResult == ruleValue)
					if (strResult != "" && ruleValue != "")
					{
						bFound = true;
						break;
					}
				}
			}
			else
			{
				if (ruleValue == strURLValue)
				{
					bFound = true;
					break;
				}
			}
		}

		itURLNames++;
	}
	if (bFound)
		return pUrlData;
	return NULL;
}

void SpyEditorScreen::OnURLListSelChanged(wxCommandEvent& event)
{
	string strURLValue = event.GetString();
	SpyUrlData *pURLData = GetMatchedURLData(strURLValue);

	if (pURLData != NULL)
	{
		urlTextTxt->SetValue(wxString(strURLValue));
		urlRuleTxt->SetValue(wxString(pURLData->GetRule().c_str()));
		urlResultTxt->SetValue(wxT(""));
		urlNameTxt->SetValue(wxString(pURLData->GetUserDefinedName().c_str()));
		urlDescTxt->SetValue(wxString(pURLData->GetDescription().c_str()));
		urlAppNameTxt->SetValue(wxString(pURLData->GetApplicationName().c_str()));
		urlAppDescTxt->SetValue(wxString(pURLData->GetApplicationDescription().c_str()));
		urlHomePage->SetValue(pURLData->GetHomePage());
	}
	else
	{
		urlTextTxt->SetValue(strURLValue);
		urlRuleTxt->SetValue(wxT(""));
		urlResultTxt->SetValue(wxT(""));
		urlNameTxt->SetValue(wxT(""));
		urlDescTxt->SetValue(wxT(""));
		urlAppNameTxt->SetValue(wxT(""));
		urlAppDescTxt->SetValue(wxT(""));
		urlHomePage->SetValue(false);
	}

	urlTextTxt->Enable(false);
	urlRuleTxt->Enable(true);
	urlExecuteButton->Enable(true);
	urlResultTxt->Enable(false);
	urlNameTxt->Enable(true);
	urlDescTxt->Enable(true);
	urlAppNameTxt->Enable(true);
	urlAppDescTxt->Enable(true);
	urlHomePage->Enable(true);
	urlControlButton->Enable(true);
	urlSaveButton->Enable(true);
}

void SpyEditorScreen::OnWindowListSelChanged(wxCommandEvent& event)
{
	string strPrcValue = event.GetString();
	SpyWindowData* pWindowData = pDataFacade->GetProcessWindow(strPrcValue);
	if (pWindowData != NULL)
	{
		winRulesTxt->SetValue(pWindowData->GetMatchingRule());
		winRulesTxt->Enable(true);
		winRuleExecBtn->Enable(true);
		winRulesValueTxt->SetValue("");
		windowTitleTxt->SetValue(pWindowData->GetTitle());
		windowNameTxt->SetValue(pWindowData->GetName());
		windowNameTxt->Enable(true);
		windowDescTxt->SetValue(pWindowData->GetDescription());
		windowDescTxt->Enable(true);
		int selected = 0;
		selected = windowTypeCmb->FindString(wxString(pWindowData->GetWindowType()));
		windowTypeCmb->SetSelection(-1);
		if (selected != wxNOT_FOUND)
		{
			windowTypeCmb->SetSelection(selected);
		}
		winHomePage->SetValue(pWindowData->GetMainWindowFlg());
		winHomePage->Enable(true);
		windowTypeCmb->Enable(true);
		winControlButton->Enable(true);
		saveWindowButton->Enable(true);
		winBackToWindowButton->Enable(true);
	}
}


void SpyEditorScreen::OnProcessListSelChanged(wxCommandEvent& event)
{
	string strPrcValue = event.GetString();


	SpyProcessData *pProcessData = NULL;
	if (pDataFacade->IsProcessPresent(strPrcValue))
	{
		pProcessData = pDataFacade->GetProcessData(strPrcValue);
		processValueTxt->SetValue(wxString(pProcessData->GetSystemName().c_str()));
		processNameTxt->SetValue(wxString(pProcessData->GetUserDefinedName().c_str()));
		processDescTxt->SetValue(wxString(pProcessData->GetDescription().c_str()));
		int selected = 0;
		selected = processTypeCmb->FindString(wxString(pProcessData->GetType()));
		processTypeCmb->SetSelection(-1);
		if (selected != wxNOT_FOUND)
		{
			processTypeCmb->SetSelection(selected);
		}
		processPlatformCmb->SetSelection(-1);
		selected = processPlatformCmb->FindString(wxString(pProcessData->GetPlatform()));
		if (selected != wxNOT_FOUND)
		{
			processPlatformCmb->SetSelection(selected);
		}
	}
	else
	{
		processValueTxt->SetValue(strPrcValue);
		processNameTxt->SetValue(wxString(""));
		processDescTxt->SetValue(wxString(""));
		processTypeCmb->SetSelection(-1);
		processPlatformCmb->SetSelection(-1);
	}
	processValueTxt->Enable(false);
	processNameTxt->Enable(true);
	processDescTxt->Enable(true);
	processTypeCmb->Enable(true);
	processPlatformCmb->Enable(true);
	windowButton->Enable(true);
	saveProcessButton->Enable(true);
}


void SpyEditorScreen::OnUpdateProcessWindows(wxCommandEvent& WXUNUSED(event))
{
	// Get the process PID if available
	int nSel = processListCmb->GetSelection();
	string strProcess = string(processListCmb->GetString(nSel).mb_str());
	DisableWindowTabControls();
	vector<DWORD> pidList = GetProcessIDs(strProcess);
	GetWindowNames(pidList);
}

vector <DWORD> SpyEditorScreen::GetProcessIDs(string strName)
{

	vector <DWORD> pids;
	pids.clear();
	vector<struct processdata *> processData = getProcesses();
	vector<struct processdata *>::iterator itProcessData = processData.begin();
	while (itProcessData != processData.end())
	{
		struct processdata *pData = *itProcessData;
		if (pData->processName == strName)
		{
			pids.push_back(pData->pid);
		}
		itProcessData++;
	}

	return pids;

}

void SpyEditorScreen::OnProcessWindowButton(wxCommandEvent& WXUNUSED(event))
{
	SaveProcessInFile();
	winControlButton->Enable(false);
	saveWindowButton->Enable(false);
	winBackToWindowButton->Enable(true);

	string key = processValueTxt->GetValue();
	if (key != oldProcessKey)
	{
		pDataFacade->ReadProcessWindowsData(key);
		vector <string> windowNames = pDataFacade->GetWindowNames();
		windowLB->Clear();
		vector <string>::iterator itWindows = windowNames.begin();
		while (itWindows != windowNames.end())
		{
			windowLB->Append(*itWindows);
			itWindows++;
		}
	}
	else
		winControlButton->Enable(true);


	wxArrayString processNames;
	processNames.Add(key);
	processListCmb->Clear();
	processListCmb->Append(processNames);
	processListCmb->Select(0);
	processListCmb->Enable(false);

	oldProcessKey = key;
	urlCaptureBtn->Enable(false);

	winRulesTxt->SetValue("");
	winRulesValueTxt->SetValue("");
	windowTitleTxt->SetValue("");
	windowNameTxt->SetValue("");
	windowDescTxt->SetValue("");
	winHomePage->SetValue(false);
	windowTypeCmb->Select(-1);

	winRulesTxt->Enable(false);
	winRuleExecBtn->Enable(false);
	winRulesValueTxt->Enable(false);
	windowTitleTxt->Enable(false);
	windowNameTxt->Enable(false);
	windowDescTxt->Enable(false);
	winHomePage->Enable(false);
	windowTypeCmb->Enable(false);




	spyMainTab->ShowNewPage(spyWindowTab);
	spyMainTab->SetSize(tabSize);
	this->SetPosition(posPoint);
	this->SetSize(initSize);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnGotoProcessWindows(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(true);
	spyMainTab->ShowNewPage(spyProcessNoteBook);
	spyMainTab->SetSize(tabSize);
	this->SetPosition(posPoint);
	this->SetSize(initSize);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnGotoURL(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(true);
	spyMainTab->ShowNewPage(spyProcessNoteBook);
	spyMainTab->SetSize(tabSize);
	this->SetPosition(posPoint);
	this->SetSize(initSize);
	this->SetMaxSize(initSize);
}


void SpyEditorScreen::OnSaveControls(wxCommandEvent& WXUNUSED(event))
{
	int index = windowControlList->GetSelection();
	vector <SpyControlData*> controlList = pDataFacade->GetControls();

	if (index <= controlList.size())
	{
		SpyControlData* pControlData = controlList[index];
		if (pControlData != NULL)
		{
			string val = ctrlUserNameTxt->GetValue().char_str();
			pControlData->SetControlUserName(val);

			val = ctrlClickAnnotationRuleTxt->GetValue().char_str();
			pControlData->SetClickAnnotation(val);

			val = ctrlTypeListCmb->GetString(ctrlTypeListCmb->GetSelection());
			pControlData->SetScreenControlType(val);
			pDataFacade->WriteControlData(pControlData);
		}

		if (windowControlList->GetCount() > 1)
		{
			ctrlUserNameTxt->Enable(false);
			ctrlClickAnnotationRuleTxt->Enable(false);
			ctrlTypeListCmb->Enable(false);
			controlBackToWindowButton->Enable(true);
			controlBackToProcessButton->Enable(true);
			controlMaskInfoButton->Enable(true);
			saveControlButton->Enable(false);
		}
		else
		{
			ctrlUserNameTxt->Enable(true);
			ctrlClickAnnotationRuleTxt->Enable(true);
			ctrlTypeListCmb->Enable(true);
			controlBackToWindowButton->Enable(true);
			controlBackToProcessButton->Enable(true);
			controlMaskInfoButton->Enable(true);
			saveControlButton->Enable(true);
		}
	}
}


void SpyEditorScreen::OnURLSaveControls(wxCommandEvent& event)
{
	int index = urlControlLB->GetSelection();

	vector <SpyControlData*> controlList = pDataFacade->GetControls();

	if (index <= controlList.size())
	{
		SpyControlData* pControl = controlList[index];
		string val = urlCtrlUserNameTxt->GetValue().mb_str();
		pControl->SetControlUserName(val);

		val = urlCtrlClickAnnotationRuleTxt->GetValue().mb_str();
		pControl->SetClickAnnotation(val);

		val = urlCtrlXPathTxt->GetValue().mb_str();
		pControl->SetEnterAnnotation(val);
		pDataFacade->WriteControlData(pControl);

		urlControlLB->Clear();

		vector <SpyControlData*> controls = pDataFacade->GetControls();
		vector <SpyControlData*>::iterator itControls = controls.begin();
		while (itControls != controls.end())
		{
			pControl = *itControls;

			urlControlLB->Append(pControl->GetControlUserName());
			itControls++;
		}

		if (urlControlLB->GetCount() > 1)
		{
			urlControlTypeTxt->Enable(false);
			urlCtrlClickAnnotationRuleTxt->Enable(false);
			urlCtrlXPathTxt->Enable(false);
			urlCtrlUserNameTxt->Enable(false);
			urlSaveControlButton->Enable(false);
		}
		else
		{
			urlControlTypeTxt->Enable(true);
			urlCtrlClickAnnotationRuleTxt->Enable(true);
			urlCtrlXPathTxt->Enable(true);
			urlCtrlUserNameTxt->Enable(true);
			urlSaveControlButton->Enable(true);
		}

	}
}


void SpyEditorScreen::OnMaskWindow(wxCommandEvent& WXUNUSED(event))
{

	string processName = "";
	string windowName = "";

	isURLMask = false;

	processLbl1->Show(true);
	processLbl2->Show(true);

	InitMaskScreen();

	windowLbl1->SetLabel(_T("Window:"));

	int nSel = controlProcessListCmb->GetSelection();
	processName = string(controlProcessListCmb->GetString(nSel).mb_str());

	nSel = controlWindowListCmb->GetSelection();
	windowName = string(controlWindowListCmb->GetString(nSel).mb_str());

	processLbl2->SetLabelText(processName);
	windowLbl2->SetLabelText(windowName);

	pDataFacade->ReadWindowMaskInfo(windowName);

	vector <string> maskNames = pDataFacade->GetMaskNames();
	masksCB->Clear();
	vector <string>::iterator itMaskNames = maskNames.begin();
	while (itMaskNames != maskNames.end())
	{
		masksCB->AppendString(*itMaskNames);
		itMaskNames++;
	}
	urlCaptureBtn->Enable(false);
	SetMaxScreenSize();
	spyMainTab->ShowNewPage(spyMaskTab);
	Maximize(true);

}

void SpyEditorScreen::SetMaxScreenSize()
{
	wxDisplay display(wxDisplay::GetFromWindow(this));
	wxRect screen = display.GetClientArea();
	wxSize size;
	size.SetWidth(screen.GetWidth());
	size.SetHeight(screen.GetHeight());
	spyMainTab->SetSize(size);
	this->SetMaxSize(size);
	this->SetPosition(wxPoint(0, 0));
	this->SetSize(size);
}

void SpyEditorScreen::OnURLMaskWindow(wxCommandEvent& WXUNUSED(event))
{
	string urlName = "";
	string layoutName = "";

	isURLMask = true;
	InitMaskScreen();

	processLbl1->Show(false);
	processLbl2->Show(false);
	maskBackToWindowButton->Enable(false);

	windowLbl1->SetLabel(_T("URL:"));

	int nSel = urlControlListCmb->GetSelection();
	urlName = string(urlControlListCmb->GetString(nSel).mb_str());


	windowLbl2->SetLabelText(urlName);
	pDataFacade->ReadURLMaskInfo(urlName);
	vector <string> maskNames = pDataFacade->GetMaskNames();
	masksCB->Clear();
	vector <string>::iterator itMaskNames = maskNames.begin();
	while (itMaskNames != maskNames.end())
	{
		masksCB->AppendString(*itMaskNames);
		itMaskNames++;
	}
	urlCaptureBtn->Enable(false);
	SetMaxScreenSize();
	spyMainTab->ShowNewPage(spyMaskTab);
}

void SpyEditorScreen::OnControlGotoWindows(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(false);
	spyMainTab->ShowNewPage(spyWindowTab);
	spyMainTab->SetSize(tabSize);
	this->SetPosition(posPoint);
	this->SetSize(initSize);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnControlGotoProcess(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(true);
	spyMainTab->ShowNewPage(spyProcessNoteBook);
	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnMaskGotoWindows(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(false);
	if (isURLMask)
	{
		urlCaptureBtn->Enable(true);
		spyMainTab->ShowNewPage(spyUrlTab);
	}
	else
		spyMainTab->ShowNewPage(spyWindowTab);
	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnMaskGotoProcess(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(true);
	spyMainTab->ShowNewPage(spyProcessNoteBook);
	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnMaskGotoControls(wxCommandEvent& WXUNUSED(event))
{
	urlCaptureBtn->Enable(false);
	spyMainTab->ShowNewPage(spyControlTab);
	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);
}



void SpyEditorScreen::SaveWindow(string pName, string wName)
{
	SpyProcessData *pProcessData = NULL;

	pProcessData = pDataFacade->GetProcessData(pName);
	if (pProcessData == NULL)
		return;
	SpyWindowData *pWindowData = pDataFacade->GetProcessWindow(wName);
	if (pWindowData == NULL)
	{
		pWindowData = DBG_NEW SpyWindowData();
		pWindowData->SetTitle(wName);
	}

	pWindowData->SetMatchingRule(string(winRulesTxt->GetValue().mb_str()));
	pWindowData->SetName(string(windowNameTxt->GetValue().mb_str()));
	pWindowData->SetDescription(string(windowDescTxt->GetValue().mb_str()));
	int nSel = windowTypeCmb->GetSelection();
	pWindowData->SetWindowType(string(windowTypeCmb->GetString(nSel).mb_str()));
	pWindowData->SetMainWindowFlg(winHomePage->GetValue());

	pDataFacade->WriteWindowDataFile(pProcessData, pWindowData);
	if (pDataFacade->GetProcessWindow(wName) == NULL)
		pDataFacade->AddWindow(wName, pWindowData);

}

void SpyEditorScreen::UpdateListBoxes()
{
	urlLB->Clear();
	list <wstring> urlNames;

	pDataFacade->GetURLList(&urlNames);
	list <wstring>::iterator itNames = urlNames.begin();
	while (itNames != urlNames.end())
	{
		wstring str = *itNames;
		urlLB->AppendString(str);
		itNames++;
	}

	list <wstring> processList;
	prc_processLB->Clear();
	processList.clear();
	pDataFacade->GetProcessList(&processList);
	list <wstring>::iterator itProcessList = processList.begin();
	while (itProcessList != processList.end())
	{
		wstring str = *itProcessList;
		prc_processLB->Append(str);
		itProcessList++;
	}
}

void SpyEditorScreen::OnURLControls(wxCommandEvent& WXUNUSED(event))
{
	string urlName = "";

	int nSel = urlLB->GetSelection();
	urlName = string(urlLB->GetString(nSel).mb_str());
	SaveURLInFile();

	if (oldURLName != urlName)
	{
		SpyUrlData *pURLData = GetMatchedURLData(urlName);
		if (pURLData != NULL)
			pDataFacade->ReadURLControlsData(pURLData->GetURL());
	}
	urlControlLB->Clear();

	oldURLName = urlName;
	urlControlListCmb->Clear();
	urlControlListCmb->AppendString(s2ws(urlName));
	urlControlListCmb->Select(0);
	vector <SpyControlData*> controls = pDataFacade->GetControls();
	vector <SpyControlData*>::iterator itControls = controls.begin();
	while (itControls != controls.end())
	{
		SpyControlData* pControl = *itControls;

		urlControlLB->Append(pControl->GetControlUserName());
		itControls++;
	}
	urlCaptureBtn->Enable(false);
	spyMainTab->ShowNewPage(spyUrlTab);
	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);
}


void SpyEditorScreen::OnWindowControls(wxCommandEvent& WXUNUSED(event))
{
	
	string processName = "";
	string windowName = "";

	int nSel = processListCmb->GetSelection();
	processName = string(processListCmb->GetString(nSel).mb_str());
	wxString itemText = windowLB->GetStringSelection();
	windowName = itemText.char_str();

	SaveWindow(processName, windowName);

	if (oldProcessName != processName || oldWindowName != windowName)
	{
		pDataFacade->ReadWindowControlsData(windowName);
		windowControlList->Clear();

		vector <SpyControlData*> controls = pDataFacade->GetControls();
		vector <SpyControlData*>::iterator itControls = controls.begin();
		while (itControls != controls.end())
		{
			SpyControlData* pControl = *itControls;

			windowControlList->Append(pControl->GetControlUserName());
			itControls++;
		}
		// Populate all the windows controls out here
	}
	controlProcessListCmb->Clear();
	controlProcessListCmb->AppendString(s2ws(processName));
	controlProcessListCmb->Select(0);
	controlProcessListCmb->Enable(false);
	controlWindowListCmb->Clear();
	controlWindowListCmb->AppendString(s2ws(windowName));
	controlWindowListCmb->Select(0);
	controlWindowListCmb->Enable(false);

	oldProcessName = processName;
	oldWindowName = windowName;
	urlCaptureBtn->Enable(false);
	spyMainTab->ShowNewPage(spyControlTab);
	spyMainTab->SetSize(tabSize);
	this->SetSize(initSize);
	this->SetPosition(posPoint);
	this->SetMaxSize(initSize);
}

void SpyEditorScreen::OnSave(wxCommandEvent& WXUNUSED(event))
{
	int tabNum = spyMainTab->GetSelection();

	if (tabNum == 0)
	{
		SaveProcessInFile();
	}
	else if (tabNum == 1)
	{
		SaveWindowInFile();
	}
	else if (tabNum == 2)
	{
		SaveControlsInFile();
	}
}

void SpyEditorScreen::OnSaveProcess(wxCommandEvent& WXUNUSED(event))
{
	SaveProcessInFile();
}

void SpyEditorScreen::OnSaveURL(wxCommandEvent& WXUNUSED(event))
{
	SaveURLInFile();
}

void SpyEditorScreen::OnSaveWindow(wxCommandEvent& WXUNUSED(event))
{
	SaveWindowInFile();
}


void SpyEditorScreen::SaveControlsInFile()
{
	/*
	string processName = "";
	string windowName = "";

	int nSel = controlProcessListCmb->GetSelection();
	processName = string(controlProcessListCmb->GetString(nSel).mb_str());

	nSel = controlWindowListCmb->GetSelection();
	windowName = string(controlWindowListCmb->GetString(nSel).mb_str());

	SpyProcessData *pProcessData = NULL;

	pProcessData = pDataFacade->GetProcessData(processName);
	if (pProcessData == NULL)
		return;
	SpyWindowData *pWindowData = pDataFacade->GetProcessWindow(windowName);

	nSel = layoutListCmb->GetSelection();
	if (nSel == -1)
		return;
	string layoutName = string(layoutListCmb->GetString(nSel).mb_str());
	SpyWindowLayout *pLayout = pDataFacade->GetLayout(layoutName);
	pLayout->SetLayoutName(string(layoutNameTxt->GetValue().mb_str()));

	wxTreeItemId selectedNode = controlTree->GetSelection();
	if (!selectedNode.IsOk())
		return;
	MyControlTreeItemData *pItemData = (MyControlTreeItemData *)controlTree->GetItemData(selectedNode);
	int controlID = pItemData->GetControlID();
	if (controlID != -1)
	{
		nSel = controlProcessListCmb->GetSelection();
		processName = string(controlProcessListCmb->GetString(nSel).mb_str());

		nSel = controlWindowListCmb->GetSelection();
		windowName = string(controlWindowListCmb->GetString(nSel).mb_str());

		pProcessData = pDataFacade->GetProcessData(processName);
		if (pProcessData == NULL)
			return;
		pWindowData = pDataFacade->GetProcessWindow(windowName);
		SpyControlData *pControlData = pDataFacade->GetControl(controlID);
		wxString value = "";
		pControlData->SetUseName(matchNameChkBox->GetValue());
		pControlData->SetDescription(string(controlDescTxt->GetValue().mb_str()));
		pControlData->SetUseRect(matchRectChkBox->GetValue());
		pControlData->SetUseValue(matchValueChkBox->GetValue());
		pControlData->SetControlUserName(string(ctrlUserNameTxt->GetValue().mb_str()));
		pControlData->SetClickAnnotation(string(ctrlClickAnnotationRuleTxt->GetValue().mb_str()));
		pControlData->SetEnterAnnotation(string(ctrlEnterAnnotationRuleTxt->GetValue().mb_str()));
		pControlData->SetScreenControlType(string(ctrlTypeListCmb->GetString(ctrlTypeListCmb->GetSelection())));
		pControlData->SetDisableFlg(actionDisableChkBox->GetValue());
		pControlData->SetCreateWindow(actionWindowChangedChkBox->GetValue());
		pControlData->SetCloseWindow(actionWindowClosedChkBox->GetValue());
		pControlData->SetUseRect(matchRectChkBox->GetValue());
		pDataFacade->WriteControlData(pControlData);

	}

	*/

}

void SpyEditorScreen::SaveWindowInFile()
{
	string processName = "";
	string windowName = "";

	int nSel = processListCmb->GetSelection();
	processName = string(processListCmb->GetString(nSel).mb_str());

	wxString itemText = windowLB->GetStringSelection();
	windowName = itemText.char_str();

	SaveWindow(processName, windowName);

	if (windowLB->GetCount() > 1)
	{
		winRulesTxt->Enable(false);
		winRuleExecBtn->Enable(false);
		winRulesValueTxt->Enable(false);
		windowTitleTxt->Enable(false);
		windowNameTxt->Enable(false);
		windowDescTxt->Enable(false);
		windowTypeCmb->Enable(false);
		winHomePage->Enable(false);
		winControlButton->Enable(true);
		saveWindowButton->Enable(false);
		winBackToWindowButton->Enable(true);
	}
	else
	{
		winRulesTxt->Enable(true);
		winRuleExecBtn->Enable(true);
		winRulesValueTxt->Enable(true);
		windowTitleTxt->Enable(true);
		windowNameTxt->Enable(true);
		windowDescTxt->Enable(true);
		windowTypeCmb->Enable(true);
		winHomePage->Enable(true);
		winControlButton->Enable(true);
		saveWindowButton->Enable(true);
		winBackToWindowButton->Enable(true);
	}

}

void SpyEditorScreen::SaveProcessInFile()
{
	string key = processValueTxt->GetValue();
	SpyProcessData* pProcessData = NULL;
	if (pDataFacade->IsProcessPresent(key))
	{
		pProcessData = pDataFacade->GetProcessData(key);
	}
	else
	{
		pProcessData = DBG_NEW SpyProcessData();
	}

	pProcessData->SetSystemName(string(processValueTxt->GetValue().mb_str()));
	pProcessData->SetUserDefinedName(string(processNameTxt->GetValue().mb_str()));
	pProcessData->SetDescription(string(processDescTxt->GetValue().mb_str()));

	int nSel = processTypeCmb->GetSelection();
	pProcessData->SetType(string(processTypeCmb->GetString(nSel).mb_str()));

	nSel = processPlatformCmb->GetSelection();
	pProcessData->SetPlatform(string(processPlatformCmb->GetString(nSel).mb_str()));

	pDataFacade->UpdateProcessData(pProcessData);
	if (prc_processLB->GetCount() > 1)
	{
		processValueTxt->Enable(false);
		processNameTxt->Enable(false);
		processDescTxt->Enable(false);
		processTypeCmb->Enable(false);
		processPlatformCmb->Enable(false);
		windowButton->Enable(true);
		saveProcessButton->Enable(false);
	}
	else
	{
		processValueTxt->Enable(true);
		processNameTxt->Enable(true);
		processDescTxt->Enable(true);
		processTypeCmb->Enable(true);
		processPlatformCmb->Enable(true);
		windowButton->Enable(true);
		saveProcessButton->Enable(true);
	}
}


void SpyEditorScreen::SaveURLInFile()
{
	string key = urlTextTxt->GetValue();

	SpyUrlData* pURLData = NULL;
	pURLData = GetMatchedURLData(key);
	if (pURLData == NULL)
	{
		pURLData = DBG_NEW SpyUrlData();
	}

	pURLData->SetRule(string(urlRuleTxt->GetValue().mb_str()));
	if (pURLData->GetURL() == "")
		pURLData->SetURL(string(urlTextTxt->GetValue().mb_str()));
	pURLData->SetUserDefinedName(string(urlNameTxt->GetValue().mb_str()));
	pURLData->SetDescription(string(urlDescTxt->GetValue().mb_str()));
	pURLData->SetApplicationName(string(urlAppNameTxt->GetValue().mb_str()));
	pURLData->SetApplicationDescription(string(urlAppDescTxt->GetValue().mb_str()));

	pURLData->SetHomePage(urlHomePage->GetValue());

	pDataFacade->UpdateURLData(pURLData);

	if (urlLB->GetCount() > 1)
	{
		urlRuleTxt->Enable(false);
		urlExecuteButton->Enable(false);
		urlNameTxt->Enable(false);
		urlDescTxt->Enable(false);
		urlAppNameTxt->Enable(false);
		urlAppDescTxt->Enable(false);
		urlHomePage->Enable(false);
		urlControlButton->Enable(true);
		urlSaveButton->Enable(false);
	}
	else
	{
		urlRuleTxt->Enable(true);
		urlExecuteButton->Enable(true);
		urlNameTxt->Enable(true);
		urlDescTxt->Enable(true);
		urlAppNameTxt->Enable(true);
		urlAppDescTxt->Enable(true);
		urlHomePage->Enable(true);
		urlControlButton->Enable(true);
		urlSaveButton->Enable(true);
	}

}



void SpyEditorScreen::OnExecuteWindowRule(wxCommandEvent& WXUNUSED(event))
{
	// Get Regular expression rule and Text
	// search rule in text and get result
	// update result in result text control

	string strText = windowTitleTxt->GetValue().mbc_str();
	string strRule = winRulesTxt->GetValue().mbc_str();
	string strResult = "Error";
	regex rgx(strRule);
	smatch matchValue;

	if (regex_search(strText, matchValue, rgx))
	{
		strResult = matchValue[0];
	}

	winRulesValueTxt->SetValue(strResult);
}



void SpyEditorScreen::OnExecuteURLRule(wxCommandEvent& WXUNUSED(event))
{
	// Get Regular expression rule and Text
	// search rule in text and get result
	// update result in result text control

	string strText = urlTextTxt->GetValue().mbc_str();
	string strRule = urlRuleTxt->GetValue().mbc_str();
	string strResult = "Error";
	regex rgx(strRule);
	smatch matchValue;

	if (regex_search(strText, matchValue, rgx))
	{
		strResult = matchValue[0];
	}

	urlResultTxt->SetValue(strResult);
}

void SpyEditorScreen::OnCaptureControls(wxCommandEvent& WXUNUSED(event))
{
	this->Hide();
	SpyCaptureScreen* captureTool = DBG_NEW SpyCaptureScreen(this, wxID_ANY, _("Capture Tool"),
		wxPoint(700, 200), wxSize(400, 400));
	captureTool->SetDataFacade(pDataFacade);
	captureTool->Show();
}

void SpyEditorScreen::OnUpdateURLs(wxCommandEvent& WXUNUSED(event))
{
	wxBusyInfo wait("Please wait, working...");

	vector <string> urlStrings;
	urlStrings.clear();

	GetEdgeURL(0, urlStrings);


	urlLB->Clear();
	list <wstring> urlNames;

	pDataFacade->GetURLList(&urlNames);
	list <wstring>::iterator itNames = urlNames.begin();
	while (itNames != urlNames.end())
	{
		wstring str = *itNames;
		urlLB->AppendString(str);
		itNames++;
	}
	vector <string>::iterator itUrlStrings = urlStrings.begin();
	while (itUrlStrings != urlStrings.end())
	{
		string urlName = *itUrlStrings;
		SpyUrlData *pUrlData = pDataFacade->GetUrlData(urlName);
		if (pUrlData == NULL)
		{
			urlLB->AppendString(urlName);
		}
		itUrlStrings++;
	}

}

map <DWORD, int> SpyEditorScreen::GetBrowserPIDs()
{
	WTS_PROCESS_INFO* pWPIs = NULL;
	DWORD dwProcCount = 0;
	map <DWORD, int> urlPIDs;

	if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
	{

		//Go through all processes retrieved
		for (DWORD i = 0; i < dwProcCount; i++)
		{
			wstring str = pWPIs[i].pProcessName;
			string strByte = ws2s(str);
			if (strByte == "MicrosoftEdge.exe")
			{
				urlPIDs.insert({ pWPIs[i].ProcessId, 1 });
			}
			else if (strByte == "chrome.exe")
				urlPIDs.insert({ pWPIs[i].ProcessId, 2 });
		}

	}

	//Free memory
	if (pWPIs)
	{
		WTSFreeMemory(pWPIs);
		pWPIs = NULL;
	}

	return urlPIDs;
}


void SpyEditorScreen::OnUpdateProcess(wxCommandEvent& WXUNUSED(event))
{
	map <wstring, wstring> processList;
	list <wstring> currentList;
	processList.clear();
	currentList.clear();

	wxBusyInfo wait("Please wait, working...");


	pDataFacade->GetProcessList(&currentList);
	list <wstring>::iterator itCurrentList = currentList.begin();
	while (itCurrentList != currentList.end())
	{
		wstring str = *itCurrentList;
		processList.insert({ str, str });
		itCurrentList++;
	}

	prc_processLB->Clear();

	vector<struct processdata *> processData = getProcesses();
	vector<struct processdata *>::iterator itProcessData = processData.begin();
	map <wstring, wstring>::iterator itMapFind = processList.begin();

	while (itProcessData != processData.end())
	{
		struct processdata *pData = *itProcessData;
		string strByte = pData->processName;
		wstring str = s2ws(strByte);
		if (strByte != "" && !pDataFacade->IsProcessPresent(strByte))
		{
			itMapFind = processList.find(str);
			if (itMapFind == processList.end())
			{
				vector <HWND> hWnds = getToplevelWindows(pData->pid);
				if (hWnds.size() > 0)
					processList.insert({ str, str });
			}

		}
		delete pData;
		itProcessData++;
	}
	
	itMapFind = processList.begin();
	while (itMapFind != processList.end())
	{
		wstring str = itMapFind->second;
		prc_processLB->Append(str);
		itMapFind++;
	}

}

void SpyEditorScreen::GetWindowNames(vector <DWORD> &pIDs)
{
	struct threaddata threadDataVal;
	wxBusyInfo wait("Please wait, working...");

	threadDataVal.pids.clear();
	threadDataVal.pids.insert(threadDataVal.pids.begin(), pIDs.begin(), pIDs.end());
	threadDataVal.windowData.clear();
	threadDataVal.pidCount.clear();
	urlThread = CreateThread(0, 0, GetWindowNameThreadFunction, (LPVOID *)&threadDataVal, 0, &urlThreadID);
	WaitForSingleObject(urlThread, INFINITE);
	CloseHandle(urlThread);

	string key = processValueTxt->GetValue();
	SpyProcessData *pProcessData = NULL;
	pProcessData = pDataFacade->GetProcessData(key);
	
	//windowTree->DeleteAllItems();
	ClearItemData();

	vector <string> processWindowNames = pDataFacade->GetWindowNames();
	vector <string>::iterator itProcessWindowNames = processWindowNames.begin();

	//char str[400];
	/*
	MyTreeItemData *pTreeItemData = DBG_NEW MyTreeItemData(wxT("Root Item"));
	pTreeItemData->SetClassName("NotConfigurable");
	pTreeItemData->SetAutoID("NotConfigurable");
	sprintf_s(str, "%s (%d)", key.c_str(), processWindowNames.size());
	wxTreeItemId rootId = windowTree->AddRoot(s2ws(str), -1, -1, pTreeItemData);
	itemDataList.push_back(pTreeItemData);

	pTreeItemData = DBG_NEW MyTreeItemData(wxT("Configured Windows"));
	pTreeItemData->SetClassName("NotConfigurable");
	pTreeItemData->SetAutoID("NotConfigurable");
	wxTreeItemId configuredID = windowTree->AppendItem(rootId, wxT("Configured Windows"), -1, -1, pTreeItemData);
	itemDataList.push_back(pTreeItemData);

	while (itProcessWindowNames != processWindowNames.end())
	{
		string windowName = *itProcessWindowNames;
		SpyWindowData *pWindowData = pDataFacade->GetProcessWindow(windowName);

		pTreeItemData = DBG_NEW MyTreeItemData(windowName);
		pTreeItemData->SetClassName(pWindowData->GetClassname());
		pTreeItemData->SetAutoID(pWindowData->GetAutomationID());
		itemDataList.push_back(pTreeItemData);

		wxTreeItemId configuredWindow = windowTree->AppendItem(configuredID, *itProcessWindowNames, -1, -1, pTreeItemData);
		itProcessWindowNames++;
	}

	map <DWORD, int>::iterator itWindowList = threadDataVal.pidCount.begin();
	while (itWindowList != threadDataVal.pidCount.end())
	{
		DWORD pid = itWindowList->first;
		sprintf_s(str, "PID %d", pid);
		pTreeItemData = DBG_NEW MyTreeItemData(s2ws(str));
		pTreeItemData->SetClassName("NotConfigurable");
		pTreeItemData->SetAutoID("NotConfigurable");
		wxTreeItemId processID = windowTree->AppendItem(rootId, s2ws(str), -1, -1, pTreeItemData);
		itemDataList.push_back(pTreeItemData);

		vector <ProcessWindowData *>::iterator itWindowData = threadDataVal.windowData.begin();
		while (itWindowData != threadDataVal.windowData.end())
		{
			ProcessWindowData *pWindowData = *itWindowData;

			if (pWindowData->GetProcessID() == pid)
			{
				string windowName = pWindowData->GetWindowTitle();

				SpyWindowData *pProcessWindow = pDataFacade->GetProcessWindow(windowName);
				if (pProcessWindow == NULL)
				{
					pTreeItemData = DBG_NEW MyTreeItemData(windowName);
					pTreeItemData->SetClassName(pWindowData->GetWindowClassName());
					pTreeItemData->SetAutoID(pWindowData->GetWindowAutomationID());
					wxTreeItemId elementID = windowTree->AppendItem(processID, s2ws(windowName), -1, -1, pTreeItemData);
					itemDataList.push_back(pTreeItemData);
				}
			}
			itWindowData++;
		}
		itWindowList++;
	}

	windowTree->ExpandAll();
	ClearThreadData(&threadDataVal);
	*/
}

void SpyEditorScreen::ClearThreadData(struct threaddata *pThreadData)
{
	vector <ProcessWindowData *>::iterator itWindowData = pThreadData->windowData.begin();
	while (itWindowData != pThreadData->windowData.end())
	{
		ProcessWindowData *pData = *itWindowData;
		delete pData;
		itWindowData++;
	}

	vector <WindowControlData *>::iterator itControlData = pThreadData->controlData.begin();
	while (itControlData != pThreadData->controlData.end())
	{
		WindowControlData *wData = *itControlData;
		delete wData;
		itControlData++;
	}

}


void SpyEditorScreen::ClearItemData()
{
	//vector <MyTreeItemData *>::iterator itItemData = itemDataList.begin();
	//while (itItemData != itemDataList.end())
	//{
	//	MyTreeItemData *pItemData = *itItemData;
	//	delete pItemData;
	//	itItemData++;
	//}
}


DWORD WINAPI GetWindowNameThreadFunction(LPVOID lpParameter)
{
	g_pUIA = NULL;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(__uuidof(CUIAutomation),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation),
			(void**)&g_pUIA);


	struct threaddata *pThreadData = (struct threaddata *)lpParameter;
	GetWindowNamesInThread(pThreadData);

	g_pUIA->Release();
	g_pUIA = NULL;
	CoUninitialize();
	return 0;
}

void GetWindowNamesInThread(struct threaddata *pThreadData)
{
	IUIAutomationElement *pDesktop = NULL;
	list <IUIAutomationElement *> pSearchWindows;
	HRESULT hr = NULL;

	pSearchWindows.clear();

	g_pUIA->GetRootElement(&pDesktop);
	if (pDesktop != NULL)
	{
		pSearchWindows.push_back(pDesktop);
		vector <DWORD>::iterator itPID = pThreadData->pids.begin();
		IUIAutomationCondition* pCombinedCondition = NULL;
		IUIAutomationCondition* pWindowTypeCondition = NULL;
		IUIAutomationCondition* pOrArrayCondition = NULL;
		IUIAutomationCondition **pOrConditionArray = (IUIAutomationCondition **)malloc(sizeof(IUIAutomationCondition *) * (pThreadData->pids.size()));

		int num = 0;
		while (itPID != pThreadData->pids.end())
		{
			VARIANT varProp;
			varProp.vt = VT_I4;
			varProp.lVal = *itPID;
			hr = g_pUIA->CreatePropertyCondition(UIA_ProcessIdPropertyId, varProp, &pOrConditionArray[num]);
			if (pOrConditionArray[num] == NULL || !SUCCEEDED(hr))
			{
				num = 0;
				break;
			}
			num++;
			itPID++;
		}

		if (num > 0)
		{
			IUIAutomationCondition **pControlTypeConditionArray = (IUIAutomationCondition **)malloc(sizeof(IUIAutomationCondition *) * 2);

			VARIANT varProp;
			varProp.vt = VT_I4;
			varProp.lVal = UIA_WindowControlTypeId;
//			hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pWindowTypeCondition);
			hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pControlTypeConditionArray[0]);

			VARIANT varProp2;
			varProp2.vt = VT_I4;
			varProp2.lVal = UIA_PaneControlTypeId;
			hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp2, &pControlTypeConditionArray[1]);
			hr = g_pUIA->CreateOrConditionFromNativeArray(pControlTypeConditionArray, 2, &pWindowTypeCondition);

			if (!SUCCEEDED(hr) || pWindowTypeCondition == NULL)
			{
				;
			}
			else if (num == 1)
			{
				hr = g_pUIA->CreateAndCondition(pOrConditionArray[0], pWindowTypeCondition, &pCombinedCondition);
			}
			else
			{
				hr = g_pUIA->CreateOrConditionFromNativeArray(pOrConditionArray, num, &pOrArrayCondition);
				if (!SUCCEEDED(hr) || pOrArrayCondition == NULL)
				{
					;
				}
				else
				{
					hr = g_pUIA->CreateAndCondition(pOrArrayCondition, pWindowTypeCondition, &pCombinedCondition);
				}
			}
		}
		if (num == 0 || !SUCCEEDED(hr) || pCombinedCondition == NULL)
			;
		else
		{
			while (pSearchWindows.size() != 0)
			{
				IUIAutomationElement *pWindow = pSearchWindows.front();
				pSearchWindows.pop_front();
				IUIAutomationElementArray *pWindowElements = NULL;
				pWindow->FindAll(TreeScope_Children, pCombinedCondition, &pWindowElements);
				if (pWindowElements != NULL)
				{
					int numWindows = 0;
					pWindowElements->get_Length(&numWindows);
					for (int numW = 0; numW < numWindows; numW++)
					{
						IUIAutomationElement *pFoundWindow = NULL;
						pWindowElements->GetElement(numW, &pFoundWindow);
						pSearchWindows.push_back(pFoundWindow);

						if (pFoundWindow != NULL)
						{

							VARIANT varValue;
							VariantInit(&varValue);
							pFoundWindow->GetCurrentPropertyValue(UIA_NamePropertyId, &varValue);
							if (varValue.vt == VT_BSTR)
							{

								wstring wStringName(varValue.bstrVal);
								string stringName = ws2s(wStringName);

								if (stringName != "")
								{
									ProcessWindowData *pWindowData = DBG_NEW ProcessWindowData();
									pWindowData->SetWindowTitle(stringName);
									VariantInit(&varValue);
									pFoundWindow->GetCurrentPropertyValue(UIA_ClassNamePropertyId, &varValue);
									if (varValue.vt == VT_BSTR)
									{

										wstring wStringNameValue(varValue.bstrVal);
										stringName = ws2s(wStringNameValue);
										pWindowData->SetWindowClassName(stringName);
									}

									VariantClear(&varValue);
									VariantInit(&varValue);
									pFoundWindow->GetCurrentPropertyValue(UIA_AutomationIdPropertyId, &varValue);
									if (varValue.vt == VT_BSTR)
									{

										wstring wStringNameValue(varValue.bstrVal);
										stringName = ws2s(wStringNameValue);
										pWindowData->SetWindowAutomationID(stringName);
									}

									VariantClear(&varValue);

									VariantInit(&varValue);
									pFoundWindow->GetCurrentPropertyValue(UIA_ProcessIdPropertyId, &varValue);
									if (varValue.vt == VT_I4)
									{
										pWindowData->SetProcessID(varValue.iVal);
									}
									pThreadData->windowData.push_back(pWindowData);

									map <DWORD, int>::iterator itPidCount = pThreadData->pidCount.find(pWindowData->GetProcessID());
									if (itPidCount != pThreadData->pidCount.end())
									{
										itPidCount->second++;
									}
									else
									{
										pThreadData->pidCount.insert({ pWindowData->GetProcessID(), 1 });
									}

								}
							}
							VariantClear(&varValue);
							pFoundWindow->Release();
							pFoundWindow = NULL;
						}
					}

				}
				pWindow->Release();
				pWindow = NULL;

			}
		}

		for (int i = 0; i < num; i++)
		{
			if (pOrConditionArray[i] != NULL)
				pOrConditionArray[i]->Release();
		}
		free(pOrConditionArray);
		if (pCombinedCondition != NULL)
			pCombinedCondition->Release();
		pCombinedCondition = NULL;
		if (pWindowTypeCondition != NULL)
			pWindowTypeCondition->Release();
		pWindowTypeCondition = NULL;
		if (pOrArrayCondition != NULL)
			pOrArrayCondition->Release();
		pOrArrayCondition = NULL;
	}

}



void GetWindowControlsInThread(struct threaddata *pThreadData)
{
	/*
	IUIAutomationElement *pDesktop = NULL;
	list <IUIAutomationElement *> pSearchWindows;
	HRESULT hr = NULL;

	pSearchWindows.clear();

	g_pUIA->GetRootElement(&pDesktop);
	if (pDesktop != NULL)
	{
		pSearchWindows.push_back(pDesktop);
		vector <DWORD>::iterator itPID = pThreadData->pids.begin();
		IUIAutomationCondition* pCombinedCondition = NULL;
		IUIAutomationCondition* pWindowTypeCondition = NULL;
		IUIAutomationCondition* pOrArrayCondition = NULL;
		IUIAutomationCondition **pOrConditionArray = DBG_NEW IUIAutomationCondition *[pThreadData->pids.size()];
		IUIAutomationElement *pMainWindowFound = NULL;

		int num = 0;
		while (itPID != pThreadData->pids.end())
		{
			VARIANT varProp;
			varProp.vt = VT_I4;
			varProp.lVal = *itPID;
			hr = g_pUIA->CreatePropertyCondition(UIA_ProcessIdPropertyId, varProp, &pOrConditionArray[num]);
			if (pOrConditionArray[num] == NULL || !SUCCEEDED(hr))
			{
				num = 0;
				break;
			}
			num++;
			itPID++;
		}

		if (num > 0)
		{
			IUIAutomationCondition **pControlTypeConditionArray = (IUIAutomationCondition **)malloc(sizeof(IUIAutomationCondition *) * 2);

			VARIANT varProp;
			varProp.vt = VT_I4;
			varProp.lVal = UIA_WindowControlTypeId;
			//			hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pWindowTypeCondition);
			hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pControlTypeConditionArray[0]);

			VARIANT varProp2;
			varProp2.vt = VT_I4;
			varProp2.lVal = UIA_PaneControlTypeId;
			hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp2, &pControlTypeConditionArray[1]);
			hr = g_pUIA->CreateOrConditionFromNativeArray(pControlTypeConditionArray, 2, &pWindowTypeCondition);


			//VARIANT varProp;
			//varProp.vt = VT_I4;
			//varProp.lVal = UIA_WindowControlTypeId;
			//hr = g_pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pWindowTypeCondition);
			if (!SUCCEEDED(hr) || pWindowTypeCondition == NULL)
			{
				;
			}
			else if (num == 1)
			{
				hr = g_pUIA->CreateAndCondition(pOrConditionArray[0], pWindowTypeCondition, &pCombinedCondition);
			}
			else
			{
				hr = g_pUIA->CreateOrConditionFromNativeArray(pOrConditionArray, num, &pOrArrayCondition);
				if (!SUCCEEDED(hr) || pOrArrayCondition == NULL)
				{
					;
				}
				else
				{
					hr = g_pUIA->CreateAndCondition(pOrArrayCondition, pWindowTypeCondition, &pCombinedCondition);
				}
			}
		}
		if (num == 0 || !SUCCEEDED(hr) || pCombinedCondition == NULL)
			;
		else
		{
			while (pSearchWindows.size() != 0)
			{
				IUIAutomationElement *pWindow = pSearchWindows.front();
				pSearchWindows.pop_front();
				IUIAutomationElementArray *pWindowElements = NULL;
				pWindow->FindAll(TreeScope_Children, pCombinedCondition, &pWindowElements);
				if (pWindowElements != NULL)
				{
					int numWindows = 0;
					pWindowElements->get_Length(&numWindows);
					for (int numW = 0; numW < numWindows; numW++)
					{
						IUIAutomationElement *pFoundWindow = NULL;
						pWindowElements->GetElement(numW, &pFoundWindow);
						pSearchWindows.push_back(pFoundWindow);

						if (pFoundWindow != NULL)
						{
							VARIANT varValue;
							VariantInit(&varValue);
							pFoundWindow->GetCurrentPropertyValue(UIA_NamePropertyId, &varValue);
							string stringName = "";
							if (varValue.vt == VT_BSTR)
							{

								wstring wStringName(varValue.bstrVal);
								stringName = ws2s(wStringName);
							}
							VariantClear(&varValue);
							if (stringName == pThreadData->windowName)
							{
								pMainWindowFound = pFoundWindow;

							}
							else
							{
								pFoundWindow->Release();
								pFoundWindow = NULL;
							}
						}
						if (pMainWindowFound != NULL)
							break;
					}

				}
				pWindow->Release();
				pWindow = NULL;
				if (pMainWindowFound != NULL)
					break;
			}
		}

		for (int i = 0; i < num; i++)
		{
			if (pOrConditionArray[i] != NULL)
				pOrConditionArray[i]->Release();
		}
		delete[] pOrConditionArray;
		if (pCombinedCondition != NULL)
			pCombinedCondition->Release();
		pCombinedCondition = NULL;
		if (pWindowTypeCondition != NULL)
			pWindowTypeCondition->Release();
		pWindowTypeCondition = NULL;
		if (pOrArrayCondition != NULL)
			pOrArrayCondition->Release();
		pOrArrayCondition = NULL;


		if (pMainWindowFound != NULL)
		{
			// Get Control Data out here
			RECT rect;
			pMainWindowFound->get_CurrentBoundingRectangle(&rect);
			GetWindowControlsForElement(&pThreadData->controlData, pMainWindowFound, &rect);
			pMainWindowFound->Release();
		}
	}
	*/
}


void GetURLControlsInThread(struct threaddata *pThreadData)
{
	/*
	IUIAutomationElement *pDesktop = NULL;
	list <IUIAutomationElement *> pSearchWindows;
	bool isChrome = false;

	pSearchWindows.clear();

	// First find the URL in the list of Browser PIDs found passed to the thread
	// if the URL is found then find the start point of the URL Search, this depends on the PID Browser Type

	g_pUIA->GetRootElement(&pDesktop);


	// Walk through the tree and find controls that matter
	IUIAutomationElement *pFound = NULL;
	IUIAutomationTreeWalker *pControlWalker = NULL;
	IUIAutomationElement *pNode = NULL;


	g_pUIA->get_ControlViewWalker(&pControlWalker);
	if (pControlWalker == NULL)
	{
		return;
	}

	pControlWalker->GetFirstChildElement(pDesktop, &pNode);

	IUIAutomationElement *pURLElement = NULL;

	while (pNode)
	{
		BSTR className;
		pNode->get_CurrentClassName(&className);
		wstring wString(className);
		SysFreeString(className);

		if (wString == L"Chrome_WidgetWin_1")
		{
			// Get Chrome URL or MsEdge Chromium Widget.

			IUIAutomationCondition* pControlNameCondition = NULL;

			// Create a property condition for the button control type.
			VARIANT varProp;
			varProp.vt = VT_BSTR;
			varProp.bstrVal = SysAllocString(L"Address and search bar");
			g_pUIA->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pControlNameCondition);
			if (pControlNameCondition != NULL)
			{
				pNode->FindFirst(TreeScope_Descendants, pControlNameCondition, &pFound);
				pControlNameCondition->Release();
			}
			if (pFound != NULL)
			{
				VARIANT varValue;
				VariantInit(&varValue);
				pFound->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);

				if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
				{
					wstring wStringValue(varValue.bstrVal);
					string urlName = ws2s(wStringValue);
					if (urlName != pThreadData->urlName)
					{
						pURLElement = NULL;
					}
					else
					{
						isChrome = true;
						pURLElement = pNode;
						pURLElement->AddRef();
						SysFreeString(varProp.bstrVal);
						pFound->Release();
						break;
					}
				}
				pFound->Release();
			}
			SysFreeString(varProp.bstrVal);
		}
		else if (wString == L"ApplicationFrameWindow")
		{
			// Get Edge URL
			IUIAutomationCondition* pWindowClassCondition = NULL;
			IUIAutomationElement *pMainWindowFound = NULL;

			VARIANT varProp;
			varProp.vt = VT_BSTR;
			varProp.bstrVal = SysAllocString(L"Windows.UI.Core.CoreWindow");
			g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp, &pWindowClassCondition);

			pNode->FindFirst(TreeScope_Descendants, pWindowClassCondition, &pMainWindowFound);
			if (pMainWindowFound != NULL)
			{
				pWindowClassCondition->Release();
				pWindowClassCondition = NULL;

				IUIAutomationCondition* pControlNameCondition = NULL;

				// Create a property condition for the button control type.
				VARIANT varNameProp;
				varNameProp.vt = VT_BSTR;
				varNameProp.bstrVal = SysAllocString(L"Search or enter web address");
				g_pUIA->CreatePropertyCondition(UIA_NamePropertyId, varNameProp, &pControlNameCondition);
				if (pControlNameCondition != NULL)
				{
					pMainWindowFound->FindFirst(TreeScope_Children, pControlNameCondition, &pFound);
					pControlNameCondition->Release();
				}
				SysFreeString(varNameProp.bstrVal);
				if (pFound != NULL)
				{
					VARIANT varValue;
					VariantInit(&varValue);
					pFound->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);
					if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
					{
						wstring wStringValue(varValue.bstrVal);
						string urlName = ws2s(wStringValue);
						if (urlName != pThreadData->urlName)
						{
							pURLElement = NULL;
						}
						else
						{
							pURLElement = pMainWindowFound;
							pURLElement->AddRef();
							pMainWindowFound->Release();
							SysFreeString(varProp.bstrVal);
							pFound->Release();
							break;
						}
					}
					pFound->Release();
				}

			}
			SysFreeString(varProp.bstrVal);
		}

		IUIAutomationElement* pNext;
		pControlWalker->GetNextSiblingElement(pNode, &pNext);
		pNode->Release();
		pNode = pNext;
	}

	if (pControlWalker != NULL)
		pControlWalker->Release();
	if (pNode != NULL)
		pNode->Release();
	pDesktop->Release();

	if (pURLElement != NULL)
	{

		if (isChrome)
		{
			// Here the URL is Found, now find the start point to get the URL controls
			// It will come here for both MSEdge Chromium as well as Chrome
			IUIAutomationCondition* pWindowChromeCondition = NULL;
			IUIAutomationElement *pMainWindowFound = NULL;
			IUIAutomationCondition *pWindowEdgeCondition = NULL;

			//LandmarkTarget
			VARIANT varProp;
			varProp.vt = VT_BSTR;
			varProp.bstrVal = SysAllocString(L"Chrome_RenderWidgetHostHWND");
			g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp, &pWindowChromeCondition);

			VARIANT varPropEdge;
			varPropEdge.vt = VT_BSTR;
			varPropEdge.bstrVal = SysAllocString(L"BrowserView");
			g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varPropEdge, &pWindowEdgeCondition);

			IUIAutomationCondition *pWindowORCondition = NULL;
			g_pUIA->CreateOrCondition(pWindowEdgeCondition, pWindowChromeCondition, &pWindowORCondition);

			pURLElement->FindFirst(TreeScope_Descendants, pWindowORCondition, &pMainWindowFound);
			VariantClear(&varProp);
			VariantClear(&varPropEdge);

			pWindowChromeCondition->Release();
			pWindowEdgeCondition->Release();
			pWindowORCondition->Release();
			if (pMainWindowFound == NULL)
				return;

			// Check if Classname of item is "BrowserView" then it is Edge window, Go down one level to the child with a view class
			//BSTR bStrVal = NULL;
			//pMainWindowFound->get_CurrentClassName(&bStrVal);
			//string foundClassName = "";
			//if (bStrVal != NULL)
			//{
			//	wstring wStringName(bStrVal);
			//	foundClassName = ws2s(wStringName);
			//}
			//SysFreeString(bStrVal);
			//if (foundClassName == "BrowserView")
			//{
			//	VARIANT varClass;
			//	varClass.vt = VT_BSTR;
			//	varClass.bstrVal = SysAllocString(L"View");
			//	IUIAutomationCondition* pWindowEdgeCondition = NULL;
			//	g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varClass, &pWindowEdgeCondition);

			//	if (pWindowEdgeCondition != NULL)
			//	{
			//		IUIAutomationElement *pEdgeBrowserElement = NULL;
			//		pMainWindowFound->FindFirst(TreeScope_Children, pWindowEdgeCondition, &pEdgeBrowserElement);
			//		if (pEdgeBrowserElement != NULL)
			//		{
			//			pMainWindowFound->Release();
			//			pMainWindowFound = pEdgeBrowserElement;
			//		}
			//		pWindowEdgeCondition->Release();
			//	}
			//}
		
			// Now capture the window and store in the thread value;
			// Get Control Data out here
			RECT rect;
			pMainWindowFound->get_CurrentBoundingRectangle(&rect);
			GetWindowControlsForElement(&pThreadData->controlData, pMainWindowFound, &rect);
			pMainWindowFound->Release();
			pURLElement->Release();
		}
		else
		{
			//IUIAutomationElement *pURLFound = NULL;

			if (pURLElement != NULL)
			{
				// Here the URL is Found, now find the start point to get the URL controls
				//IUIAutomationCondition* pWindowTypeCondition = NULL;
				//IUIAutomationCondition* pWindowNameCondition = NULL;
				IUIAutomationCondition* pWindowClassCondition = NULL;
				IUIAutomationElement *pMainWindowFound = NULL;


				//LandmarkTarget
				VARIANT varProp;
				varProp.vt = VT_BSTR;
				varProp.bstrVal = SysAllocString(L"LandmarkTarget");
				g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp, &pWindowClassCondition);
				pURLElement->FindFirst(TreeScope_Children, pWindowClassCondition, &pMainWindowFound);
				VariantClear(&varProp);

				if (pMainWindowFound != NULL)
				{
					pWindowClassCondition->Release();
					pWindowClassCondition = NULL;

					pControlWalker = NULL;
					IUIAutomationElement *pNodeChild = NULL;

					g_pUIA->get_ControlViewWalker(&pControlWalker);
					if (pControlWalker == NULL)
					{
						return;
					}

					int i = 0;
					while (i < 2)
					{
						pControlWalker->GetFirstChildElement(pMainWindowFound, &pNodeChild);
						pMainWindowFound->Release();
						pMainWindowFound = pNodeChild;
						if (pNodeChild == NULL)
							break;
						pNodeChild = NULL;
						i++;
					}
					pControlWalker->Release();

					if (pMainWindowFound == NULL)
						return;


					// Now capture the window and store in the thread value;
					// Get Control Data out here
					RECT rect;
					pMainWindowFound->get_CurrentBoundingRectangle(&rect);
					GetWindowControlsForElement(&pThreadData->controlData, pMainWindowFound, &rect);
					pMainWindowFound->Release();
				}
			}

		}
	}
	*/
}


void GetChromeURLControlsInThread(struct threaddata *pThreadData)
{
	/*
	IUIAutomationElement *pDesktop = NULL;
	list <IUIAutomationElement *> pSearchWindows;
	HRESULT hr = NULL;

	pSearchWindows.clear();

	// First find the URL in the list of Browser PIDs found passed to the thread
	// if the URL is found then find the start point of the URL Search, this depends on the PID Browser Type

	IUIAutomationElement *pURLElement = NULL;


	g_pUIA->GetRootElement(&pDesktop);

	if (pDesktop != NULL)
	{
		IUIAutomationCondition* pProcessIDCondition = NULL;
		// Create a property condition for the button control type.
		VARIANT varProp;
		varProp.vt = VT_I4;
		varProp.lVal = pThreadData->pid;
		g_pUIA->CreatePropertyCondition(UIA_ProcessIdPropertyId, varProp, &pProcessIDCondition);
		if (pProcessIDCondition != NULL)
		{
			pDesktop->FindFirst(TreeScope_Children, pProcessIDCondition, &pURLElement);
			if (pProcessIDCondition != NULL)
				pProcessIDCondition->Release();
		}
	}

	IUIAutomationElement *pFound = pURLElement;
	if (SUCCEEDED(hr) && pFound != NULL)
	{
		IUIAutomationCondition* pControlNameCondition = NULL;
		IUIAutomationElement *pURLFound = NULL;
		// Create a property condition for the button control type.
		VARIANT varProp;
		varProp.vt = VT_BSTR;
		varProp.bstrVal = SysAllocString(L"Address and search bar");
		g_pUIA->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pControlNameCondition);
		if (pControlNameCondition != NULL)
		{
			pFound->FindFirst(TreeScope_Descendants, pControlNameCondition, &pURLFound);
		}
		if (pControlNameCondition != NULL)
			pControlNameCondition->Release();
		if (pURLFound != NULL)
		{
			VARIANT varValue;
			VariantInit(&varValue);
			pURLFound->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);
			if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
			{
				wstring wString(varValue.bstrVal);
				pThreadData->url = ws2s(wString);
			}
			if (pURLFound != NULL)
				pURLFound->Release();
			if (pThreadData->url != pThreadData->urlName)
			{
				pURLElement->Release();
				pURLElement = NULL;
			}

		}
	}

	if (pURLElement != NULL)
	{
		// Here the URL is Found, now find the start point to get the URL controls
		IUIAutomationCondition* pWindowClassCondition = NULL;
		IUIAutomationElement *pMainWindowFound = NULL;

		//LandmarkTarget
		VARIANT varProp;
		varProp.vt = VT_BSTR;
		varProp.bstrVal = SysAllocString(L"Chrome_RenderWidgetHostHWND");
		g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp, &pWindowClassCondition);
		pURLElement->FindFirst(TreeScope_Children, pWindowClassCondition, &pMainWindowFound);
		VariantClear(&varProp);

		if (pMainWindowFound == NULL)
			return;


		// Now capture the window and store in the thread value;
		// Get Control Data out here
		RECT rect;
		pMainWindowFound->get_CurrentBoundingRectangle(&rect);
		GetWindowControlsForElement(&pThreadData->controlData, pMainWindowFound, &rect);
		pMainWindowFound->Release();
		pURLElement->Release();

	}
	*/
}

void GetWindowControlsForElement(vector <WindowControlData *> *pControlData, IUIAutomationElement *pMainWindowFound, LPRECT pRect)
{
	pControlData->clear();
	if (pMainWindowFound == NULL)
		return;
	HRESULT hr = NULL;
	IUIAutomationTreeWalker *pControlWalker = NULL;
	IUIAutomationElement *pNode = NULL;

	g_pUIA->get_ControlViewWalker(&pControlWalker);
	if (pControlWalker == NULL)
	{
		return;
	}

	pControlWalker->GetFirstChildElement(pMainWindowFound, &pNode);

	while (pNode != NULL)
	{
		IUIAutomationElement *pFoundWindow = pNode;
		if (pFoundWindow != NULL)
		{
			WindowControlData *pWindowData = DBG_NEW WindowControlData();

			BSTR bstrVal = NULL;
			pFoundWindow->get_CurrentName(&bstrVal);
			if (bstrVal != NULL)
			{
				wstring wStringName(bstrVal);
				string stringName = ws2s(wStringName);
				pWindowData->SetName(stringName);
			}
			SysFreeString(bstrVal);

			//7pFoundWindow->get_Cu
			//if (varValue.vt == VT_BSTR)
			//{

			//	wstring wStringName(varValue.bstrVal);
			//	string stringName = ws2s(wStringName);
			//	pWindowData->SetValue(stringName);
			//}
			//VariantClear(&varValue);

			pFoundWindow->get_CurrentClassName(&bstrVal);
			if (bstrVal != NULL)
			{
				wstring wStringName(bstrVal);
				string stringName = ws2s(wStringName);
				pWindowData->SetClassname(stringName);
			}
			SysFreeString(bstrVal);

/*			
*			Auto ID is not consistent for a control, it depends on whether the control is expanded or not so disabling it from the search.
			pFoundWindow->get_CurrentAutomationId(&bstrVal);
			if (bstrVal != NULL)
			{
				wstring wStringName(bstrVal);
				string stringName = ws2s(wStringName);
				pWindowData->SetAutoID(stringName);
			}

			SysFreeString(bstrVal);

*/
			pWindowData->SetAutoID("");
			CONTROLTYPEID controlID = NULL;
			pFoundWindow->get_CurrentControlType(&controlID);
			pWindowData->SetType(controlID);

			RECT rect;

			hr = pFoundWindow->get_CurrentBoundingRectangle(&rect);
			if (SUCCEEDED(hr))
			{
				rect.top -= pRect->top;
				rect.left -= pRect->left;
				rect.bottom -= pRect->top;
				rect.right -= pRect->left;
				pWindowData->SetRect(&rect);
			}

			pControlData->push_back(pWindowData);
			GetWindowControlsForElement(pWindowData->GetChildControls(), pFoundWindow, pRect);
			pControlWalker->GetNextSiblingElement(pFoundWindow, &pNode);
			pFoundWindow->Release();
			pFoundWindow = NULL;
		}
	}

	if (pControlWalker != NULL)
		pControlWalker->Release();
	
}

DWORD WINAPI GetWindowControlsThreadFunction(LPVOID lpParameter)
{
	g_pUIA = NULL;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(__uuidof(CUIAutomation),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation),
			(void**)&g_pUIA);

	struct threaddata *pThreadData = (struct threaddata *)lpParameter;
	GetWindowControlsInThread(pThreadData);
	g_pUIA->Release();
	g_pUIA = NULL;
	CoUninitialize();
	return 0;
}

DWORD WINAPI GetEdgeURLControlsThreadFunction(LPVOID lpParameter)
{
	g_pUIA = NULL;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(__uuidof(CUIAutomation),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation),
			(void**)&g_pUIA);

	struct threaddata *pThreadData = (struct threaddata *)lpParameter;
	GetURLControlsInThread(pThreadData);
	g_pUIA->Release();
	g_pUIA = NULL;
	CoUninitialize();
	return 0;
}

DWORD WINAPI GetChromeURLControlsThreadFunction(LPVOID lpParameter)
{
	g_pUIA = NULL;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(__uuidof(CUIAutomation),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation),
			(void**)&g_pUIA);

	struct threaddata *pThreadData = (struct threaddata *)lpParameter;
	GetChromeURLControlsInThread(pThreadData);
	g_pUIA->Release();
	g_pUIA = NULL;
	CoUninitialize();
	return 0;
}


void SpyEditorScreen::GetEdgeURL(DWORD pid, vector <string> &URLs)
{
	// Start a Thread, 
	// From the root element (Desktop) find all windows that have the same pid
	// for each window as a root, find the element having automationID = "addressEditBox"
	// then the value of the element and return
	struct threaddata threadDataVal;

	threadDataVal.pid = pid;
	threadDataVal.url = "";
	threadDataVal.className = "";
	threadDataVal.name = "";

	urlThread = CreateThread(0, 0, GetURLThreadFunction, (LPVOID *)&threadDataVal, 0, &urlThreadID);
	WaitForSingleObject(urlThread, INFINITE);
	if (threadDataVal.urlNames.size() > 0)
		URLs = threadDataVal.urlNames;
	CloseHandle(urlThread);
}


void SpyEditorScreen::GetChromeURL(DWORD pid, vector <string> &URLs)
{
	// Start a Thread, 
	// From the root element (Desktop) find all windows that have the same pid
	// for each window as a root, find the element having automationID = "addressEditBox"
	// then the value of the element and return
	struct threaddata threadDataVal;

	threadDataVal.pid = pid;
	threadDataVal.url = "";
	chromeUrlThread = CreateThread(0, 0, GetChromeURLThreadFunction, (LPVOID *)&threadDataVal, 0, &chromeUrlThreadID);
	WaitForSingleObject(chromeUrlThread, INFINITE);
	if (threadDataVal.url != "")
		URLs.push_back(threadDataVal.url);
	CloseHandle(chromeUrlThread);
	//	itHWND++;
	//}
}

DWORD WINAPI GetChromeURLThreadFunction(LPVOID lpParameter)
{
	g_pUIA = NULL;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr =
		CoCreateInstance(__uuidof(CUIAutomation),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation),
			(void**)&g_pUIA);

	IUIAutomationElement *pDesktop = NULL;
	struct threaddata *pThreadData = (struct threaddata *)lpParameter;
	IUIAutomationElement* pFound = NULL;
	IUIAutomationElement* pURLFound = NULL;

	g_pUIA->GetRootElement(&pDesktop);
	if (pDesktop != NULL)
	{
		IUIAutomationCondition* pProcessIDCondition = NULL;
		// Create a property condition for the button control type.
		VARIANT varProp;
		varProp.vt = VT_I4;
		varProp.lVal = pThreadData->pid;
		g_pUIA->CreatePropertyCondition(UIA_ProcessIdPropertyId, varProp, &pProcessIDCondition);
		if (pProcessIDCondition != NULL)
		{
			pDesktop->FindFirst(TreeScope_Children, pProcessIDCondition, &pFound);
			if (pProcessIDCondition != NULL)
				pProcessIDCondition->Release();
		}
	}

	if (SUCCEEDED(hr) && pFound != NULL)
	{
		IUIAutomationCondition* pControlNameCondition = NULL;

		// Create a property condition for the button control type.
		VARIANT varProp;
		varProp.vt = VT_BSTR;
		varProp.bstrVal = SysAllocString(L"Address and search bar");
		g_pUIA->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pControlNameCondition);
		if (pControlNameCondition != NULL)
		{
			pFound->FindFirst(TreeScope_Descendants, pControlNameCondition, &pURLFound);
		}
		if (pControlNameCondition != NULL)
			pControlNameCondition->Release();
		if (pURLFound != NULL)
		{
			VARIANT varValue;
			VariantInit(&varValue);
			pURLFound->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);
			if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
			{
				wstring wString(varValue.bstrVal);
				pThreadData->url = ws2s(wString);
			}

		}
		if (pURLFound != NULL)
			pURLFound->Release();
	}

	if (pFound != NULL)
		pFound->Release();

	g_pUIA->Release();
	g_pUIA = NULL;
	CoUninitialize();
	return 0;
}



DWORD WINAPI GetURLThreadFunction(LPVOID lpParameter)
{
	g_pUIA = NULL;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(__uuidof(CUIAutomation),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation),
			(void**)&g_pUIA);

	IUIAutomationElement *pDesktop = NULL;
	struct threaddata *pThreadData = (struct threaddata *)lpParameter;
	IUIAutomationElement* pFound = NULL;

	g_pUIA->GetRootElement(&pDesktop);


// Walk through the tree and find controls that matter

	IUIAutomationTreeWalker *pControlWalker = NULL;
	IUIAutomationElement *pNode = NULL;
	

	g_pUIA->get_ControlViewWalker(&pControlWalker);
	if (pControlWalker == NULL)
	{
		return 0;
	}

	pControlWalker->GetFirstChildElement(pDesktop, &pNode);

	while (pNode)
	{
		BSTR className;
		pNode->get_CurrentClassName(&className);
		wstring wString(className);
		SysFreeString(className);

		if (wString == L"Chrome_WidgetWin_1")
		{
			// Get Chrome URL
			IUIAutomationCondition* pControlNameCondition = NULL;

			// Create a property condition for the button control type.
			VARIANT varProp;
			varProp.vt = VT_BSTR;
			varProp.bstrVal = SysAllocString(L"Address and search bar");
			g_pUIA->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pControlNameCondition);
			if (pControlNameCondition != NULL)
			{
				pNode->FindFirst(TreeScope_Descendants, pControlNameCondition, &pFound);
				pControlNameCondition->Release();
			}
			if (pFound != NULL)
			{
				VARIANT varValue;
				VariantInit(&varValue);
				pFound->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);
				if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
				{
					wstring wStringValue(varValue.bstrVal);
					string urlName = ws2s(wStringValue);
					pThreadData->urlNames.push_back(urlName);
				}
				pFound->Release();
			}
			SysFreeString(varProp.bstrVal);
		}
		else if (wString == L"ApplicationFrameWindow")
		{
			// Get Edge URL
			IUIAutomationCondition* pWindowClassCondition = NULL;
			IUIAutomationElement *pMainWindowFound = NULL;

			VARIANT varProp;
			varProp.vt = VT_BSTR;
			varProp.bstrVal = SysAllocString(L"Windows.UI.Core.CoreWindow");
			g_pUIA->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp, &pWindowClassCondition);

			pNode->FindFirst(TreeScope_Descendants, pWindowClassCondition, &pMainWindowFound);
			if (pMainWindowFound != NULL)
			{
				pWindowClassCondition->Release();
				pWindowClassCondition = NULL;

				IUIAutomationCondition* pControlNameCondition = NULL;

				// Create a property condition for the button control type.
				VARIANT varNameProp;
				varNameProp.vt = VT_BSTR;
				varNameProp.bstrVal = SysAllocString(L"Search or enter web address");
				g_pUIA->CreatePropertyCondition(UIA_NamePropertyId, varNameProp, &pControlNameCondition);
				if (pControlNameCondition != NULL)
				{
					pMainWindowFound->FindFirst(TreeScope_Children, pControlNameCondition, &pFound);
					pControlNameCondition->Release();
				}
				SysFreeString(varNameProp.bstrVal);
				if (pFound != NULL)
				{
					VARIANT varValue;
					VariantInit(&varValue);
					pFound->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);
					if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
					{
						wstring wStringValue(varValue.bstrVal);
						string urlName = ws2s(wStringValue);
						pThreadData->urlNames.push_back(urlName);
					}
					pFound->Release();
				}

			}
			SysFreeString(varProp.bstrVal);
		}

		IUIAutomationElement* pNext;
		pControlWalker->GetNextSiblingElement(pNode, &pNext);
		pNode->Release();
		pNode = pNext;
	}
	
	if (pControlWalker != NULL)
		pControlWalker->Release();

	if (pNode != NULL)
		pNode->Release();

	pDesktop->Release();
	g_pUIA->Release();
	g_pUIA = NULL;
	CoUninitialize();
	return 0;
}


