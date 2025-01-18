#include "stdafx.h"
#include "SOPWordReport.h"
#include "DocXDocPart.h"
#include "DocXTag.h"
#include "DocXTagAttributes.h"
#include "zip.h"
#include <string>
using namespace std;

#include "WordDocument.h"
#include "DocumentBody.h"
#include "Table.h"
#include "TableRow.h"
#include "TableCell.h"
#include "Paragraph.h"
#include "ParagraphRun.h"
#include "DrawingAnchor.h"
#include "PropertyStructures.h"
#include "Footer.h"
#include "Header.h"
#include "Font.h"
#include "Numbering.h"
#include "TOCsdt.h"
#include "json11.hpp"
#include "BPMNElement.h"
#include "BPMNDiagram.h"
#include "BPMNSubProcess.h"
#include "Utility.h"

using namespace json11;

SOPWordReport::SOPWordReport(string docName)  
{ 
	reportName = docName;
	bSetting = false;
	bWebSetting = false;
	bStyle = false;
	bFontTable = false;
	bTheme1 = false;
	bNumbering = false;
}


SOPWordReport::~SOPWordReport()
{

}

void SOPWordReport::SetProcessValue(string value)
{
	string err;
	jSonElement = Json::parse(value, err);
	processInfo = jSonElement["ProcessInfo"].object_items();
}

void SOPWordReport::SetPlayerManager(PlayerDataFileManager *pVal)
{
	pPlayerManager = pVal;
}

void SOPWordReport::SetDiagram(list <BPMNElement *> *pVal)
{
	pElementList = pVal;
	if (pElementList == NULL)
		return;
	pDiagramMap = DBG_NEW BPMNDiagram();
	pDiagramMap->SetController(pPlayerManager);
	list<BPMNElement *>::iterator it;

	HDC hDC = GetDC(NULL);
	SetMapMode(hDC, MM_HIMETRIC);
	HFONT hFont = CreateFont(382, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);

	it = pElementList->begin();

	while (it != pElementList->end())
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
	pDiagramMap->FindDiagramDimensions(pElementList);
	pDiagramMap->SetDrawPosition();
}


void SOPWordReport::CreateSOPReport()
{
	WordDocument *pDocument = DBG_NEW WordDocument(reportName.c_str());
	DocumentBody *pBody = pDocument->AddBody();
	pDocument->BuildStyleDocument();
	CreateSettingsFile(pDocument);
	CreateWebSettingsFile(pDocument);
	//CreateFootnotesFile(pDocument);
	//CreateEndnotesFile(pDocument);
	/*----------------------------------------------------------------------------------------*/
	// Create the remaining documents
	/*----------------------------------------------------------------------------------------*/
	CreateApplicationPropertiesFile(pDocument);
	CreateCorePropertiesFile(pDocument);
	CreateHeaderFile(pDocument);
	CreateMainFooterFile(pDocument);
	CreateFooterFile(pDocument);
	CreateFontTableFile(pDocument);
	CreateTheme1File(pDocument);
	CreateNumberingFile(pDocument);

	DocXDocPart::pageNumber = 1;
	CreateCoverPage(pDocument, pBody);
	CreateControlPage(pBody);
	CreateTOC(pBody);
	CreateProcessSummaryPage(pDocument, pBody);
	CreateProcessStep(pDocument, pBody);

	EndTOC();
	Paragraph *paragraph = pBody->AddParagraph();

	string bookMarkId = GetNextBookmarkID();
	paragraph->SetBookmarkStart(bookMarkId, "_GoBack");
	paragraph->SetBookmarkEnd(bookMarkId);

	pBody->SetHeaderReference("default", pDocument->GetHeaderReferenceID("default"));
	pBody->SetFooterReference("default", pDocument->GetFooterReferenceID("default"));
	pBody->SetFooterReference("first", pDocument->GetFooterReferenceID("first"));
	pBody->SetPageSize(12240, 15840);
	pBody->SetPageMargin(1440, 1440, 1440, 1440, 720, 720, 0);
	pBody->SetColumnSpace(720);
	pBody->SetTitlePage();
	pBody->SetDocGridLinePitch(360);


	pDocument->GenerateDocument();
	delete pDocument;
}

void SOPWordReport::CreateProcessStep(WordDocument *pDocument, DocumentBody *pBody)
{
	//TOCsdt *toc = DocXDocPart::TOC;
	Paragraph *para = NULL;
	ParagraphRun *pRun = NULL;

	/*------------------------------ Adding a Header -----------------------------------------------------------*/
	para = pBody->AddParagraph();
	para->SetStyle("Heading1");
	para->SetNumberProperties(3, 0);
	para->SetLeftIndent(540, 540);
	para->SetFont("Arial");

	string bookMarkId = GetNextBookmarkID();
	string tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId, tocID);
	AddTOC("TOC1", 440, 9350, "Process Steps", tocID, "2.", DocXDocPart::pageNumber);
	string bookMarkId2 = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId2, tocID);

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText("Process Steps", false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);


	/*----------------------------------End of Header ------------------------------------------------------------*/
	WriteProcessSteps(pElementList, "2", 1, pDocument, pBody);
}

void SOPWordReport::WriteProcessSteps(list <BPMNElement *> *eventList, string strLevel, int levelDepth, WordDocument *pDocument, DocumentBody *pBody)
{
	list <BPMNElement *>::iterator itEvent = eventList->begin();
	int subLevelNo = 0;
	while (itEvent != eventList->end())
	{
		BPMNElement *pElement = *itEvent;
		if (pElement->GetType() == 1 || pElement->GetType() == 5)
		{
			// if start and stop elementns then continue to next
			itEvent++;
			continue;
		}
		subLevelNo++;
		if (pElement->GetType() == 2)
		{ // Task Event
			WriteTaskSteps(pElement, strLevel, subLevelNo, levelDepth, pDocument, pBody);
		}
		else if (pElement->GetType() == 3)
		{
			WriteSubProcessSteps(pElement, strLevel, subLevelNo, levelDepth, pDocument, pBody);
		}
		itEvent++;
	}
}

