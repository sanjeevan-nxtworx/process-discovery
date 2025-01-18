#include "stdafx.h"
#include "PDFSOPDoc.h"


PDFSOPDoc::PDFSOPDoc()
{
}


PDFSOPDoc::~PDFSOPDoc()
{
}

void PDFSOPDoc::SetMyTitle(const wxString& title)
{
	m_myTitle = title;
	SetTitle(title);
}

void PDFSOPDoc::Header()
{
	SetFont(wxS("Arial"), wxS(""), 10.0);
	SetTextColour(0);
	// Title
	Cell(0, 12.7, wxS("SOP Report"), wxPDF_BORDER_NONE, 1, wxPDF_ALIGN_CENTER);
}

void PDFSOPDoc::ReportSubTitle(const wxString& label)
{
	// Helvetica 12
	SetFont(wxS("Arial"), wxS("B"), 12.0);
	// Background color
	SetFillColour(wxColour(200, 220, 255));
	// Title
	Cell(0, 6, label, 0, 1, wxPDF_ALIGN_LEFT, 1);
	// Line break
	Ln(5.0);
}

void PDFSOPDoc::ReportSubTitleLink(const wxString& label, int link)
{
	// Helvetica 12
	SetTextColour(0);
	SetFont(wxS("Arial"), wxS("B"), 10.0);
	// Background color
	SetFillColour(wxColour(200, 220, 255));
	// Title
	SetLink(link);
	MultiCell(0, 6, label, 0, wxPDF_ALIGN_LEFT, 1);
	// Line break
	Ln(5.0);
}

void PDFSOPDoc::ReportParagraph(const wxString& paraText)
{
	// Helvetica 12
	SetTextColour(0);
	SetFont(wxS("Arial"), wxS(""), 10.0);
	// Title
	MultiCell(0, 5, paraText);
	// Line break
	Ln(5.0);
}

void PDFSOPDoc::Footer()
{
	// Position at 1.5 cm from bottom
	SetY(-2.5);
	// Helvetica italic 8
	SetFont(wxS("Arial"), wxS("I"), 8.0);
	SetTextColour(0);
	// Page number
	Cell(0, 0, wxString::Format(wxS("Page %d/{nb}"), PageNo()), 0, 0, wxPDF_ALIGN_CENTER);
	Ln(5.0);
}


// Colored table
void PDFSOPDoc::DataTable(wxArrayString& header, wxArrayPtrVoid& data, double *w, wxPdfAlignment *align)
{

	// Colors, line width and bold font
	SetFillColour(wxColour(104, 164, 237));
	SetTextColour(0);
	SetDrawColour(wxColour(64, 64, 64));
	SetLineWidth(.3);
	SetFont(wxS("Arial"), wxS("B"), 10.0);
	//Header
	double xOrg = GetX();
	double yCoord = GetY();
	double xCoord = GetX();
	double yCoord2 = 0.0;
	double yHeight = 0.0;
	double pageHeight = GetPageHeight() - 50.4;
	PrintDataTableHeader(header, w);
	// Color and font restoration
	SetFillColour(wxColour(224, 235, 255));
	SetTextColour(0);
	SetFont(wxS("Arial"), wxS(""), 10.0);
	SetX(xOrg);
	// Data
	int fill = 0;
	size_t j;
	//
	for (j = 0; j < data.GetCount(); j++)
	{
		wxArrayString* row = (wxArrayString*)data[j];
		xOrg = GetX();
		yCoord = GetY();
		xCoord = GetX();
		yCoord2 = 0.0;
		yHeight = 0.0;

		for (size_t i = 0; i < row->GetCount(); i++)
		{
			MultiCell(w[i], 4.0, (*row)[i], 0, align[i], 0);
			yCoord2 = GetY();
			SetY(yCoord);
			if (yCoord2 - yCoord > yHeight)
				yHeight = yCoord2 - yCoord;
			xCoord += w[i];
			SetX(xCoord);
		}
		SetX(xOrg);
		SetY(yCoord);
		xCoord = GetX();
		for (size_t i = 0; i < row->GetCount(); i++)
		{
			MultiCell(w[i], yHeight, wxT(""), wxPDF_BORDER_FRAME, 0, wxPDF_ALIGN_LEFT, fill);
			SetY(yCoord);
			xCoord += w[i];
			SetX(xCoord);
		}
		SetY(yCoord + yHeight);
		//Ln();
		if (yCoord + yHeight > pageHeight)
		{
			AddPage(wxPORTRAIT, wxPAPER_A4);
			PrintDataTableHeader(header, w);
		}
		else
			SetY(yCoord + yHeight);
		fill = 1 - fill;
	}

	double widthVal = 0.0f;
	for (size_t i = 0; i < header.GetCount(); i++)
	{
		widthVal += w[i];
	}

	Cell(widthVal, 0, wxS(""), wxPDF_BORDER_TOP);
	Ln(5.0);
}


