#include "stdafx.h"
#include "WordDocument.h"
#include "DocumentBody.h"
#include "FootnotesDocument.h"
#include "EndnotesDocument.h"
#include "Settings.h"
#include "WebSettings.h"
#include "FontTableDoc.h"
#include "Font.h"
#include "Footer.h"
#include "Header.h"
#include "StyleDocument.h"
#include "Theme1.h"
#include "AppProps.h"
#include "CoreProps.h"
#include "zip.h"
#include "lodepng.h"
#include "lodepng_util.h"
#include "Numbering.h"
#include <iostream>

WordDocument::WordDocument(string fileName): DocXDocPart("word", "document.xml")
{
	pFootnotes = NULL;
	pEndnotes = NULL;
	pSettings = NULL;
	pWebSettings = NULL;
	pFooter1 = pFooter2 = pFooter3 = pFooter4 = NULL;
	pHeader1 = pHeader2 = pHeader3 = pHeader4 = NULL;
	pStyle = NULL;
	pTheme1 = NULL;
	pAppProps = NULL;
	pCoreProps = NULL;
	pNumbering = NULL;
	pFontTable = NULL;
	mainTag = AddTag("w:document");
	mainTag->AddAttribute("xmlns:wpc", "http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas");
	mainTag->AddAttribute("xmlns:cx", "http://schemas.microsoft.com/office/drawing/2014/chartex");
	mainTag->AddAttribute("xmlns:cx1", "http://schemas.microsoft.com/office/drawing/2015/9/8/chartex");
	mainTag->AddAttribute("xmlns:cx2", "http://schemas.microsoft.com/office/drawing/2015/10/21/chartex");
	mainTag->AddAttribute("xmlns:cx3", "http://schemas.microsoft.com/office/drawing/2016/5/9/chartex");
	mainTag->AddAttribute("xmlns:cx4", "http://schemas.microsoft.com/office/drawing/2016/5/10/chartex");
	mainTag->AddAttribute("xmlns:cx5", "http://schemas.microsoft.com/office/drawing/2016/5/11/chartex");
	mainTag->AddAttribute("xmlns:cx6", "http://schemas.microsoft.com/office/drawing/2016/5/12/chartex");
	mainTag->AddAttribute("xmlns:cx7", "http://schemas.microsoft.com/office/drawing/2016/5/13/chartex");
	mainTag->AddAttribute("xmlns:cx8", "http://schemas.microsoft.com/office/drawing/2016/5/14/chartex");
	mainTag->AddAttribute("xmlns:mc", "http://schemas.openxmlformats.org/markup-compatibility/2006");
	mainTag->AddAttribute("xmlns:aink", "http://schemas.microsoft.com/office/drawing/2016/ink");
	mainTag->AddAttribute("xmlns:am3d", "http://schemas.microsoft.com/office/drawing/2017/model3d");
	mainTag->AddAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
	mainTag->AddAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
	mainTag->AddAttribute("xmlns:m", "http://schemas.openxmlformats.org/officeDocument/2006/math");
	mainTag->AddAttribute("xmlns:v", "urn:schemas-microsoft-com:vml");
	mainTag->AddAttribute("xmlns:wp14", "http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing");
	mainTag->AddAttribute("xmlns:wp", "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing");
	mainTag->AddAttribute("xmlns:w10", "urn:schemas-microsoft-com:office:word");
	mainTag->AddAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
	mainTag->AddAttribute("xmlns:w14", "http://schemas.microsoft.com/office/word/2010/wordml");
	mainTag->AddAttribute("xmlns:w15", "http://schemas.microsoft.com/office/word/2012/wordml");
	mainTag->AddAttribute("xmlns:w16cid", "http://schemas.microsoft.com/office/word/2016/wordml/cid");
	mainTag->AddAttribute("xmlns:w16se", "http://schemas.microsoft.com/office/word/2015/wordml/symex");
	mainTag->AddAttribute("xmlns:wpg", "http://schemas.microsoft.com/office/word/2010/wordprocessingGroup");
	mainTag->AddAttribute("xmlns:wpi", "http://schemas.microsoft.com/office/word/2010/wordprocessingInk");
	mainTag->AddAttribute("xmlns:wne", "http://schemas.microsoft.com/office/word/2006/wordml");
	mainTag->AddAttribute("xmlns:wps", "http://schemas.microsoft.com/office/word/2010/wordprocessingShape");
	mainTag->AddAttribute("mc:Ignorable", "w14 w15 w16se w16cid wp14");

	zip = zip_open(fileName.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

}


WordDocument::~WordDocument()
{
	zip_close(zip);
}

void WordDocument::GenerateDocument()
{
	CreateDocumentRelations();
	CreatePackageRelations();
	CreateContentRelations();

	if (pNumbering)
		WriteZipFile(zip, "word/numbering.xml", pNumbering);
	if (pSettings)
		WriteZipFile(zip, "word/settings.xml", pSettings);
	if (pStyle)
		WriteZipFile(zip, "word/styles.xml", pStyle);
	if (pWebSettings)
		WriteZipFile(zip, "word/webSettings.xml", pWebSettings);
	if (pFontTable)
		WriteZipFile(zip, "word/fontTable.xml", pFontTable);
	if (pFootnotes)
		WriteZipFile(zip, "word/footnotes.xml", pFootnotes);
	if (pEndnotes)
		WriteZipFile(zip, "word/endnotes.xml", pEndnotes);
	if (pFooter1)
		WriteZipFile(zip, "word/footer1.xml", pFooter1);
	if (pFooter2)
		WriteZipFile(zip, "word/footer2.xml", pFooter2);
	if (pFooter3)
		WriteZipFile(zip, "word/footer3.xml", pFooter3);
	if (pFooter4)
		WriteZipFile(zip, "word/footer4.xml", pFooter4);

	if (pHeader1)
		WriteZipFile(zip, "word/header1.xml", pHeader1);
	if (pHeader2)
		WriteZipFile(zip, "word/header2.xml", pHeader2);
	if (pHeader3)
		WriteZipFile(zip, "word/header3.xml", pHeader3);
	if (pHeader4)
		WriteZipFile(zip, "word/header4.xml", pHeader4);

	if (pTheme1)
		WriteZipFile(zip, "word/theme/theme1.xml", pTheme1);
	if (pAppProps)
		WriteZipFile(zip, "docProps/app.xml", pAppProps);
	if (pCoreProps)
		WriteZipFile(zip, "docProps/core.xml", pCoreProps);
	WriteZipFile(zip, "word/document.xml", this);

	if (pDocumentRels)
		WriteZipFile(zip, "word/_rels/document.xml.rels", pDocumentRels);
	if (pPackageRels)
		WriteZipFile(zip, "_rels/.rels", pPackageRels);
	WriteZipFile(zip, "[Content_Types].xml", pContentType);
}

void WordDocument::CreateDocumentRelations()
{
	pDocumentRels = DBG_NEW DocXDocPart("word/_rels", "document.xml.rels");
	DocXTag *pMainTag = pDocumentRels->AddTag("Relationships");
	pMainTag->AddAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships");
	string strID = "rId";
	int val = 1;
	string strIDVal;
	DocXTag* pSingleTag = NULL;// , * pNested1Tag = NULL;
	char str[100];
	map <string, string> mapRelations;
	mapRelations.clear();

	sprintf_s(str, "%d", val);
	if (pNumbering)
	{

		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "numbering", strIDVal });
	}

	if (pStyle)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "style", strIDVal });
	}

	if (pSettings)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "settings", strIDVal });
	}

	if (pWebSettings)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "webSettings", strIDVal });
	}

	if (pFootnotes)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "footnotes", strIDVal });
	}

	if (pEndnotes)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "endnotes", strIDVal });
	}
	list <string>::iterator itImages = imageNames.begin();
	while (itImages != imageNames.end())
	{
		string strName = *itImages;
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ strName, strIDVal });
		itImages++;
	}

	if (pHeader1)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "header1", strIDVal });
	}

	if (pFooter1)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "footer1", strIDVal });
	}

	if (pFooter2)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "footer2", strIDVal });
	}

	if (pFontTable)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "fontTable", strIDVal });
	}

	if (pTheme1)
	{
		strIDVal = strID + str;
		val++;
		sprintf_s(str, "%d", val);
		mapRelations.insert({ "theme1", strIDVal });
	}

	map <string, string>::iterator itRelations;

	if (pHeader1)
	{
		itRelations = mapRelations.find("header1");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/header");
		pSingleTag->AddAttribute("Target", "header1.xml");
	}

	if (pSettings)
	{
		itRelations = mapRelations.find("settings");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings");
		pSingleTag->AddAttribute("Target", "settings.xml");
	}

	if (pNumbering)
	{
		itRelations = mapRelations.find("numbering");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/numbering");
		pSingleTag->AddAttribute("Target", "numbering.xml");
	}


	if (pFontTable)
	{
		itRelations = mapRelations.find("fontTable");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/fontTable");
		pSingleTag->AddAttribute("Target", "fontTable.xml");
	}

	if (pWebSettings)
	{
		itRelations = mapRelations.find("webSettings");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/webSettings");
		pSingleTag->AddAttribute("Target", "webSettings.xml");
	}

	if (pFooter2)
	{
		itRelations = mapRelations.find("footer2");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/footer");
		pSingleTag->AddAttribute("Target", "footer2.xml");
	}


	if (pStyle)
	{
		itRelations = mapRelations.find("style");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles");
		pSingleTag->AddAttribute("Target", "styles.xml");
	}

	if (pFooter1)
	{
		itRelations = mapRelations.find("footer1");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/footer");
		pSingleTag->AddAttribute("Target", "footer1.xml");
	}


	if (pFootnotes)
	{
		itRelations = mapRelations.find("footnotes");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/footnotes");
		pSingleTag->AddAttribute("Target", "footnotes.xml");
	}

	if (pEndnotes)
	{
		itRelations = mapRelations.find("endnotes");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/endnotes");
		pSingleTag->AddAttribute("Target", "endnotes.xml");
	}


	itImages = imageNames.begin();
	while (itImages != imageNames.end())
	{
		string strName = *itImages;
		itRelations = mapRelations.find(strName);
		strIDVal = itRelations->second;

		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image");
		pSingleTag->AddAttribute("Target", "media/" + strName);
		itImages++;
	}

	if (pTheme1)
	{
		itRelations = mapRelations.find("theme1");
		strIDVal = itRelations->second;
		pSingleTag = pMainTag->AddTag("Relationship");
		pSingleTag->AddAttribute("Id", strIDVal);
		pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme");
		pSingleTag->AddAttribute("Target", "theme/theme1.xml");
	}
}