void SOPWordReport::WriteSubProcessSteps(BPMNElement *pElement, string strLevel, int subLevelNo, int levelDepth, WordDocument *pDocument, DocumentBody *pBody)
{
	BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pElement;

	Paragraph *para = NULL;
	para = pBody->AddParagraph();
	para->SetStyle("Heading2");
	para->SetNumberProperties(3, levelDepth);
	para->SetLeftIndent(540, 540);
	para->SetFont("Arial");

	string strAnnotation = pElement->GetKeyValueAsString("Annotation");
	string strNotes = pElement->GetKeyValueAsString("Notes");

	string bookMarkId = GetNextBookmarkID();
	string tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId, tocID);
	char str[100];
	string tocStr;

	sprintf_s(str, "TOC%d", levelDepth + 1);
	tocStr = str;

	sprintf_s(str, ".%d", subLevelNo);
	int tabIndent = 440 + (220 * levelDepth);
	AddTOC(tocStr, tabIndent, 0, strAnnotation, tocID, strLevel + str, DocXDocPart::pageNumber);
	string bookMarkId2 = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId2, tocID);

	ParagraphRun *pRun = NULL;
	pRun = para->AddRun();
	//pRun->SetFont("Arial");
	pRun->SetText(strAnnotation, false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);

	para = pBody->AddParagraph();
	para->SetFont("Arial");

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText(strNotes, false);

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText("Sub Proces Flow", false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);

	/*----------------------------------End of Header ------------------------------------------------------------*/

	para = pBody->AddParagraph();
	para->SetFont("Arial");

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText("Below is the Sub Process Flow", false);


	para = pBody->AddParagraph();
	para->SetFont("Arial");
	pRun = para->AddRun();
	pRun->SetNoProof();
	struct PictureInsertionProperties pictInsProperties;
	struct PictureProperties pictProperties;
	vector <unsigned char> buffer;
	SIZE szBmp;
	
	pDiagramMap->CreateLevelDiagram(pSubProcess->GetSubProcessElements());
	pDiagramMap->CreateBPMNBitmap(buffer, &szBmp);
	double aspectRatio = (double)szBmp.cy / (double)szBmp.cx;

	double heightEmu = 7.5f * 914400.0f;
	double widthEmu = 5.9f * 914400.0f;
	long cx = szBmp.cx, cy = szBmp.cy;
	long offsetcx = 0L;
	if (szBmp.cx > widthEmu || szBmp.cy > heightEmu)
	{
		if (szBmp.cy > szBmp.cx)
		{
			// portrait image
			cy = (LONG)heightEmu;
			// scale the width accordingly
			cx = (LONG)((double)cy / aspectRatio);
			offsetcx = (widthEmu - cx) / 2;
			if (cx > widthEmu)
			{
				cx = (LONG)widthEmu;
				cy = (LONG)((double)cx * aspectRatio);
				offsetcx = 0;
			}
		}
		else
		{
			// landscape image
			cx = (LONG)widthEmu;
			// scale the width accordingly
			cy = (LONG)((double)cx * aspectRatio);
			if (cy > heightEmu)
			{
				cy = (LONG)heightEmu;
				cx = (LONG)((double)cy / aspectRatio);
				offsetcx = (widthEmu - cx) / 2;
			}
		}
	}
	else
	{
		if (cx < widthEmu)
			offsetcx = (widthEmu - cx) / 2;
	}

	pictInsProperties.bFloating = false;
	pictInsProperties.bInline = true;
	pictInsProperties.simplePosition.x = 0;	pictInsProperties.simplePosition.y = 0;
	pictInsProperties.relativeHPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.relativeVPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.extent.cx = cx; pictInsProperties.extent.cy = cy;
	pictInsProperties.effectiveExtent.top = 0L; pictInsProperties.effectiveExtent.left = 0L;
	pictInsProperties.effectiveExtent.right = 0; pictInsProperties.effectiveExtent.bottom = 0;
	pictInsProperties.wrapSquare = "";
	pictInsProperties.graphicFrameLockChangeAspect = "1";
	pictInsProperties.bFillRect = true;
	pictInsProperties.shapeTransform.left = offsetcx; pictInsProperties.shapeTransform.top = 91440L;
	pictInsProperties.shapeTransform.right = cx; pictInsProperties.shapeTransform.bottom = cy;
	pictInsProperties.presetGeometry = "rect";
	pictInsProperties.relativeHSize.lVal = 0; pictInsProperties.relativeHSize.strVal = "";
	pictInsProperties.relativeVSize.lVal = 0; pictInsProperties.relativeVSize.strVal = "";

	pictProperties.pictureName = "Sub Process Overview";
	pictProperties.pictureDesc = "High level overview of the Sub process";
	pictProperties.pictureTitle = "bothSides";
	pictProperties.pictureFileName = "process_overview.bmp";
	pictProperties.compressionState = "";
	pictProperties.uri = "{28A0092B-C50C-407E-A947-70E740481C1C}";
	pictProperties.rID = pDocument->WriteImageFile(buffer);
	//pictProperties.rID = pDocument->WriteImageFile("C:\\Recorder\\screenTest.bmp", "bitmap");
	pRun->InsertInlinePictureFromFile(pRun, &pictInsProperties, &pictProperties);

	para = pBody->AddParagraph();
	para->SetPageBreak();

	WriteProcessSteps(pSubProcess->GetSubProcessElements(),
		strLevel + str, levelDepth + 1, pDocument, pBody);

}

void SOPWordReport::WriteTaskSteps(BPMNElement *pElement, string strLevel, int subLevelNo, int levelDepth, WordDocument *pDocument, DocumentBody *pBody)
{
	if (pElement == NULL)
		return;
	Paragraph *para = NULL;
	para = pBody->AddParagraph();
	para->SetStyle("Heading2");
	para->SetNumberProperties(3, levelDepth);
	para->SetLeftIndent(540, 540);
	para->SetFont("Arial");

	string strAnnotation = pElement->GetKeyValueAsString("Annotation");
	string strNotes = pElement->GetKeyValueAsString("Notes");

	string bookMarkId = GetNextBookmarkID();
	string tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId, tocID);
	char str[100];
	string tocStr;

	sprintf_s(str, "TOC%d", levelDepth + 1);
	tocStr = str;

	sprintf_s(str, ".%d",subLevelNo);
	
	int tabIndent = 440 + (220 * levelDepth);
	AddTOC(tocStr, tabIndent, 0, strAnnotation, tocID, strLevel + str, DocXDocPart::pageNumber);
	string bookMarkId2 = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId2, tocID);

	ParagraphRun *pRun = NULL;
	pRun = para->AddRun();
	//pRun->SetFont("Arial");
	pRun->SetText(strAnnotation, false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);


	para = pBody->AddParagraph();
	para->SetFont("Arial");

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText(strNotes, false);

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText("Screen:", false);

	vector <unsigned char> buffer;
	long bmpWidth = 0L, bmpHeight = 0L;
	//pDiagramMap->GetEventBitmap(buffer, bmpWidth, bmpHeight, pElement->GetEventIndex());

	double aspectRatio = (double)bmpHeight / (double)bmpWidth;

	double heightEmu = 4.5f * 914400.0f;
	double widthEmu = 5.9f * 914400.0f;
	long cx = bmpWidth, cy = bmpHeight;
	long offsetcx = 0L;
	if (bmpWidth > widthEmu || bmpHeight > heightEmu)
	{
		if (bmpHeight > bmpWidth)
		{
			// portrait image
			cy = (LONG)heightEmu;
			// scale the width accordingly
			cx = (LONG)((double)cy / aspectRatio);
			offsetcx = (widthEmu - cx) / 2;
			if (cx > widthEmu)
			{
				cx = (LONG)widthEmu;
				cy = (LONG)((double)cx * aspectRatio);
				offsetcx = 0;
			}
		}
		else
		{
			// landscape image
			cx = (LONG)widthEmu;
			// scale the width accordingly
			cy = (LONG)((double)cx * aspectRatio);
			if (cy > heightEmu)
			{
				cy = (LONG)heightEmu;
				cx = (LONG)((double)cy / aspectRatio);
				offsetcx = (widthEmu - cx) / 2;
			}
		}
	}
	else
	{
		if (cx < widthEmu)
			offsetcx = (widthEmu - cx) / 2;
	}

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	pRun = para->AddRun();
	pRun->SetNoProof();
	struct PictureInsertionProperties pictInsProperties;
	struct PictureProperties pictProperties;

	pictInsProperties.bFloating = false;
	pictInsProperties.bInline = true;
	pictInsProperties.simplePosition.x = 0;	pictInsProperties.simplePosition.y = 0;
	pictInsProperties.relativeHPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.relativeVPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.extent.cx = cx; pictInsProperties.extent.cy = cy;
	pictInsProperties.effectiveExtent.top = 0; pictInsProperties.effectiveExtent.left = 0;
	pictInsProperties.effectiveExtent.right = 0; pictInsProperties.effectiveExtent.bottom = 0;
	pictInsProperties.wrapSquare = "";

	pictInsProperties.graphicFrameLockChangeAspect = "1";
	pictInsProperties.bFillRect = true;
	pictInsProperties.shapeTransform.left = offsetcx; pictInsProperties.shapeTransform.top = 91440L;
	pictInsProperties.shapeTransform.right = cx; pictInsProperties.shapeTransform.bottom = cy;
	pictInsProperties.presetGeometry = "rect";
	pictInsProperties.relativeHSize.lVal = 0; pictInsProperties.relativeHSize.strVal = "";
	pictInsProperties.relativeVSize.lVal = 0; pictInsProperties.relativeVSize.strVal = "";

	string picName = "Picture " + strLevel + str;
	pictProperties.pictureName = picName;
	pictProperties.pictureDesc = "Activity Step " + strLevel + str;
	pictProperties.pictureTitle = "bothSides";
	
	sprintf_s(str, "%03d", subLevelNo);
	pictProperties.pictureFileName = "Picture" + strLevel + str + ".bmp";
	pictProperties.compressionState = "";
	pictProperties.uri = "";
