#include "stdafx.h"
#include "DocXTag.h"
#include "Table.h"
#include "Paragraph.h"
#include "ParagraphRun.h"
#include "DrawingInline.h"
#include "PropertyStructures.h"

DocXTag::DocXTag(string name)
{
	tagName = name;
	listAttributes.clear();
	listTags.clear();
	bFullTag = false;
}

DocXTag::DocXTag(string name, bool fullTag)
{
	tagName = name;
	listAttributes.clear();
	listTags.clear();
	bFullTag = fullTag;
}

DocXTag::DocXTag(string name, string value)
{
	tagName = name;
	tagValue = value;
}


DocXTag::~DocXTag()
{
	list <DocXTag *>::iterator itTags = listTags.begin();
	while (itTags != listTags.end())
	{
		DocXTag *pTag = *itTags;
		delete pTag;
		itTags++;
	}
	listTags.clear();

	list <DocXTagAttributes *>::iterator itAttr = listAttributes.begin();
	while (itAttr != listAttributes.end())
	{
		DocXTagAttributes *pAttribute = *itAttr;
		delete pAttribute;
		itAttr++;
	}
	listAttributes.clear();
}

void DocXTag::AddAttribute(string key, string value)
{
	DocXTagAttributes *pAttribute = DBG_NEW DocXTagAttributes(key, value);
	listAttributes.push_back(pAttribute);
}

void DocXTag::AddAttribute(string key, int value)
{
	char str[100];

	sprintf_s(str, "%d", value);
	DocXTagAttributes *pAttribute = DBG_NEW DocXTagAttributes(key, str);

	listAttributes.push_back(pAttribute);
}

void DocXTag::AddAttribute(string key, long value)
{
	char str[100];

	sprintf_s(str, "%ld", value);
	DocXTagAttributes *pAttribute = DBG_NEW DocXTagAttributes(key, str);

	listAttributes.push_back(pAttribute);
}

DocXTag *DocXTag::AddTag(string name)
{
	DocXTag *pTag = DBG_NEW DocXTag(name);
	listTags.push_back(pTag);
	return pTag;
}

DocXTag *DocXTag::AddTag(DocXTag *pTag)
{
	listTags.push_back(pTag);
	return pTag;
}

DocXTag *DocXTag::AddTag(string name, bool fullTag)
{
	DocXTag *pTag = DBG_NEW DocXTag(name, fullTag);
	listTags.push_back(pTag);
	return pTag;
}

Table *DocXTag::AddTable()
{
	Table *pTable = DBG_NEW Table();
	listTags.push_back(pTable);
	return pTable;
}

Paragraph *DocXTag::AddParagraph()
{
	Paragraph *pParagraph = DBG_NEW Paragraph();
	listTags.push_back(pParagraph);
	return pParagraph;
}

ParagraphRun *DocXTag::AddParagraphRun()
{
	ParagraphRun *pParaRun = DBG_NEW ParagraphRun();
	listTags.push_back(pParaRun);
	return pParaRun;
}


void DocXTag::SetValue(string value)
{
	tagValue = value;
}

void DocXTag::SetBookmarkStart(string id, string name)
{
	DocXTag *pTag = AddTag("w:bookmarkStart");
	pTag->AddAttribute("w:id", id);
	pTag->AddAttribute("w:name", name);
}

void DocXTag::SetBookmarkEnd(string id)
{
	DocXTag *pTag = AddTag("w:bookmarkEnd");
	pTag->AddAttribute("w:id", id);
}


string DocXTag::GetTagString()
{
	string strVal = "<" + tagName;
	if (listAttributes.size() > 0)
	{
		list <DocXTagAttributes *>::iterator itAttr = listAttributes.begin();
		while (itAttr != listAttributes.end())
		{
			DocXTagAttributes *pAttribute = *itAttr;
			strVal = strVal + " " + pAttribute->GetKey() + "=\"" + pAttribute->GetValue() + "\"";
			itAttr++;
		}
	}

	if (listTags.size() == 0 && tagValue.size() == 0 && !bFullTag)
	{
		strVal += "/>";
	}
	else
	{
		strVal += ">";
		if (tagValue.size() > 0)
			strVal += tagValue;

		list <DocXTag *>::iterator itTags = listTags.begin();
		while (itTags != listTags.end())
		{
			DocXTag *pTag = *itTags;
			strVal += pTag->GetTagString();
			itTags++;
		}
		strVal = strVal + "</" + tagName + ">";
	}

	return strVal;
}

