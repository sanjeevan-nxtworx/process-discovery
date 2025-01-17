#pragma once
#include "DocXTag.h"
#include <list>
#include <string>


using namespace std;
class DocumentBody;
class TOCsdt;

class DocXDocPart
{
	string hdrComment;
	string docName;
	string docPath;
protected:
	DocXTag *mainTag;

public:
	static string str_rsidR;
	static string str_rsidTr;
	static long paraID;
	static long pictureID;
	static long bookmarkID;

	static long totalTime;
	static long noPages;
	static long noParagraphs;
	static long noLines;
	static long noWords;
	static long noCharacters;
	static long noCharactersWithSpaces;
	static long abstractNumID;
	static int imageNumber;
	static TOCsdt *TOC;
	static long tocNumber;
	static int  pageNumber;
	static int  bookmarkNumber;

public:
	DocXDocPart(string path, string name);
	DocXDocPart(string path, string name, string header);
	~DocXDocPart();
	DocXTag *AddTag(string tagName);
	string GetDocString();

};