//	pictProperties.rID = pDocument->WriteImageFile("C:\\Recorder\\TestImage.bmp", "bitmap");
	pictProperties.rID = pDocument->WriteImageFile(buffer, false);
	pRun->InsertInlinePictureFromFile(pRun, &pictInsProperties, &pictProperties);

	para = pBody->AddParagraph();
	para->SetPageBreak();
}

string SOPWordReport::GetNextBookmarkID()
{
	string strID;

	char str[100];
	sprintf_s(str, "%d", DocXDocPart::bookmarkNumber);
	DocXDocPart::bookmarkNumber++;
	strID = str;
	return strID;
}

string SOPWordReport::GetNextTOCID()
{
	string strID;

	char str[100];
	sprintf_s(str, "_Toc%d", DocXDocPart::tocNumber);
	DocXDocPart::tocNumber++;
	strID = str;
	return strID;
}


void SOPWordReport::CreateProcessSummaryPage(WordDocument *pDocument, DocumentBody *pBody)
{
	//TOCsdt *toc = DocXDocPart::TOC;
	DocXDocPart::tocNumber = 18403400L;
	DocXDocPart::bookmarkNumber = 0;
	DocXDocPart::pageNumber++;

	Paragraph *para = pBody->AddParagraph();
	para->SetStyle("TOCHeading");
	para->SetNumberProperties(0, 0);
	para->SetLeftIndent(432);

	para = pBody->AddParagraph();
	para->SetTabs("left", 3045);
	para->SetFontSize(40);

	para = pBody->AddParagraph();
	para->SetStyle("Heading1");
	para->SetNumberProperties(3, 0);
	para->SetPageBreak();

	string bookMarkId = GetNextBookmarkID();
	string bookMarkId2;
	string tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId, tocID);

	bookMarkId2 = GetNextBookmarkID();
	tocID = GetNextTOCID();
	AddTOC("TOC1", 440, 9350, "Summary", tocID, "1.", DocXDocPart::pageNumber, true);
	para->SetBookmarkStart(bookMarkId, tocID);

	ParagraphRun *pRun = para->AddRun();
	pRun->SetText("Summary", false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);
	/*------------------------------ Adding a Header -----------------------------------------------------------*/
	para = pBody->AddParagraph();
	para->SetStyle("Heading2");
	para->SetNumberProperties(3, 1);
	para->SetLeftIndent(540, 540);

	bookMarkId = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId, tocID);
	AddTOC("TOC2", 660, 0, "Process Description", tocID, "1.1", DocXDocPart::pageNumber);
	bookMarkId2 = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId2, tocID);

	pRun = para->AddRun();
	pRun->SetText("Process Description", false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);
	/*----------------------------------End of Header ------------------------------------------------------------*/
	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(24);

	string strVal = "";
	map<string, Json>::iterator itProcess = processInfo.find("Description");
	if (itProcess != processInfo.end())
	{
		strVal = (itProcess->second).string_value();
	}

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(22);
	
	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(22);
	pRun->SetText(strVal, false);

	itProcess = processInfo.find("SPDescription");
	if (itProcess != processInfo.end())
	{
		strVal = (itProcess->second).string_value();
	}
	para = pBody->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(22);

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(22);
	pRun->SetText(strVal, false);

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(40);

	//para = pBody->AddParagraph();
	//para->SetStyle("Heading2");
	//para->SetNumberProperties(3, 1);
	//para->SetLeftIndent(540, 540);
	//para->SetBookmarkStart("4", "_Toc18403415");
	//para->SetBookmarkStart("5", "_Toc18486357");

	//pRun = para->AddRun();
	//pRun->SetText("Frequency of Use", false);
	//para->SetBookmarkEnd("4");
	//para->SetBookmarkEnd("5");

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(24);

	//pRun = para->AddRun();
	//pRun->SetFont("Arial");
	//pRun->SetFontSize(24);
	//pRun->SetText("how frequent is the process executed", false);

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(40);

	///*------------------------------ Adding a Header -----------------------------------------------------------*/
	//para = pBody->AddParagraph();
	//para->SetStyle("Heading2");
	//para->SetNumberProperties(3, 1);
	//para->SetLeftIndent(540, 540);
	//para->SetBookmarkStart("6", "_Toc18403416");
	//para->SetBookmarkStart("7", "_Toc18486358");

	//pRun = para->AddRun();
	//pRun->SetText("Executing roles", false);
	//para->SetBookmarkEnd("6");
	//para->SetBookmarkEnd("7");
	///*----------------------------------End of Header ------------------------------------------------------------*/

	///*----------------------------------State of Table -----------------------------------------------------------*/
	//Table *pTable = pBody->AddTable();
	//pTable->SetTableStyle("TableGrid");
	//pTable->SetTableWidthAuto();
	//pTable->SetTableLookNormal();
	//pTable->SetColumnWidth(3116);
	//pTable->SetColumnWidth(3116);

	//TableRow *pRow = pTable->AddTableRow();
	//AddFirstRowTableCell(pRow, 3116, "Role");
	//AddFirstRowTableCell(pRow, 3116, "Name");

	//pRow = pTable->AddTableRow();
	//AddRowTableCell(pRow, 3116, "Approver");
	//AddRowTableCell(pRow, 3116, "Approver Name");

	//pRow = pTable->AddTableRow();
	//AddRowTableCell(pRow, 3116, "Executive");
	//AddRowTableCell(pRow, 3116, "Executive Name");
	///*----------------------------------End of Table -----------------------------------------------------------*/

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(40);

	///*------------------------------ Adding a Header -----------------------------------------------------------*/
	//para = pBody->AddParagraph();
	//para->SetStyle("Heading2");
	//para->SetNumberProperties(3, 1);
	//para->SetLeftIndent(540, 540);
	//para->SetFont("Arial");
	//para->SetBookmarkStart("8", "_Toc18403417");
	//para->SetBookmarkStart("9", "_Toc18486359");

	//pRun = para->AddRun();
	//pRun->SetText("Applications Used", false);
	//para->SetBookmarkEnd("8");
	//para->SetBookmarkEnd("9");
	///*----------------------------------End of Header ------------------------------------------------------------*/

	//pTable = pBody->AddTable();
	//pTable->SetTableStyle("TableGrid");
	//pTable->SetTableWidthAuto();;
	//pTable->SetTableLookNormal();
	//pTable->SetColumnWidth(3116);
	//pTable->SetColumnWidth(3116);
	//pTable->SetColumnWidth(3116);
	//
	//pRow = pTable->AddTableRow();
	//AddFirstRowTableCell(pRow, 3116, "Application Name");
	//AddFirstRowTableCell(pRow, 3116, "Type");
	//AddFirstRowTableCell(pRow, 3116, "Internal // External");

	//pRow = pTable->AddTableRow();
	//AddRowTableCell(pRow, 3116, "App Name 1");
	//AddRowTableCell(pRow, 3116, "Desktop");
	//AddRowTableCell(pRow, 3116, "External");

	//pRow = pTable->AddTableRow();
	//AddRowTableCell(pRow, 3116, "App Name 2");
	//AddRowTableCell(pRow, 3116, "Web");
	//AddRowTableCell(pRow, 3116, "Internal");

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(40);

	///*------------------------------ Adding a Header -----------------------------------------------------------*/
	//para = pBody->AddParagraph();
	//para->SetStyle("Heading2");
	//para->SetNumberProperties(3, 1);
	//para->SetLeftIndent(540, 540);
	//para->SetFont("Arial");
	//para->SetBookmarkStart("10", "_Toc18403418");
	//para->SetBookmarkStart("11", "_Toc18486360");

	//pRun = para->AddRun();
	//pRun->SetText("Inputs Used", false);
	//para->SetBookmarkEnd("10");
	//para->SetBookmarkEnd("11");
	///*----------------------------------End of Header ------------------------------------------------------------*/

	//pTable = pBody->AddTable();
	//pTable->SetTableStyle("TableGrid");
	//pTable->SetTableWidthAuto();
	//pTable->SetTableLookNormal();
	//pTable->SetColumnWidth(3116);
	//pTable->SetColumnWidth(3116);
	//pTable->SetColumnWidth(3116);
	//
	//pRow = pTable->AddTableRow();
	//AddFirstRowTableCell(pRow, 3116, "Name");
	//AddFirstRowTableCell(pRow, 3116, "Type");
	//AddFirstRowTableCell(pRow, 3116, "Location");

	//pRow = pTable->AddTableRow();
	//AddRowTableCell(pRow, 3116, "Input Name 1");
	//AddRowTableCell(pRow, 3116, "Excel");
	//AddRowTableCell(pRow, 3116, "Team Folder");

	//pRow = pTable->AddTableRow();
	//AddRowTableCell(pRow, 3116, "Input Name 2");
	//AddRowTableCell(pRow, 3116, "Web");
	//AddRowTableCell(pRow, 3116, "www.google.com");

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(40);

	///*------------------------------ Adding a Header -----------------------------------------------------------*/
	//para = pBody->AddParagraph();
	//para->SetStyle("Heading2");
	//para->SetNumberProperties(3, 1);
	//para->SetLeftIndent(540, 540);
	//para->SetFont("Arial");

	//para->SetBookmarkStart("12", "_Toc18403419");
	//para->SetBookmarkStart("13", "_Toc18486361");

	//pRun = para->AddRun();
	//pRun->SetText("Risks", false);
	//para->SetBookmarkEnd("12");
	//para->SetBookmarkEnd("13");
	///*----------------------------------End of Header ------------------------------------------------------------*/

	//AddListItemParagraph(pBody, "ListParagraph", 0, 1, "Risk 1");
	//AddListItemParagraph(pBody, "ListParagraph", 0, 1, "Risk 2");
	//AddListItemParagraph(pBody, "ListParagraph", 0, 1, "Risk 3");
	//AddListItemParagraph(pBody, "ListParagraph", 0, 1, "Risk 4");

	//para = pBody->AddParagraph();
	//para->SetFont("Arial");
	//para->SetFontSize(24);

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(24);

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(24);
	para->SetPageBreak();

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(24);

	/*------------------------------ Adding a Header -----------------------------------------------------------*/
	para = pBody->AddParagraph();
	para->SetStyle("Heading2");
	para->SetNumberProperties(3, 1);
	para->SetLeftIndent(540, 540);
	//para->SetFont("Arial");
	bookMarkId = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId, tocID);
	AddTOC("TOC2", 660, 0, "Process Flow", tocID, "1.2", DocXDocPart::pageNumber);
	bookMarkId2 = GetNextBookmarkID();
	tocID = GetNextTOCID();
	para->SetBookmarkStart(bookMarkId2, tocID);

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText("Process Flow", false);
	para->SetBookmarkEnd(bookMarkId);
	para->SetBookmarkEnd(bookMarkId2);

	/*----------------------------------End of Header ------------------------------------------------------------*/

	para = pBody->AddParagraph();
	para->SetFont("Arial");

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetText("Below is the Process Flow", false);


	para = pBody->AddParagraph();
	para->SetFont("Arial");
	pRun = para->AddRun();
	pRun->SetNoProof();
	struct PictureInsertionProperties pictInsProperties;
	struct PictureProperties pictProperties;
	vector <unsigned char> buffer;
	SIZE szBmp;
	pDiagramMap->CreateLevelDiagram(pElementList);
	pDiagramMap->CreateBPMNBitmap(buffer, &szBmp);
	double aspectRatio = (double)szBmp.cy / (double)szBmp.cx;

	double heightEmu = 7.5f * 914400.0f;
	double widthEmu = 5.9f * 914400.0f;
	long cx = szBmp.cx, cy = szBmp.cy;
	long offsetcx = 0L;
	if (szBmp.cx > widthEmu || szBmp.cy > heightEmu)
	{
		if (szBmp.cy > szBmp.cx)
		{
			// portrait image
			cy = (LONG)heightEmu;
			// scale the width accordingly
			cx = (LONG)((double)cy / aspectRatio);
			offsetcx = (widthEmu - cx) / 2;
			if (cx > widthEmu)
			{
				cx = (LONG)widthEmu;
				cy = (LONG)((double)cx * aspectRatio);
				offsetcx = 0;
			}
		}
		else
		{
			// landscape image
			cx = (LONG)widthEmu;
			// scale the width accordingly
			cy = (LONG)((double)cx * aspectRatio);
			if (cy > heightEmu)
			{
				cy = (LONG)heightEmu;
				cx = (LONG)((double)cy /  aspectRatio);
				offsetcx = (widthEmu - cx) / 2;
			}
		}
	}
	else
	{
		if (cx < widthEmu)
			offsetcx = (widthEmu - cx) / 2;
	}

	pictInsProperties.bFloating = false;
	pictInsProperties.bInline = true;
	pictInsProperties.simplePosition.x = 0;	pictInsProperties.simplePosition.y = 0;
	pictInsProperties.relativeHPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.relativeVPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.extent.cx = cx; pictInsProperties.extent.cy = cy;
	pictInsProperties.effectiveExtent.top = 0L; pictInsProperties.effectiveExtent.left = 0L;
	pictInsProperties.effectiveExtent.right = 0; pictInsProperties.effectiveExtent.bottom = 0;
	pictInsProperties.wrapSquare = "";
	pictInsProperties.graphicFrameLockChangeAspect = "1";
	pictInsProperties.bFillRect = true;
	pictInsProperties.shapeTransform.left = offsetcx; pictInsProperties.shapeTransform.top = 91440L;
	pictInsProperties.shapeTransform.right = cx; pictInsProperties.shapeTransform.bottom = cy;
	pictInsProperties.presetGeometry = "rect";
	pictInsProperties.relativeHSize.lVal = 0; pictInsProperties.relativeHSize.strVal = "";
	pictInsProperties.relativeVSize.lVal = 0; pictInsProperties.relativeVSize.strVal = "";

	pictProperties.pictureName = "Process Overview";
	pictProperties.pictureDesc = "High level overview of the process";
	pictProperties.pictureTitle = "bothSides";
	pictProperties.pictureFileName = "process_overview.bmp";
	pictProperties.compressionState = "";
	pictProperties.uri = "{28A0092B-C50C-407E-A947-70E740481C1C}";
	pictProperties.rID = pDocument->WriteImageFile(buffer); 			
	//pictProperties.rID = pDocument->WriteImageFile("C:\\Recorder\\screenTest.bmp", "bitmap");
	pRun->InsertInlinePictureFromFile(pRun, &pictInsProperties, &pictProperties);

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(40);

	para = pBody->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(40);

	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(40);

	para = pBody->AddParagraph();
	para->SetPageBreak();
}