void DocXTag::InsertInlinePictureFromFile(ParagraphRun *pRun, struct PictureInsertionProperties *pInsProps, struct PictureProperties *pPicProps)
{
	DrawingInline *pDrawing = pRun->AddInlineImage();
	pDrawing->SetExtent(pInsProps->extent.cx, pInsProps->extent.cy);
	pDrawing->SetEffectiveExtent(pInsProps->effectiveExtent.left, pInsProps->effectiveExtent.top, pInsProps->effectiveExtent.right, pInsProps->effectiveExtent.bottom);
	pDrawing->SetPictureProperties(pPicProps->pictureName, pPicProps->pictureDesc, pPicProps->pictureTitle);
	pDrawing->SetGraphicFrameLockChangeAspect(pInsProps->graphicFrameLockChangeAspect);
	pDrawing->SetPictureName(pPicProps->pictureFileName);
	pDrawing->SetBlipExt(pPicProps->rID, pPicProps->uri, "0");
	if (pInsProps->bFillRect)
		pDrawing->SetBlipStretch("a:fillRect");
	pDrawing->SetShapeTransform(pInsProps->shapeTransform.left, pInsProps->shapeTransform.top, pInsProps->shapeTransform.right, pInsProps->shapeTransform.bottom);
	pDrawing->SetPresetGeometry(pInsProps->presetGeometry, true);
	pDrawing->SetSolidFillLine("accent1");
	pDrawing->SetEffect("tl", "50800", "2700000", "38100", "0", "black", "40000");

}

void DocXTag::AddFldChar(string text, bool bPreserve, bool bNoProof, bool bWebHidden)
{
	ParagraphRun *pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);

	if (bNoProof)
		pRun->SetNoProof();
	if (bWebHidden)
		pRun->SetWebHidden();

	DocXTag *pTag = pRun->AddTag("w:fldChar");
	pTag->AddAttribute("w:fldCharType", "begin");

	pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);
	if (bNoProof)
		pRun->SetNoProof();
	if (bWebHidden)
		pRun->SetWebHidden();
	pTag = pRun->AddTag("w:instrText");
	if (bPreserve)
	{
		pTag->AddAttribute("xml:space", "preserve");
	}
	pTag->SetValue(text);

	//pRun = new ParagraphRun();
	//listTags.push_back(pRun);
	//pTag = pRun->AddTag("w:instrText");
	//if (bPreserve)
	//{
	//	pTag->AddAttribute("xml:space", "preserve");
	//}

	pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);
	if (bNoProof)
		pRun->SetNoProof();
	if (bWebHidden)
		pRun->SetWebHidden();

	pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);
	if (bNoProof)
		pRun->SetNoProof();
	if (bWebHidden)
		pRun->SetWebHidden();
	pTag = pRun->AddTag("w:fldChar");
	pTag->AddAttribute("w:fldCharType", "separate");
}

void DocXTag::AddFldValue(int val, bool bNoProof, bool bWebHidden)
{
	ParagraphRun *pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);
	if (bNoProof)
		pRun->SetNoProof();
	if (bWebHidden)
		pRun->SetWebHidden();
	char str[100];

	sprintf_s(str, "%d", val);
	pRun->SetText(str, false);

}

void DocXTag::EndFldChar(bool bNoProof, bool bWebHidden)
{
	ParagraphRun *pRun = DBG_NEW ParagraphRun();
	listTags.push_back(pRun);
	if (bNoProof)
		pRun->SetNoProof();
	if (bWebHidden)
		pRun->SetWebHidden();

	DocXTag *pTag = pRun->AddTag("w:fldChar");
	pTag->AddAttribute("w:fldCharType", "end");
}


