#include "stdafx.h"
#include "TOCsdt.h"
#include "Paragraph.h"
#include "ParagraphRun.h"

TOCsdt::TOCsdt():DocXTag("w:sdt")
{
	pSdtContent = NULL;
	contentStyle = "";
	lefttabPos = 0;
	lefttabLeader = "";
	righttabPos = 0;
	righttabLeader = "";
	fontTheme = "";
	tocStyle = "";
	hyperlinkFont = "";
}


TOCsdt::~TOCsdt()
{
}


void TOCsdt::SetStandardProperties()
{
	DocXTag *pTag = AddTag("w:sdtPr");
	//DocXTag *pTag1 = pTag->AddTag("w:rPr");
	//DocXTag *pTag2 = pTag1->AddTag("w:rFonts");
	//pTag2->AddAttribute("w:cstheme", "minorBidi");
	//pTag2->AddAttribute("w:hAnsiTheme", "minorHAnsi");
	//pTag2->AddAttribute("w:eastAsiaTheme", "minorHAnsi");
	//pTag2->AddAttribute("w:asciiTheme", "minorHAnsi");

	//pTag2 = pTag1->AddTag("w:color");
	//pTag2->AddAttribute("w:val", "auto");
	//pTag2 = pTag1->AddTag("w:sz");
	//pTag2->AddAttribute("w:val", "22");
	//pTag2 = pTag1->AddTag("w:szCs");
	//pTag2->AddAttribute("w:val", "22");

	DocXTag * pTag1 = pTag->AddTag("w:id");
	pTag1->AddAttribute("w:val", "-1967273602");

	pTag1 = pTag->AddTag("w:docPartObj");
	DocXTag *pTag2 = pTag1->AddTag("w:docPartGallery");
	pTag2->AddAttribute("w:val", "Table of Contents");
	pTag2 = pTag1->AddTag("w:docPartUnique");

	pTag = AddTag("w:sdtEndPr");
	pTag1 = pTag->AddTag("w:rPr");
	pTag2 = pTag1->AddTag("w:rFonts");
	pTag2->AddAttribute("w:cstheme", "minorBidi");
	pTag2->AddAttribute("w:hAnsiTheme", "minorHAnsi");
	pTag2->AddAttribute("w:eastAsiaTheme", "minorHAnsi");
	pTag2->AddAttribute("w:asciiTheme", "minorHAnsi");
	pTag2 = pTag1->AddTag("w:color");
	pTag2->AddAttribute("w:val", "auto");
	pTag2 = pTag1->AddTag("w:sz");
	pTag2->AddAttribute("w:val", "22");
	pTag2 = pTag1->AddTag("w:szCs");
	pTag2->AddAttribute("w:val", "22");
	pTag2 = pTag1->AddTag("w:b");
	pTag2 = pTag1->AddTag("w:bCs");
	pTag2 = pTag1->AddTag("w:noProof");
}

void TOCsdt::SetStandardTocContentStart()
{
	pSdtContent = AddTag("w:sdtContent");

	Paragraph *pPara = pSdtContent->AddParagraph();

	pPara->SetStyle(contentStyle);
	pPara->SetNumberProperties(0, 0);
	pPara->SetLeftIndent(432);
	
	ParagraphRun *pParaRun = pPara->AddRun();
	pParaRun->SetText("Contents", false);
}

void TOCsdt::SetContentStyle(string val)
{
	contentStyle = val;
}

void TOCsdt::SetContentEnd()
{
	Paragraph *para = NULL;
	para = pSdtContent->AddParagraph();
	ParagraphRun *pRun = para->AddParagraphRun();
	pRun->SetBold();
	pRun->SetbCs();
	pRun->SetNoProof();

	DocXTag *pTag = pRun->AddTag("w:fldChar");
	pTag->AddAttribute("w:fldCharType", "end");
}

void TOCsdt::SetLeftTabProperties(int pos, string leader)
{
	lefttabPos = pos;
	lefttabLeader = leader;

}

void TOCsdt::SetRightTabProperties(int pos, string leader)
{
	righttabPos = pos;
	righttabLeader = leader;
}

void TOCsdt::SetFontTheme(string val)
{
	fontTheme = val;
}

void TOCsdt::SetTOCStyle(string val)
{
	tocStyle = val;
}

void TOCsdt::SetHyperlinkFont(string val)
{
	hyperlinkFont = val;
}

void TOCsdt::SetHyperlink(string text, string anchor, string listNumber, int pageNo, bool bFirst)
{
	Paragraph *pPara = pSdtContent->AddParagraph();
	DocXTag *pTag;

	pPara->SetStyle(tocStyle);
	pPara->SetTabs("left", lefttabPos, lefttabLeader);
	pPara->SetTabs("right", righttabPos, righttabLeader);
	pPara->SetFontTheme(fontTheme);
	pPara->SetNoProof();
	if (bFirst)
		pPara->AddFldChar(" TOC \\o \"1-3\" \\h \\z \\u ", true);
	pTag = pPara->AddTag("w:hyperlink");
	pTag->AddAttribute("w:history", "1");
	pTag->AddAttribute("w:anchor", anchor);
	
	ParagraphRun *pRun = pTag->AddParagraphRun();
	pRun->SetRunStyle("Hyperlink");
	//pRun->SetFont("Arial");
	pRun->SetNoProof();
	pRun->SetText(listNumber, false);

	pRun = pTag->AddParagraphRun();
	pRun->SetFontTheme(fontTheme);
	pRun->SetNoProof();
	pRun->AddTab();

	
	pRun = pTag->AddParagraphRun();
	pRun->SetRunStyle("Hyperlink");
	pRun->SetFont("Arial");
	pRun->SetNoProof();
	pRun->SetText(text, false);

	pRun = pTag->AddParagraphRun();
	pRun->SetNoProof();
	pRun->SetWebHidden();
	pRun->AddTab();

	string val = " PAGEREF " + anchor + " \\h ";
	pTag->AddFldChar(val, true, true, true);
	pTag->AddFldValue(pageNo, true, true);
	pTag->EndFldChar(true, true);

}
