#include "stdafx.h"
#include "PDFDocument.h"
#include "PlayerDataFileManager.h"
#include "PDFSOPDoc.h"
#include "MMHeader.h"
#include "ScenarioFacade.h"
#include "ScenarioDefintion.h"
#include "BPMNDiagram.h"
#include "BPMNElement.h"
#include "BPMNSubProcess.h"
#include "BPMNDecision.h"
#include ".\WordReport\lodepng.h"
#include ".\WordReport\lodepng_util.h"

using namespace json11;

PDFDocument::PDFDocument()
{
	pDataFileManager = NULL;
	reportElementList.clear();
}


PDFDocument::~PDFDocument()
{
}

void PDFDocument::SetPlayerManager(PlayerDataFileManager *pValue)
{
	pDataFileManager = pValue;
}

void PDFDocument::GenerateReport()
{
	PDFSOPDoc pdf;

	pdf.AliasNbPages();
	pdf.SetLeftMargin(25.4);
	pdf.SetRightMargin(25.4);
	pdf.SetTopMargin(5.0);

	PrintCoverPage(pdf);

	PrintSummaryPage(pdf);

	pdf.AddPage(wxPORTRAIT, wxPAPER_A4);
	pdf.ReportSubTitle(wxS("Process Steps"));
	pdf.ReportParagraph(wxS("Below are details of the process steps"));
	pdf.Ln(4);

	nLevelNumber = 1;
	list <BPMNElement *> *mainDiagram = pDataFileManager->GetBPMNDiagram();
	psSNo = "";
	PrintReportTable("", pdf, mainDiagram);
	nLevelNumber = 2;
	psSNo = "A";
	PrintReport(pdf, mainDiagram);
	while (reportSections.size() > 0)
	{
		// Get first section
		struct linkReportSections *section = reportSections.front();
		reportSections.pop_front();
		BPMNElement *pElement = section->pElement;
		if (pElement->GetType() == 3)
		{
			PrintSubProcessActivities(pdf, pElement);
		}
		else if (pElement->GetType() == 4)
		{
			PrintBranchActivities(pdf, pElement, section);
		}
	}
	pdf.SaveAsFile(wxS("c:\\Projects\\SOPReport.pdf"));
}

