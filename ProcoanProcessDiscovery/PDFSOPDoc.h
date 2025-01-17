#pragma once
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/pdfdoc.h"

class PDFSOPDoc : public wxPdfDocument
{
	wxString m_myTitle;

public:
	void ReportSubTitle(const wxString& paraText);
	void ReportSubTitleLink(const wxString& paraText, int link);
	void ReportParagraph(const wxString& label);
	void SetMyTitle(const wxString& title);
	void DataTable(wxArrayString& header, wxArrayPtrVoid& data, double *w, wxPdfAlignment *align);
	void DataLinkTable(wxArrayString& header, wxArrayPtrVoid& data, wxArrayPtrVoid& links, double *w, wxPdfAlignment *align);
	void PrintDataLinkHeader(wxArrayString& header, double *w);
	void PrintDataTableHeader(wxArrayString& header, double *w);
	void Header();
	void Footer();
	PDFSOPDoc();
	~PDFSOPDoc();
};

