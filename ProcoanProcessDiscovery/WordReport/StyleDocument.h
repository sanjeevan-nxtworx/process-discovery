#pragma once
#include "DocXDocPart.h"
class StyleDocument :
	public DocXDocPart
{
	DocXTag *Set_lsdException(DocXTag *pTag, string name, string semiHidden, string unhideWhenUsed, string uiPriority, string qFormat);
public:
	StyleDocument();
	~StyleDocument();
	void BuildStyleDocument();
};

