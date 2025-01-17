#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <string>
using namespace std;

class BPMNElement;

class InfoTabPanel : public wxPanel
{
	wxNotebook *pParent;

	wxStaticText *pProcessNameLabel;
	wxStaticText *pWindowNameLabel;
	wxStaticText *pWindowPositionLabel;
	wxStaticText *pWindowSizeLabel;
	wxStaticText *pControlTypeNameLabel;
	wxStaticText *pControlPositionLabel;
	wxStaticText *pControlSizeLabel;
	wxStaticText *pDocumentNameLabel;
	wxStaticText *pActivityDurationLabel;

	wxStaticText *pProcessName;
	wxStaticText *pWindowName;
	wxStaticText *pWindowPosition;
	wxStaticText *pWindowSize;
	wxStaticText *pControlTypeName;
	wxStaticText *pControlPosition;
	wxStaticText *pControlSize;
	wxStaticText *pDocumentName;
	wxStaticText *pActivityDuration;

	string strProcessName;
	string strWindowName;
	string strWindowPosition;
	string strWindowSize;
	string strControlType;
	string strControlPosition;
	string strControlSize;
	string strDocumentName;
	double duration;

public:
	InfoTabPanel(wxNotebook *parent, wxSize size);
	~InfoTabPanel();

	void SetProcessName(string value);
	void SetWindowName(string value);
	void SetWindowPosition(string value);
	void SetWindowSize(string value);
	void SetControlType(string value);
	void SetControlPosition(string value);
	void SetControlSize(string value);
	void SetDocumentName(string value);
	void SetActivityDuration(double value);
	void ShowTabData();




};