void PDFDocument::PrintReportTable(string pMainSNo, PDFSOPDoc &doc, list <BPMNElement *> *pElementList)
{
	list <BPMNElement *>::iterator itAction = pElementList->begin();
	BPMNElement *pElement = NULL;

	//wxArrayInt *link = new wxArrayInt;
	wxArrayPtrVoid linkData;
	linkData.clear();
	wxArrayString* row = NULL;
	wxArrayInt* links = NULL;
	wxArrayPtrVoid data;
	data.clear();
	int linkSNo = 0;
	int sNo = 0;
	int linkVal = 0;
	string strProcessName = "";
	string strWindowName = "";
	string strDocumentName = "";
	string strOldProcessName = "";
	string strOldWindowName = "";
	string strOldDocumentName = "";
	bool bFirst = true;
	while (itAction != pElementList->end())
	{
		pElement = *itAction;
		wxString cellData = "";

		string strAnnotation = pElement->GetKeyValueAsString("Annotation");
		wxString actionString = "";
		if (pElement->GetType() == 2)
			actionString = "Action: " + strAnnotation;
		else if (pElement->GetType() == 3)
			actionString = "Subprocess: " + strAnnotation;
		else if (pElement->GetType() == 4)
			actionString = "Branch: " + strAnnotation;
		if (actionString != "")
		{
			row = DBG_NEW wxArrayString;
			links = DBG_NEW wxArrayInt;

			linkData.Add(links);
			data.Add(row);

			sNo++;
			cellData = wxString::Format("%d", sNo);
			row->Add(cellData);
			links->Add(0L);
			cellData = actionString;
			row->Add(cellData);
			links->Add(0L);

			strProcessName = pElement->GetProcessName();
			strWindowName = pElement->GetKeyValueAsString("WName");
			strDocumentName = pElement->GetKeyValueAsString("DocName");
			if (bFirst)
			{
				strOldProcessName = strProcessName;
				strOldWindowName = strWindowName;
				strOldDocumentName = strDocumentName;
				linkVal = doc.AddLink();
				linkSNo++;
			}
			if ((strProcessName != "" && strWindowName != "") && (strProcessName == strOldProcessName && strWindowName == strOldWindowName && strDocumentName == strOldDocumentName))
			{
				;
			}
			else if (!bFirst)
			{
				linkSNo++;
				linkVal = doc.AddLink();
			}
			bFirst = false;
			strOldProcessName = strProcessName;
			strOldWindowName = strWindowName;
			strOldDocumentName = strDocumentName;
			if (pMainSNo == "")
			{
				cellData = wxString::Format("%d", linkSNo);
			}
			else
			{
				cellData = wxS("Section: ") + pMainSNo + wxString::Format(".%d", linkSNo);
			}
			row->Add(cellData);
			links->Add(linkVal);
			struct linkReportSections *pSections = DBG_NEW struct linkReportSections;
			pSections->linkSNo = cellData;
			pSections->link = linkVal;
			pSections->pElement = pElement;
			reportSections.push_back(pSections);
		}
		itAction++;
	}

	double w[3];
	wxArrayString header;
	header.clear();
	header.Add(wxT("S No."));
	header.Add(wxT("Action"));
	header.Add(wxT("Link"));

	wxPdfAlignment align[3];

	align[0] = wxPDF_ALIGN_CENTER;
	align[1] = wxPDF_ALIGN_LEFT;
	align[2] = wxPDF_ALIGN_CENTER;

	double pWidth = doc.GetPageWidth();
	pWidth -= 50.8;
	w[0] = pWidth * 0.15;
	w[1] = pWidth * 0.60;
	w[2] = pWidth * 0.25;

	doc.SetFont(wxS("Arial"), wxS(""), 10.0);
	doc.DataLinkTable(header, data, linkData, w, align);
}


void PDFDocument::PrintCoverPage(PDFSOPDoc &doc)
{

	string strValue = pDataFileManager->ReadProcessValue();
	string err;
	Json jSonElement;
	map <string, Json> processInfo;
	string processName = "";
	string processDescription = "";

	jSonElement = Json::parse(strValue, err);
	processInfo = jSonElement["ProcessInfo"].object_items();
	map<string, Json>::iterator itProcessInfo = processInfo.begin();
	while (itProcessInfo != processInfo.end())
	{
		string key = itProcessInfo->first;
		string val = itProcessInfo->second.string_value();
		if (key == "Name")
		{
			processName = val;
		}
		else if (key == "Description")
		{
			processDescription = val;
		}
		itProcessInfo++;
	}

	doc.AddPage(wxPORTRAIT, wxPAPER_A4);
	doc.SetFont(wxS("Arial"), wxS("B"), 18.0);
	doc.Ln(71.2);
	doc.MultiCell(0, 8, wxS("Standard Operating Procedure for \"") + processName + wxS("\" Process"), wxPDF_BORDER_NONE, wxPDF_ALIGN_CENTER);

}

void PDFDocument::PrintSummaryPage(PDFSOPDoc &doc)
{
	string strValue = pDataFileManager->ReadProcessValue();
	string err;
	Json jSonElement;
	map <string, Json> processInfo;
	string processName = "";
	string processDescription = "";

	jSonElement = Json::parse(strValue, err);
	processInfo = jSonElement["ProcessInfo"].object_items();
	map<string, Json>::iterator itProcessInfo = processInfo.begin();
	while (itProcessInfo != processInfo.end())
	{
		string key = itProcessInfo->first;
		string val = itProcessInfo->second.string_value();
		if (key == "Name")
		{
			processName = val;
		}
		else if (key == "Description")
		{
			processDescription = val;
		}
		itProcessInfo++;
	}

	doc.AddPage(wxPORTRAIT, wxPAPER_A4);
	doc.ReportSubTitle(wxS("Process Name"));
	doc.ReportParagraph(processName);
	doc.Ln(3.0);
	doc.ReportSubTitle(wxS("Process Description"));
	doc.ReportParagraph(processDescription);
	
	ReportScenarioTable(doc);
	PrintProcessWorkflow(doc);

}

