#include "stdafx.h"
#include "ProcessWindowData.h"


ProcessWindowData::ProcessWindowData()
{
	windowTitle = "";
	className = "";
	automationID = "";
}


ProcessWindowData::~ProcessWindowData()
{
}


void ProcessWindowData::SetWindowTitle(string value)
{
	windowTitle = value;
}

void ProcessWindowData::SetWindowClassName(string value)
{
	className = value;
}

void ProcessWindowData::SetWindowAutomationID(string value)
{
	automationID = value;
}

void ProcessWindowData::SetProcessID(DWORD value)
{
	pid = value;
}


string ProcessWindowData::GetWindowTitle()
{
	return windowTitle;
}

string ProcessWindowData::GetWindowClassName()
{
	return className;
}

string ProcessWindowData::GetWindowAutomationID()
{
	return automationID;
}

DWORD ProcessWindowData::GetProcessID()
{
	return pid;
}