void WordDocument::CreatePackageRelations()
{
	pPackageRels = DBG_NEW DocXDocPart("_rels", ".rels");
		
	DocXTag *pTag = pPackageRels->AddTag("Relationships");
	pTag->AddAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships");
	DocXTag* pSingleTag = NULL;// , * pNested1Tag = NULL;

	pSingleTag = pTag->AddTag("Relationship");
	pSingleTag->AddAttribute("Id", "rId3");
	pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties");
	pSingleTag->AddAttribute("Target", "docProps/app.xml");

	pSingleTag = pTag->AddTag("Relationship");
	pSingleTag->AddAttribute("Id", "rId2");
	pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties");
	pSingleTag->AddAttribute("Target", "docProps/core.xml");

	pSingleTag = pTag->AddTag("Relationship");
	pSingleTag->AddAttribute("Id", "rId1");
	pSingleTag->AddAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument");
	pSingleTag->AddAttribute("Target", "word/document.xml");

}

void WordDocument::CreateContentRelations()
{
	pContentType = DBG_NEW DocXDocPart("", "[Content_Types].xml");
	DocXTag *pMainTag = pContentType->AddTag("Types");
	pMainTag->AddAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/content-types");
	DocXTag* pSingleTag = NULL; // *pNested1Tag = NULL;


	map<string, string>::iterator itMap = imageFormats.begin();
	while (itMap != imageFormats.end())
	{
		string strImageType = itMap->second;
		if (strImageType == "png")
		{
			pSingleTag = pMainTag->AddTag("Default");
			pSingleTag->AddAttribute("ContentType", "image/png");
			pSingleTag->AddAttribute("Extension", "png");
		}
		itMap++;
	}
	pSingleTag = pMainTag->AddTag("Default");
	pSingleTag->AddAttribute("Extension", "rels");
	pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-package.relationships+xml");

	pSingleTag = pMainTag->AddTag("Default");
	pSingleTag->AddAttribute("Extension", "xml");
	pSingleTag->AddAttribute("ContentType", "application/xml");

	pSingleTag = pMainTag->AddTag("Override");
	
	pSingleTag->AddAttribute("PartName", "/word/document.xml");
	pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml");

	if (pNumbering)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/numbering.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.numbering+xml");
	}

	if (pStyle)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/styles.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml");
	}

	if (pSettings)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/settings.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml");
	}

	if (pWebSettings)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/webSettings.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.webSettings+xml");
	}

	if (pFootnotes)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/footnotes.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.footnotes+xml");
	}

	if (pEndnotes)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/endnotes.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.endnotes+xml");
	}

	if (pHeader1)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/header1.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml");
	}

	if (pFooter1)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/footer1.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml");
	}

	if (pFooter2)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/footer2.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml");
	}


	if (pFontTable)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/fontTable.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.wordprocessingml.fontTable+xml");
	}

	if (pTheme1)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/word/theme/theme1.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.theme+xml");
	}

	if (pCoreProps)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/docProps/core.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-package.core-properties+xml");
	}

	if (pAppProps)
	{
		pSingleTag = pMainTag->AddTag("Override");
		pSingleTag->AddAttribute("PartName", "/docProps/app.xml");
		pSingleTag->AddAttribute("ContentType", "application/vnd.openxmlformats-officedocument.extended-properties+xml");
	}
}

