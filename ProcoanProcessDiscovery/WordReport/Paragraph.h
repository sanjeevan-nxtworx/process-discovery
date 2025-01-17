#pragma once
#include "DocXTag.h"

class ParagraphRun;

class Paragraph :
	public DocXTag
{
	DocXTag *properties;
	DocXTag *runProperties;
	DocXTag *tabProperties;
	DocXTag *numProperties;

	void SetProperties();
	void SetRunProperties();
	void SetTabProperties();
	void SetNumProperties();

public:
	Paragraph();
	~Paragraph();

	ParagraphRun *AddRun();

	void SetFont(string fontName, string size, bool bCS);
	void SetFont(string fontName, int size, int csSize);
	void SetFontColor(string color, string themeColor, string themeShade);
	void SetFont(string fontName, string size, string color, string themeColor, string themeShade);
	void SetFontSize(int size);
	void SetFontTheme(string val);
	void SetFontTheme(string asciiTheme, string csTheme, string eastAsiaTheme, string ansiTheme);
	void SetFontThemeColor(string themeColor, string themeShade, string colorVal);

	void SetTabs(string tabType, int position);
	void SetTabs(string tabType, int position, string leader);
	void SetJustification(string type);
	void SetSpacing(string type, int line, int after = 0);
	void SetStyle(string val);
	void AddPageBreak();
	void StartProof();
	void EndProof();
	void SetNoProof();

	void SetNumberProperties(int numID, int level);
	void SetLeftIndent(int val, int hanging = 0);

	void SetFont(string fontName);
	void SetPageBreak();

};