void PDFDocument::ReportScenarioTable(PDFSOPDoc &doc)
{
	string strScenarioValue = pDataFileManager->GetScenarioJSON();
	ScenarioFacade scenarios(strScenarioValue);
	vector <string> scenarioNames = scenarios.GetScenarioNames();
	vector <string>::iterator itScenarioName = scenarioNames.begin();
	struct scenarioReportData scenarioData;
	wxArrayString* row = new wxArrayString;
	wxArrayPtrVoid data;
	data.clear();
	int sNo = 1;
	row = DBG_NEW wxArrayString;
	scenarioData.scenarioName = "Default";
	scenarioData.scenarioDesc = "";
	scenarioData.noOfSteps = 0;
	scenarioData.timeInSecs = 0.0;
	pDataFileManager->GetScenarioData(&scenarioData, NULL);
	data.Add(row);
	wxString cellData = "";

	cellData = wxString::Format("%d", sNo);
	row->Add(cellData);

	cellData = scenarioData.scenarioName + "\r\n" + scenarioData.scenarioDesc;
	row->Add(cellData);

	cellData = wxString::Format("%7.2lf", scenarioData.timeInSecs);
	row->Add(cellData);

	cellData = wxString::Format("%d", scenarioData.noOfSteps);
	row->Add(cellData);

	while (itScenarioName != scenarioNames.end())
	{
		string scenarioName = *itScenarioName;
		ScenarioDefintion *pScenarioDef = scenarios.GetScenario(scenarioName);
		scenarioData.scenarioName = pScenarioDef->GetName();
		scenarioData.scenarioDesc = pScenarioDef->GetDesc();
		scenarioData.noOfSteps = 0;
		scenarioData.timeInSecs = 0.0;
		pDataFileManager->GetScenarioData(&scenarioData, pScenarioDef);
		row = DBG_NEW wxArrayString;
		data.Add(row);
		cellData = "";
		sNo++;
		cellData = wxString::Format("%d", sNo);
		row->Add(cellData);

		cellData = scenarioData.scenarioName + "\n" + scenarioData.scenarioDesc;
		row->Add(cellData);

		cellData = wxString::Format("%7.2lf", scenarioData.timeInSecs);
		row->Add(cellData);

		cellData = wxString::Format("%d", scenarioData.noOfSteps);
		row->Add(cellData);

		itScenarioName++;
	}

	double w[4];
	wxArrayString header;
	header.clear();
	header.Add(wxT("S.No"));
	header.Add(wxT("Scenario Name & Description"));
	header.Add(wxT("Total Time\n(in Secs)"));
	header.Add(wxT("No. of Steps"));

	wxPdfAlignment align[4];

	align[0] = wxPDF_ALIGN_CENTER;
	align[1] = wxPDF_ALIGN_LEFT;
	align[2] = wxPDF_ALIGN_RIGHT;
	align[3] = wxPDF_ALIGN_RIGHT;


	double pWidth = doc.GetPageWidth();
	pWidth -= 50.8;
	w[0] = pWidth * 0.1;
	w[1] = pWidth * 0.6;
	w[2] = w[3] = pWidth * 0.15;

	doc.ReportSubTitle(wxT("Process Scenarios"));
	doc.ReportParagraph(wxT("The following are the list of scenario's defined within the process."));
	doc.SetFont(wxS("Arial"), wxS(""), 10.0);
	doc.DataTable(header, data, w, align);
}

