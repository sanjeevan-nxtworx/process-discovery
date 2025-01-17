#include "stdafx.h"
#include "Paragraph.h"
#include "DocXDocPart.h"
#include "ParagraphRun.h"

Paragraph::Paragraph():DocXTag("w:p")
{
	char str[100];

	DocXDocPart::paraID++;
	sprintf_s(str, "%08X", DocXDocPart::paraID);
	AddAttribute("w14:paraId", str);
	AddAttribute("w14:textId", "77777777");
	AddAttribute("w:rsidR", DocXDocPart::str_rsidR);
	AddAttribute("w:rsidRDefault", DocXDocPart::str_rsidR);
	AddAttribute("w:rsidP", DocXDocPart::str_rsidR);

	properties = NULL;
	runProperties = NULL;
	numProperties = NULL;
	tabProperties = NULL;
	DocXDocPart::noParagraphs++;
}

void Paragraph::SetNumProperties()
{
	SetProperties();
	if (numProperties == NULL)
	{
		numProperties = properties->AddTag("w:numPr");
	}
}

void Paragraph::SetProperties()
{
	if (properties == NULL)
	{
		properties = DBG_NEW DocXTag("w:pPr");
		listTags.push_back(properties);
	}
}

void Paragraph::SetRunProperties()
{
	SetProperties();
	if (runProperties == NULL)
	{
		runProperties = properties->AddTag("w:rPr");
	}
}

void Paragraph::SetTabProperties()
{
	SetProperties();
	if (tabProperties == NULL)
	{
		tabProperties = properties->AddTag("w:tabs");
	}
}


Paragraph::~Paragraph()
{
}

ParagraphRun* Paragraph::AddRun()
{
	ParagraphRun *pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);

	return pRun;
}

void Paragraph::SetFont(string fontName, string size, bool bCS)
{
	SetRunProperties();
	DocXTag *pTag = NULL;
	
	if (fontName != "")
	{
		pTag = runProperties->AddTag("w:rFonts");
		pTag->AddAttribute("w:ascii", fontName);
		pTag->AddAttribute("w:hAnsi", fontName);
		if (bCS)
			pTag->AddAttribute("w:cs", fontName);
	}
	pTag = runProperties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = runProperties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", size);
}

void Paragraph::SetFont(string fontName, int size, int csSize)
{
	SetRunProperties();
	DocXTag *pTag = NULL;

	if (fontName != "")
	{
		pTag = runProperties->AddTag("w:rFonts");
		pTag->AddAttribute("w:ascii", fontName);
		pTag->AddAttribute("w:hAnsi", fontName);
	}
	pTag = runProperties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = runProperties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", csSize);
}

void Paragraph::SetFontTheme(string val)
{
	SetRunProperties();
	DocXTag *pTag = runProperties->AddTag("w:rFonts");
	pTag->AddAttribute("w:eastAsiaTheme", val);
}

void Paragraph::SetFontTheme(string asciiTheme, string csTheme, string eastAsiaTheme, string ansiTheme)
{
	SetRunProperties();
	DocXTag *pTag = runProperties->AddTag("w:rFonts");
	pTag->AddAttribute("w:asciiTheme", asciiTheme);
	pTag->AddAttribute("w:cstheme", csTheme);
	pTag->AddAttribute("w:eastAsiaTheme", eastAsiaTheme);
	pTag->AddAttribute("w:hAnsiTheme", ansiTheme);
}

void Paragraph::SetFontThemeColor(string themeColor, string themeShade, string colorVal)
{
	SetRunProperties();
	DocXTag *pTag = runProperties->AddTag("w:color");
	pTag->AddAttribute("w:themeColor", themeColor);
	pTag->AddAttribute("w:themeShade", themeShade);
	pTag->AddAttribute("w:val", colorVal);
}


void Paragraph::SetFontSize(int size)
{
	SetRunProperties();
	DocXTag *pTag = NULL;

	pTag = runProperties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = runProperties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", size);
}


