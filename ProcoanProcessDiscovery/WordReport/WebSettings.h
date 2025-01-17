#pragma once
#include "DocXDocPart.h"
class WebSettings :
	public DocXDocPart
{
public:
	WebSettings();
	~WebSettings();
	void SetOptimizeForBrowser();
	void SetAllowPNG();
};

