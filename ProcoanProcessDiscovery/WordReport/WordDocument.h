#pragma once
#include "DocXDocPart.h"
#include <list>
#include <map>
#include <vector>
using namespace std;

class FootnotesDocument;
class EndnotesDocument;
class DocumentBody;
class Settings;
class WebSettings;
class FontTableDoc;
class Font;
class Footer;
class StyleDocument;
class Theme1;
class CoreProps;
class AppProps;
struct zip_t;
class Header;
class Numbering;

class WordDocument :
	public DocXDocPart
{
	FootnotesDocument *pFootnotes;
	EndnotesDocument *pEndnotes;
	Settings *pSettings;
	WebSettings *pWebSettings;
	FontTableDoc *pFontTable;
	Footer *pFooter1;
	Footer *pFooter2;
	Footer *pFooter3;
	Footer *pFooter4;
	Numbering *pNumbering;

	Header *pHeader1;
	Header *pHeader2;
	Header *pHeader3;
	Header *pHeader4;

	StyleDocument *pStyle;
	Theme1 *pTheme1;
	CoreProps *pCoreProps;
	AppProps *pAppProps;
	DocXDocPart *pDocumentRels;
	DocXDocPart *pPackageRels;
	DocXDocPart *pContentType;

	list <string> imageNames;
	map <string, string> imageFormats;
	struct zip_t *zip;

	void SetFootnoteDocument();
	void SetEndnoteDocument();
	void SetSettingsDocument();
	void SetWebSettingsDocument();
	void SetFontTableDocument();
	void SetStyleDocument();
	void SetTheme1Document();
	void SetAppPropsDocument();
	void SetCorePropsDocument();
	void WriteZipFile(struct zip_t *zip, string path, DocXDocPart *pDoc);
	void CreateDocumentRelations();
	void CreatePackageRelations();
	void CreateContentRelations();
public:
	WordDocument(string fileName);
	~WordDocument();

	void GenerateDocument();
	DocumentBody *AddBody();

	// Footnote
	void AddFootnoteSeperator();
	void AddFootnoteContinuousSeperator();

	//End Note
	void AddEndnoteSeperator();
	void AddEndnoteContinuousSeperator();

	// Settings
	void SetZoom(int val);
	void SetProofState(string spelling, string grammar);
	void SetDefaultTabStop(int val);
	void SetCharacterSpacingControl(string val);
	void SetFootnote(int id);
	void SetEndnote(int id);
	void SetCompatabilitySetting(string name, string uri, string val);
	void SetRSID(string type, string rsID);
	void SetMathFont(string val);
	void SetBrkBin(string val);
	void SetBrkBinSub(string val);
	void SetSmallFrac(string val);
	void SetDisplayDef();
	void SetMathLeftMargin(int val);
	void SetMathRightMargin(int val);
	void SetMathJustification(string val);
	void SetMathWrapIndent(int val);
	void SetMathIntLim(string val);
	void SetMathNAryLim(string val);
	void SetThemeFontLang(string val);
	void SetClrSchemeMapping(string type, string val);
	void SetShapeDefaults(string ext, string spidmax);
	void SetShapeDefaultIDMap(string ext, string data);
	void SetDecimalSystem(string val);
	void SetListSeperator(string val);
	void SetW14DocID(string val);
	void SetChartTrackingRefBased();
	void SetW15DocID(string val);

	//Web Settings
	void SetOptimizeForBrowser();
	void SetAllowPNG();

	// Numbering
	Numbering *AddNumbering();

	//Font Table
	Font *AddFontTable(string fontName);

	//Header 
	Header *AddHeader(string name);

	//Footer 
	Footer *AddFooter(string footerName);

	//Style
	void BuildStyleDocument();

	//Theme
	void BuildNormalTheme();

	// App Props
	void BuildAppProps();

	//Core Props
	void BuildCoreProps();

	//Write Images
	string WriteImageFile(string filename, string extensionType);
	string WriteImageFile(vector <unsigned char> &buffer, bool bDecode = true);
	int GetImageIDNumber();
	string GetFooterReferenceID(string footerType);
	string GetHeaderReferenceID(string headerType);
};

