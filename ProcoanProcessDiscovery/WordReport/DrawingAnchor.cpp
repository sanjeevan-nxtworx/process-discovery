#include "stdafx.h"
#include "DrawingAnchor.h"
#include "DocXDocPart.h"

DrawingAnchor::DrawingAnchor():DocXTag("w:drawing")
{
	pAnchor = AddTag("wp:anchor");
	pAnchor->AddAttribute("relativeHeight", "251659264");
	pAnchor->AddAttribute("simplePos", "0");
	pAnchor->AddAttribute("behindDoc", "0");
	pAnchor->AddAttribute("locked", "0");
	pAnchor->AddAttribute("allowOverlap", "1");

	pGraphicFrameLockProperties = NULL;
	pNvGraphicFrameProperties = NULL;

	pGraphicProperties = NULL;
	pGraphicDataProperties = NULL;
	pPicProperties = NULL;
	pNvPicProperties = NULL;
	pcNvPicProperties = NULL;

	pBlipFill = NULL;
	pShapeProperties = NULL;

	char str[100];
	DocXDocPart::paraID++;
	sprintf_s(str, "%08X", DocXDocPart::paraID);
	AddAttribute("w14:anchorId", str);
	DocXDocPart::paraID++;
	sprintf_s(str, "%08X", DocXDocPart::paraID);
	AddAttribute("w14:editId", str);
}


DrawingAnchor::~DrawingAnchor()
{
}

void DrawingAnchor::SetGraphicProperties()
{
	if (pGraphicProperties == NULL)
	{
		pGraphicProperties = DBG_NEW DocXTag("a:graphic");
		listTags.push_back(pGraphicProperties);
		pGraphicProperties->AddAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main");
	}
}

void DrawingAnchor::SetShapeProperties()
{
	SetPicProperties();
	if (pShapeProperties == NULL)
	{
		pShapeProperties = pPicProperties->AddTag("pic:spPr");
	}
}

void DrawingAnchor::SetGraphicDataProperties()
{
	SetGraphicProperties();
	if (pGraphicDataProperties == NULL)
	{
		pGraphicDataProperties = pGraphicProperties->AddTag("a:graphicData");
		pGraphicDataProperties->AddAttribute("uri", "http://schemas.openxmlformats.org/drawingml/2006/picture");
	}
}

void DrawingAnchor::SetPicProperties()
{
	SetGraphicDataProperties();
	if (pPicProperties == NULL)
	{
		pPicProperties = pGraphicDataProperties->AddTag("pic:pic");
		pPicProperties->AddAttribute("xmlns:pic", "http://schemas.openxmlformats.org/drawingml/2006/picture");
	}
}

void DrawingAnchor::SetNvPicProperties()
{
	SetPicProperties();
	if (pNvPicProperties == NULL)
	{
		pNvPicProperties = pGraphicDataProperties->AddTag("pic:nvPicPr");
		pcNvPicProperties = pNvPicProperties->AddTag("pic:cNvPicPr");
	}
}

void DrawingAnchor::SetCNvPicProperties()
{
	SetNvPicProperties();
	if (pcNvPicProperties == NULL)
	{
		pcNvPicProperties = pNvPicProperties->AddTag("pic:cNvPicPr");
	}
}

void DrawingAnchor::SetBlipFill()
{
	SetPicProperties();
	if (pBlipFill == NULL)
	{
		pBlipFill = pPicProperties->AddTag("a:blip");
	}
}
void DrawingAnchor::SetNvGraphicFrameProperties()
{
	if (pNvGraphicFrameProperties == NULL)
	{
		pNvGraphicFrameProperties = DBG_NEW DocXTag("wp:cNvGraphicFramePr");
		listTags.push_back(pNvGraphicFrameProperties);
	}
}

void DrawingAnchor::SetGraphicFrameLockProperties()
{
	SetNvGraphicFrameProperties();
	if (pGraphicFrameLockProperties == NULL)
	{
		pGraphicFrameLockProperties = pNvGraphicFrameProperties->AddTag("a:graphicFrameLocks");
		pGraphicFrameLockProperties->AddAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main");
	}
}

void DrawingAnchor::SetAnchorDistance(long left, long top, long right, long bottom)
{
	char str[100];

	sprintf_s(str, "%ld", top);
	pAnchor->AddAttribute("distT", str);
	sprintf_s(str, "%ld", bottom);
	pAnchor->AddAttribute("distB", str);
	sprintf_s(str, "%ld", left);
	pAnchor->AddAttribute("distL", str);
	sprintf_s(str, "%ld", right);
	pAnchor->AddAttribute("distr", str);
}

void DrawingAnchor::SetDrawingInCell()
{
	pAnchor->AddAttribute("layoutInCell", "1");
}

void DrawingAnchor::SetSimplePosition(long x, long y)
{
	DocXTag *pTag = pAnchor->AddTag("wp:simplePos");

	pTag->AddAttribute("y", y);
	pTag->AddAttribute("x", x);
}


void DrawingAnchor::SetRelativeHorizontalPosition(long offset, string relativeFrom)
{
	DocXTag *pTag = pAnchor->AddTag("wp:positionH");
	pTag->AddAttribute("relativeFrom", relativeFrom);
	DocXTag *pTag1 = pTag->AddTag("wp:posOffset");
	
	char str[100];
	sprintf_s(str, "%ld", offset);
	pTag1->SetValue(str);
}