void PDFDocument::PrintProcessWorkflow(PDFSOPDoc &doc)
{
	list <BPMNElement *> *mainDiagram = pDataFileManager->GetBPMNDiagram();
	if (mainDiagram == NULL)
		return;
	BPMNDiagram *pDiagramMap = DBG_NEW BPMNDiagram();
	pDiagramMap->SetController(pDataFileManager);
	list<BPMNElement *>::iterator it;

	HDC hDC = GetDC(NULL);
	SetMapMode(hDC, MM_HIMETRIC);
	HFONT hFont = CreateFont(382, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);

	it = mainDiagram->begin();

	while (it != mainDiagram->end())
	{
		BPMNElement *pElement = *it;
		pElement->GetControlDimension(hDC);
		it++;
	}
	SelectObject(hDC, hFontOld);
	DeleteObject(hFont);
	DeleteDC(hDC);
	// Get dimensions of all controls
	pDiagramMap->Clear();
	pDiagramMap->FindDiagramDimensions(mainDiagram);
	pDiagramMap->SetDrawPosition();

	vector <unsigned char> buffer;
	SIZE szBmp;
	pDiagramMap->CreateLevelDiagram(mainDiagram);
	pDiagramMap->CreateBPMNBitmap(buffer, &szBmp);
	// Convert szBMP to cms
	if (szBmp.cy < 0)
		szBmp.cy = -szBmp.cy;

	double widthCMS = (szBmp.cx / 914400.0f) * 25.4f;
	double heightCMS = szBmp.cy / 914400.0f * 25.4f;

	double aspectRatio = (double)heightCMS / (double)widthCMS;
	
	double heightPage = (doc.GetPageHeight() - 50.8);
	double widthPage = (doc.GetPageWidth() - 50.8);
	double cx, cy;
	long offsetcx = 0L;
	cx = widthCMS;
	cy = heightCMS;
	if (widthCMS > widthPage || heightCMS > heightPage)
	{
		if (heightCMS > widthCMS)
		{
			// portrait image
			cy = heightPage;
			// scale the width accordingly
			cx = cy / aspectRatio;
			offsetcx = (widthPage - cx) / 2;
			if (cx > widthPage)
			{
				cx = widthPage;
				cy = cx * aspectRatio;
				offsetcx = 0;
			}
		}
		else
		{
			// landscape image
			cx = widthPage;
			// scale the width accordingly
			cy = cx * aspectRatio;
			if (cy > heightPage)
			{
				cy = heightPage;
				cx = cy / aspectRatio;
				offsetcx = (widthPage - cx) / 2;
			}
		}
	}
	else
	{
		if (cx < widthPage)
			offsetcx = (widthPage - cx) / 2;
	}

	doc.AddPage(wxPORTRAIT, wxPAPER_A4);
	doc.ReportSubTitle(wxT("Process Workflow"));
	string imageName = WritePDFImageFile(buffer, true);
	doc.Image(imageName, offsetcx + 25.4, doc.GetY(), cx, cy);
	doc.Ln();
	delete pDiagramMap;

}


string PDFDocument::WritePDFImageFile(vector <unsigned char> &buffer, bool bDecode)
{
	static int imageNum;

	imageNum++;
	string imageName;
	
	char str[100];
	sprintf_s(str, "Image%03d.png", imageNum);
	imageName = str;
	std::vector<unsigned char> image;
	unsigned w, h;
	unsigned error = 0;
	if (bDecode)
		lodepng::decodeBMP(image, w, h, buffer);
	else
		lodepng::decodeBMPNoInvert(image, w, h, buffer);
	if (!error)
	{
		std::vector<unsigned char> png;
		error = lodepng::encode(png, image, w, h);
		if (!error)
		{
			lodepng::save_file(png, imageName);
		}
	}

	return imageName;
}

