#pragma once
#include "DocXDocPart.h"
#include <string>
using namespace std;

class Font;

class FontTableDoc :
	public DocXDocPart
{
public:
	FontTableDoc();
	~FontTableDoc();
	Font *AddFontTable(string fontName);
};

