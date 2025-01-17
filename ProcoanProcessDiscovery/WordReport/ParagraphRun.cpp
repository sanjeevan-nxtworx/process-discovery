#include "stdafx.h"
#include "ParagraphRun.h"
#include "DrawingAnchor.h"
#include "DrawingInline.h"
#include "DocXDocPart.h"
#include <algorithm>
#include <cctype>
#include <functional>

ParagraphRun::ParagraphRun() : DocXTag("w:r")
{
	properties = NULL;
}

ParagraphRun::~ParagraphRun()
{
}

void ParagraphRun::SetProperties()
{
	if (properties == NULL)
	{
		properties = DBG_NEW DocXTag("w:rPr");
		listTags.push_back(properties);
	}
}

void ParagraphRun::SetNoProof()
{
	SetProperties();
	properties->AddTag("w:noProof");
}

void ParagraphRun::SetWebHidden()
{
	SetProperties();
	properties->AddTag("w:webHidden");
}

void ParagraphRun::SetBold()
{
	SetProperties();
	properties->AddTag("w:b");
}

void ParagraphRun::SetbCs()
{
	SetProperties();
	properties->AddTag("w:bCs");
}

void ParagraphRun::SetRunStyle(string val)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:rStyle");
	pTag->AddAttribute("w:val", val);
}

DrawingAnchor *ParagraphRun::AddFloatingImage()
{
	DrawingAnchor *pDrawing = DBG_NEW DrawingAnchor();
	listTags.push_back(pDrawing);
	return pDrawing;
}

DrawingInline *ParagraphRun::AddInlineImage()
{
	DrawingInline *pDrawing = DBG_NEW DrawingInline(0,0,0,0);
	listTags.push_back(pDrawing);
	return pDrawing;
}

void ParagraphRun::SetFont(string fontName)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:rFonts");
	pTag->AddAttribute("w:ascii", fontName);
	pTag->AddAttribute("w:hAnsi", fontName);
	pTag->AddAttribute("w:cs", fontName);
}

void ParagraphRun::SetFontTheme(string val)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:rFonts");
	pTag->AddAttribute("w:eastAsiaTheme", val);
}


void ParagraphRun::AddTab(bool bNoProof, bool bWebHidden)
{
	SetProperties();
	if (bNoProof)
		SetNoProof();
	if (bWebHidden)
		SetWebHidden();
	AddTag("w:tab");
}

void ParagraphRun::SetFont(string fontName, string size, bool bCS)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:rFonts");
	pTag->AddAttribute("w:ascii", fontName);
	pTag->AddAttribute("w:hAnsi", fontName);
	if (bCS)
		pTag->AddAttribute("w:cs", fontName);

	pTag = properties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = properties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", size);
}

void ParagraphRun::SetFontSize(int size)
{
	SetProperties();
	DocXTag *pTag = NULL;

	pTag = properties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = properties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", size);
}

void ParagraphRun::SetFont(string fontName, string size, string color, string themeColor, string themeShade)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:rFonts");
	pTag->AddAttribute("w:ascii", fontName);
	pTag->AddAttribute("w:hAnsi", fontName);

	pTag = properties->AddTag("w:color");
	pTag->AddAttribute("w:val", color);
	pTag->AddAttribute("w:themeColor", themeColor);
	pTag->AddAttribute("w:themeShade", themeShade);

	pTag = properties->AddTag("w:sz");
	pTag->AddAttribute("w:val", size);

	pTag = properties->AddTag("w:szCs");
	pTag->AddAttribute("w:val", size);
}

void ParagraphRun::SetFontColor(string color, string themeColor, string themeShade)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:color");
	pTag->AddAttribute("w:val", color);
	pTag->AddAttribute("w:themeColor", themeColor);
	pTag->AddAttribute("w:themeShade", themeShade);
}

void ParagraphRun::SetCSTheme(string val)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:rFonts");
	pTag->AddAttribute("w:cstheme", val);
}

void ParagraphRun::SetText(string text, bool bPreserve = false)
{
	DocXTag *pTag = AddTag("w:t");
	if (bPreserve)
	{
		pTag->AddAttribute("xml:space", "preserve");
	}
	UpdateMetrics(text);
	pTag->SetValue(text);
}


void ParagraphRun::UpdateMetrics(string text)
{
	DocXDocPart::noCharactersWithSpaces += text.size();

	int spaces = std::count_if(text.begin(), text.end(),
		[](unsigned char c) { return std::isspace(c); });
	DocXDocPart::noCharacters += (text.size() - spaces);

	size_t words = 1;
	if (spaces != 0)
	{
		words = ((size_t)std::count_if(text.begin(), text.end(), std::ptr_fun <int, int>(std::isspace)) == text.length())
			? 0
			: std::count_if(
				std::find_if(
					text.begin(),
					text.end(),
					std::not1(std::ptr_fun <int, int>(std::isspace))
				),
				std::unique(
					text.begin(),
					std::replace_copy_if(
						text.begin(),
						text.end(),
						text.begin(),
						std::ptr_fun <int, int>(std::isspace),
						' '
					)
				),
				std::ptr_fun <int, int>(std::isspace)
			)
			+ !std::isspace(*text.rbegin());
	}
	DocXDocPart::noWords += words;
}

void ParagraphRun::SetSeperator()
{
	DocXTag *pTag = DBG_NEW DocXTag("w:separator");
	listTags.push_back(pTag);
}

void ParagraphRun::SetContinuationSeperator()
{
	DocXTag *pTag = DBG_NEW DocXTag("w:continuationSeparator");
	listTags.push_back(pTag);
}