void PDFDocument::PrintReport(PDFSOPDoc &doc, list <BPMNElement *> *pElementList)
{

	list <BPMNElement *>::iterator itEvent = pElementList->begin();
	//int subLevelNo = 0;
	string oldProcessName = "";
	string oldWindowName = "";
	vector <BPMNElement *> taskGroup;
	taskGroup.clear();
	bool bNewTaskGroup = true;
	BPMNElement *pElement = NULL;
	int oldLinkNum = 0;
	//int linkNum = 0;
	struct linkReportSections *sectionDetails = NULL;
	struct linkReportSections *oldSectionDetails = NULL;
	while (itEvent != pElementList->end())
	{
		pElement = *itEvent;
		if (reportSections.size() > 0)
			sectionDetails = reportSections.front();
		else
			sectionDetails = NULL;
		if (sectionDetails != NULL && sectionDetails->pElement == pElement)
		{
			reportSections.pop_front();
			if (pElement->GetType() == 2)
			{

				if (bNewTaskGroup)
				{
					oldLinkNum = sectionDetails->link;
					oldSectionDetails = sectionDetails;
				}
				bNewTaskGroup = false;
				if (oldLinkNum == sectionDetails->link)
				{
					taskGroup.push_back(pElement);
					bNewTaskGroup = false;
				}
				else
				{
					PrintTaskGroup(doc, taskGroup, oldSectionDetails);
					oldLinkNum = sectionDetails->link;
					oldSectionDetails = sectionDetails;
					taskGroup.clear();
					taskGroup.push_back(pElement);
				}
			}
			else if (pElement->GetType() == 3)
			{
				if (taskGroup.size() > 0)
				{
					PrintTaskGroup(doc, taskGroup, oldSectionDetails);
					taskGroup.clear();
				}
				oldLinkNum = sectionDetails->link;
				bNewTaskGroup = true;
				reportSections.push_back(sectionDetails);
				PrintSubProcess(doc, pElement, sectionDetails);
				oldSectionDetails = sectionDetails;
			}
			else if (pElement->GetType() == 4)
			{
				if (taskGroup.size() > 0)
				{
					PrintTaskGroup(doc, taskGroup, oldSectionDetails);
					taskGroup.clear();
				}
				oldLinkNum = sectionDetails->link;
				bNewTaskGroup = true;
				reportSections.push_back(sectionDetails);
				PrintRightBranch(doc, pElement, sectionDetails);
				PrintBottomBranch(doc, pElement, sectionDetails);
				oldSectionDetails = sectionDetails;
			}
		}
		itEvent++;
	}

	if (taskGroup.size() > 0)
		PrintTaskGroup(doc, taskGroup, oldSectionDetails);
	if (pElement != NULL)
	{
		if (pElement->GetType() == 3)
		{

		}
		else if (pElement->GetType() == 4)
		{
			reportSections.push_back(sectionDetails);
			PrintRightBranch(doc, pElement, sectionDetails);
			PrintBottomBranch(doc, pElement, sectionDetails);
		}
	}
}


