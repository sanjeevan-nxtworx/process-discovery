#include "stdafx.h"
#include "FootnotesDocument.h"
#include "Paragraph.h"
#include "ParagraphRun.h"

FootnotesDocument::FootnotesDocument():DocXDocPart("word","footnotes.xml")
{
	footnoteID = -1;
	mainTag = AddTag("w:footnotes");
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


FootnotesDocument::~FootnotesDocument()
{
	
}

void FootnotesDocument::AddFootnoteSeperator()
{
	DocXTag *pTag = mainTag->AddTag("w:footnote");
	pTag->AddAttribute("w:id", footnoteID);
	pTag->AddAttribute("w:type", "separator");
	footnoteID++;

	Paragraph *paragraph = pTag->AddParagraph();
	paragraph->SetSpacing("auto", 240, 0);
	ParagraphRun *run = paragraph->AddRun();
	run->SetSeperator();
}

void FootnotesDocument::AddFootnoteContinuousSeperator()
{
	DocXTag *pTag = mainTag->AddTag("w:footnote");
	pTag->AddAttribute("w:id", footnoteID);
	pTag->AddAttribute("w:type", "continuationSeparator");
	footnoteID++;

	Paragraph *paragraph = pTag->AddParagraph();
	paragraph->SetSpacing("auto", 240, 0);
	ParagraphRun *run = paragraph->AddRun();
	run->SetContinuationSeperator();
}
