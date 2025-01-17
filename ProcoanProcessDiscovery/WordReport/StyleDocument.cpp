#include "stdafx.h"
#include "StyleDocument.h"


StyleDocument::StyleDocument() : DocXDocPart("word","styles.xml")
{
	mainTag = AddTag("w:styles");
	mainTag->AddAttribute("xmlns:mc", "http://schemas.openxmlformats.org/markup-compatibility/2006");
	mainTag->AddAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
	mainTag->AddAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
	mainTag->AddAttribute("xmlns:w14", "http://schemas.microsoft.com/office/word/2010/wordml");
	mainTag->AddAttribute("xmlns:w15", "http://schemas.microsoft.com/office/word/2012/wordml");
	mainTag->AddAttribute("xmlns:w16cid", "http://schemas.microsoft.com/office/word/2016/wordml/cid");
	mainTag->AddAttribute("xmlns:w16se", "http://schemas.microsoft.com/office/word/2015/wordml/symex");
	mainTag->AddAttribute("mc:Ignorable", "w14 w15 w16se w16cid");
}

StyleDocument::~StyleDocument()
{
}

void StyleDocument::BuildStyleDocument()
{
	DocXTag* pNested1Tag = NULL, * pNested2Tag = NULL, * pNested3Tag = NULL; // , * pNested4Tag = NULL;
	DocXTag *pSingleTag = NULL;
	pSingleTag = mainTag->AddTag("w:docDefaults");

	pNested1Tag = pSingleTag->AddTag("w:rPrDefault");
	pNested2Tag = pNested1Tag->AddTag("w:rPr");
	pNested3Tag = pNested2Tag->AddTag("w:rFonts");
	pNested3Tag->AddAttribute("w:asciiTheme", "minorHAnsi");
	pNested3Tag->AddAttribute("w:eastAsiaTheme", "minorHAnsi");
	pNested3Tag->AddAttribute("w:hAnsiTheme", "minorHAnsi");
	pNested3Tag->AddAttribute("w:cstheme", "minorBidi");
	pNested3Tag = pNested2Tag->AddTag("w:sz");
	pNested3Tag->AddAttribute("w:val", "22");
	pNested3Tag = pNested2Tag->AddTag("w:szCs");
	pNested3Tag->AddAttribute("w:val", "22");
	pNested3Tag = pNested2Tag->AddTag("w:lang");
	pNested3Tag->AddAttribute("w:val", "en-US");
	pNested3Tag->AddAttribute("w:eastAsia", "en-US");
	pNested3Tag->AddAttribute("w:bidi", "ar-SA");

	pNested1Tag = pSingleTag->AddTag("w:pPrDefault");
	pNested2Tag = pNested1Tag->AddTag("w:pPr");
	pNested3Tag = pNested2Tag->AddTag("w:spacing");
	pNested3Tag->AddAttribute("w:after", "160");
	pNested3Tag->AddAttribute("w:line", "259");
	pNested3Tag->AddAttribute("w:lineRule", "auto");

	pSingleTag = mainTag->AddTag("w:latentStyles");
	pSingleTag->AddAttribute("w:defLockedState", "0");
	pSingleTag->AddAttribute("w:defUIPriority", "99");
	pSingleTag->AddAttribute("w:defSemiHidden", "0");
	pSingleTag->AddAttribute("w:defUnhideWhenUsed", "0");
	pSingleTag->AddAttribute("w:defQFormat", "0");
	pSingleTag->AddAttribute("w:count", "377");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Normal");
	pNested1Tag->AddAttribute("w:uiPriority", "0");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 6");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 7");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 8");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "heading 9");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "9");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 1");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 6");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 7");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 8");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index 9");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 1");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 6");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 7");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 8");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toc 9");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "39");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Normal Indent");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "footnote text");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "annotation text");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "header");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "footer");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "index heading");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "caption");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "35");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "table of figures");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "envelope address");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "envelope return");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "footnote reference");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "annotation reference");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "line number");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "page number");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "endnote reference");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "endnote text");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "table of authorities");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "macro");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "toa heading");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Bullet");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Number");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Bullet 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Bullet 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Bullet 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Bullet 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Number 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Number 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Number 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Number 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Title");
	pNested1Tag->AddAttribute("w:uiPriority", "10");
	pNested1Tag->AddAttribute("w:qFormat", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Closing");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Signature");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Default Paragraph Font");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:uiPriority", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Body Text");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "Body Text Indent");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Continue");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Continue 2");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Continue 3");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Continue 4");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	pNested1Tag = pSingleTag->AddTag("w:lsdException");
	pNested1Tag->AddAttribute("w:name", "List Continue 5");
	pNested1Tag->AddAttribute("w:semiHidden", "1");
	pNested1Tag->AddAttribute("w:unhideWhenUsed", "1");

	//	Set_lsdException(pSingleTag, "name", "semiHidded", "unhideWhenUsed", "uiPriority", "qFormat");
	Set_lsdException(pSingleTag, "Message Header", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Subtitle", "", "", "11", "1");
	Set_lsdException(pSingleTag, "Salutation", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Date", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Body Text First Indent", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Body Text First Indent 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Note Heading", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Body Text 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Body Text 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Body Text Indent 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Body Text Indent 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Block Text", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Hyperlink", "1", "1", "", "");
	Set_lsdException(pSingleTag, "FollowedHyperlink", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Strong", "", "", "22", "1");
	Set_lsdException(pSingleTag, "Emphasis", "", "", "20", "1");
	Set_lsdException(pSingleTag, "Document Map", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Plain Text", "1", "1", "", "");
	Set_lsdException(pSingleTag, "E-mail Signature", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Top of Form", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Bottom of Form", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Normal (Web)", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Acronym", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Address", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Cite", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Code", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Definition", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Keyboard", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Preformatted", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Sample", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Typewriter", "1", "1", "", "");
	Set_lsdException(pSingleTag, "HTML Variable", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Normal Table", "1", "1", "", "");
	Set_lsdException(pSingleTag, "annotation subject", "1", "1", "", "");
	Set_lsdException(pSingleTag, "No List", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Outline List 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Outline List 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Outline List 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Simple 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Simple 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Simple 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Classic 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Classic 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Classic 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Classic 4", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Colorful 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Colorful 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Colorful 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Columns 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Columns 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Columns 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Columns 4", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Columns 5", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 4", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 5", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 6", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 7", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid 8", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 4", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 5", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 6", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 7", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table List 8", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table 3D effects 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table 3D effects 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table 3D effects 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Contemporary", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Elegant", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Professional", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Subtle 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Subtle 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Web 1", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Web 2", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Web 3", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Balloon Text", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Table Grid", "", "", "39", "");
	Set_lsdException(pSingleTag, "Table Theme", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Placeholder Text", "1", "", "", "");
	Set_lsdException(pSingleTag, "No Spacing", "", "", "1", "1");
	Set_lsdException(pSingleTag, "Light Shading", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1", "", "", "65", "");
	Set_lsdException(pSingleTag, "Medium List 2", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List", "", "", "70", "");
	Set_lsdException(pSingleTag, "Colorful Shading", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid", "", "", "73", "");
	Set_lsdException(pSingleTag, "Light Shading Accent 1", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List Accent 1", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid Accent 1", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1 Accent 1", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2 Accent 1", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1 Accent 1", "", "", "65", "");
	Set_lsdException(pSingleTag, "Revision", "1", "", "", "");
	Set_lsdException(pSingleTag, "List Paragraph", "", "", "34", "1");
	Set_lsdException(pSingleTag, "Quote", "", "", "29", "1");
	Set_lsdException(pSingleTag, "Intense Quote", "", "", "30", "1");
	Set_lsdException(pSingleTag, "Medium List 2 Accent 1", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1 Accent 1", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2 Accent 1", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3 Accent 1", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List Accent 1", "", "", "70", "");

	Set_lsdException(pSingleTag, "Colorful Shading Accent 1", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List Accent 1", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid Accent 1", "", "", "73", "");
	Set_lsdException(pSingleTag, "Light Shading Accent 2", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List Accent 2", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid Accent 2", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1 Accent 2", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2 Accent 2", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1 Accent 2", "", "", "65", "");
	Set_lsdException(pSingleTag, "Medium List 2 Accent 2", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1 Accent 2", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2 Accent 2", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3 Accent 2", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List Accent 2", "", "", "70", "");
	Set_lsdException(pSingleTag, "Colorful Shading Accent 2", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List Accent 2", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid Accent 2", "", "", "73", "");
	Set_lsdException(pSingleTag, "Light Shading Accent 3", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List Accent 3", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid Accent 3", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1 Accent 3", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2 Accent 3", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1 Accent 3", "", "", "65", "");
	Set_lsdException(pSingleTag, "Medium List 2 Accent 3", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1 Accent 3", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2 Accent 3", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3 Accent 3", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List Accent 3", "", "", "70", "");
	Set_lsdException(pSingleTag, "Colorful Shading Accent 3", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List Accent 3", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid Accent 3", "", "", "73", "");
	Set_lsdException(pSingleTag, "Light Shading Accent 4", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List Accent 4", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid Accent 4", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1 Accent 4", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2 Accent 4", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1 Accent 4", "", "", "65", "");
	Set_lsdException(pSingleTag, "Medium List 2 Accent 4", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1 Accent 4", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2 Accent 4", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3 Accent 4", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List Accent 4", "", "", "70", "");
	Set_lsdException(pSingleTag, "Colorful Shading Accent 4", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List Accent 4", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid Accent 4", "", "", "73", "");
	Set_lsdException(pSingleTag, "Light Shading Accent 5", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List Accent 5", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid Accent 5", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1 Accent 5", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2 Accent 5", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1 Accent 5", "", "", "65", "");
	Set_lsdException(pSingleTag, "Medium List 2 Accent 5", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1 Accent 5", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2 Accent 5", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3 Accent 5", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List Accent 5", "", "", "70", "");
	Set_lsdException(pSingleTag, "Colorful Shading Accent 5", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List Accent 5", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid Accent 5", "", "", "73", "");

	Set_lsdException(pSingleTag, "Light Shading Accent 6", "", "", "60", "");
	Set_lsdException(pSingleTag, "Light List Accent 6", "", "", "61", "");
	Set_lsdException(pSingleTag, "Light Grid Accent 6", "", "", "62", "");
	Set_lsdException(pSingleTag, "Medium Shading 1 Accent 6", "", "", "63", "");
	Set_lsdException(pSingleTag, "Medium Shading 2 Accent 6", "", "", "64", "");
	Set_lsdException(pSingleTag, "Medium List 1 Accent 6", "", "", "65", "");
	Set_lsdException(pSingleTag, "Medium List 2 Accent 6", "", "", "66", "");
	Set_lsdException(pSingleTag, "Medium Grid 1 Accent 6", "", "", "67", "");
	Set_lsdException(pSingleTag, "Medium Grid 2 Accent 6", "", "", "68", "");
	Set_lsdException(pSingleTag, "Medium Grid 3 Accent 6", "", "", "69", "");
	Set_lsdException(pSingleTag, "Dark List Accent 6", "", "", "70", "");
	Set_lsdException(pSingleTag, "Colorful Shading Accent 6", "", "", "71", "");
	Set_lsdException(pSingleTag, "Colorful List Accent 6", "", "", "72", "");
	Set_lsdException(pSingleTag, "Colorful Grid Accent 6", "", "", "73", "");
	Set_lsdException(pSingleTag, "Subtle Emphasis", "", "", "19", "1");
	Set_lsdException(pSingleTag, "Intense Emphasis", "", "", "21", "1");
	Set_lsdException(pSingleTag, "Subtle Reference", "", "", "31", "1");
	Set_lsdException(pSingleTag, "Intense Reference", "", "", "32", "1");
	Set_lsdException(pSingleTag, "Book Title", "", "", "33", "1");
	Set_lsdException(pSingleTag, "Bibliography", "1", "1", "37", "");
	Set_lsdException(pSingleTag, "TOC Heading", "1", "1", "39", "1");
	Set_lsdException(pSingleTag, "Plain Table 1", "", "", "41", "");
	Set_lsdException(pSingleTag, "Plain Table 2", "", "", "42", "");
	Set_lsdException(pSingleTag, "Plain Table 3", "", "", "43", "");
	Set_lsdException(pSingleTag, "Plain Table 4", "", "", "44", "");
	Set_lsdException(pSingleTag, "Plain Table 5", "", "", "45", "");
	Set_lsdException(pSingleTag, "Grid Table Light", "", "", "40", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful", "", "", "52", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light Accent 1", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2 Accent 1", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3 Accent 1", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4 Accent 1", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark Accent 1", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful Accent 1", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful Accent 1", "", "", "52", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light Accent 2", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2 Accent 2", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3 Accent 2", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4 Accent 2", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark Accent 2", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful Accent 2", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful Accent 2", "", "", "52", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light Accent 3", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2 Accent 3", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3 Accent 3", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4 Accent 3", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark Accent 3", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful Accent 3", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful Accent 3", "", "", "52", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light Accent 4", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2 Accent 4", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3 Accent 4", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4 Accent 4", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark Accent 4", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful Accent 4", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful Accent 4", "", "", "52", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light Accent 5", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2 Accent 5", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3 Accent 5", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4 Accent 5", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark Accent 5", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful Accent 5", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful Accent 5", "", "", "52", "");
	Set_lsdException(pSingleTag, "Grid Table 1 Light Accent 6", "", "", "46", "");
	Set_lsdException(pSingleTag, "Grid Table 2 Accent 6", "", "", "47", "");
	Set_lsdException(pSingleTag, "Grid Table 3 Accent 6", "", "", "48", "");
	Set_lsdException(pSingleTag, "Grid Table 4 Accent 6", "", "", "49", "");
	Set_lsdException(pSingleTag, "Grid Table 5 Dark Accent 6", "", "", "50", "");
	Set_lsdException(pSingleTag, "Grid Table 6 Colorful Accent 6", "", "", "51", "");
	Set_lsdException(pSingleTag, "Grid Table 7 Colorful Accent 6", "", "", "52", "");
	Set_lsdException(pSingleTag, "List Table 1 Light", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful", "", "", "52", "");
	Set_lsdException(pSingleTag, "List Table 1 Light Accent 1", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2 Accent 1", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3 Accent 1", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4 Accent 1", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark Accent 1", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful Accent 1", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful Accent 1", "", "", "52", "");

	Set_lsdException(pSingleTag, "List Table 1 Light Accent 2", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2 Accent 2", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3 Accent 2", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4 Accent 2", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark Accent 2", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful Accent 2", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful Accent 2", "", "", "52", "");
	Set_lsdException(pSingleTag, "List Table 1 Light Accent 3", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2 Accent 3", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3 Accent 3", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4 Accent 3", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark Accent 3", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful Accent 3", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful Accent 3", "", "", "52", "");

	Set_lsdException(pSingleTag, "List Table 1 Light Accent 4", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2 Accent 4", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3 Accent 4", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4 Accent 4", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark Accent 4", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful Accent 4", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful Accent 4", "", "", "52", "");
	Set_lsdException(pSingleTag, "List Table 1 Light Accent 5", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2 Accent 5", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3 Accent 5", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4 Accent 5", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark Accent 5", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful Accent 5", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful Accent 5", "", "", "52", "");
	Set_lsdException(pSingleTag, "List Table 1 Light Accent 6", "", "", "46", "");
	Set_lsdException(pSingleTag, "List Table 2 Accent 6", "", "", "47", "");
	Set_lsdException(pSingleTag, "List Table 3 Accent 6", "", "", "48", "");
	Set_lsdException(pSingleTag, "List Table 4 Accent 6", "", "", "49", "");
	Set_lsdException(pSingleTag, "List Table 5 Dark Accent 6", "", "", "50", "");
	Set_lsdException(pSingleTag, "List Table 6 Colorful Accent 6", "", "", "51", "");
	Set_lsdException(pSingleTag, "List Table 7 Colorful Accent 6", "", "", "52", "");

	Set_lsdException(pSingleTag, "Mention", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Smart Hyperlink", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Hashtag", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Unresolved Mention", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Smart Link", "1", "1", "", "");
	Set_lsdException(pSingleTag, "Smart Link Error", "1", "1", "", "");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pSingleTag->AddAttribute("w:default", "1");
	pSingleTag->AddAttribute("w:styleId", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:qFormat");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pSingleTag->AddAttribute("w:styleId", "Heading1");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "heading 1");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:next");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:link");
	pNested1Tag->AddAttribute("w:val", "Heading1Char");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "9");
	pNested1Tag = pSingleTag->AddTag("w:qFormat");
	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:keepNext");
	pNested2Tag = pNested1Tag->AddTag("w:keepLines");
	pNested2Tag = pNested1Tag->AddTag("w:spacing");
	pNested2Tag->AddAttribute("w:after", "0");
	pNested2Tag->AddAttribute("w:before", "240");
	pNested2Tag = pNested1Tag->AddTag("w:outlineLvl");
	pNested2Tag->AddAttribute("w:val", "0");
	pNested1Tag = pSingleTag->AddTag("w:rPr");
	pNested2Tag = pNested1Tag->AddTag("w:rFonts");
	pNested2Tag->AddAttribute("w:asciiTheme", "majorHAnsi");
	pNested2Tag->AddAttribute("w:cstheme", "majorBidi");
	pNested2Tag->AddAttribute("w:eastAsiaTheme", "majorEastAsia");
	pNested2Tag->AddAttribute("w:hAnsiTheme", "majorHAnsi");
	pNested2Tag = pNested1Tag->AddTag("w:color");
	pNested2Tag->AddAttribute("w:themeColor", "accent1");
	pNested2Tag->AddAttribute("w:themeShade", "BF");
	pNested2Tag->AddAttribute("w:val", "2F5496");
	pNested2Tag = pNested1Tag->AddTag("w:sz");
	pNested2Tag->AddAttribute("w:val", "32");
	pNested2Tag = pNested1Tag->AddTag("w:szCs");
	pNested2Tag->AddAttribute("w:val", "32");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pSingleTag->AddAttribute("w:styleId", "Heading2");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "heading 2");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:next");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:link");
	pNested1Tag->AddAttribute("w:val", "Heading2Char");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "9");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");
	pNested1Tag = pSingleTag->AddTag("w:qFormat");
	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:keepNext");
	pNested2Tag = pNested1Tag->AddTag("w:keepLines");
	pNested2Tag = pNested1Tag->AddTag("w:spacing");
	pNested2Tag->AddAttribute("w:after", "0");
	pNested2Tag->AddAttribute("w:before", "40");
	pNested2Tag = pNested1Tag->AddTag("w:outlineLvl");
	pNested2Tag->AddAttribute("w:val", "1");
	pNested1Tag = pSingleTag->AddTag("w:rPr");
	pNested2Tag = pNested1Tag->AddTag("w:rFonts");
	pNested2Tag->AddAttribute("w:asciiTheme", "majorHAnsi");
	pNested2Tag->AddAttribute("w:cstheme", "majorBidi");
	pNested2Tag->AddAttribute("w:eastAsiaTheme", "majorEastAsia");
	pNested2Tag->AddAttribute("w:hAnsiTheme", "majorHAnsi");
	pNested2Tag = pNested1Tag->AddTag("w:color");
	pNested2Tag->AddAttribute("w:themeColor", "accent1");
	pNested2Tag->AddAttribute("w:themeShade", "BF");
	pNested2Tag->AddAttribute("w:val", "2F5496");
	pNested2Tag = pNested1Tag->AddTag("w:sz");
	pNested2Tag->AddAttribute("w:val", "26");
	pNested2Tag = pNested1Tag->AddTag("w:szCs");
	pNested2Tag->AddAttribute("w:val", "26");


	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:type", "character");
	pSingleTag->AddAttribute("w:default", "1");
	pSingleTag->AddAttribute("w:styleId", "DefaultParagraphFont");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Default Paragraph Font");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "1");
	pNested1Tag = pSingleTag->AddTag("w:semiHidden");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:type", "table");
	pSingleTag->AddAttribute("w:default", "1");
	pSingleTag->AddAttribute("w:styleId", "TableNormal");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Normal Table");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");
	pNested1Tag = pSingleTag->AddTag("w:semiHidden");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");

	pNested1Tag = pSingleTag->AddTag("w:tblPr");
	pNested2Tag = pNested1Tag->AddTag("w:tblInd");
	pNested2Tag->AddAttribute("w:w", "0");
	pNested2Tag->AddAttribute("w:type", "dxa");
	pNested2Tag = pNested1Tag->AddTag("w:tblCellMar");
	pNested3Tag = pNested2Tag->AddTag("w:top");
	pNested3Tag->AddAttribute("w:w", "0");
	pNested3Tag->AddAttribute("w:type", "dxa");
	pNested3Tag = pNested2Tag->AddTag("w:left");
	pNested3Tag->AddAttribute("w:w", "108");
	pNested3Tag->AddAttribute("w:type", "dxa");
	pNested3Tag = pNested2Tag->AddTag("w:bottom");
	pNested3Tag->AddAttribute("w:w", "0");
	pNested3Tag->AddAttribute("w:type", "dxa");
	pNested3Tag = pNested2Tag->AddTag("w:right");
	pNested3Tag->AddAttribute("w:w", "108");
	pNested3Tag->AddAttribute("w:type", "dxa");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:type", "numbering");
	pSingleTag->AddAttribute("w:default", "1");
	pSingleTag->AddAttribute("w:styleId", "NoList");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "No List");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");
	pNested1Tag = pSingleTag->AddTag("w:semiHidden");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");


	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "TableGrid");
	pSingleTag->AddAttribute("w:type", "table");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Table Grid");
	pNested1Tag = pSingleTag->AddTag("w:basedOn ");
	pNested1Tag->AddAttribute("w:val", "TableNormal");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "39");

	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:spacing");
	pNested2Tag->AddAttribute("w:lineRule", "auto");
	pNested2Tag->AddAttribute("w:line", "240");
	pNested2Tag->AddAttribute("w:after", "0");

	pNested1Tag = pSingleTag->AddTag("w:tblPr");
	pNested2Tag = pNested1Tag->AddTag("w:tblBorders");
	pNested3Tag = pNested2Tag->AddTag("w:top");
	pNested3Tag->AddAttribute("w:val", "single");
	pNested3Tag->AddAttribute("w:color", "auto");
	pNested3Tag->AddAttribute("w:space", "0");
	pNested3Tag->AddAttribute("w:sz", "4");
	pNested3Tag = pNested2Tag->AddTag("w:left");
	pNested3Tag->AddAttribute("w:val", "single");
	pNested3Tag->AddAttribute("w:color", "auto");
	pNested3Tag->AddAttribute("w:space", "0");
	pNested3Tag->AddAttribute("w:sz", "4");
	pNested3Tag = pNested2Tag->AddTag("w:right");
	pNested3Tag->AddAttribute("w:val", "single");
	pNested3Tag->AddAttribute("w:color", "auto");
	pNested3Tag->AddAttribute("w:space", "0");
	pNested3Tag->AddAttribute("w:sz", "4");
	pNested3Tag = pNested2Tag->AddTag("w:bottom");
	pNested3Tag->AddAttribute("w:val", "single");
	pNested3Tag->AddAttribute("w:color", "auto");
	pNested3Tag->AddAttribute("w:space", "0");
	pNested3Tag->AddAttribute("w:sz", "4");
	pNested3Tag = pNested2Tag->AddTag("w:insideH");
	pNested3Tag->AddAttribute("w:val", "single");
	pNested3Tag->AddAttribute("w:color", "auto");
	pNested3Tag->AddAttribute("w:space", "0");
	pNested3Tag->AddAttribute("w:sz", "4");
	pNested3Tag = pNested2Tag->AddTag("w:insideV");
	pNested3Tag->AddAttribute("w:val", "single");
	pNested3Tag->AddAttribute("w:color", "auto");
	pNested3Tag->AddAttribute("w:space", "0");
	pNested3Tag->AddAttribute("w:sz", "4");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "Header");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "header");
	pNested1Tag = pSingleTag->AddTag("w:basedOn ");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:link  ");
	pNested1Tag->AddAttribute("w:val", "HeaderChar");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");

	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:tabs");
	pNested3Tag = pNested2Tag->AddTag("w:tab");
	pNested3Tag->AddAttribute("w:val", "center");
	pNested3Tag->AddAttribute("w:pos", "4680");
	pNested3Tag = pNested2Tag->AddTag("w:tab");
	pNested3Tag->AddAttribute("w:val", "right");
	pNested3Tag->AddAttribute("w:pos", "9360");

	pNested2Tag = pNested1Tag->AddTag("w:spacing ");
	pNested2Tag->AddAttribute("w:lineRule", "auto");
	pNested2Tag->AddAttribute("w:line", "240");
	pNested2Tag->AddAttribute("w:after", "0");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "HeaderChar");
	pSingleTag->AddAttribute("w:type", "character");
	pSingleTag->AddAttribute("w:customStyle", "1");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Header Char");
	pNested1Tag = pSingleTag->AddTag("w:basedOn ");
	pNested1Tag->AddAttribute("w:val", "DefaultParagraphFont");
	pNested1Tag = pSingleTag->AddTag("w:link  ");
	pNested1Tag->AddAttribute("w:val", "Header");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "Footer");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "footer");
	pNested1Tag = pSingleTag->AddTag("w:basedOn ");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:link  ");
	pNested1Tag->AddAttribute("w:val", "FooterChar");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");

	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:tabs");
	pNested3Tag = pNested2Tag->AddTag("w:tab");
	pNested3Tag->AddAttribute("w:val", "center");
	pNested3Tag->AddAttribute("w:pos", "4680");
	pNested3Tag = pNested2Tag->AddTag("w:tab");
	pNested3Tag->AddAttribute("w:val", "right");
	pNested3Tag->AddAttribute("w:pos", "9360");

	pNested2Tag = pNested1Tag->AddTag("w:spacing ");
	pNested2Tag->AddAttribute("w:lineRule", "auto");
	pNested2Tag->AddAttribute("w:line", "240");
	pNested2Tag->AddAttribute("w:after", "0");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "FooterChar");
	pSingleTag->AddAttribute("w:type", "character");
	pSingleTag->AddAttribute("w:customStyle", "1");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Footer Char");
	pNested1Tag = pSingleTag->AddTag("w:basedOn ");
	pNested1Tag->AddAttribute("w:val", "DefaultParagraphFont");
	pNested1Tag = pSingleTag->AddTag("w:link");
	pNested1Tag->AddAttribute("w:val", "Footer");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:customStyle", "1");
	pSingleTag->AddAttribute("w:styleId", "Heading1Char");
	pSingleTag->AddAttribute("w:type", "character");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Heading 1 Char");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "DefaultParagraphFont");
	pNested1Tag = pSingleTag->AddTag("w:link");
	pNested1Tag->AddAttribute("w:val", "Heading1");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "9");
	pNested1Tag = pSingleTag->AddTag("w:rPr");
	pNested2Tag = pNested1Tag->AddTag("w:rFonts");
	pNested2Tag->AddAttribute("w:asciiTheme", "majorHAnsi");
	pNested2Tag->AddAttribute("w:cstheme", "majorBidi");
	pNested2Tag->AddAttribute("w:eastAsiaTheme", "majorEastAsia");
	pNested2Tag->AddAttribute("w:hAnsiTheme", "majorHAnsi");
	pNested2Tag = pNested1Tag->AddTag("w:color");
	pNested2Tag->AddAttribute("w:themeColor", "accent1");
	pNested2Tag->AddAttribute("w:themeShade", "BF");
	pNested2Tag->AddAttribute("w:val", "2F5496");
	pNested2Tag = pNested1Tag->AddTag("w:sz");
	pNested2Tag->AddAttribute("w:val", "32");
	pNested2Tag = pNested1Tag->AddTag("w:szCs");
	pNested2Tag->AddAttribute("w:val", "32");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:customStyle", "1");
	pSingleTag->AddAttribute("w:styleId", "Heading2Char");
	pSingleTag->AddAttribute("w:type", "character");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Heading 2 Char");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "DefaultParagraphFont");
	pNested1Tag = pSingleTag->AddTag("w:link");
	pNested1Tag->AddAttribute("w:val", "Heading2");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "9");
	pNested1Tag = pSingleTag->AddTag("w:rPr");
	pNested2Tag = pNested1Tag->AddTag("w:rFonts");
	pNested2Tag->AddAttribute("w:asciiTheme", "majorHAnsi");
	pNested2Tag->AddAttribute("w:cstheme", "majorBidi");
	pNested2Tag->AddAttribute("w:eastAsiaTheme", "majorEastAsia");
	pNested2Tag->AddAttribute("w:hAnsiTheme", "majorHAnsi");
	pNested2Tag = pNested1Tag->AddTag("w:color");
	pNested2Tag->AddAttribute("w:themeColor", "accent1");
	pNested2Tag->AddAttribute("w:themeShade", "BF");
	pNested2Tag->AddAttribute("w:val", "2F5496");
	pNested2Tag = pNested1Tag->AddTag("w:sz");
	pNested2Tag->AddAttribute("w:val", "26");
	pNested2Tag = pNested1Tag->AddTag("w:szCs");
	pNested2Tag->AddAttribute("w:val", "26");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "ListParagraph");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "List Paragraph");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "34");
	pNested1Tag = pSingleTag->AddTag("w:qFormat");
	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:ind");
	pNested2Tag->AddAttribute("w:left", "720");
	pNested2Tag = pNested1Tag->AddTag("w:contextualSpacing");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "TOCHeading");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "TOC Heading");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "Heading1");
	pNested1Tag = pSingleTag->AddTag("w:next");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "39");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");
	pNested1Tag = pSingleTag->AddTag("w:qFormat");
	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:outlineLvl");
	pNested2Tag->AddAttribute("w:val", "9");

	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "TOC1");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "toc 1");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:next");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:autoRedefine");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "39");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");
	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:spacing");
	pNested2Tag->AddAttribute("w:after", "100");


	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "TOC2");
	pSingleTag->AddAttribute("w:type", "paragraph");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "toc 2");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:next");
	pNested1Tag->AddAttribute("w:val", "Normal");
	pNested1Tag = pSingleTag->AddTag("w:autoRedefine");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "39");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");
	pNested1Tag = pSingleTag->AddTag("w:pPr");
	pNested2Tag = pNested1Tag->AddTag("w:spacing");
	pNested2Tag->AddAttribute("w:after", "100");
	pNested2Tag = pNested1Tag->AddTag("w:ind");
	pNested2Tag->AddAttribute("w:left", "220");


	pSingleTag = mainTag->AddTag("w:style");
	pSingleTag->AddAttribute("w:styleId", "Hyperlink");
	pSingleTag->AddAttribute("w:type", "character");
	pNested1Tag = pSingleTag->AddTag("w:name");
	pNested1Tag->AddAttribute("w:val", "Hyperlink");
	pNested1Tag = pSingleTag->AddTag("w:basedOn");
	pNested1Tag->AddAttribute("w:val", "DefaultParagraphFont");
	pNested1Tag = pSingleTag->AddTag("w:uiPriority");
	pNested1Tag->AddAttribute("w:val", "99");
	pNested1Tag = pSingleTag->AddTag("w:unhideWhenUsed");
	pNested1Tag = pSingleTag->AddTag("w:rPr");
	pNested2Tag = pNested1Tag->AddTag("w:color");
	pNested2Tag->AddAttribute("w:themeColor", "hyperlink");
	pNested2Tag->AddAttribute("w:val", "0563C1");
	pNested2Tag = pNested1Tag->AddTag("w:u");
	pNested2Tag->AddAttribute("w:val", "single");

}

DocXTag *StyleDocument::Set_lsdException(DocXTag *pTag, string name, string semiHidden, string unhideWhenUsed, string uiPriority, string qFormat)
{
	DocXTag *pNewTag = pTag->AddTag("w:lsdException");
	pNewTag->AddAttribute("w:name", name);
	if (semiHidden != "")
	{
		pNewTag->AddAttribute("w:semiHidden", semiHidden);
	}
	if (unhideWhenUsed != "")
	{
		pNewTag->AddAttribute("w:unhideWhenUsed", unhideWhenUsed);
	}
	if (uiPriority != "")
	{
		pNewTag->AddAttribute("w:uiPriority", uiPriority);
	}
	if (qFormat != "")
	{
		pNewTag->AddAttribute("w:qFormat", qFormat);
	}

	return pNewTag;
}

