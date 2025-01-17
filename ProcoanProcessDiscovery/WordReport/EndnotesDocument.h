#pragma once
#include "DocXDocPart.h"


class EndnotesDocument :
	public DocXDocPart
{
	int endnoteID;
public:
	EndnotesDocument();
	~EndnotesDocument();
	void AddEndnoteSeperator();
	void AddEndnoteContinuousSeperator();
};

