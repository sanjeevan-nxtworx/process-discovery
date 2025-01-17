#pragma once
#include <wx/print.h>

class SOPPrintReport : public wxPrintout
{
	int m_numPages;

public:
	SOPPrintReport();
	~SOPPrintReport();
	void GetPageInfo(int *minPage, int *maxPage,
		int *pageFrom, int *pageTo);
	bool HasPage(int pageNum);
	virtual bool OnBeginDocument(int startPage, int endPage);
	virtual void OnBeginPrinting();
	virtual void OnEndDocument();
	virtual void OnEndPrinting();
	virtual void OnPreparePrinting();
	bool OnPrintPage(int pageNum);
	virtual wxString GetTitle()	const;
};

