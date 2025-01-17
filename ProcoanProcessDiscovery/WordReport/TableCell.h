#pragma once
#include "DocXTag.h"

class ParagraphRun;
struct PictureInsertionProperties;
struct PictureProperties;

class TableCell :
	public DocXTag
{

	DocXTag *properties;
	void SetProperties();
public:
	TableCell();
	~TableCell();
	void SetCellWidthAndType(int val, string type);
	void SetTableCellBorders(string color, int space, int sz, string val);
	void SetCellShade(string shade, string themeFillShade, string themeFill, string fill, string color);
	void SetVerticalAlign(string val);
	void InsertFloatingPictureFromFile(ParagraphRun *pRun, struct PictureInsertionProperties *pInsProps, struct PictureProperties *pPicProps);
	void SetHideMark();
};