void WordDocument::WriteZipFile(struct zip_t *zipHandle, string path, DocXDocPart *pDoc)
{
	zip_entry_open(zipHandle, path.c_str());
	{
		string strResult = pDoc->GetDocString() + "\r\n";
		zip_entry_write(zipHandle, strResult.c_str(), strResult.size());
	}
	zip_entry_close(zipHandle);
}

DocumentBody *WordDocument::AddBody()
{
	DocumentBody *pBody = DBG_NEW DocumentBody();
	mainTag->AddTag(pBody);
	return pBody;
}

void WordDocument::SetFootnoteDocument()
{
	if (pFootnotes == NULL)
	{
		pFootnotes = DBG_NEW FootnotesDocument();
	}
}

void WordDocument::SetEndnoteDocument()
{
	if (pEndnotes == NULL)
	{
		pEndnotes = DBG_NEW EndnotesDocument();
	}
}

void WordDocument::AddFootnoteSeperator()
{
	SetFootnoteDocument();
	pFootnotes->AddFootnoteSeperator();
}

void WordDocument::AddFootnoteContinuousSeperator()
{
	SetFootnoteDocument();
	pFootnotes->AddFootnoteContinuousSeperator();
}

void WordDocument::AddEndnoteSeperator()
{
	SetEndnoteDocument();
	pEndnotes->AddEndnoteSeperator();
}

