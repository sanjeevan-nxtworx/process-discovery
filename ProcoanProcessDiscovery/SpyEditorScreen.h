#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/simplebook.h>
#include <wx/event.h>
#include <wx/toolbar.h>
#include <vector>
#include <map>
#include <wx/choicebk.h>

class SPYDataFacade;
class SpyControlProxy;
struct threaddata;
class SpyControlData;
class SpyWindowData;
class SpyUrlData;

using namespace std;

class VarName;
class MaskImagePanel;


class SpyEditorScreen : public wxFrame
{

	wxListBox *prc_processLB;
	SPYDataFacade *pDataFacade;
	wxSimplebook *spyMainTab;
	wxNotebook *spyProcessNoteBook;
	wxButton* urlCaptureBtn;
	string oldProcessKey;
	string oldProcessName;
	string oldURLName;
	string oldWindowName;
	bool isURLMask;

	wxPoint posPoint;
	wxSize initSize;
	wxSize tabSize;
	wxWindow* m_pParent;
	string copyControlName;
	string copyClickString;
	string copyEnterString;


	wxWindow *spyProcessTab;
	wxWindow *spyWindowTab;
	wxWindow *spyControlTab;
	wxWindow *spyMaskTab;
	wxWindow *spyWebPageTab;
	wxWindow *spyUrlTab;


	
	wxTextCtrl *processValueTxt;
	wxTextCtrl *processNameTxt;
	wxTextCtrl *processDescTxt;
	wxChoice *processTypeCmb;
	wxChoice *processPlatformCmb;
	wxButton *windowButton;
	wxButton *saveProcessButton;
	wxButton *saveWindowButton;

	wxChoice *processListCmb;
	wxButton *windowUpdateBtn;
	wxListBox* windowLB;

	wxListBox *urlLB;
	wxCheckBox *urlHomePage;
	wxTextCtrl *urlTextTxt;
	wxTextCtrl *urlRuleTxt;
	wxTextCtrl *urlResultTxt;
	wxTextCtrl *urlNameTxt;
	wxTextCtrl *urlDescTxt;
	wxTextCtrl *urlAppNameTxt;
	wxTextCtrl *urlAppDescTxt;
	wxButton *urlControlButton;
	wxButton *urlSaveButton;
	wxButton *urlExecuteButton;
	wxButton *urlMaskInfoButton;

	HANDLE urlThread;
	DWORD urlThreadID;
	HANDLE chromeUrlThread;
	DWORD chromeUrlThreadID;

	HANDLE windowControlThread;
	DWORD windowControlThreadID;

	wxTextCtrl *winRulesTxt;
	wxButton *winRuleExecBtn;
	wxCheckBox* winHomePage;
	wxTextCtrl *winRulesValueTxt;
	wxTextCtrl *windowTitleTxt;
	wxTextCtrl *windowNameTxt;
	wxTextCtrl *windowDescTxt;
	wxChoice *windowTypeCmb;
	wxButton *winControlButton;
	wxButton *winMaskInfoButton;
	wxButton *winBackToWindowButton;
	
	wxChoice *controlProcessListCmb;
	wxChoice *controlWindowListCmb;
	wxButton *controlUpdateBtn;
	wxListBox* windowControlList;
	wxTextCtrl *ctrlUserNameTxt;
	wxTextCtrl *controlTypeTxt;
	wxTextCtrl *ctrlClickAnnotationRuleTxt;
	wxButton *controlMaskInfoButton;
	wxButton *saveControlButton;
	wxButton *controlBackToWindowButton;
	wxButton *controlBackToProcessButton;
	wxChoice *ctrlTypeListCmb;


	// Mask Screen
	wxButton *openFileBtn;
	wxImage *screenImage;
	MaskImagePanel *imagePanel;
	wxStaticText *areaLbl2;
	wxStaticText *matchDesc1;
	wxStaticText *matchDesc2;
	wxStaticText *matchDesc3;
	wxStaticText *matchDesc4;
	wxStaticText *matchDesc5;
	wxStaticText *matchDesc6;
	wxButton *saveMaskBtn;
	wxButton *maskBackToControlButton;
	wxButton *maskBackToWindowButton;
	wxButton *maskBackToProcessButton;
	wxStaticText *processLbl1;
	wxTextCtrl *processLbl2;
	wxStaticText *windowLbl1;
	wxTextCtrl* windowLbl2;
	wxStaticText *layoutLbl2;
	wxChoice *masksCB;
	wxButton *newMaskBtn;
	wxTextCtrl *maskNameTxt;
	wxChoice *maskTypeCmb;
	wxButton *saveAreaBtn;
	wxButton *saveMatchBtn1;
	wxButton *saveMatchBtn2;
	wxButton *saveMatchBtn3;
	wxButton *saveMatchBtn4;
	wxButton *saveMatchBtn5;
	wxButton *saveMatchBtn6;

