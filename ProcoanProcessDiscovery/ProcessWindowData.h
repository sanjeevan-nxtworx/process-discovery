#pragma once
#include <string>

using namespace std;

class ProcessWindowData
{
	string windowTitle;
	string className;
	string automationID;
	DWORD  pid;

public:
	ProcessWindowData();
	~ProcessWindowData();

	void SetWindowTitle(string value);
	void SetWindowClassName(string value);
	void SetWindowAutomationID(string value);
	void SetProcessID(DWORD value);

	string GetWindowTitle();
	string GetWindowClassName();
	string GetWindowAutomationID();
	DWORD GetProcessID();
};

