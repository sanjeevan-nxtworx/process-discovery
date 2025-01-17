#include "stdafx.h"
#include "AppProps.h"


AppProps::AppProps():DocXDocPart("docProps","app.xml")
{
	mainTag = AddTag("Properties");
	mainTag->AddAttribute("xmlns", "http://schemas.openxmlformats.org/officeDocument/2006/extended-properties");
	mainTag->AddAttribute("xmlns:vt", "http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes");
}


AppProps::~AppProps()
{
}

void AppProps::BuildAppProps()
{
	DocXTag *pSingleTag = NULL, *pNested1Tag = NULL, *pNested2Tag = NULL, *pNested3Tag = NULL;

	pSingleTag = mainTag->AddTag("Template");
	pSingleTag->SetValue("Normal.dotm");
	pSingleTag = mainTag->AddTag("TotalTime");

	DocXDocPart::totalTime = 10;
	char str[100];
	sprintf_s(str, "%d", DocXDocPart::totalTime);
	pSingleTag->SetValue(str);

	DocXDocPart::noPages = 6;
	pSingleTag = mainTag->AddTag("Pages");
	sprintf_s(str, "%d", DocXDocPart::noPages);
	pSingleTag->SetValue(str);

	DocXDocPart::noWords = 247;
	pSingleTag = mainTag->AddTag("Words");
	sprintf_s(str, "%d", DocXDocPart::noWords);
	pSingleTag->SetValue(str);

	DocXDocPart::noCharacters = 1412;
	pSingleTag = mainTag->AddTag("Characters");
	sprintf_s(str, "%d", DocXDocPart::noCharacters);
	pSingleTag->SetValue(str);

	pSingleTag = mainTag->AddTag("Application");
	pSingleTag->SetValue("Microsoft Office Word");
	pSingleTag = mainTag->AddTag("DocSecurity");
	pSingleTag->SetValue("0");
	pSingleTag = mainTag->AddTag("Lines");
	DocXDocPart::noLines = 11;
	sprintf_s(str, "%d", DocXDocPart::noLines);
	pSingleTag->SetValue(str);
	DocXDocPart::noParagraphs = 3;
	pSingleTag = mainTag->AddTag("Paragraphs");
	sprintf_s(str, "%d", DocXDocPart::noParagraphs);
	pSingleTag->SetValue(str);
	pSingleTag = mainTag->AddTag("ScaleCrop");
	pSingleTag->SetValue("false");

	pSingleTag = mainTag->AddTag("HeadingPairs");
	pNested1Tag = pSingleTag->AddTag("vt:vector");
	pNested1Tag->AddAttribute("size", "2");
	pNested1Tag->AddAttribute("baseType", "variant");
	pNested2Tag = pNested1Tag->AddTag("vt:variant");
	pNested3Tag = pNested2Tag->AddTag("vt:lpstr");
	pNested3Tag->SetValue("Title");

	pNested2Tag = pNested1Tag->AddTag("vt:variant");
	pNested3Tag = pNested2Tag->AddTag("vt:i4");
	pNested3Tag->SetValue("1");

	pSingleTag = mainTag->AddTag("TitlesOfParts");
	pNested1Tag = pSingleTag->AddTag("vt:vector");
	pNested1Tag->AddAttribute("size", "1");
	pNested1Tag->AddAttribute("baseType", "lpstr");
	pNested2Tag = pNested1Tag->AddTag("vt:lpstr", true);

	pSingleTag = mainTag->AddTag("Company", true);
	pSingleTag = mainTag->AddTag("LinksUpToDate");
	pSingleTag->SetValue("false");
	pSingleTag = mainTag->AddTag("CharactersWithSpaces");
	DocXDocPart::noCharactersWithSpaces = 1656;
	sprintf_s(str, "%d", DocXDocPart::noCharactersWithSpaces);
	pSingleTag->SetValue(str);

	pSingleTag = mainTag->AddTag("SharedDoc");
	pSingleTag->SetValue("false");
	pSingleTag = mainTag->AddTag("HyperlinksChanged");
	pSingleTag->SetValue("false");
	pSingleTag = mainTag->AddTag("AppVersion");
	pSingleTag->SetValue("16.0000");
}