void PDFDocument::PrintTaskGroup(PDFSOPDoc &doc, vector <BPMNElement *> &pElementList, struct linkReportSections *pSectionDetails)
{
	if (pElementList.size() == 0)
		return;
	BPMNElement *pElement = NULL;

	pElement = pElementList.front();
	string strProcessName = pElement->GetProcessName();
	string strWindowName = pElement->GetKeyValueAsString("WName");
	string strDocumentName = pElement->GetKeyValueAsString("Document");

	wxString actionString = wxS("");
	if (strProcessName != "")
	{
		actionString = strProcessName;
		if (strDocumentName != "")
			actionString += wxS(" Url: ") + strDocumentName;
		else if (strWindowName != "")
			actionString += wxS(" Window: ") + strWindowName;
	}
	else
	{
		actionString = "";
		if (strDocumentName != "")
			actionString += wxS("Url: ") + strDocumentName;
		else if (strWindowName != "")
			actionString += wxS("Window: ") + strWindowName;
	}
	
	if (pElementList.size() == 1)
	{
		actionString = pSectionDetails->linkSNo + wxS(" ") + actionString + wxS(" Action ") + pElement->GetKeyValueAsString("Annotation");
	}
	else
	{
		actionString = pSectionDetails->linkSNo + wxS(" ") + actionString + wxS(" Action Group");
	}
	doc.ReportSubTitleLink(actionString, pSectionDetails->link);
	doc.Ln(4);
	ReportGroupTable(doc, pElementList);
	
	double yPos = doc.GetY();
	double screenHeight = doc.GetPageHeight() - 25.4 - yPos;

	vector <unsigned char> buffer;
	ULONG bmpWidth = 0L, bmpHeight = 0L;
	BPMNDiagram *pDiagramMap = DBG_NEW BPMNDiagram();
	pDiagramMap->SetController(pDataFileManager);

	pDiagramMap->GetEventBitmap(buffer, bmpWidth, bmpHeight, pElement->GetEventIndex(), pElementList);
	//bmpWidth = 13106400;
	//bmpHeight = 7410450;

	double widthCMS = ((double)bmpWidth * 25.4f) / 914400.0f;
	double heightCMS = ((double)bmpHeight * 25.4f) / 914400.0f;

	double aspectRatio = (double)heightCMS / (double)widthCMS;
	double heightPage = screenHeight;
	double widthPage = (doc.GetPageWidth() - 50.8);

	double cx, cy;
	long offsetcx = 0L;
	cx = widthCMS;
	cy = heightCMS;
	if (widthCMS > widthPage || heightCMS > heightPage)
	{
		if (heightCMS > widthCMS)
		{
			// portrait image
			cy = heightPage;
			// scale the width accordingly
			cx = cy / aspectRatio;
			offsetcx = (widthPage - cx) / 2;
			if (cx > widthPage)
			{
				cx = widthPage;
				cy = cx * aspectRatio;
				offsetcx = 0;
			}
		}
		else
		{
			// landscape image
			cx = widthPage;
			// scale the width accordingly
			cy = cx * aspectRatio;
			if (cy > heightPage)
			{
				cy = heightPage;
				cx = cy / aspectRatio;
				offsetcx = (widthPage - cx) / 2;
			}
		}
	}
	else
	{
		if (cx < widthPage)
			offsetcx = (widthPage - cx) / 2;
	}
	yPos = doc.GetY();
	doc.Ln(12.7);
	yPos = doc.GetY();
	//string imageName = "Image002.png";
	string imageName = WritePDFImageFile(buffer, false);
	doc.Image(imageName, offsetcx + 25.4, doc.GetY(), cx, cy);
	doc.SetX(0);
	doc.Rect(25.4 + offsetcx, yPos, cx, cy);
	doc.SetY(yPos + cy);
	doc.Ln(12.7);
	yPos = doc.GetY();
	screenHeight = doc.GetPageHeight() - 25.4 - yPos;

	if (screenHeight < pElementList.size() * 12.7)
		doc.AddPage(wxPORTRAIT, wxPAPER_A4);

	ReportGroupActionsTable(doc, pElementList);
	doc.AddPage(wxPORTRAIT, wxPAPER_A4);
	delete pDiagramMap;
}

void PDFDocument::ReportGroupActionsTable(PDFSOPDoc &doc, vector <BPMNElement *> &pElementList)
{
	vector <BPMNElement *>::iterator itAction = pElementList.begin();
	BPMNElement *pElement = NULL;

	wxArrayString* row = new wxArrayString;
	wxArrayPtrVoid data;
	data.clear();
	int sNo = 0;
	while (itAction != pElementList.end())
	{
		pElement = *itAction;

		row = DBG_NEW wxArrayString;
		data.Add(row);
		wxString cellData = "";

		sNo++;
		cellData = wxString::Format("%d", sNo);
		row->Add(cellData);

		string strAnnotation = pElement->GetKeyValueAsString("Annotation");
		string strNotes = pElement->GetKeyValueAsString("Notes");
		wxString actionString = "";
		actionString = strAnnotation;
		cellData = actionString;
		row->Add(cellData);
		double dur = (double)pElement->GetKeyValueAsInt("Duration") / 1000000.0f;
		cellData = wxString::Format("%7.2lf secs", dur);
		row->Add(cellData);

		cellData = strNotes;
		row->Add(cellData);
		itAction++;
	}

	double w[4];
	wxArrayString header;
	header.clear();
	header.Add(wxT("Step"));
	header.Add(wxT("Action"));
	header.Add(wxT("Duration (in Secs)"));
	header.Add(wxT("Remarks"));

	wxPdfAlignment align[4];

	align[0] = wxPDF_ALIGN_CENTER;
	align[1] = wxPDF_ALIGN_LEFT;
	align[2] = wxPDF_ALIGN_CENTER;
	align[3] = wxPDF_ALIGN_LEFT;

	double pWidth = doc.GetPageWidth();
	pWidth -= 50.8;
	w[0] = pWidth * 0.1;
	w[1] = pWidth * 0.35;
	w[2] = pWidth * 0.2;
	w[3] = pWidth * 0.35;

	doc.SetFont(wxS("Arial"), wxS(""), 10.0);
	doc.DataTable(header, data, w, align);
}

