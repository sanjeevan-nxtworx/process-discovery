#pragma once

#include <string>
#include "..\json11.hpp"
#include <list>

using namespace std;
using namespace json11;

class DocXDocPart;
class DocXTag;
class WordDocument;
class DocumentBody;
class Table;
class TableRow;
class BPMNElement;
class BPMNDiagram;
class PlayerDataFileManager;

class SOPWordReport
{
	string reportName;
	Json jSonElement;
	map <string, Json> processInfo;

	list <BPMNElement *> *pElementList;
	BPMNDiagram *pDiagramMap;
	PlayerDataFileManager *pPlayerManager;
	bool bSetting;
	bool bWebSetting;
	bool bStyle;
	bool bFontTable;
	bool bTheme1;
	bool bNumbering;

	string strStyleRID;
	string strSettingRID;
	string strWebSettingRID;
	string strFontTableRID;
	string strTheme1RID;

	void CreateSettingsFile(WordDocument *pDoc);
	void CreateWebSettingsFile(WordDocument *pDoc);
	void CreateFootnotesFile(WordDocument *pDoc);
	void CreateEndnotesFile(WordDocument *pDoc);
	void CreateApplicationPropertiesFile(WordDocument *pDoc);
	void CreateCorePropertiesFile(WordDocument *pDoc);
	void CreateFooterFile(WordDocument *pDoc);
	void CreateHeaderFile(WordDocument *pDoc);
	void CreateMainFooterFile(WordDocument *pDoc);
	void CreateCoverPage(WordDocument *pDoc, DocumentBody *pBody);
	void CreateControlPage(DocumentBody *pBody);
	void CreateTOC(DocumentBody *pBody);
	void CreateFontTableFile(WordDocument *pDoc);
	void CreateTheme1File(WordDocument *pDoc);
	void CreateNumberingFile(WordDocument *pDoc);
	void CreateProcessSummaryPage(WordDocument *pDoc, DocumentBody *pBody);
	void CreateProcessStep(WordDocument *pDoc, DocumentBody *pBody);

	void AddControlTableRow(Table *pTable, string column1Text, string column2Text);
	void AddEmptyParagraph(DocumentBody *pBody, string tabSide, int tabPos, string justification, string runFont, int runFontSize, int runCsSize);
	void AddFirstRowTableCell(TableRow *pRow, int cellWidth, string text);
	void AddRowTableCell(TableRow *pRow, int cellWidth, string text);
	void AddListItemParagraph(DocXTag *pInsTag, string listStyle, int level, int numID, string text);
	void AddTOC(string tocStyle, int leftTab, int rightTab, string tocVal, string bookmark, string numString, int pageNum, bool bFirst = false);
	void EndTOC();
	string GetNextBookmarkID();
	string GetNextTOCID();
	void WriteProcessSteps(list <BPMNElement *> *eventList, string strLevel, int levelDepth, WordDocument *pDocument, DocumentBody *pBody);
	void WriteTaskSteps(BPMNElement *pElement, string strLevel, int subLevelNo, int levelDepth, WordDocument *pDocument, DocumentBody *pBody);
	void WriteSubProcessSteps(BPMNElement *pElement, string strLevel, int subLevelNo, int levelDepth, WordDocument *pDocument, DocumentBody *pBody);


public:
	SOPWordReport(string docName);
	~SOPWordReport();
	void CreateSOPReport();
	void SetProcessValue(string value);
	void SetPlayerManager(PlayerDataFileManager *pVal);
	void SetDiagram(list <BPMNElement *> *pVal);
	
};