// Colored table
void PDFSOPDoc::DataLinkTable(wxArrayString& header, wxArrayPtrVoid& data, wxArrayPtrVoid& links, double *w, wxPdfAlignment *align)
{
	// Colors, line width and bold font
	SetFillColour(wxColour(104, 164, 237));
	SetTextColour(0);
	SetDrawColour(wxColour(64, 64, 64));
	SetLineWidth(.3);
	SetFont(wxS("Arial"), wxS(""), 10.0);
	//Header
	
	double xOrg = GetX();
	double yCoord = GetY();
	double xCoord = GetX();
	double yCoord2 = 0.0;
	double yHeight = 0.0;
	PrintDataLinkHeader(header, w);

	double pageHeight = GetPageHeight() - 50.4;
	// Color and font restoration

	SetFillColour(wxColour(224, 235, 255));
	SetTextColour(0);
	SetFont(wxS("Arial"), wxS(""), 10.0);
	// Data
	
	//
	for (size_t j = 0; j < data.GetCount(); j++)
	{
		wxArrayString* row = (wxArrayString*)data[j];
		wxArrayInt* linksData = (wxArrayInt *)links[j];

		xOrg = GetX();
		yCoord = GetY();
		xCoord = GetX();
		yCoord2 = 0.0;
		yHeight = 0.0;

		for (size_t i = 0; i < row->GetCount(); i++)
		{
			SetTextColour(0);
			if ((*linksData)[i] == 0)
				MultiCell(w[i], 4.0, (*row)[i], 0, align[i], 0);
			else
			{
				SetTextColour(wxColour(0,0, 200));
				int linkVal = (*linksData)[i];
				Cell(w[i], 4.0, (*row)[i], 0, 0, align[i], 0, wxPdfLink(linkVal));
			}
			yCoord2 = GetY();
			SetY(yCoord);
			if (yCoord2 - yCoord > yHeight)
				yHeight = yCoord2 - yCoord;
			xCoord += w[i];
			SetX(xCoord);
		}
		SetX(xOrg);
		SetY(yCoord);
		xCoord = GetX();
		//double widthVal = 0.0f;
		for (size_t i = 0; i < row->GetCount(); i++)
		{
			Cell(w[i], yHeight, wxS(""), wxPDF_BORDER_FRAME);
			SetY(yCoord);
			xCoord += w[i];
			SetX(xCoord);
		}

		if (yCoord + yHeight > pageHeight)
		{
			AddPage(wxPORTRAIT, wxPAPER_A4);
			PrintDataLinkHeader(header, w);
		}
		else
			SetY(yCoord + yHeight);
	}

	double widthVal = 0.0f;
	for (size_t i = 0; i < header.GetCount(); i++)
	{
		widthVal += w[i];
	}

	Cell(widthVal, 0, wxS(""), wxPDF_BORDER_TOP);

	Ln(5.0);
}

void PDFSOPDoc::PrintDataLinkHeader(wxArrayString& header, double *w)
{
	double xOrg = GetX();
	double yCoord = GetY();
	double xCoord = GetX();
	double yCoord2 = 0.0;
	double yHeight = 0.0;

	SetTextColour(wxColour(255, 255, 255));
	for (size_t i = 0; i < header.GetCount(); i++)
	{
		MultiCell(w[i], 4.0, header[i], 0, wxPDF_ALIGN_CENTER, 0);
		yCoord2 = GetY();
		if (yCoord2 - yCoord > yHeight)
			yHeight = yCoord2 - yCoord;
		xCoord += w[i];
		SetY(yCoord);
		SetX(xCoord);
	}

	SetX(xOrg);
	xCoord = GetX();
	SetTextColour(0);
	for (size_t i = 0; i < header.GetCount(); i++)
	{
		SetY(yCoord);
		SetX(xCoord);
		MultiCell(w[i], yHeight, header[i], wxPDF_BORDER_FRAME, wxPDF_ALIGN_CENTER, 1);
		xCoord += w[i];
	}
	SetX(xOrg);
}


void PDFSOPDoc::PrintDataTableHeader(wxArrayString& header, double *w)
{
	double xOrg = GetX();
	double yCoord = GetY();
	double xCoord = GetX();
	double yCoord2 = 0.0;
	double yHeight = 0.0;

	for (size_t i = 0; i < header.GetCount(); i++)
	{
		MultiCell(w[i], 4.0, header[i], 0, wxPDF_ALIGN_CENTER, 0);
		yCoord2 = GetY();
		if (yCoord2 - yCoord > yHeight)
			yHeight = yCoord2 - yCoord;
		xCoord += w[i];
		SetY(yCoord);
		SetX(xCoord);
	}

	SetX(xOrg);
	xCoord = GetX();
	for (size_t i = 0; i < header.GetCount(); i++)
	{
		SetY(yCoord);
		SetX(xCoord);
		MultiCell(w[i], yHeight, wxT(""), wxPDF_BORDER_FRAME, wxPDF_ALIGN_CENTER, 0);
		xCoord += w[i];
	}
	SetX(xOrg);
}