void PDFDocument::ReportGroupTable(PDFSOPDoc &doc, vector <BPMNElement *> &pElementList)
{
	wxArrayString* row = new wxArrayString;
	wxArrayPtrVoid data;
	data.clear();
	BPMNElement *pElement = pElementList.front();
	string strProcessName = pElement->GetProcessName();
	string strWindowName = pElement->GetKeyValueAsString("WName");

	wxString actionString = wxS("");
	actionString = strProcessName + wxS("-") + strWindowName;

	row = DBG_NEW wxArrayString;
	data.Add(row);
	wxString cellData = "";

	cellData = actionString;
	row->Add(cellData);

	vector <BPMNElement *>::iterator itElement = pElementList.begin();
	double groupDuration = 0.0;
	while (itElement != pElementList.end())
	{
		pElement = *itElement;
		double dur = (double)pElement->GetKeyValueAsInt("Duration") / 1000000.0f;
		groupDuration += dur;
		itElement++;
	}

	cellData = wxString::Format("%d", pElementList.size());
	row->Add(cellData);

	cellData = wxString::Format("%7.2lf Seconds", groupDuration);
	row->Add(cellData);

	double w[3];
	wxArrayString header;
	header.clear();
	header.Add(wxT("Group"));
	header.Add(wxT("Total Steps"));
	header.Add(wxT("Total Duration (in Secs)"));
	wxPdfAlignment align[3];

	align[0] = wxPDF_ALIGN_LEFT;
	align[1] = wxPDF_ALIGN_CENTER;
	align[2] = wxPDF_ALIGN_CENTER;

	double pWidth = doc.GetPageWidth();
	pWidth -= 50.8;
	w[0] = pWidth * 0.6;
	w[1] = pWidth * 0.2;
	w[2] = pWidth * 0.2;

	doc.SetFont(wxS("Arial"), wxS(""), 10.0);
	doc.DataTable(header, data, w, align);
}

void PDFDocument::PrintSubProcessActivities(PDFSOPDoc &doc, BPMNElement *pElement)
{
	//wxString actionString = wxS("");
	//actionString = wxS(" Sub-Process: ") + pElement->GetKeyValueAsString("Annotation");
	//doc.ReportSubTitle(actionString);
	//doc.ReportParagraph(pElement->GetKeyValueAsString("Notes"));

	BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pElement;
	PrintReport(doc, pSubProcess->GetSubProcessElements());
}