void SOPWordReport::AddListItemParagraph(DocXTag *pInsTag, string listStyle, int level, int numID, string text)
{
	Paragraph *para = pInsTag->AddParagraph();
	para->SetStyle(listStyle);
	para->SetNumberProperties(numID, level);
	para->SetSpacing("auto", 256);

	para->SetFont("Arial");
	para->SetFontSize(24);

	ParagraphRun *pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(24);
	pRun->SetText(text, false);
}

void SOPWordReport::AddFirstRowTableCell(TableRow *pRow, int cellWidth, string text)
{
	Paragraph *para = NULL;
	ParagraphRun *pRun = NULL;

	TableCell *pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(cellWidth, "dxa");
	pCell->SetTableCellBorders("auto", 0, 4, "single");
	pCell->SetCellShade("clear", "E6", "background2", "D0CECE", "auto");
	pCell->SetHideMark();
	para = pCell->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(24);
	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(24);
	pRun->SetText(text, false);
}

void SOPWordReport::AddRowTableCell(TableRow *pRow, int cellWidth, string text)
{
	Paragraph *para = NULL;
	ParagraphRun *pRun = NULL;

	TableCell *pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(cellWidth, "dxa");
	pCell->SetTableCellBorders("auto", 0, 4, "single");
	pCell->SetHideMark();
	para = pCell->AddParagraph();
	para->SetFont("Arial");
	para->SetFontSize(24);
	pRun = para->AddRun();
	pRun->SetFont("Arial");
	pRun->SetFontSize(24);
	pRun->SetText(text, false);
}

