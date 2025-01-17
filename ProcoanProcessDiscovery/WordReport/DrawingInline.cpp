#include "stdafx.h"
#include "DrawingInline.h"
#include "DocXDocPart.h"

DrawingInline::DrawingInline(int distL, int distT, int distR, int distB):DocXTag("w:drawing")
{
	pInline = AddTag("wp:inline");
	char str[100];
	DocXDocPart::paraID++;
	sprintf_s(str, "%08X", DocXDocPart::paraID);
	pInline->AddAttribute("wp14:editId", str);
	DocXDocPart::paraID++;
	sprintf_s(str, "%08X", DocXDocPart::paraID);
	pInline->AddAttribute("wp14:anchorId", str);
	pInline->AddAttribute("distR", distR);
	pInline->AddAttribute("distL", distL);
	pInline->AddAttribute("distB", distB);
	pInline->AddAttribute("distT", distT);

	pGraphicFrameLockProperties = NULL;
	pNvGraphicFrameProperties = NULL;

	pGraphicProperties = NULL;
	pGraphicDataProperties = NULL;
	pPicProperties = NULL;
	pNvPicProperties = NULL;
	pcNvPicProperties = NULL;

	pBlipFill = NULL;
	pShapeProperties = NULL;
}


DrawingInline::~DrawingInline()
{
}

void DrawingInline::SetExtent(long cx, long cy)
{
	DocXTag *pTag = pInline->AddTag("wp:extent");

	pTag->AddAttribute("cx", cx);
	pTag->AddAttribute("cy", cy);
}

void DrawingInline::SetEffectiveExtent(long l, long t, long r, long b)
{
	DocXTag *pTag = pInline->AddTag("wp:effectExtent");

	pTag->AddAttribute("l", l);
	pTag->AddAttribute("t", t);
	pTag->AddAttribute("r", r);
	pTag->AddAttribute("b", b);
}

void DrawingInline::SetNvGraphicFrameProperties()
{
	if (pNvGraphicFrameProperties == NULL)
	{
		pNvGraphicFrameProperties = pInline->AddTag("wp:cNvGraphicFramePr");
	}
}

void DrawingInline::SetGraphicFrameLockProperties()
{
	SetNvGraphicFrameProperties();
	if (pGraphicFrameLockProperties == NULL)
	{
		pGraphicFrameLockProperties = pNvGraphicFrameProperties->AddTag("a:graphicFrameLocks");
		pGraphicFrameLockProperties->AddAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main");
	}
}

void DrawingInline::SetGraphicFrameLockChangeAspect(string val)
{
	SetNvGraphicFrameProperties();
	SetGraphicFrameLockProperties();
	pGraphicFrameLockProperties->AddAttribute("noChangeAspect", val);
}

void DrawingInline::SetPictureProperties(string name, string desc, string wrapText)
{
	DocXTag *pTag = pInline->AddTag("wp:docPr");
	DocXDocPart::pictureID++;
	pTag->AddAttribute("id", DocXDocPart::pictureID);
	pTag->AddAttribute("name", name);
	pTag->AddAttribute("descr", desc);
	//pTag->AddAttribute("wrapText", wrapText);

}

void DrawingInline::SetGraphicProperties()
{
	if (pGraphicProperties == NULL)
	{
		pGraphicProperties = pInline->AddTag("a:graphic");
		pGraphicProperties->AddAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main");
	}
}


void DrawingInline::SetGraphicDataProperties()
{
	SetGraphicProperties();
	if (pGraphicDataProperties == NULL)
	{
		pGraphicDataProperties = pGraphicProperties->AddTag("a:graphicData");
		pGraphicDataProperties->AddAttribute("uri", "http://schemas.openxmlformats.org/drawingml/2006/picture");
	}
}

void DrawingInline::SetNvPicProperties()
{
	SetPicProperties();
	if (pNvPicProperties == NULL)
	{
		pNvPicProperties = pPicProperties->AddTag("pic:nvPicPr");
	}
}


void DrawingInline::SetPictureName(string name)
{
	SetNvPicProperties();
	DocXTag *pTag = pNvPicProperties->AddTag("pic:cNvPr");

	DocXDocPart::pictureID++;
	pTag->AddAttribute("id", DocXDocPart::pictureID);
	pTag->AddAttribute("name", name);
	pcNvPicProperties = pNvPicProperties->AddTag("pic:cNvPicPr");

}

