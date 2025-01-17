#include "stdafx.h"
#include "TableCell.h"
#include "ParagraphRun.h"
#include "DrawingAnchor.h"
#include "PropertyStructures.h"
#include "DrawingInline.h"

TableCell::TableCell():DocXTag("w:tc")
{
	properties = NULL;
}


TableCell::~TableCell()
{
}


void TableCell::SetProperties()
{
	if (properties == NULL)
	{
		properties = DBG_NEW DocXTag("w:tcPr");
		listTags.push_back(properties);
	}
}

void TableCell::SetTableCellBorders(string color, int space, int sz, string val)
{
	SetProperties();
	DocXTag *pTag, *pTag1;

	pTag = properties->AddTag("w:tcBorders");
	pTag1 = pTag->AddTag("w:top");
	pTag1->AddAttribute("w:color", color);
	pTag1->AddAttribute("w:space", space);
	pTag1->AddAttribute("w:sz", sz);
	pTag1->AddAttribute("w:val", val);

	pTag1 = pTag->AddTag("w:left");
	pTag1->AddAttribute("w:color", color);
	pTag1->AddAttribute("w:space", space);
	pTag1->AddAttribute("w:sz", sz);
	pTag1->AddAttribute("w:val", val);

	pTag1 = pTag->AddTag("w:bottom");
	pTag1->AddAttribute("w:color", color);
	pTag1->AddAttribute("w:space", space);
	pTag1->AddAttribute("w:sz", sz);
	pTag1->AddAttribute("w:val", val);

	pTag1 = pTag->AddTag("w:right");
	pTag1->AddAttribute("w:color", color);
	pTag1->AddAttribute("w:space", space);
	pTag1->AddAttribute("w:sz", sz);
	pTag1->AddAttribute("w:val", val);


}

void TableCell::SetCellWidthAndType(int val, string type)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:tcW");

	pTag->AddAttribute("w:w", val);
	pTag->AddAttribute("w:type", type);
}

void TableCell::SetCellShade(string shade, string themeFillShade, string themeFill, string fill, string color)
{
	SetProperties();
	// w:shd w:val="clear" w:themeFillShade="E6" w:themeFill="background2" w:fill="D0CECE" w:color="auto"
	DocXTag *pTag = properties->AddTag("w:shd");
	pTag->AddAttribute("w:val", shade);
	pTag->AddAttribute("w:themeFillShade", themeFillShade);
	pTag->AddAttribute("w:themeFill", themeFill);
	pTag->AddAttribute("w:fill", fill);
	pTag->AddAttribute("w:color", color);
}

void TableCell::SetHideMark()
{
	SetProperties();
	properties->AddTag("w:hideMark");
}

void TableCell::SetVerticalAlign(string val)
{
	SetProperties();
	DocXTag *pTag = properties->AddTag("w:vAlign");
	pTag->AddAttribute("w:val", val);
}


void TableCell::InsertFloatingPictureFromFile(ParagraphRun *pRun, struct PictureInsertionProperties *pInsProps, struct PictureProperties *pPicProps)
{

	DrawingAnchor *pDrawing = pRun->AddFloatingImage();
	pDrawing->SetSimplePosition(pInsProps->simplePosition.x, pInsProps->simplePosition.y);
	pDrawing->SetRelativeHorizontalPosition(pInsProps->relativeHPos.lVal, pInsProps->relativeHPos.strVal);
	pDrawing->SetRelativeVerticalPosition(pInsProps->relativeVPos.lVal, pInsProps->relativeVPos.strVal);
	pDrawing->SetExtent(pInsProps->extent.cx, pInsProps->extent.cy);
	pDrawing->SetEffectiveExtent(0, 0, 2540, 9525);
	pDrawing->WrapSquare(pInsProps->wrapSquare);
	pDrawing->SetPictureProperties(pPicProps->pictureName, pPicProps->pictureDesc, pPicProps->pictureTitle);
	pDrawing->SetGraphicFrameLockChangeAspect(pInsProps->graphicFrameLockChangeAspect);
	pDrawing->SetPictureName(pPicProps->pictureFileName);
	pDrawing->SetBlip(pPicProps->rID, pPicProps->compressionState, pPicProps->uri);
	if (pInsProps->bFillRect)
		pDrawing->SetBlipStretch("a:fillRect");
	pDrawing->SetShapeTransform(pInsProps->shapeTransform.left, pInsProps->shapeTransform.top, pInsProps->shapeTransform.right, pInsProps->shapeTransform.bottom);
	pDrawing->SetPresetGeometry(pInsProps->presetGeometry, true);
	pDrawing->SetRelativeHorizontalSize(pInsProps->relativeHSize.lVal, pInsProps->relativeHSize.strVal);
	pDrawing->SetRelativeVerticalSize(pInsProps->relativeVSize.lVal, pInsProps->relativeVSize.strVal);
}



