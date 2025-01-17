#pragma once
class LineConnectionInfo
{
	RECT fromRect;
	RECT toRect;
	RECT wndRect;

public:
	LineConnectionInfo();
	~LineConnectionInfo();
	void SetFromRect(LPRECT value);
	void SetToRect(LPRECT value);
	void SetWndRect(LPRECT value);
	LPRECT GetFromRect();
	LPRECT GetToRect();
	LPRECT GetWndRect();
};

