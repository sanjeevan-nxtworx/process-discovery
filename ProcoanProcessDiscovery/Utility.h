#pragma once
#include <string>
#include "ConfigClass.h"
#include "ExecutionManager.h"
#include "RecorderWindow.h"
#include "SPDLogFile.h"
#include <wx/wx.h>
#include "KeyState.h"
#include "TransparentWindow.h"

int CompareBitmapMemory(long *src, long *dest, long numBytes);
int CopyBitmapMemory(long *dest, long *src, long numBytes);
int CopyBitmapMemoryMultiple(long *dest1, long *dest2, long *src, long numBytes);
std::wstring s2ws(const std::string& s);
std::string ws2s(const std::wstring &ws);
bool RectInRect(LPRECT wndRect, LPRECT rect);
std::string ProcessIdToName(DWORD processId);

long CompressScreenData(char *dest, long *src, long size);
long PackData(long *dest, long *src, int height, int width, long *dest2 = NULL);
long UnpackData(long *dest, long *src, int height, int width);
long CompareData(long *dest, long *src, int height, int width);
long UnpackCompareData(long *dest, long *src, int width, long numBlocks);
long CompressCompareBuffer(char *dest, long *src, long numBlocks);
long ExpandScreenData(long *dest, unsigned char *src, long size);
long ExpandCompareData(long *dest, char *src, long numBlocks);

string FindReplaceString(string str, string strFind, string strReplace);
string EscapeString(string strVal);
string EscapeJSonString(string str);
string EscapeJSonAnnotationString(string str);
string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
string ConvertBSTRToMBS(BSTR bstr);

ConfigClass *GetConfigClass();
RecorderWindow *GetRecorderWnd();
ExecutionManager *GetExecutionManager();
//LogFile *CreateLogFile(wxString logFileName);
SPDLogFile *CreateSPDLogFile(wxString logFileName);
RecorderWindow *CreateRecorderWindow();
TransparentWindow* CreateTransparentWindow();

ExecutionManager *CreateExecutionManager();
ConfigClass *CreateConfigClass(wxString configFileName);
void DeleteConfigClass();
//LogFile *GetLogFile();
void DeleteRecorderWindow();
void DeleteExecutionManager();
KeyState *GetKeyState();
void DeleteKeyState();
void DeleteSPDLogFile();
wstring getexepath();
string getcurrentpath();
VOID startup(LPCTSTR lpApplicationName, STARTUPINFO *pSI, PROCESS_INFORMATION *pPI);
vector<HWND> getToplevelWindows(DWORD pID);
void GetAllWindowsFromProcessID(DWORD dwProcessID, std::vector <HWND> &vhWnds);
vector <HWND> getChildWindows(HWND hWnd);
string ProcessIdToName(DWORD processId);
vector<struct processdata *> getProcesses();
vector<string> split(const string& i_str, const string& i_delim);

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

extern wxFrame *pVideoFrame;
extern wxPanel *pPicControlPanel;