void PDFDocument::PrintSubProcess(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails)
{
	wxString actionString = wxS("");
	actionString = pSectionDetails->linkSNo + wxS(" Sub-Process: ") +  pElement->GetKeyValueAsString("Annotation");
	doc.ReportSubTitleLink(actionString, pSectionDetails->link);
	doc.Ln(4);
	doc.ReportParagraph(pElement->GetKeyValueAsString("Notes"));
	doc.Ln(4);

	double yPos = doc.GetY();
	double pageHeight = doc.GetPageHeight();
	if (yPos > pageHeight / 2.0f)
	{
		// Go to Next Page
		doc.AddPage(wxPORTRAIT, wxPAPER_A4);
	}

	yPos = doc.GetY();
	pageHeight = pageHeight - 25.4;
	//double pageWidth = doc.GetPageWidth() - 50.8f;
	BPMNDiagram *pDiagramMap = DBG_NEW BPMNDiagram();
	pDiagramMap->SetController(pDataFileManager);
	BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pElement;
	
	pDiagramMap->CreateLevelDiagram(pSubProcess->GetSubProcessElements());

	vector <unsigned char> buffer;
	SIZE szBmp;
	pDiagramMap->CreateBPMNBitmap(buffer, &szBmp);
	// Convert szBMP to cms
	szBmp.cy = szBmp.cy;

	double widthCMS = (szBmp.cx / 914400.0f) * 25.4f;
	double heightCMS = szBmp.cy / 914400.0f * 25.4f;

	double aspectRatio = (double)heightCMS / (double)widthCMS;

	double heightPage = pageHeight - yPos - 12.0;
	double widthPage = (doc.GetPageWidth() - 50.8);
	double cx, cy;
	long offsetcx = 0L;
	cx = widthCMS;
	cy = heightCMS;
	if (widthCMS > widthPage || heightCMS > heightPage)
	{
		if (heightCMS > widthCMS)
		{
			// portrait image
			cy = heightPage;
			// scale the width accordingly
			cx = cy / aspectRatio;
			offsetcx = (widthPage - cx) / 2;
			if (cx > widthPage)
			{
				cx = widthPage;
				cy = cx * aspectRatio;
				offsetcx = 0;
			}
		}
		else
		{
			// landscape image
			cx = widthPage;
			// scale the width accordingly
			cy = cx * aspectRatio;
			if (cy > heightPage)
			{
				cy = heightPage;
				cx = cy / aspectRatio;
				offsetcx = (widthPage - cx) / 2;
			}
		}
	}
	else
	{
		if (cx < widthPage)
			offsetcx = (widthPage - cx) / 2;
	}

	string imageName = WritePDFImageFile(buffer, true);
	doc.Image(imageName, offsetcx + 25.4, doc.GetY(), cx, cy);
	doc.Ln();
	delete pDiagramMap;

	doc.AddPage(wxPORTRAIT, wxPAPER_A4);
	reportSections.push_back(pSectionDetails);
	if (pSectionDetails->linkSNo[0] >= '0' && pSectionDetails->linkSNo <= '9')
	{
		PrintReportTable(psSNo, doc, pSubProcess->GetSubProcessElements());
		psSNo[0] = psSNo[0] + 1;
	}
	else
		PrintReportTable(pSectionDetails->linkSNo,doc, pSubProcess->GetSubProcessElements());
}

void PDFDocument::PrintBranchActivities(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails)
{
	wxString actionString = wxS("");
	actionString = pSectionDetails->linkSNo + wxS("Branch: ") + pElement->GetKeyValueAsString("Annotation");
	doc.ReportSubTitle(actionString);
	doc.Ln(4);
	doc.ReportParagraph(pElement->GetKeyValueAsString("Notes"));
	doc.Ln(4);

	BPMNDecision *pDecision = (BPMNDecision *)pElement;
	PrintReport(doc, pDecision->GetBranchRightElements());
	PrintReport(doc, pDecision->GetBranchBottomElements());

}

void PDFDocument::PrintRightBranch(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails)
{
	wxString actionString = wxS("");
	actionString = pSectionDetails->linkSNo + wxS("Branch Right: ") + pElement->GetKeyValueAsString("Annotation");
	doc.ReportSubTitleLink(actionString, pSectionDetails->link);
	doc.Ln(4);
	doc.ReportParagraph(pElement->GetKeyValueAsString("Notes"));
	doc.Ln(4);
	BPMNDecision *pDecision = (BPMNDecision *)pElement;

	PrintReportTable(pSectionDetails->linkSNo, doc, pDecision->GetBranchRightElements());
}

void PDFDocument::PrintBottomBranch(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails)
{
	wxString actionString = wxS("");
	actionString = pSectionDetails->linkSNo + wxS(" Branch Bottom: ") + pElement->GetKeyValueAsString("AnnotationBottom");

	doc.ReportSubTitleLink(actionString, pSectionDetails->link);
	doc.Ln(4);
	doc.ReportParagraph(pElement->GetKeyValueAsString("Notes"));
	doc.Ln(4);
	BPMNDecision *pDecision = (BPMNDecision *)pElement;

	PrintReportTable(pSectionDetails->linkSNo, doc, pDecision->GetBranchBottomElements());
}




