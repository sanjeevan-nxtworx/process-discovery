#pragma once
#include "DocXTag.h"

class TableRow;
class Table :
	public DocXTag
{
	DocXTag *pTableGrid;
	DocXTag *pTableProperties;
	void SetTableProperties();
	void SetTableGrid();

public:
	Table();
	~Table();
	void SetTableStyle(string style);
	void SetTableWidthAuto();
	void SetTableWidthDxa(int val);
	void SetTableWidth(int width, string val);
	void SetTableInd(int l, string val);
	void SetTableBordersNone();
	void SetTableLookNormal();
	void SetColumnWidth(int width);
	TableRow *AddTableRow();
};