void WordDocument::AddEndnoteContinuousSeperator()
{
	SetEndnoteDocument();
	pEndnotes->AddEndnoteContinuousSeperator();
}

// Settings 

void WordDocument::SetSettingsDocument()
{
	if (pSettings == NULL)
	{
		pSettings = DBG_NEW Settings();
	}
}

void WordDocument::SetZoom(int val)
{
	SetSettingsDocument();
	pSettings->SetZoom(val);
}

void WordDocument::SetProofState(string spelling, string grammar)
{
	SetSettingsDocument();
	pSettings->SetProofState(spelling, grammar);

}

void WordDocument::SetDefaultTabStop(int val)
{
	SetSettingsDocument();
	pSettings->SetDefaultTabStop(val);
}

void WordDocument::SetCharacterSpacingControl(string val)
{
	SetSettingsDocument();
	pSettings->SetCharacterSpacingControl(val);

}

void WordDocument::SetFootnote(int id)
{
	SetSettingsDocument();
	pSettings->SetFootnote(id);
}

void WordDocument::SetEndnote(int id)
{
	SetSettingsDocument();
	pSettings->SetEndnote(id);

}

void WordDocument::SetCompatabilitySetting(string name, string uri, string val)
{
	SetSettingsDocument();
	pSettings->SetCompatabilitySetting(name, uri, val);
}

void WordDocument::SetRSID(string type, string rsID)
{
	SetSettingsDocument();
	pSettings->SetRSID(type, rsID);
}

void WordDocument::SetMathFont(string val)
{
	SetSettingsDocument();
	pSettings->SetMathFont(val);
}

void WordDocument::SetBrkBin(string val)
{
	SetSettingsDocument();
	pSettings->SetBrkBin(val);
}

void WordDocument::SetBrkBinSub(string val)
{
	SetSettingsDocument();
	pSettings->SetBrkBinSub(val);
}

void WordDocument::SetSmallFrac(string val)
{
	SetSettingsDocument();
	pSettings->SetSmallFrac(val);
}

void WordDocument::SetDisplayDef()
{
	SetSettingsDocument();
	pSettings->SetDisplayDef();
}

void WordDocument::SetMathLeftMargin(int val)
{
	SetSettingsDocument();
	pSettings->SetMathLeftMargin(val);
}

void WordDocument::SetMathRightMargin(int val)
{
	SetSettingsDocument();
	pSettings->SetMathRightMargin(val);
}

