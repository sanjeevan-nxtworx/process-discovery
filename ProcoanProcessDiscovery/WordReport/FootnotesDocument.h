#pragma once
#include "DocXDocPart.h"
class FootnotesDocument :
	public DocXDocPart
{
	int footnoteID;

public:
	FootnotesDocument();
	~FootnotesDocument();
	void AddFootnoteSeperator();
	void AddFootnoteContinuousSeperator();
};

