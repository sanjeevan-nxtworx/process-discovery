#pragma once
#include "DocXTag.h"
class DrawingAnchor :
	public DocXTag
{
	DocXTag *pAnchor;
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
	void SetCNvPicProperties();
	void SetBlipFill();
	void SetShapeProperties();

public:
	DrawingAnchor();
	~DrawingAnchor();
	void SetAnchorDistance(long left, long top, long right, long bottom);
	void SetDrawingInCell();
	void SetRelativeHorizontalPosition(long offset, string relativeFrom);
	void SetRelativeVerticalPosition(long offset, string relativeFrom);
	void SetRelativeHorizontalSize(long size, string relativeFrom);
	void SetRelativeVerticalSize(long size, string relativeFrom);
	void SetExtent(long cx, long cy);
	void SetEffectiveExtent(long l, long t, long r, long b);
	void WrapSquare(string val);
	void SetPictureProperties(string name, string desc, string title);
	void SetPictureName(string name);

	void SetGraphicFrameLockChangeAspect(string val);
	void SetBlip(string strId, string compressionState, string link);
	void SetBlipExt(string strId, string compressionState, string link, string useLocalDpi);
	void SetBlipStretch(string valTag);
	void SetShapeTransform(long offsetX, long offsetY, long cx, long cy);
	void SetPresetGeometry(string val, bool bShapeAdust);
	void SetSimplePosition(long x, long y);
};

