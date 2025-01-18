#pragma once
#include <vector>
#include <string>
#include <list>
using namespace std;

class PlayerDataFileManager;
class PDFSOPDoc;
class BPMNElement;

struct linkReportSections;

class PDFDocument
{
	PlayerDataFileManager *pDataFileManager;
	list <BPMNElement *> reportElementList;
	list <struct linkReportSections *> reportSections;
	int nLevelNumber;
	string psSNo;

public:
	void GenerateReport();
	void SetPlayerManager(PlayerDataFileManager *pValue);
	void PrintCoverPage(PDFSOPDoc &doc);
	void PrintSummaryPage(PDFSOPDoc &doc);
	void PrintReport(PDFSOPDoc &doc, list <BPMNElement *> *pElementList);
	void PrintReportTable(string pMainSNo, PDFSOPDoc &doc, list <BPMNElement *> *pElementList);
	void PrintTaskGroup(PDFSOPDoc &doc, vector <BPMNElement *> &pElementList, struct linkReportSections *pSectionDetails);
	void PrintSubProcess(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails);
	void PrintSubProcessActivities(PDFSOPDoc &doc, BPMNElement *pElement);
	void PrintBranchActivities(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails);
	void PrintRightBranch(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails);
	void PrintBottomBranch(PDFSOPDoc &doc, BPMNElement *pElement, struct linkReportSections *pSectionDetails);
	void ReportGroupTable(PDFSOPDoc &doc, vector <BPMNElement *> &pElementList);
	void ReportGroupActionsTable(PDFSOPDoc &doc, vector <BPMNElement *> &pElementList);


	void ReportScenarioTable(PDFSOPDoc &doc);
	void PrintProcessWorkflow(PDFSOPDoc &doc);
	string WritePDFImageFile(vector <unsigned char> &buffer, bool bDecode = true);
	PDFDocument();
	~PDFDocument();

};