void Paragraph::SetFont(string fontName, string size, string color, string themeColor, string themeShade)
{
	SetRunProperties();
	DocXTag *pTag = runProperties->AddTag("w:rFonts");
	pTag->AddAttribute("w:ascii", fontName);
	pTag->AddAttribute("w:hAnsi", fontName);

	pTag = runProperties->AddTag("w:color");
	pTag->AddAttribute("w:val", color);
	pTag->AddAttribute("w:themeColor", themeColor);
	pTag->AddAttribute("w:themeShade", themeShade);

	pTag = runProperties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = runProperties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", size);
}

void Paragraph::SetFontColor(string color, string themeColor, string themeShade)
{
	SetRunProperties();
	DocXTag *pTag = runProperties->AddTag("w:color");
	pTag->AddAttribute("w:val", color);
	pTag->AddAttribute("w:themeColor", themeColor);
	pTag->AddAttribute("w:themeShade", themeShade);
}


void Paragraph::SetTabs(string tabType, int position)
{
	SetTabProperties();
	DocXTag *pTag = tabProperties->AddTag("w:tab");
	pTag->AddAttribute("w:val", tabType);
	pTag->AddAttribute("w:pos", position);
}

void Paragraph::SetTabs(string tabType, int position, string leader)
{
	SetTabProperties();
	DocXTag *pTag = tabProperties->AddTag("w:tab");
	pTag->AddAttribute("w:val", tabType);
	pTag->AddAttribute("w:pos", position);
	if (leader != "")
		pTag->AddAttribute("w:leader", leader);
}

void Paragraph::SetJustification(string type)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:jc");
	pTag->AddAttribute("w:val", type);
}

void Paragraph::SetSpacing(string type, int line, int after)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:spacing ");
	if (after != 0)
		pTag->AddAttribute("w:after", after);
	pTag->AddAttribute("w:line", line);
	pTag->AddAttribute("w:lineRule", type);
}

void Paragraph::SetStyle(string val)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:pStyle ");
	pTag->AddAttribute("w:val", val);
}

void Paragraph::AddPageBreak()
{
	ParagraphRun *pRun = AddRun();

	DocXTag *pTag = pRun->AddTag("w:br");
	pTag->AddAttribute("w:type", "page");

}

void Paragraph::StartProof()
{
	DocXTag *pTag = AddTag("w:proofErr ");
	pTag->AddAttribute("w:type", "spellStart");
}

void Paragraph::EndProof()
{
	DocXTag *pTag = AddTag("w:proofErr ");
	pTag->AddAttribute("w:type", "spellEnd");
}

void Paragraph::SetNoProof()
{
	runProperties->AddTag("w:noProof");
}

void Paragraph::SetNumberProperties(int numID, int level)
{
	SetNumProperties();
	DocXTag *pTag = numProperties->AddTag("w:ilvl");
	pTag->AddAttribute("w:val", level);
	pTag = numProperties->AddTag("w:numId");
	pTag->AddAttribute("w:val", numID);
}

void Paragraph::SetLeftIndent(int left, int hanging)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:ind ");
	pTag->AddAttribute("w:left", left);
	if (hanging != 0)
	{
		pTag->AddAttribute("w:hanging", hanging);
	}
}

void Paragraph::SetFont(string fontName)
{
	SetRunProperties();
	DocXTag *pTag = runProperties->AddTag("w:rFonts");
	pTag->AddAttribute("w:cs", fontName);
	pTag->AddAttribute("w:hAnsi", fontName);
	pTag->AddAttribute("w:ascii", fontName);

}

void Paragraph::SetPageBreak()
{
	ParagraphRun *pRun = AddParagraphRun();
	DocXTag *pTag = pRun->AddTag("w:br");
	pTag->AddAttribute("w:type", "page");
	DocXDocPart::pageNumber++;
}



