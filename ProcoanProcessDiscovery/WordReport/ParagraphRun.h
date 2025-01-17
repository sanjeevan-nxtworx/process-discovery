#pragma once
#include "DocXTag.h"
#include <string>

using namespace std;

class DrawingAnchor;
class DrawingInline;

class ParagraphRun :
	public DocXTag
{
	DocXTag *properties;

	void SetProperties();
	void UpdateMetrics(string texgt);
public:
	ParagraphRun();
	~ParagraphRun();
	void SetNoProof();
	void SetWebHidden();
	void SetBold();
	void SetbCs();
		DrawingAnchor *AddFloatingImage();
	DrawingInline *AddInlineImage();
	void SetFont(string fontName, string size, bool bCS);
	void SetFont(string fontName);
	void SetFontSize(int size);
	void SetFontTheme(string val);
	void SetFont(string fontName, string size, string color, string themeColor, string themeShade);
	void SetFontColor(string color, string themeColor, string themeShade);
	void SetSeperator();
	void SetContinuationSeperator();
	void SetText(string text, bool bPreserve);
	void SetCSTheme(string val);
	void SetRunStyle(string val);
	void AddTab(bool bNoProof = false, bool bWebHidden = false);
	
};