	// URL Control Screen
	wxChoice *urlControlListCmb;
	wxListBox* urlControlLB;
	wxButton *urlControlUpdateBtn;
	wxTextCtrl *urlControlIDTxt;
	wxTextCtrl *urlControlNameTxt;
	wxCheckBox *urlMatchNameChkBox;
	wxTextCtrl *urlControlDescTxt;
	wxTextCtrl *urlControlTypeTxt;
	wxTextCtrl *urlControlRectTxt;
	wxCheckBox *urlMatchRectChkBox;
	wxTextCtrl *urlControlClassNameTxt;
	wxTextCtrl *urlControlAutoIDTxt;
	wxTextCtrl *urlControlValueTxt;
	wxCheckBox *urlMatchValueChkBox;
	wxTextCtrl *urlCtrlUserNameTxt;
	wxTextCtrl *urlCtrlClickAnnotationRuleTxt;
	wxTextCtrl *urlCtrlXPathTxt;
	wxTextCtrl *urlCtrlEnterAnnotationRuleTxt;
	wxCheckBox *urlActionDisableChkBox;
	wxCheckBox *urlActionWindowChangedChkBox;
	wxCheckBox *urlActionWindowClosedChkBox;
	wxButton *urlControlCopyInfoButton;
	wxButton *urlControlPasteInfoButton;
	wxButton *urlControlClearInfoButton;
	wxButton *urlControlMaskInfoButton;
	wxButton *urlSaveControlButton;
	wxButton *urlControlBackToURLButton;


	void InitializeScreen();
	wxNotebook *CreateMainProcessTab();
	wxWindow *CreateProcessTab();
	wxWindow *CreateWindowTab();
	wxWindow *CreateControlTab();
	wxWindow *CreateURLControlTab();
	wxWindow *CreateMaskTab();
	wxWindow *CreateWebPageTab();
	void DisableWindowTabControls();
	void EnableWindowTabControls();

	SpyUrlData *GetMatchedURLData(string strURLValue);
	void OnUpdateProcess(wxCommandEvent& event);
	void OnUpdateURLs(wxCommandEvent& event);
	void OnCaptureControls(wxCommandEvent& event);
	void OnExecuteURLRule(wxCommandEvent& event);
	void OnExecuteWindowRule(wxCommandEvent& event);
	
	void OnURLControlSelChanged(wxCommandEvent& event);
	void OnWindowControlSelChanged(wxCommandEvent& event);
	void OnWindowListSelChanged(wxCommandEvent& event);
	void OnProcessListSelChanged(wxCommandEvent& event);
	void OnIsHomeChanged(wxCommandEvent& event);
	void OnURLListSelChanged(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveProcess(wxCommandEvent &event);
	void OnSaveURL(wxCommandEvent &event);
	void OnSaveWindow(wxCommandEvent &event);
	void OnProcessWindowButton(wxCommandEvent& event);
	void OnGotoProcessWindows(wxCommandEvent& event);
	void OnGotoURL(wxCommandEvent& event);
	void OnSaveControls(wxCommandEvent& event);
	void OnURLSaveControls(wxCommandEvent& event);

	void OnMaskWindow(wxCommandEvent& event);
	void OnURLMaskWindow(wxCommandEvent& event);

	void OnControlGotoWindows(wxCommandEvent& event);
	void OnControlGotoProcess(wxCommandEvent& event);

	void OnMaskGotoProcess(wxCommandEvent& event);
	void OnMaskGotoWindows(wxCommandEvent& event);
	void OnMaskGotoControls(wxCommandEvent& event);

	void OnWindowControls(wxCommandEvent& event);
	void OnURLControls(wxCommandEvent& event);
	void OnUpdateProcessWindows(wxCommandEvent& event);
	void GetEdgeURL(DWORD pid, vector <string> &URLs);
	void GetChromeURL(DWORD pid, vector <string> &URLs);
	void GetWindowNames(vector <DWORD> &pIDs);
	vector<DWORD> GetProcessIDs(string strName);
	void ClearItemData();
	void SaveWindow(string pName, string wName);
	void SaveProcessInFile();
	void SaveURLInFile();
	void SaveWindowInFile();
	void SaveControlsInFile();
	void SetMaxScreenSize();

	//void ClearWindowMatchControl(vector <WindowControlData *> *pControlArray);
	//void SetMatchedControlIDs(vector <SpyControlData *> *pControlList);
	//void IdentifyControlMatch(SpyControlData *pControlData);
	map <DWORD, int> GetBrowserPIDs();

	void OnOpenScreenFile(wxCommandEvent& event);
	void OnSaveMaskImage(wxCommandEvent& event);
	void OnSaveMatchImage1(wxCommandEvent& event);
	void OnSaveMatchImage2(wxCommandEvent& event);
	void OnSaveMatchImage3(wxCommandEvent& event);
	void OnSaveMatchImage4(wxCommandEvent& event);
	void OnSaveMatchImage5(wxCommandEvent& event);
	void OnSaveMatchImage6(wxCommandEvent& event);
	void OnNewMask(wxCommandEvent& event);
	void OnSaveMask(wxCommandEvent& event);
	void OnMaskSelChanged(wxCommandEvent& event);
	void InitMaskScreen();
	void ClearThreadData(struct threaddata *pThreadData);


public:
	void UpdateListBoxes();

	SpyEditorScreen(wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & position, const wxSize & size);
	~SpyEditorScreen();

private:
	wxDECLARE_EVENT_TABLE();
};

