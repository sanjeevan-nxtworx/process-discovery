#pragma once
#include "DocXDocPart.h"
#include <string>

using namespace std;

class DocXTag;

class Settings :
	public DocXDocPart
{
	DocXTag *pColorSchemeMapping;
	DocXTag *pShapeDefaults;
	DocXTag *pShapeLayout;
	DocXTag *pFootnoteProperties;
	DocXTag *pEndnoteProperties;
	DocXTag *pCompat;
	DocXTag *pRSIDs;
	DocXTag *pMathProperties;

	void SetColorSchemeMapping();
	void SetShapeDefaults();
	void SetShapeLayout(string type);
	void SetFootnoteProperties();
	void SetEndnoteProperties();
	void SetCompat();
	void SetRSIDs();
	void SetMathProperties();


public:
	Settings();
	~Settings();

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
};

