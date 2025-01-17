#pragma once
#include "DocXTag.h"
class DrawingInline :
	public DocXTag
{
	DocXTag *pInline;
	DocXTag *pGraphicFrameLockProperties;
	DocXTag *pNvGraphicFrameProperties;
	DocXTag *pGraphicProperties;
	DocXTag *pGraphicDataProperties;
	DocXTag *pPicProperties;
	DocXTag *pNvPicProperties;
	DocXTag *pcNvPicProperties;
	DocXTag *pShapeProperties;
	DocXTag *pBlipFill;

	void SetNvGraphicFrameProperties();
	void SetGraphicFrameLockProperties();
	void SetGraphicProperties();
	void SetGraphicDataProperties();
	void SetPicProperties();
	void SetNvPicProperties();
	void SetBlipFill();
	void SetShapeProperties();

public:
	DrawingInline(int distL, int distT, int distR, int distB);
	~DrawingInline();
	void SetExtent(long cx, long cy);
	void SetEffectiveExtent(long l, long t, long r, long b);
	void SetPictureProperties(string name, string desc, string wrapText);
	void SetPictureName(string name);

	void SetGraphicFrameLockChangeAspect(string val);
	void SetBlip(string strId, string link);
	void SetBlipExt(string strId, string link, string useLocalDpi);
	void SetBlipStretch(string valTag);
	void SetShapeTransform(long offsetX, long offsetY, long cx, long cy);
	void SetPresetGeometry(string val, bool bShapeAdust);
	void SetSolidFillLine(string schemeClr);
	void SetEffect(string align, string blurRad, string dir, string dist, string rotWithShape, string prstClr, string alpha);
};