void WordDocument::SetMathJustification(string val)
{
	SetSettingsDocument();
	pSettings->SetMathJustification(val);
}

void WordDocument::SetMathWrapIndent(int val)
{
	SetSettingsDocument();
	pSettings->SetMathWrapIndent(val);
}

void WordDocument::SetMathIntLim(string val)
{
	SetSettingsDocument();
	pSettings->SetMathIntLim(val);
}

void WordDocument::SetMathNAryLim(string val)
{
	SetSettingsDocument();
	pSettings->SetMathNAryLim(val);
}

void WordDocument::SetThemeFontLang(string val)
{
	SetSettingsDocument();
	pSettings->SetThemeFontLang(val);
}

void WordDocument::SetClrSchemeMapping(string type, string val)
{
	SetSettingsDocument();
	pSettings->SetClrSchemeMapping(type, val);
}

void WordDocument::SetShapeDefaults(string ext, string spidmax)
{
	SetSettingsDocument();
	pSettings->SetShapeDefaults(ext, spidmax);
}

void WordDocument::SetShapeDefaultIDMap(string ext, string data)
{
	SetSettingsDocument();
	pSettings->SetShapeDefaultIDMap(ext, data);
}

void WordDocument::SetDecimalSystem(string val)
{
	SetSettingsDocument();
	pSettings->SetDecimalSystem(val);
}

void WordDocument::SetListSeperator(string val)
{
	SetSettingsDocument();
	pSettings->SetListSeperator(val);
}

void WordDocument::SetW14DocID(string val)
{
	SetSettingsDocument();
	pSettings->SetW14DocID(val);
}

void WordDocument::SetChartTrackingRefBased()
{
	SetSettingsDocument();
	pSettings->SetChartTrackingRefBased();
}

void WordDocument::SetW15DocID(string val)
{
	SetSettingsDocument();
	pSettings->SetW15DocID(val);
}

// Web Settings

void WordDocument::SetWebSettingsDocument()
{
	if (pWebSettings == NULL)
	{
		pWebSettings = DBG_NEW WebSettings();
	}
}

void WordDocument::SetOptimizeForBrowser()
{
	SetWebSettingsDocument();
	pWebSettings->SetOptimizeForBrowser();

}

void WordDocument::SetAllowPNG()
{
	SetWebSettingsDocument();
	pWebSettings->SetAllowPNG();
}

//Font Table
void WordDocument::SetFontTableDocument()
{
	if (pFontTable == NULL)
	{
		pFontTable = DBG_NEW FontTableDoc();
	}
}


Numbering *WordDocument::AddNumbering()
{
	if (pNumbering == NULL)
	{
		pNumbering = DBG_NEW Numbering();
	}

	return pNumbering;
}

Font *WordDocument::AddFontTable(string fontName)
{
	SetFontTableDocument();
	return pFontTable->AddFontTable(fontName);
}

// Header
Header *WordDocument::AddHeader(string name)
{
	Header *pHeader = NULL;
	if (pHeader1 == NULL)
	{
		pHeader = pHeader1 = DBG_NEW Header(name);
	}
	else if (pHeader2 == NULL)
	{
		pHeader = pHeader2 = DBG_NEW Header(name);
	}
	else if (pHeader3 == NULL)
	{
		pHeader = pHeader3 = DBG_NEW Header(name);
	}
	else if (pHeader4 == NULL)
	{
		pHeader = pHeader4 = DBG_NEW Header(name);
	}

	return pHeader;
}


// Footer
Footer *WordDocument::AddFooter(string footerType)
{
	Footer *pFooter = NULL;
	if (pFooter1 == NULL)
	{
		pFooter = pFooter1 = DBG_NEW Footer(footerType);
	}
	else if (pFooter2 == NULL)
	{
		pFooter = pFooter2 = DBG_NEW Footer(footerType);
	}
	else if (pFooter3 == NULL)
	{
		pFooter = pFooter3 = DBG_NEW Footer(footerType);
	}
	else if (pFooter4 == NULL)
	{
		pFooter = pFooter4 = DBG_NEW Footer(footerType);
	}

	return pFooter;
}

// Style
void WordDocument::SetStyleDocument()
{
	if (pStyle == NULL)
	{
		pStyle = DBG_NEW StyleDocument();
	}
}

void WordDocument::BuildStyleDocument()
{
	SetStyleDocument();
	pStyle->BuildStyleDocument();
}

// Theme1
void WordDocument::SetTheme1Document()
{
	if (pTheme1 == NULL)
	{
		pTheme1 = DBG_NEW Theme1();
	}
}

