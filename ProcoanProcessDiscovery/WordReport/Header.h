#pragma once
#include "DocXDocPart.h"
class Paragraph;
class Header;
class Header :
	public DocXDocPart
{
public:
	Header(string val);
	~Header();
	Paragraph *AddParagraph();
	Table *AddTable();

};