void SOPWordReport::CreateTOC(DocumentBody *pBody)
{
	TOCsdt *toc = DBG_NEW TOCsdt();

	DocXDocPart::TOC = toc;
	Paragraph *para = pBody->AddParagraph();
	para->SetStyle("TOCHeading");
	para->SetNumberProperties(0, 0);
	para->SetLeftIndent(432);

	para = pBody->AddParagraph();
	para->SetFontTheme("majorHAnsi", "majorBidi", "majorEastAsia", "majorHAnsi");
	para->SetFontThemeColor("accent1", "BF", "2F5496");
	para->SetFontSize(32);

	//para = pBody->AddParagraph();
	//para->SetPageBreak();

	pBody->AddTag(toc);
	toc->SetContentStyle("TOCHeading");
	toc->SetStandardProperties();

	toc->SetStandardTocContentStart();
	pDiagramMap->GetNoOfItems();
}

void SOPWordReport::EndTOC()
{
	TOCsdt *toc = DocXDocPart::TOC;
	toc->SetContentEnd();
}

void SOPWordReport::AddTOC(string tocStyle, int leftTab, int rightTab, string tocVal, string bookmark, string numString, int pageNum, bool bFirst )
{
	TOCsdt *toc = DocXDocPart::TOC;

	if (tocStyle != "")
		toc->SetTOCStyle("TOC1");
	if (leftTab != 0)
		toc->SetLeftTabProperties(leftTab);
	if (rightTab != 0)	
		toc->SetRightTabProperties(rightTab, "dot");
	toc->SetFontTheme("minorEastAsia");
	toc->SetHyperlinkFont("Arial");
	toc->SetHyperlink(tocVal, bookmark, numString, pageNum, bFirst);

}

void SOPWordReport::AddEmptyParagraph(DocumentBody *pBody, string tabSide, int tabPos, string justification, string runFont, int runFontSize, int runCsSize)
{
	Paragraph *para = pBody->AddParagraph();
	para->SetTabs(tabSide, tabPos);
	para->SetJustification(justification);
	para->SetFont(runFont, runFontSize, runCsSize);
}

void SOPWordReport::CreateControlPage(DocumentBody *pBody)
{
	Table *pTable = pBody->AddTable();
	pTable->SetTableStyle("TableGrid");
	pTable->SetTableWidthAuto();
	pTable->SetTableLookNormal();
	pTable->SetColumnWidth(3325);
	pTable->SetColumnWidth(6025);

	string strVal = "";
	map<string, Json>::iterator itProcess = processInfo.find("Name");
	if (itProcess != processInfo.end())
	{
		strVal = (itProcess->second).string_value();
	}

	AddControlTableRow(pTable, "Process Name", strVal);
	itProcess = processInfo.find("SPName");
	if (itProcess != processInfo.end())
	{
		strVal = (itProcess->second).string_value();
	}
	AddControlTableRow(pTable, "Sub-Process Name", strVal);
	itProcess = processInfo.find("CreatedBy");
	if (itProcess != processInfo.end())
	{
		strVal = (itProcess->second).string_value();
	}
	AddControlTableRow(pTable, "Created By", strVal);
	
	AddControlTableRow(pTable, "Date", "1 September 2019");

	Paragraph *pPara = pBody->AddParagraph();
	pPara->SetPageBreak();
}

void SOPWordReport::AddControlTableRow(Table *pTable, string column1Text, string column2Text)
{
	TableRow *pRow = pTable->AddTableRow();

	TableCell *pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(3325, "dxa");
	Paragraph *pPara = pCell->AddParagraph();
	pPara->SetTabs("left", 3045);
	pPara->SetFontSize(40);

	ParagraphRun *pParaRun = pPara->AddRun();
	pParaRun->SetFontSize(40);
	pParaRun->SetText(column1Text, false);

	pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(6025, "dxa");
	pPara = pCell->AddParagraph();
	pPara->SetTabs("left", 3045);
	pPara->SetFontSize(40);
	pPara->StartProof();

	pParaRun = pPara->AddRun();
	pParaRun->SetFontSize(40);
	pParaRun->SetText(column2Text, false);
	pPara->EndProof();

	

}

