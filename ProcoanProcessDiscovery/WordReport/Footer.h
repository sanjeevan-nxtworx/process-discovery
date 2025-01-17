#pragma once
#include "DocXDocPart.h"

class Paragraph;
class Footer :
	public DocXDocPart
{
public:
	Footer(string val);
	~Footer();

	Paragraph *AddParagraph();
	Table *AddTable();
};

