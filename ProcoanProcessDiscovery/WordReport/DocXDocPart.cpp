#include "stdafx.h"
#include "DocXDocPart.h"
#include "DocumentBody.h"
#include "TOCsdt.h"

string DocXDocPart::str_rsidR;
string DocXDocPart::str_rsidTr;
long DocXDocPart::paraID;
long DocXDocPart::pictureID;
long DocXDocPart::bookmarkID;

long DocXDocPart::totalTime;
long DocXDocPart::noPages;
long DocXDocPart::noParagraphs;
long DocXDocPart::noLines;
long DocXDocPart::noWords;
long DocXDocPart::noCharacters;
long DocXDocPart::noCharactersWithSpaces;
long DocXDocPart::abstractNumID;
int DocXDocPart::imageNumber;
TOCsdt *DocXDocPart::TOC;
long DocXDocPart::tocNumber;
int  DocXDocPart::pageNumber;
int  DocXDocPart::bookmarkNumber;

DocXDocPart::DocXDocPart(string path, string name)
{
	docPath = path;
	docName = name;
	hdrComment = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
	mainTag = NULL;
}

DocXDocPart::DocXDocPart(string path, string name, string header)
{
	docPath = path;
	docName = name;
	hdrComment = header;
	mainTag = NULL;
}


DocXDocPart::~DocXDocPart()
{
	if (mainTag != NULL)
		delete mainTag;
	mainTag = NULL;
}

DocXTag *DocXDocPart::AddTag(string tagName)
{
	mainTag = DBG_NEW DocXTag(tagName);
	return mainTag;
}

string DocXDocPart::GetDocString()
{
	string strDoc = hdrComment + "\r\n";

	strDoc += mainTag->GetTagString();
	return strDoc;
}



