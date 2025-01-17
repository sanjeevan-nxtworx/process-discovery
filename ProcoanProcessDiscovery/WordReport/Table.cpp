#include "stdafx.h"
#include "Table.h"
#include "TableRow.h"
#include "DocXDocPart.h"

Table::Table():DocXTag("w:tbl")
{
	pTableProperties = NULL;
	pTableGrid = NULL;
}


Table::~Table()
{
}

void Table::SetTableProperties()
{
	if (pTableProperties == NULL)
	{
		pTableProperties = DBG_NEW DocXTag("w:tblPr");
		listTags.push_back(pTableProperties);
	}

}

TableRow *Table::AddTableRow()
{
	TableRow *pRow = DBG_NEW TableRow();
	listTags.push_back(pRow);
	return pRow;
}

void Table::SetTableGrid()
{
	if (pTableGrid == NULL)
	{
		pTableGrid = DBG_NEW DocXTag("w:tblGrid");
		listTags.push_back(pTableGrid);
	}
}

void Table::SetTableStyle(string style)
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:tblStyle");
	pTag->AddAttribute("w:val", style);
}

void Table::SetTableWidthAuto()
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:tblW");
	pTag->AddAttribute("w:w", "0");
	pTag->AddAttribute("w:type", "auto");

}

void Table::SetTableWidthDxa(int val)
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:tblW");
	pTag->AddAttribute("w:w", val);
	pTag->AddAttribute("w:type", "dxa");

}
void Table::SetTableWidth(int width, string val)
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:w");
	pTag->AddAttribute("w:w", width);
	pTag->AddAttribute("w:type", val);
}

void Table::SetTableInd(int l, string val)
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:tblInd");
	pTag->AddAttribute("w:w", l);
	pTag->AddAttribute("w:type", val);
}


void Table::SetTableBordersNone()
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:tblBorders");
	DocXTag *pTagNext = pTag->AddTag("w:top");
	pTagNext->AddAttribute("w:val", "none");
	pTagNext->AddAttribute("w:sz", "0");
	pTagNext->AddAttribute("w:space", "0");
	pTagNext->AddAttribute("w:color", "auto");

	pTagNext = pTag->AddTag("w:left");
	pTagNext->AddAttribute("w:val", "none");
	pTagNext->AddAttribute("w:sz", "0");
	pTagNext->AddAttribute("w:space", "0");
	pTagNext->AddAttribute("w:color", "auto");

	pTagNext = pTag->AddTag("w:bottom");
	pTagNext->AddAttribute("w:val", "none");
	pTagNext->AddAttribute("w:sz", "0");
	pTagNext->AddAttribute("w:space", "0");
	pTagNext->AddAttribute("w:color", "auto");

	pTagNext = pTag->AddTag("w:right");
	pTagNext->AddAttribute("w:val", "none");
	pTagNext->AddAttribute("w:sz", "0");
	pTagNext->AddAttribute("w:space", "0");
	pTagNext->AddAttribute("w:color", "auto");

	pTagNext = pTag->AddTag("w:insideH");
	pTagNext->AddAttribute("w:val", "none");
	pTagNext->AddAttribute("w:sz", "0");
	pTagNext->AddAttribute("w:space", "0");
	pTagNext->AddAttribute("w:color", "auto");

	pTagNext = pTag->AddTag("w:insideV");
	pTagNext->AddAttribute("w:val", "none");
	pTagNext->AddAttribute("w:sz", "0");
	pTagNext->AddAttribute("w:space", "0");
	pTagNext->AddAttribute("w:color", "auto");
}

void Table::SetTableLookNormal()
{
	SetTableProperties();
	DocXTag *pTag = pTableProperties->AddTag("w:tblLook");
	pTag->AddAttribute("w:val", "04A0");
	pTag->AddAttribute("w:firstRow", "1");
	pTag->AddAttribute("w:lastRow", "0");
	pTag->AddAttribute("w:firstColumn", "1");
	pTag->AddAttribute("w:lastColumn", "0");
	pTag->AddAttribute("w:noHBand", "0");
	pTag->AddAttribute("w:noVBand", "1");
}

void Table::SetColumnWidth(int width)
{
	char str[100];
	SetTableGrid();
	sprintf_s(str, "%d", width);
	DocXTag *pTag1 = pTableGrid->AddTag("w:gridCol");
	pTag1->AddAttribute("w:w", str);
}
