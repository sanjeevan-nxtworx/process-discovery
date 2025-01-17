#pragma once
#include "DocXDocPart.h"
#include "string"

using namespace std;

class DocXTag;

class Numbering :
	public DocXDocPart
{
	DocXTag *properties;
	DocXTag *runProperties;
	DocXTag *pAbstractTag;
	DocXTag *pLevel;
	void SetProperties();
	void SetRunProperties();
public:
	Numbering();
	~Numbering();
	void AddAbstractNumbering(string nsid, string multilevelType, string templ);
	void AddLevelNumbering(string tplc, int lev, int start, string numFmt = "", string pStyle = "", string levelText = "", string justification = "", bool tentative = false);
	void SetLevelIndent(int hanging, int left);
	void SetFontHint(string hint, string ansi, string ascii);	
	void SetFontHint(string hint, string ansi, string ascii, string cs);
	DocXTag *AddAbstractNumbering(int nsid, int id);

};

