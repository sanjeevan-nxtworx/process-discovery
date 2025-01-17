#include "stdafx.h"
#include "DocumentBody.h"
#include "DocXDocPart.h"
#include "Paragraph.h"


DocumentBody::DocumentBody(): DocXTag("w:body")
{
	sectionProperties = NULL;
}


DocumentBody::~DocumentBody()
{
}

void DocumentBody::SetBookmark(string val)
{
	Paragraph *pParagraph = DBG_NEW Paragraph();
	listTags.push_back(pParagraph);
	
	DocXTag *pTag = pParagraph->AddTag("w:bookmarkStart");

	char str[100];
	sprintf_s(str, "%d", DocXDocPart::bookmarkID);
	pTag->AddAttribute("w:id", str);
	pTag->AddAttribute("w:name", val);

	pTag = pParagraph->AddTag("w:bookmarkEnd");
	pTag->AddAttribute("w:id", str);

	DocXDocPart::bookmarkID++;

}

void DocumentBody::SetSectionProperties()
{
	if (sectionProperties == NULL)
	{
		sectionProperties = DBG_NEW DocXTag("w:sectPr");
		listTags.push_back(sectionProperties);
		sectionProperties->AddAttribute("w:rsidR", DocXDocPart::str_rsidR);
		sectionProperties->AddAttribute("w:rsidSect", DocXDocPart::str_rsidR);
	}
}

void DocumentBody::SetFooterReference(string type, string id)
{
	SetSectionProperties();

	DocXTag *pTag = sectionProperties->AddTag("w:footerReference");
	pTag->AddAttribute("w:type", type);
	pTag->AddAttribute("r:id", id);
}

void DocumentBody::SetHeaderReference(string type, string id)
{
	SetSectionProperties();

	DocXTag *pTag = sectionProperties->AddTag("w:headerReference");
	pTag->AddAttribute("w:type", type);
	pTag->AddAttribute("r:id", id);
}

void DocumentBody::SetPageSize(int width, int height)
{
	SetSectionProperties();

	DocXTag *pTag = sectionProperties->AddTag("w:pgSz");
	char str[100];
	sprintf_s(str, "%d", width);
	pTag->AddAttribute("w:w", str);

	sprintf_s(str, "%d", height);
	pTag->AddAttribute("w:h", str);
}

void DocumentBody::SetPageMargin(int top, int left, int bottom, int right, int header, int footer, int gutter)
{
	SetSectionProperties();

	DocXTag *pTag = sectionProperties->AddTag("w:pgMar");
	char str[100];
	sprintf_s(str, "%d", top);
	pTag->AddAttribute("w:top", str);

	sprintf_s(str, "%d", left);
	pTag->AddAttribute("w:left", str);

	sprintf_s(str, "%d", bottom);
	pTag->AddAttribute("w:bottom", str);

	sprintf_s(str, "%d", right);
	pTag->AddAttribute("w:right", str);

	sprintf_s(str, "%d", header);
	pTag->AddAttribute("w:header", str);

	sprintf_s(str, "%d", footer);
	pTag->AddAttribute("w:footer", str);

	sprintf_s(str, "%d", gutter);
	pTag->AddAttribute("w:gutter", str);
}

void DocumentBody::SetColumnSpace(int val)
{
	SetSectionProperties();

	DocXTag *pTag = sectionProperties->AddTag("w:cols");
	char str[100];
	sprintf_s(str, "%d", val);
	pTag->AddAttribute("w:space", str);
}

void DocumentBody::SetTitlePage()
{
	SetSectionProperties();

	sectionProperties->AddTag("w:titlePg");
}

void DocumentBody::SetDocGridLinePitch(int val)
{
	SetSectionProperties();

	DocXTag *pTag = sectionProperties->AddTag("w:docGrid");
	char str[100];
	sprintf_s(str, "%d", val);
	pTag->AddAttribute("w:linePitch", str);
}