void DrawingInline::SetBlip(string strId, string link)
{
	SetBlipFill();
	DocXTag *pTag = pBlipFill->AddTag("a:blip");

	pTag->AddAttribute("r:embed", strId);
	if (link != "")
	{
		pTag->AddAttribute("link", link);
	}
}

void DrawingInline::SetPicProperties()
{
	SetGraphicDataProperties();
	if (pPicProperties == NULL)
	{
		pPicProperties = pGraphicDataProperties->AddTag("pic:pic");
		pPicProperties->AddAttribute("xmlns:pic", "http://schemas.openxmlformats.org/drawingml/2006/picture");
	}
}

void DrawingInline::SetBlipFill()
{
	SetPicProperties();
	if (pBlipFill == NULL)
	{
		pBlipFill = pPicProperties->AddTag("pic:blipFill");
	}
}

void DrawingInline::SetBlipExt(string strId, string link, string useLocalDpi)
{
	SetBlipFill();
	DocXTag *pTag = pBlipFill->AddTag("a:blip");

	pTag->AddAttribute("r:embed", strId);
	
	if (link != "")
	{
		DocXTag *pTag1 = pTag->AddTag("a:extLst");
		DocXTag *pTag2 = pTag1->AddTag("a:ext");
		pTag2->AddAttribute("uri", link);

		DocXTag *pTag3 = pTag2->AddTag("a14:useLocalDpi");
		pTag3->AddAttribute("xmlns:a14", "http://schemas.microsoft.com/office/drawing/2010/main");
		pTag3->AddAttribute("val", useLocalDpi);
	}
}


void DrawingInline::SetBlipStretch(string valTag)
{
	SetBlipFill();
	DocXTag *pTag = pBlipFill->AddTag("a:stretch");
	pTag->AddTag(valTag);
}


void DrawingInline::SetShapeProperties()
{
	SetPicProperties();
	if (pShapeProperties == NULL)
	{
		pShapeProperties = pPicProperties->AddTag("pic:spPr");
	}
}

void DrawingInline::SetShapeTransform(long offsetX, long offsetY, long cx, long cy)
{
	SetShapeProperties();
	DocXTag *pTag = pShapeProperties->AddTag("a:xfrm");

	DocXTag *pTag1 = pTag->AddTag("a:off");

	pTag1->AddAttribute("x", offsetX);
	pTag1->AddAttribute("y", offsetY);

	pTag1 = pTag->AddTag("a:ext");
	pTag1->AddAttribute("cx", cx);
	pTag1->AddAttribute("cy", cy);
}


void DrawingInline::SetPresetGeometry(string val, bool bShapeAdjust)
{
	SetShapeProperties();
	DocXTag *pTag = pShapeProperties->AddTag("a:prstGeom");
	pTag->AddAttribute("prst", val);

	if (bShapeAdjust)
	{
		pTag->AddTag("a:avLst");
	}
}

void DrawingInline::SetSolidFillLine(string schemeClr)
{
	SetShapeProperties();
	DocXTag *pTag = pShapeProperties->AddTag("a:ln");
	DocXTag *pTag1 = pTag->AddTag("a:solidFill");
	DocXTag *pTag2 = pTag1->AddTag("a:schemeClr");
	
	pTag2->AddAttribute("val", schemeClr);
}

void DrawingInline::SetEffect(string align, string blurRad, string dir, string dist, string rotWithShape, string prstClr, string alpha)
{
	DocXTag *pTag = pShapeProperties->AddTag("a:effectLst");
	DocXTag *pTag1 = pTag->AddTag("a:outerShdw");
	pTag1->AddAttribute("algn", align);
	pTag1->AddAttribute("blurRad", blurRad);
	pTag1->AddAttribute("dir", dir);
	pTag1->AddAttribute("dist", dist);
	pTag1->AddAttribute("rotWithShape", rotWithShape);

	DocXTag *pTag2 = pTag1->AddTag("a:prstClr");
	pTag2->AddAttribute("val", prstClr);

	DocXTag *pTag3 = pTag2->AddTag("a:alpha");
	pTag3->AddAttribute("val", alpha);
}
