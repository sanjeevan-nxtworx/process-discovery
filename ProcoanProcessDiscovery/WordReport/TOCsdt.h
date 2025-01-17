#pragma once
#include "DocXTag.h"
#include <string>

using namespace std;

class TOCsdt :
	public DocXTag
{
	DocXTag *pSdtContent;
	string contentStyle;
	int lefttabPos;
	int righttabPos;
	string lefttabLeader;
	string righttabLeader;
	string fontTheme;
	string tocStyle;
	string hyperlinkFont;


public:
	TOCsdt();
	~TOCsdt();
	void SetStandardProperties();
	void SetStandardTocContentStart();
	void SetContentStyle(string val);
	void SetLeftTabProperties(int pos, string leader = "");
	void SetRightTabProperties(int pos, string leader = "");
	void SetFontTheme(string val);
	void SetTOCStyle(string val);
	void SetHyperlinkFont(string val);
	void SetHyperlink(string text, string anchor, string listNumber, int pageNo, bool bFirst = false);
	void SetContentEnd();
};

