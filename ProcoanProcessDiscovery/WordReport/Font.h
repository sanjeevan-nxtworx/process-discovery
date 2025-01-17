#pragma once
#include "DocXTag.h"
#include <string>
using namespace std;

class Font :
	public DocXTag
{
	DocXTag *psig;

	void SetSig();
public:
	Font(string name);
	~Font();
	void SetFamily(string val);
	void SetCharset(string val);
	void SetPitch(string val);
	void SetPanose1(string val);
	void SetSigcsb(string val0, string val1);
	void SetSigusb(string val0, string val1, string val2, string val3);
};