void SOPWordReport::CreateCoverPage(WordDocument *pDocument, DocumentBody *pBody)
{
	Table *pTable = pBody->AddTable();
	pTable->SetTableStyle("TableGrid");
	pTable->SetTableWidthAuto();
	pTable->SetTableBordersNone();
	pTable->SetTableLookNormal();
	pTable->SetColumnWidth(5046);
	pTable->SetColumnWidth(4314);

	TableRow *pRow = pTable->AddTableRow();
	pRow->SetRowHeight(5480);
	TableCell *pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(5046, "dxa");

	Paragraph *paragraph = pCell->AddParagraph();
	ParagraphRun *pRun = paragraph->AddRun();
	pRun->SetNoProof();

	struct PictureInsertionProperties pictInsProperties;
	struct PictureProperties pictProperties;

	pictInsProperties.bFloating = false;
	pictInsProperties.bInline = true;
	pictInsProperties.simplePosition.x = 0;	pictInsProperties.simplePosition.y = 0;
	pictInsProperties.relativeHPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.relativeVPos.lVal = 0; pictInsProperties.relativeHPos.strVal = "";
	pictInsProperties.extent.cx = 3063388; pictInsProperties.extent.cy = 3229650;
	pictInsProperties.effectiveExtent.top = 0L; pictInsProperties.effectiveExtent.left = 0L;
	pictInsProperties.effectiveExtent.right = 3810; pictInsProperties.effectiveExtent.bottom = 8890;
	pictInsProperties.wrapSquare = "";
	pictInsProperties.graphicFrameLockChangeAspect = "1";
	pictInsProperties.bFillRect = true;
	pictInsProperties.shapeTransform.left = 0; pictInsProperties.shapeTransform.top = 0;
	pictInsProperties.shapeTransform.right = 3063388; pictInsProperties.shapeTransform.bottom = 3229650;
	pictInsProperties.presetGeometry = "rect";
	pictInsProperties.relativeHSize.lVal = 0; pictInsProperties.relativeHSize.strVal = "";
	pictInsProperties.relativeVSize.lVal = 0; pictInsProperties.relativeVSize.strVal = "";

	pictProperties.pictureName = "Picture 1";
	pictProperties.pictureDesc = "A path with trees on the side of a road Description automatically generated";
	pictProperties.pictureTitle = "bothSides";
	pictProperties.pictureFileName = "coverimage.bmp";
	pictProperties.compressionState = "";
	pictProperties.uri = "{28A0092B-C50C-407E-A947-70E740481C1C}";

	string strPath = getcurrentpath() + "resources\\coverimage.bmp";

	pictProperties.rID = pDocument->WriteImageFile((char *)strPath.c_str(), "bitmap");

	// Write the bmp file in the zip and update referenceID in properties
	if (pictInsProperties.bFloating)
		pCell->InsertFloatingPictureFromFile(pRun, &pictInsProperties, &pictProperties);
	else if (pictInsProperties.bInline)
		pCell->InsertInlinePictureFromFile(pRun, &pictInsProperties, &pictProperties);


	pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(4314, "dxa");
	pCell->SetVerticalAlign("center");
	paragraph = pCell->AddParagraph();
	paragraph->SetFont("Arial", "52", true);
	pRun = paragraph->AddRun();
	pRun->SetFont("Arial", "52", true);
	pRun->SetText("Client Name", false);

	paragraph = pBody->AddParagraph();
	paragraph->SetTabs("left", 2400);
	paragraph->SetJustification("center");
	paragraph->SetFont("Georgia", "52", "1F3864", "accent1", "80");
	//paragraph->SetFont("Georgia", "52", false);
	//paragraph->SetFontColor("1F3864", "accent1", "80");

	paragraph = pBody->AddParagraph();
	paragraph->SetTabs("left", 2400);
	paragraph->SetJustification("center");
	paragraph->SetFont("Georgia", "52", "1F3864", "accent1", "80");
	//paragraph->SetFont("Georgia", "52", false);
	//paragraph->SetFontColor("1F3864", "accent1", "80");
	pRun = paragraph->AddRun();
	//pRun->SetFont("Georgia", "52", false);
	//pRun->SetFontColor("1F3864", "accent1", "80");
	pRun->SetFont("Georgia", "52", "1F3864", "accent1", "80");
	pRun->SetText("Standard Operating Procedure", false);


	paragraph = pBody->AddParagraph();
	paragraph->SetTabs("left", 2400);
	paragraph->SetJustification("center");
	paragraph->SetFont("Georgia", "52", "1F3864", "accent1", "80");

	//paragraph->SetFont("Georgia", "52", false);
	//paragraph->SetFontColor("1F3864", "accent1", "80");
	pRun = paragraph->AddRun();
	//pRun->SetFont("Georgia", "52", false);
	//pRun->SetFontColor("1F3864", "accent1", "80");
	pRun->SetFont("Georgia", "52", "1F3864", "accent1", "80");
	pRun->SetText("For", false);

	paragraph = pBody->AddParagraph();
	paragraph->SetTabs("left", 2400);
	paragraph->SetJustification("center");
	//paragraph->SetFont("Georgia", "52", false);
	//paragraph->SetFontColor("1F3864", "accent1", "80");
	paragraph->SetFont("Georgia", "52", "1F3864", "accent1", "80");
	pRun = paragraph->AddRun();
	//pRun->SetFont("Georgia", "52", false);
	//pRun->SetFontColor("1F3864", "accent1", "80");
	pRun->SetFont("Georgia", "52", "1F3864", "accent1", "80");
	string strVal = "";
	map<string, Json>::iterator itProcess = processInfo.find("Name");
	if (itProcess != processInfo.end())
	{
		strVal = (itProcess->second).string_value();
	}
	pRun->SetText(strVal, false);

	paragraph = pBody->AddParagraph();
	//paragraph->SetBookmarkStart("0", "_GoBack");
	//paragraph->SetBookmarkEnd("0");
	paragraph = pBody->AddParagraph();
	paragraph->AddPageBreak();

}

