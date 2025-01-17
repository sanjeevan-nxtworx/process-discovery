#include "stdafx.h"
#include "Numbering.h"
#include "DocXTag.h"

Numbering::Numbering():DocXDocPart("word","numbering.xml")
{
	properties = NULL;
	runProperties = NULL;
	pLevel = NULL;
	pAbstractTag = NULL;
	mainTag = AddTag("w:numbering");
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

}


Numbering::~Numbering()
{
}

void Numbering::AddAbstractNumbering(string nsid, string multilevelType, string templ)
{
	
	DocXTag *pTag = mainTag->AddTag("w:abstractNum");
	pAbstractTag = pTag;
	pTag->AddAttribute("w:abstractNumId", DocXDocPart::abstractNumID);
	DocXDocPart::abstractNumID++;
	pTag->AddAttribute("w15:restartNumberingAfterBreak", 0);


	DocXTag *pTag1 = pTag->AddTag("w:nsid");
	pTag1->AddAttribute("w:val", nsid);
	pTag1 = pTag->AddTag("w:multiLevelType");
	pTag1->AddAttribute("w:val", multilevelType);
	pTag1 = pTag->AddTag("w:tmpl");
	pTag1->AddAttribute("w:val", templ);

}

void Numbering::AddLevelNumbering(string tplc, int lev, int start, string numFmt, string pStyle, string levelText, string justification, bool tentative)
{
	DocXTag *pTag = pAbstractTag->AddTag("w:lvl ");
	properties = NULL;
	runProperties = NULL;
	pLevel = pTag;
	if (tplc != "")
		pTag->AddAttribute("w:tplc", tplc);
	pTag->AddAttribute("w:ilvl", lev);
	if (tentative)
	{
		pTag->AddAttribute("w:tentative", "1");
	}
	DocXTag *pTag1 = NULL;

	pTag1 = pTag->AddTag("w:start");
	pTag1->AddAttribute("w:val", start);
	if (numFmt != "")
	{
		pTag1 = pTag->AddTag("w:numFmt");
		pTag1->AddAttribute("w:val", numFmt);
	}
	if (pStyle != "")
	{
		pTag1 = pTag->AddTag("w:pStyle");
		pTag1->AddAttribute("w:val", pStyle);
	}

	if (levelText != "")
	{
		pTag1 = pTag->AddTag("w:lvlText");
		pTag1->AddAttribute("w:val", levelText);
	}
	if (justification != "")
	{
		pTag1 = pTag->AddTag("w:lvlJc");
		pTag1->AddAttribute("w:val", justification);
	}
}

void Numbering::SetProperties()
{
	if (properties == NULL && pLevel != NULL)
	{
		properties = pLevel->AddTag("w:pPr");
	}
}

void Numbering::SetLevelIndent(int hanging, int left)
{
	SetProperties();
	if (properties != NULL)
	{
		DocXTag *pTag = properties->AddTag("w:ind");
		pTag->AddAttribute("w:hanging", hanging);
		pTag->AddAttribute("w:left", left);
	}
}

void Numbering::SetRunProperties()
{
	if (runProperties == NULL && pLevel != NULL)
	{
		runProperties = pLevel->AddTag("w:rPr");
	}
}

void Numbering::SetFontHint(string hint, string ansi, string ascii)
{
	SetRunProperties();
	if (runProperties != NULL)
	{
		DocXTag *pTag = runProperties->AddTag("w:rFonts");
		pTag->AddAttribute("w:hint", hint);
		if (ansi != "")
			pTag->AddAttribute("w:hAnsi", ansi);
		if (ascii != "")
			pTag->AddAttribute("w:ascii", ascii);
	}
}

void Numbering::SetFontHint(string hint, string ansi, string ascii, string cs)
{
	SetRunProperties();
	if (runProperties != NULL)
	{
		DocXTag *pTag = runProperties->AddTag("w:rFonts");
		pTag->AddAttribute("w:hint", hint);
		if (ansi != "")
			pTag->AddAttribute("w:hAnsi", ansi);
		if (ascii != "")
			pTag->AddAttribute("w:ascii", ascii);
		if (cs != "")
			pTag->AddAttribute("w:cs", cs);
	}
}

DocXTag *Numbering::AddAbstractNumbering(int nsid, int id)
{
	DocXTag *pTag = mainTag->AddTag("w:num");
	pTag->AddAttribute("w:numId", nsid);
	DocXTag *pTag1 = pTag->AddTag("w:abstractNumId");
	pTag1->AddAttribute("w:val", id);
	return pTag;
}



