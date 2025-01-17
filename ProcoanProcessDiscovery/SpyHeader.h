#pragma once

#pragma pack(push,1)
#include <string>
#include <vector>
#include <map>

using namespace std;

class ProcessWindowData;
class WindowControlData;
class SpyControlData;

struct spyRoot
{
	char key[25];
	long long fPosProcessData;
	long long fPosDocumentData;
	long long fPosURLData;
	char filler[120];
};


struct threaddata
{
	DWORD pid;
	HANDLE hWND;
	string url;
	string className;
	string name;
	vector <DWORD> pids;
	vector <ProcessWindowData *> windowData;
	map <DWORD, int> pidCount;

	string processName;
	string windowName;
	string urlName;
	vector <WindowControlData *> controlData;
	vector <string> urlNames;

};

typedef struct processdata
{
	DWORD pid;
	DWORD parent;
	string processName;
} PROCESSDATA;


struct matchResult
{
	bool primaryMatch;
	bool rectMatch;
	bool nameMatch;
	bool valueMatch;
	DWORD matchAncestors;
	DWORD matchLeftSibling;
	DWORD matchRightSibling;
	DWORD matchChildren;
	DWORD matchNeighbors;
	WindowControlData *wndControlData;
	SpyControlData* spyControlData;
};

struct processRecord
{
	char recType; // 'P' for Process Record, 'U' for URL Record
	ULONG dataSize;
	ULONG compSize;
	long long fPosData;
	long long fPosWindow;
	long long fPosNextProcess;
};

struct URLRecord
{
	char recType; // 'P' for Process Record, 'U' for URL Record
	ULONG dataSize;
	ULONG compSize;
	long long fPosData;
	long long fPosMask;
	long long fPosControls;
	long long fPosNextURL;
};

struct windowRecord
{
	char recType; // 'W' for Window Record
	ULONG dataSize;
	ULONG compSize;
	long long fPosData;
	long long fPosMask;
	long long fPosControls;
	long long fPosNextWindow;
};


struct controlRecord
{
	char recType; // 'C' for Control Record
	DWORD controlIndex;
	ULONG dataSize;
	ULONG compSize;
	long long fPosData;
	long long fPosParent;
	long long fPosLeftSibling;
	long long fPosRightSibling;
	long long fPosNextControl;
};

struct controlElementRecord
{
	char recType; // 'L' for Control Element Record
	ULONG dataSize;
	ULONG compSize;
	long long fPosData;
	long long fPosNextControl;
};


struct maskRecord
{
	char recType; // 'M' for record type

	char maskName[128];
	char maskType[32];
	RECT imageRect;
	RECT maskRect;
	RECT match1Rect;
	RECT match2Rect;
	RECT match3Rect;
	RECT match4Rect;
	RECT match5Rect;
	RECT match6Rect;

	long long fPosData;
	ULONG dataSize;
	ULONG compSize;

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bmiHeader;

	long long fPosNextMask;
};

/**************************************

This is JSON structure for ProcessData
{
	"SysName": "ProcessSystemName",
	"ID": 1000,
	"UserName": "Process User friendly Name",
	"Description": "Description of Application",
	"Type": "Utility/Application",
	"Platform": "MainFrame/Desktop/Web Application/Citrix",
	"HomeURL": "URL",
	"IgnoreEvents": false,
	"IgnoreVideo": false
}


[{
	"ID": 1000,
	"Title": "Window Title",
	"WindowName": "Window Name",
	"Description": "Description",
	"ClassName": "ClassName",
	"AutomationID": "Automation ID",
	"Type": "Type",
	"MatchRule": "Match Rule",
	"IncludeClassName": false,
	"IncludeAutoID": false,
	"Variables": [{
			"VarName": "Name",
			"Rule": "Rule"
		},
		{
			"VarName": "Name2",
			"Rule": "Rule"
		}
	],
	"Controls": [{
			"ControlID": xxx,
			"ID": XXX,
			"Name": "Control Name",
			"Description": "Control Description",
			"Type": xxxx,
			"Rect": {
				"Left": xxx,
				"Top": xxx,
				"Right": xxx,
				"Bottom": xxx
				},
			"ClassName": "Control Classname",
			"AutoID": "Control Automation ID",
			"ClickAnnotation": "Control Automation ID",
			"EnterAnnotation": "Control Automation ID",
			"UseRect": true,
			"UseName": true,
			"UseValue": true,
			"Disable": true,
			"CreatesWindow": true,
			"ClosesWindow": true
			}
		],
	"Layouts": [
		{
			"Name": "Name of Layout",
			"Controls":[
				{
				"ControlID": xxxx
				"Controls": [
				]
				}
			]
		},
	"AnnotationRule": "Rule"
}]





********************************************/

#pragma pack(pop)