void SOPWordReport::CreateNumberingFile(WordDocument *pDoc)
{
	Numbering *pNumbering = pDoc->AddNumbering();

	pNumbering->AddAbstractNumbering("090F45BA", "multilevel", "0409001F");
	pNumbering->AddLevelNumbering("", 0, 1, "decimal", "", "%1.", "left", false);
	pNumbering->SetLevelIndent(360, 360);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 1, 1, "decimal", "","%1.%2.", "left", false);
	pNumbering->SetLevelIndent(432, 792);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 2, 1, "decimal", "", "%1.%2.%3.", "left", false);
	pNumbering->SetLevelIndent(504, 1224);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 3, 1, "decimal", "", "%1.%2.%3.%4.", "left", false);
	pNumbering->SetLevelIndent(648, 1728);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 4, 1, "decimal", "", "%1.%2.%3.%4.%5.", "left", false);
	pNumbering->SetLevelIndent(792, 2232);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 5, 1, "decimal", "", "%1.%2.%3.%4.%5.%6.", "left", false);
	pNumbering->SetLevelIndent(936, 2736);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 6, 1, "decimal", "", "%1.%2.%3.%4.%5.%6.%7.", "left", false);
	pNumbering->SetLevelIndent(1080, 3240);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 7, 1, "decimal", "", "%1.%2.%3.%4.%5.%6.%7.%8.", "left", false);
	pNumbering->SetLevelIndent(1224, 3744);
	//pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 8, 1, "decimal", "", "%1.%2.%3.%4.%5.%6.%7.%8.%9.", "left", false);
	pNumbering->SetLevelIndent(1440, 4320);
	//pNumbering->SetFontHint("default", "", "");

	pNumbering->AddAbstractNumbering("6A8A6B40", "hybridMultilevel", "6D2E1C80");
	pNumbering->AddLevelNumbering("04090001", 0, 1, "bullet", "", "ï‚·", "left", false);
	pNumbering->SetLevelIndent(360, 720);
	pNumbering->SetFontHint("default", "Symbol", "Symbol");
	pNumbering->AddLevelNumbering("04090003", 1, 1, "bullet", "", "o", "left", true);
	pNumbering->SetLevelIndent(360, 1440);
	pNumbering->SetFontHint("default", "Courier New", "Courier New", "Courier New");
	pNumbering->AddLevelNumbering("04090005", 2, 1, "bullet", "", "ï‚§", "left", false);
	pNumbering->SetLevelIndent(360, 2160);
	pNumbering->SetFontHint("default", "Wingdings", "Wingdings");
	pNumbering->AddLevelNumbering("04090001", 3, 1, "bullet", "", "ï‚·", "left", false);
	pNumbering->SetLevelIndent(360, 2880);
	pNumbering->SetFontHint("default", "Symbol", "Symbol");
	pNumbering->AddLevelNumbering("04090003", 4, 1, "bullet", "", "o", "left", true);
	pNumbering->SetLevelIndent(360, 3600);
	pNumbering->SetFontHint("default", "Courier New", "Courier New", "Courier New");
	pNumbering->AddLevelNumbering("04090005", 5, 1, "bullet", "", "ï‚§", "left", true);
	pNumbering->SetLevelIndent(360, 4320);
	pNumbering->SetFontHint("default", "Wingdings", "Wingdings");
	pNumbering->AddLevelNumbering("04090001", 6, 1, "bullet", "", "ï‚·", "left", true);
	pNumbering->SetLevelIndent(360, 5040);
	pNumbering->SetFontHint("default", "Symbol", "Symbol");
	pNumbering->AddLevelNumbering("04090003", 7, 1, "bullet", "", "o", "left", true);
	pNumbering->SetLevelIndent(360, 5760);
	pNumbering->SetFontHint("default", "Courier New", "Courier New", "Courier New");
	pNumbering->AddLevelNumbering("04090005", 8, 1, "bullet", "", "ï‚§", "left", true);
	pNumbering->SetLevelIndent(360, 6480);
	pNumbering->SetFontHint("default", "Wingdings", "Wingdings");


	pNumbering->AddAbstractNumbering("71ED611A", "multilevel", "4D923C22");
	pNumbering->AddLevelNumbering("", 0, 2, "decimal", "Heading1", "%1", "left", false);
	pNumbering->SetLevelIndent(432, 432);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 1, 1, "decimal", "Heading2", "%1.%2", "left", false);
	pNumbering->SetLevelIndent(576, 576);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 2, 1, "decimal", "Heading3", "%1.%2.%3", "left", false);
	pNumbering->SetLevelIndent(720, 720);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 3, 1, "decimal", "Heading4", "%1.%2.%3.%4", "left", false);
	pNumbering->SetLevelIndent(864, 864);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 4, 1, "decimal", "Heading5", "%1.%2.%3.%4.%5", "left", false);
	pNumbering->SetLevelIndent(1008, 1008);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 5, 1, "decimal", "Heading6", "%1.%2.%3.%4.%5.%6", "left", false);
	pNumbering->SetLevelIndent(1152, 1152);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 6, 1, "decimal", "Heading7", "%1.%2.%3.%4.%5.%6.%7", "left", false);
	pNumbering->SetLevelIndent(1296, 1296);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 7, 1, "decimal", "Heading8", "%1.%2.%3.%4.%5.%6.%7.%8", "left", false);
	pNumbering->SetLevelIndent(1440, 1440);
	pNumbering->SetFontHint("default", "", "");
	pNumbering->AddLevelNumbering("", 8, 1, "decimal", "Heading9", "%1.%2.%3.%4.%5.%6.%7.%8.%9", "left", false);
	pNumbering->SetLevelIndent(1584, 1584);
	pNumbering->SetFontHint("default", "", "");

	DocXTag *pTag = pNumbering->AddAbstractNumbering(1, 1);
	DocXTag *pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "0");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "1");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "2");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "3");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "4");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "5");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "6");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "7");
	pTag1 = pTag->AddTag("w:lvlOverride");
	pTag1->AddAttribute("w:ilvl", "8");

	pNumbering->AddAbstractNumbering(2, 2);
	pNumbering->AddAbstractNumbering(3, 0);

}

void SOPWordReport::CreateApplicationPropertiesFile(WordDocument *pDoc)
{
	pDoc->BuildAppProps();
}

void SOPWordReport::CreateCorePropertiesFile(WordDocument *pDoc)
{
	pDoc->BuildCoreProps();
}

void SOPWordReport::CreateHeaderFile(WordDocument *pDoc)
{
	Header *pHeader = pDoc->AddHeader("header1");
	Paragraph *pPara = pHeader->AddParagraph();
	pPara->SetStyle("Header");
	pPara->SetJustification("center");

	ParagraphRun *pParaRun = pPara->AddRun();
	pParaRun->SetText("Client Name", false);
}


void SOPWordReport::CreateFooterFile(WordDocument *pDoc)
{
	Footer *pFooter = pDoc->AddFooter("footer2");
	Paragraph *pPara = pFooter->AddParagraph();
	pPara->SetStyle("Footer");
	pPara->SetJustification("center");
	pPara->SetBookmarkStart("1", "_Hlk16936514");

	ParagraphRun *pParaRun = pPara->AddRun();
	pParaRun->SetText("Copyright Notice: This document is protected by U.S. and International copyright laws. Reproduction and distribution of this document without written permission of the sponsor is prohibited", false);

	pPara = pFooter->AddParagraph();
	pPara->SetStyle("Footer");
	pPara->SetJustification("center");

	pParaRun = pPara->AddRun();
	pParaRun->SetText("Copyright ", true);

	pParaRun = pPara->AddRun();
	pParaRun->SetCSTheme("minorHAnsi");
	pParaRun->SetText("(C)", false);

	pParaRun = pPara->AddRun();
	pParaRun->SetText("2019 Procoan All Rights Reserved", true);
	pPara->SetBookmarkEnd("1");

	pPara = DBG_NEW Paragraph();
	pPara->SetStyle("Footer");
}

void SOPWordReport::CreateMainFooterFile(WordDocument *pDoc)
{
	Footer *pFooter = pDoc->AddFooter("footer1");
	Table *pTable = pFooter->AddTable();
	pTable->SetTableStyle("TableGrid");
	pTable->SetTableWidthDxa(10350);
	pTable->SetTableInd(-635, "dxa");
	pTable->SetTableBordersNone();
	pTable->SetTableLookNormal();
	pTable->SetColumnWidth(5310);
	pTable->SetColumnWidth(5040);

	TableRow *pRow = pTable->AddTableRow();
	TableCell *pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(5310, "dxa");

	Paragraph *pPara = pCell->AddParagraph();
	pPara->SetStyle("Footer");

	ParagraphRun *pRun = pPara->AddRun();
	pRun->SetText("Standard Operating Procedure for [Process Name]", true);

	pCell = pRow->AddTableCell();
	pCell->SetCellWidthAndType(5310, "dxa");

	pPara = pCell->AddParagraph();
	pPara->SetStyle("Footer");
	pPara->SetJustification("right");

	pRun = pPara->AddRun();
	pRun->SetText("Page ", true);

	pPara->AddFldChar("PAGE  \\* Arabic  \\* MERGEFORMAT", true);
	pPara->AddFldValue(2);
	pPara->EndFldChar();

	pRun = pPara->AddRun();
	pRun->SetText(" of ", true);

	pPara->AddFldChar("NUMPAGES   \\* MERGEFORMAT", true);
	pPara->AddFldValue(2);
	pPara->EndFldChar();

	pPara = pFooter->AddParagraph();
	pPara->SetStyle("Footer");

}

