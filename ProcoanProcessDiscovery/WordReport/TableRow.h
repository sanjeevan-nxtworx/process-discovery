#pragma once
#include "DocXTag.h"

class TableCell;
class TableRow :
	public DocXTag
{
	DocXTag *properties;
	void SetProperties();
public:
	TableRow();
	~TableRow();
	void SetRowHeight(int val);
	TableCell *AddTableCell();
};