void DrawingAnchor::SetRelativeVerticalPosition(long offset, string relativeFrom)
{
	DocXTag *pTag = pAnchor->AddTag("wp:positionV");
	pTag->AddAttribute("relativeFrom", relativeFrom);
	DocXTag *pTag1 = pTag->AddTag("wp:posOffset");

	char str[100];
	sprintf_s(str, "%ld", offset);
	pTag1->SetValue(str);
}

void DrawingAnchor::SetExtent(long cx, long cy)
{
	DocXTag *pTag = pAnchor->AddTag("wp:extent");

	char str[100];

	sprintf_s(str, "%ld", cx);
	pTag->AddAttribute("cx", str);
	sprintf_s(str, "%ld", cy);
	pTag->AddAttribute("cy", str);
}

void DrawingAnchor::SetEffectiveExtent(long l, long t, long r, long b)
{
	DocXTag *pTag = pAnchor->AddTag("wp:effectExtent");

	pTag->AddAttribute("l", l);
	pTag->AddAttribute("t", t);
	pTag->AddAttribute("r", r);
	pTag->AddAttribute("b", b);
}

void DrawingAnchor::WrapSquare(string val)
{
	DocXTag *pTag = pAnchor->AddTag("wp:wrapSquare");
	pTag->AddAttribute("wrapText", "bothSides");
}

void DrawingAnchor::SetPictureProperties(string name, string desc, string title)
{
	DocXTag *pTag = pAnchor->AddTag("wp:docPr");
	pTag->AddAttribute("wrapText", "bothSides");

	DocXDocPart::pictureID++;
	pTag->AddAttribute("id", DocXDocPart::pictureID);
	pTag->AddAttribute("name", name);
	pTag->AddAttribute("descr", desc);
	pTag->AddAttribute("title", title);
}

void DrawingAnchor::SetPictureName(string name)
{
	SetNvPicProperties();
	DocXTag *pTag = pcNvPicProperties->AddTag("pic:cNvPr");

	DocXDocPart::pictureID++;
	pTag->AddAttribute("id", DocXDocPart::pictureID);
	pTag->AddAttribute("name", name);
}

void DrawingAnchor::SetGraphicFrameLockChangeAspect(string val)
{
	SetNvGraphicFrameProperties();
	SetGraphicFrameLockProperties();
	pGraphicFrameLockProperties->AddAttribute("noChangeAspect", val);
}

void DrawingAnchor::SetBlip(string strId, string compressionState, string link)
{
	SetBlipFill();
	DocXTag *pTag = pBlipFill->AddTag("a:blip");
	
	pTag->AddAttribute("r:embed", strId);
	pTag->AddAttribute("cstate", compressionState);
	if (link != "")
	{
		pTag->AddAttribute("link", link);
	}
}

void DrawingAnchor::SetBlipExt(string strId, string compressionState, string link, string useLocalDpi)
{
	SetBlipFill();
	DocXTag *pTag = pBlipFill->AddTag("a:blip");

	pTag->AddAttribute("r:embed", strId);
	pTag->AddAttribute("cstate", compressionState);
	if (link != "")
	{
		pTag->AddAttribute("link", link);
	}

	DocXTag *pTag1 = pTag->AddTag("a14:useLocalDpi");
	pTag1->AddAttribute("xmlns:a14", "http://schemas.microsoft.com/office/drawing/2010/main");
	pTag1->AddAttribute("val", useLocalDpi);
}

void DrawingAnchor::SetBlipStretch(string valTag)
{
	SetBlipFill();
	DocXTag *pTag = pBlipFill->AddTag("a:stretch");
	pTag->AddTag(valTag);
}


void DrawingAnchor::SetShapeTransform(long offsetX, long offsetY, long cx, long cy)
{
	SetShapeProperties();
	DocXTag *pTag = pShapeProperties->AddTag("a:xfrm");
	
	DocXTag *pTag1 = pTag->AddTag("a:off");

	pTag->AddAttribute("x", offsetX);
	pTag->AddAttribute("y", offsetY);

	pTag1 = pTag->AddTag("a:ext");
	pTag->AddAttribute("cx", cx);
	pTag->AddAttribute("cy", cy);
}

void DrawingAnchor::SetPresetGeometry(string val, bool bShapeAdjust)
{
	SetShapeProperties();
	DocXTag *pTag = pShapeProperties->AddTag("a:prstGeom");
	pTag->AddAttribute("prst", val);
	
	if (bShapeAdjust)
	{
		pTag->AddTag("a:avLst");
	}
}

void DrawingAnchor::SetRelativeHorizontalSize(long size, string relativeFrom)
{
	DocXTag *pTag = pAnchor->AddTag("wp14:sizeRelH");
	pTag->AddAttribute("relativeFrom", relativeFrom);
	DocXTag *pTag1 = pTag->AddTag("wp14:pctWidth");

	char str[100];
	sprintf_s(str, "%ld", size);
	pTag1->SetValue(str);
}

void DrawingAnchor::SetRelativeVerticalSize(long size, string relativeFrom)
{
	DocXTag *pTag = pAnchor->AddTag("wp14:sizeRelV");
	pTag->AddAttribute("relativeFrom", relativeFrom);
	DocXTag *pTag1 = pTag->AddTag("wp14:pctHeight");

	char str[100];
	sprintf_s(str, "%ld", size);
	pTag1->SetValue(str);
}