void SOPWordReport::CreateFontTableFile(WordDocument *pDoc)
{
	Font *pFont = NULL;

	pFont = pDoc->AddFontTable("Times New Roman");
	pFont->SetPanose1("02020603050405020304");
	pFont->SetCharset("00");
	pFont->SetFamily("roman");
	pFont->SetPitch("variable");
	pFont->SetSigusb("E0002AFF", "C0007843", "00000009", "00000000");
	pFont->SetSigcsb("000001FF", "00000000");

	pFont = pDoc->AddFontTable("Symbol");
	pFont->SetPanose1("05050102010706020507");
	pFont->SetCharset("00");
	pFont->SetFamily("roman");
	pFont->SetPitch("variable");
	pFont->SetSigusb("00000000", "10000000", "00000000", "00000000");
	pFont->SetSigcsb("80000000", "00000000");

	pFont = pDoc->AddFontTable("Courier New");
	pFont->SetPanose1("02070309020205020404");
	pFont->SetCharset("00");
	pFont->SetFamily("modern");
	pFont->SetPitch("fixed");
	pFont->SetSigusb("E0002EFF", "C0007843", "00000009", "00000000");
	pFont->SetSigcsb("000001FF", "00000000");

	pFont = pDoc->AddFontTable("Wingdings");
	pFont->SetPanose1("05000000000000000000");
	pFont->SetCharset("00");
	pFont->SetFamily("auto");
	pFont->SetPitch("variable");
	pFont->SetSigusb("00000000", "10000000", "00000000", "00000000");
	pFont->SetSigcsb("80000000", "00000000");

	pFont = pDoc->AddFontTable("Calibri");
	pFont->SetPanose1("020F0502020204030204");
	pFont->SetCharset("00");
	pFont->SetFamily("swiss");
	pFont->SetPitch("variable");
	pFont->SetSigusb("E0002AFF", "4000ACFF", "00000001", "00000000");
	pFont->SetSigcsb("000001FF", "00000000");

	pFont = pDoc->AddFontTable("Calibri Light");
	pFont->SetPanose1("020F0302020204030204");
	pFont->SetCharset("00");
	pFont->SetFamily("swiss");
	pFont->SetPitch("variable");
	pFont->SetSigusb("A0002AEF", "4000207B", "00000000", "00000000");
	pFont->SetSigcsb("000001FF", "00000000");


	pFont = pDoc->AddFontTable("Arial");
	pFont->SetPanose1("020B0604020202020204");
	pFont->SetCharset("00");
	pFont->SetFamily("swiss");
	pFont->SetPitch("variable");
	pFont->SetSigusb("E0002AFF", "C0007843", "00000009", "00000000");
	pFont->SetSigcsb("000001FF", "00000000");

	pFont = pDoc->AddFontTable("Georgia");
	pFont->SetPanose1("02040502050405020303");
	pFont->SetCharset("00");
	pFont->SetFamily("roman");
	pFont->SetPitch("variable");
	pFont->SetSigusb("00000287", "00000000", "00000000", "00000000");
	pFont->SetSigcsb("0000009F", "00000000");

}

void SOPWordReport::CreateTheme1File(WordDocument *pDoc)
{
	pDoc->BuildNormalTheme();
}


void SOPWordReport::CreateFootnotesFile(WordDocument *pDoc)
{
	pDoc->AddFootnoteSeperator();
	pDoc->AddFootnoteContinuousSeperator();
}

void SOPWordReport::CreateEndnotesFile(WordDocument *pDoc)
{
	pDoc->AddEndnoteSeperator();
	pDoc->AddEndnoteContinuousSeperator();
}

void SOPWordReport::CreateWebSettingsFile(WordDocument *pDoc)
{
	pDoc->SetOptimizeForBrowser();
	pDoc->SetAllowPNG();
}


void SOPWordReport::CreateSettingsFile(WordDocument *pDoc)
{
	pDoc->SetZoom(100);
	pDoc->SetProofState("clean", "clean");
	pDoc->SetDefaultTabStop(720);
	pDoc->SetCharacterSpacingControl("doNotCompress");
	//pDoc->SetFootnote(-1);
	//pDoc->SetFootnote(0);
	//pDoc->SetEndnote(-1);
	//pDoc->SetEndnote(0);
	pDoc->SetCompatabilitySetting("compatibilityMode", "http://schemas.microsoft.com/office/word", "15");
	pDoc->SetCompatabilitySetting("overrideTableStyleFontSizeAndJustification", "http://schemas.microsoft.com/office/word", "1");
	pDoc->SetCompatabilitySetting("enableOpenTypeFeatures", "http://schemas.microsoft.com/office/word", "1");
	pDoc->SetCompatabilitySetting("doNotFlipMirrorIndents", "http://schemas.microsoft.com/office/word", "1");
	pDoc->SetCompatabilitySetting("differentiateMultirowTableHeaders", "http://schemas.microsoft.com/office/word", "1");
	pDoc->SetCompatabilitySetting("useWord2013TrackBottomHyphenation", "http://schemas.microsoft.com/office/word", "0");

	DocXDocPart::str_rsidR = "00AB0EDA";
	DocXDocPart::str_rsidTr = "00AB0EDA";

	pDoc->SetRSID("rsidRoot", DocXDocPart::str_rsidR);
	pDoc->SetRSID("rsid", DocXDocPart::str_rsidR);

	pDoc->SetMathFont("Cambria Math");
	pDoc->SetBrkBin("before");
	pDoc->SetBrkBinSub("--");
	pDoc->SetSmallFrac("0");
	pDoc->SetDisplayDef();
	pDoc->SetMathLeftMargin(0);
	pDoc->SetMathRightMargin(0);
	pDoc->SetMathJustification("centerGroup");
	pDoc->SetMathWrapIndent(1440);
	pDoc->SetMathIntLim("subSup");
	pDoc->SetMathNAryLim("undOvr");
	pDoc->SetThemeFontLang("en-US");
	pDoc->SetClrSchemeMapping("w:followedHyperlink", "followedHyperlink");
	pDoc->SetClrSchemeMapping("w:hyperlink", "hyperlink");
	pDoc->SetClrSchemeMapping("w:accent6", "accent6");
	pDoc->SetClrSchemeMapping("w:accent5", "accent5");	
	pDoc->SetClrSchemeMapping("w:accent4", "accent4");
	pDoc->SetClrSchemeMapping("w:accent3", "accent3");
	pDoc->SetClrSchemeMapping("w:accent2", "accent2");
	pDoc->SetClrSchemeMapping("w:accent1", "accent1");
	pDoc->SetClrSchemeMapping("w:t2", "dark2");
	pDoc->SetClrSchemeMapping("w:bg2", "light2");
	pDoc->SetClrSchemeMapping("w:t1", "dark1");
	pDoc->SetClrSchemeMapping("w:bg1", "light1");
	pDoc->SetShapeDefaults("edit", "1026");
	pDoc->SetShapeDefaultIDMap("edit", "1");
	pDoc->SetDecimalSystem(".");
	pDoc->SetListSeperator(",");
	pDoc->SetW14DocID("392D05BB");
	pDoc->SetChartTrackingRefBased();
	pDoc->SetW15DocID("{669B4CBD-CC19-4D9F-BADC-A2B67F040A9C}");
}

