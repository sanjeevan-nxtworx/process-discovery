#include "stdafx.h"
#include "CoreProps.h"


CoreProps::CoreProps():DocXDocPart("docProps", "core.xml")
{
	mainTag = AddTag("cp:coreProperties");
	mainTag->AddAttribute("xmlns:cp", "http://schemas.openxmlformats.org/package/2006/metadata/core-properties");
	mainTag->AddAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
	mainTag->AddAttribute("xmlns:dcterms", "http://purl.org/dc/terms/");
	mainTag->AddAttribute("xmlns:dcmitype", "http://purl.org/dc/dcmitype/");
	mainTag->AddAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
}


CoreProps::~CoreProps()
{
}

void CoreProps::BuildCoreProps()
{
	DocXTag *pSingleTag = NULL;

	pSingleTag = mainTag->AddTag("dc:title", true);
	pSingleTag = mainTag->AddTag("dc:subject", true);
	pSingleTag = mainTag->AddTag("dc:creator");
	pSingleTag->SetValue("Procoan");

	pSingleTag = mainTag->AddTag("cp:keywords", true);
	pSingleTag = mainTag->AddTag("dc:description", true);

	pSingleTag = mainTag->AddTag("dcterms:created");
	pSingleTag->AddAttribute("xsi:type", "dcterms:W3CDTF");
	pSingleTag->SetValue("2019-08-17T06:45:00Z");
}
