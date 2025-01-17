#pragma once
#include <wx/wx.h>
#include "wx/dcbuffer.h"
#include <list>


void DrawBevel(wxDC& dc, wxColor color, wxColor borderColor, wxPoint top, wxSize size, int depth, bool raisedFlg);
void DrawLED(wxDC& dc, wxColor color, int left, int top, int width, int height);
void DrawVideo(wxDC& dc, wxColor color, int left, int top);
void DrawAudio(wxDC& dc, wxColor color, int left, int top);
void DrawBell(wxDC& dc, wxColor color, int left, int top);
void DrawMultipleMonitors(wxDC& dc, wxColor color, int left, int top);
void DrawSingleMonitor(wxDC& dc, wxColor color, int left, int top);
void DrawLEDString(wxDC& dc, char *text, wxColor color, int left, int top, int thickness, int height, int gap);
void GetScreenDataDetails(std::list <struct monitorDetails *> *pMonitorList);

struct monitorDetails
{
	HMONITOR hMon;
	tagMONITORINFOEXA monitorInfo;
	bool bSelect;
};


