#include "stdafx.h"
#include "SOPPrintReport.h"


SOPPrintReport::SOPPrintReport()
{
	m_numPages = 0;
}


SOPPrintReport::~SOPPrintReport()
{
}


void SOPPrintReport::GetPageInfo(int *minPage, int *maxPage,
	int *pageFrom, int *pageTo)
{
	*minPage = 1; *maxPage = m_numPages;
	*pageFrom = 1; *pageTo = m_numPages;
}

bool SOPPrintReport::HasPage(int pageNum)
{
	return (pageNum >= 1 && pageNum <= m_numPages);
}


bool SOPPrintReport::OnBeginDocument(int startPage, int endPage)
{
	return true;
}

void SOPPrintReport::OnBeginPrinting()
{

}

void SOPPrintReport::OnEndDocument()
{

}

void SOPPrintReport::OnEndPrinting()
{

}

void SOPPrintReport::OnPreparePrinting()
{

}

bool SOPPrintReport::OnPrintPage(int pageNum)
{

	return false;
}

wxString SOPPrintReport::GetTitle()	const
{
	return wxT("Hello World");
}
