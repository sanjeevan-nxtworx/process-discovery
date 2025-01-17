#pragma once
#include "DocXTag.h"
#include <string>

using namespace std;

class DocumentBody :
	public DocXTag
{
	DocXTag *sectionProperties;

	void SetSectionProperties();



public:
	DocumentBody();
	~DocumentBody();
	void SetBookmark(string val);
	void SetFooterReference(string type, string id);
	void SetHeaderReference(string type, string id);
	void SetPageSize(int width, int height);
	void SetPageMargin(int top, int left, int bottom, int right, int header, int footer, int gutter);
	void SetColumnSpace(int val);
	void SetTitlePage();
	void SetDocGridLinePitch(int val);
};