void WordDocument::BuildNormalTheme()
{
	SetTheme1Document();
	pTheme1->BuildNormalTheme();
}

// AppProps
void WordDocument::SetAppPropsDocument()
{
	if (pAppProps == NULL)
	{
		pAppProps = DBG_NEW AppProps();
	}
}

void WordDocument::BuildAppProps()
{
	SetAppPropsDocument();
	pAppProps->BuildAppProps();
}

// Core Props
void WordDocument::SetCorePropsDocument()
{
	if (pCoreProps == NULL)
	{
		pCoreProps = DBG_NEW CoreProps();
	}
}

void WordDocument::BuildCoreProps()
{
	SetCorePropsDocument();
	pCoreProps->BuildCoreProps();
}


string WordDocument::WriteImageFile(vector <unsigned char> &buffer, bool bDecode)
{
	string imageName = "word/media/image";
	char str[100];

	DocXDocPart::imageNumber++;
	sprintf_s(str, "%d", DocXDocPart::imageNumber);
	imageName += str;
	string extension = ".png";
	imageName += ".png";
	map <string, string>::iterator itMap = imageFormats.find("png");
	if (itMap == imageFormats.end())
		imageFormats.insert({ "png","png" });
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
			zip_entry_open(zip, imageName.c_str());
			{
				zip_entry_write(zip, &png[0], png.size());
			}
			zip_entry_close(zip);
		}
	}

	sprintf_s(str, "%d", DocXDocPart::imageNumber);
	imageName = "image";
	imageName += str;
	imageName += extension;
	imageNames.push_back(imageName);

	int idNo = GetImageIDNumber();
	imageName = "rId";

	sprintf_s(str, "%d", idNo);
	imageName += str;
	return imageName;
}

string WordDocument::WriteImageFile(string filename, string extensionType)
{
	string imageName = "word/media/image";
	char str[100];
	
	DocXDocPart::imageNumber++;
	sprintf_s(str, "%d", DocXDocPart::imageNumber);
	imageName +=  str;
	string extension = "";
	if (extensionType == "bitmap")
	{
		extension = ".png";
		imageName += ".png";
		map <string, string>::iterator itMap = imageFormats.find("png");
		if (itMap == imageFormats.end())
			imageFormats.insert({ "png","png" });
		std::vector<unsigned char> bmp;
		lodepng::load_file(bmp, filename.c_str());
		std::vector<unsigned char> image;
		unsigned w, h;
		unsigned error = lodepng::decodeBMP(image, w, h, bmp);
		if (!error)
		{
			std::vector<unsigned char> png;
			error = lodepng::encode(png, image, w, h);
			if (!error)
			{
				zip_entry_open(zip, imageName.c_str());
				{
					zip_entry_write(zip, &png[0], png.size());
				}
				zip_entry_close(zip);
			}
		}

	}

	sprintf_s(str, "%d", DocXDocPart::imageNumber);
	imageName = "image";
	imageName += str;
	imageName += extension;
	imageNames.push_back(imageName);

	int idNo = GetImageIDNumber();
	imageName = "rId";
	
	sprintf_s(str, "%d", idNo);
	imageName += str;
	return imageName;
}

string WordDocument::GetFooterReferenceID(string footerType)
{
	int val = GetImageIDNumber();
	if (pHeader1 != 0)
	{
		val++;
	}
	if (pHeader2 != 0)
	{
		val++;
	}
	if (pHeader3 != 0)
	{
		val++;
	}
	if (pHeader4 != 0)
	{
		val++;
	}
	if (footerType == "first")
		val++;

	val++;
	string strRet = "rId";
	char str[10];
	sprintf_s(str, "%d", val);
	strRet += str;
	return strRet;
}

string WordDocument::GetHeaderReferenceID(string headerType)
{
	int val = GetImageIDNumber();
	
	val++;
	string strRet = "rId";
	char str[10];
	sprintf_s(str, "%d", val);
	strRet += str;
	return strRet;
}

int WordDocument::GetImageIDNumber()
{
	int val = 0;
	string strIDVal;
	if (pNumbering)
	{
		val++;
	}

	if (pStyle)
	{
		val++;
	}

	if (pSettings)
	{
		val++;
	}

	if (pWebSettings)
	{
		val++;
	}

	if (pFootnotes)
	{
		val++;
	}

	if (pEndnotes)
	{
		val++;
	}
	val += DocXDocPart::imageNumber;

	return val;
}
