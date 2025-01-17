#include "stdafx.h"
#include "TableRow.h"
#include "TableCell.h"
#include "DocXDocPart.h"

TableRow::TableRow():DocXTag("w:tr")
{
	properties = NULL;
	AddAttribute("w:rsidR", DocXDocPart::str_rsidR);

	char str[100];
	DocXDocPart::paraID++;
	sprintf_s(str, "%08X", DocXDocPart::paraID);
	AddAttribute("w14:paraId", str);
	AddAttribute("w14:textId", "77777777");
	AddAttribute("w:rsidTr", DocXDocPart::str_rsidTr);

}

TableRow::~TableRow()
{
}

void TableRow::SetProperties()
{
	if (properties == NULL)
	{
		properties = DBG_NEW DocXTag("w:trPr");
		listTags.push_back(properties);
	}
}

void TableRow::SetRowHeight(int val)
{
	SetProperties();
	char str[100];

	sprintf_s(str, "%d", val);
	DocXTag *pTag = properties->AddTag("w:trHeight");
	pTag->AddAttribute("w:val", str);
}

TableCell *TableRow::AddTableCell()
{
	TableCell *pCell = DBG_NEW TableCell();
	listTags.push_back(pCell);
	return pCell;
}

