#include "stdafx.h"
#include "Settings.h"


Settings::Settings():DocXDocPart("word","settings.xml")
{
	pColorSchemeMapping = NULL;
	pShapeDefaults = NULL;
	pShapeLayout = NULL;
	pFootnoteProperties = NULL;
	pEndnoteProperties = NULL;
	pCompat = NULL;
	pRSIDs = NULL;
	pMathProperties = NULL;

	AddTag("w:settings");
	mainTag->AddAttribute("xmlns:mc", "http://schemas.openxmlformats.org/markup-compatibility/2006");
	mainTag->AddAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
	mainTag->AddAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
	mainTag->AddAttribute("xmlns:m", "http://schemas.openxmlformats.org/officeDocument/2006/math");
	mainTag->AddAttribute("xmlns:v", "urn:schemas-microsoft-com:vml");
	mainTag->AddAttribute("xmlns:w10", "urn:schemas-microsoft-com:office:word");
	mainTag->AddAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
	mainTag->AddAttribute("xmlns:w14", "http://schemas.microsoft.com/office/word/2010/wordml");
	mainTag->AddAttribute("xmlns:w15", "http://schemas.microsoft.com/office/word/2012/wordml");
	mainTag->AddAttribute("xmlns:w16cid", "http://schemas.microsoft.com/office/word/2016/wordml/cid");
	mainTag->AddAttribute("xmlns:w16se", "http://schemas.microsoft.com/office/word/2015/wordml/symex");
	mainTag->AddAttribute("xmlns:sl", "http://schemas.openxmlformats.org/schemaLibrary/2006/main");
	mainTag->AddAttribute("mc:Ignorable", "w14 w15 w16se w16cid");
}

Settings::~Settings()
{
}

void Settings::SetZoom(int val)
{
	DocXTag *pTag = mainTag->AddTag("w:zoom");
	pTag->AddAttribute("w:percent", val);
}

void Settings::SetProofState(string spelling, string grammar)
{
	DocXTag *pTag = mainTag->AddTag("w:proofState");
	pTag->AddAttribute("w:spelling", spelling);
	pTag->AddAttribute("w:grammar", grammar);
}

void Settings::SetDefaultTabStop(int val)
{
	DocXTag *pTag = mainTag->AddTag("w:defaultTabStop");
	pTag->AddAttribute("w:val", val);
}

void Settings::SetCharacterSpacingControl(string val)
{
	DocXTag *pTag = mainTag->AddTag("w:characterSpacingControl");
	pTag->AddAttribute("w:val", val);
}

void Settings::SetFootnoteProperties()
{
	if (pFootnoteProperties == NULL)
	{
		pFootnoteProperties = mainTag->AddTag("w:footnotePr");
	}
}

void Settings::SetFootnote(int id)
{
	SetFootnoteProperties();
	DocXTag *pTag = pFootnoteProperties->AddTag("w:footnote");
	pTag->AddAttribute("w:id", id);
}


void Settings::SetEndnoteProperties()
{
	if (pEndnoteProperties == NULL)
	{
		pEndnoteProperties = mainTag->AddTag("w:endnotePr");
	}
}

void Settings::SetEndnote(int id)
{
	SetEndnoteProperties();
	DocXTag *pTag = pEndnoteProperties->AddTag("w:endnote");
	pTag->AddAttribute("w:id", id);
}

void Settings::SetCompat()
{
	if (pCompat == NULL)
	{
		pCompat = mainTag->AddTag("w:compat");
	}
}

void Settings::SetCompatabilitySetting(string name, string uri, string val)
{
	SetCompat();
	DocXTag *pTag = pCompat->AddTag("w:compatSetting");
	pTag->AddAttribute("w:name", name);
	pTag->AddAttribute("w:uri", uri);
	pTag->AddAttribute("w:val", val);

}

void Settings::SetRSIDs()
{
	if (pRSIDs == NULL)
	{
		pRSIDs = mainTag->AddTag("w:rsids");
	}
}

void Settings::SetRSID(string type, string rsID)
{
	SetRSIDs();
	DocXTag *pTag = pRSIDs->AddTag("w:"+type);
	pTag->AddAttribute("w:val", rsID);
}

void Settings::SetMathProperties()
{
	if (pMathProperties == NULL)
	{
		pMathProperties = mainTag->AddTag("m:mathPr");
	}
}

void Settings::SetMathFont(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:mathFont");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetBrkBin(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:brkBin");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetBrkBinSub(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:brkBinSub");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetSmallFrac(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:smallFrac");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetDisplayDef()
{
	SetMathProperties();
	pMathProperties->AddTag("m:dispDef");
}

void Settings::SetMathLeftMargin(int val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:lMargin");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetMathRightMargin(int val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:rMargin");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetMathJustification(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:defJc");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetMathWrapIndent(int val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:wrapIndent");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetMathIntLim(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:intLim");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetMathNAryLim(string val)
{
	SetMathProperties();
	DocXTag *pTag = pMathProperties->AddTag("m:naryLim");
	pTag->AddAttribute("m:val", val);
}

void Settings::SetThemeFontLang(string val)
{
	DocXTag *pTag = mainTag->AddTag("w:themeFontLang");
	pTag->AddAttribute("w:val", val);
}

void Settings::SetClrSchemeMapping(string type, string val)
{
	SetColorSchemeMapping();
	pColorSchemeMapping->AddAttribute(type, val);
}

void Settings::SetColorSchemeMapping()
{
	if (pColorSchemeMapping == NULL)
	{
		pColorSchemeMapping = mainTag->AddTag("w:clrSchemeMapping");
	}
}

void Settings::SetShapeDefaults()
{
	if (pShapeDefaults == NULL)
	{
		pShapeDefaults = mainTag->AddTag("w:shapeDefaults");
	}
}

void Settings::SetShapeLayout(string type)
{
	SetShapeDefaults();
	if (pShapeLayout == NULL)
	{
		pShapeLayout = pShapeDefaults->AddTag("o:shapelayout");
		pShapeLayout->AddAttribute("v:ext", type);
	}
}


void Settings::SetShapeDefaults(string ext, string spidmax)
{
	SetShapeDefaults();
	DocXTag *pTag = pShapeDefaults->AddTag("o:shapedefaults");
	pTag->AddAttribute("v:ext", ext);
	pTag->AddAttribute("spidmax", spidmax);
}

void Settings::SetShapeDefaultIDMap(string ext, string data)
{
	SetShapeLayout(ext);
	DocXTag *pTag = pShapeLayout->AddTag("o:idmap");
	pTag->AddAttribute("v:ext", ext);
	pTag->AddAttribute("data", data);
}

void Settings::SetDecimalSystem(string val)
{
	DocXTag *pTag = mainTag->AddTag("w:decimalSymbol");
	pTag->AddAttribute("w:val", val);
}

void Settings::SetListSeperator(string val)
{
	DocXTag *pTag = mainTag->AddTag("w:listSeparator");
	pTag->AddAttribute("w:val", val);
}

void Settings::SetW14DocID(string val)
{
	DocXTag *pTag = mainTag->AddTag("w14:docId");
	pTag->AddAttribute("w14:val", val);
}

void Settings::SetChartTrackingRefBased()
{
	mainTag->AddTag("w15:chartTrackingRefBased");
}

void Settings::SetW15DocID(string val)
{
	DocXTag *pTag = mainTag->AddTag("w15:docId");
	pTag->AddAttribute("w15:val", val);
}


