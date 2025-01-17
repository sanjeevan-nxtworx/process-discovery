#pragma once
#include <wx/wx.h>
#include <wx/event.h>

enum
{
	BTN_UPDATE_PROCESS = wxID_HIGHEST + 1, // declares an id which will be used to call our button
	LIST_PROCESSES = wxID_HIGHEST + 2,
	PROCESS_ID = wxID_HIGHEST + 3,
	PROCESS_VALUE_NAME = wxID_HIGHEST + 4,
	PROCESS_NAME = wxID_HIGHEST + 5,
	PROCESS_DESC = wxID_HIGHEST + 6,
	PROCESS_TYPE = wxID_HIGHEST + 7,
	PROCESS_PLATFORM = wxID_HIGHEST + 8,
	PROCESS_IGNORE_EVENTS = wxID_HIGHEST + 9,
	PROCESS_IGNORE_VIDEO = wxID_HIGHEST + 10,
	BTN_UPDATE_WINDOW = wxID_HIGHEST + 11,
	BTN_SAVE = wxID_HIGHEST + 12,
	BTN_CLOSE_SPY = wxID_HIGHEST + 13,
	LIST_URLS = wxID_HIGHEST + 14,
	BTN_UPDATE_URLS = wxID_HIGHEST + 15,
	URL_TEXT = wxID_HIGHEST + 16,
	URL_RULE = wxID_HIGHEST + 17,
	URL_RESULT = wxID_HIGHEST + 18,
	URL_ID = wxID_HIGHEST + 19,
	URL_NAME = wxID_HIGHEST + 20,
	URL_DESC = wxID_HIGHEST + 21,
	URL_APPNAME = wxID_HIGHEST + 20,
	URL_APPDESC = wxID_HIGHEST + 21,
	URL_EXECUTE_BUTTON = wxID_HIGHEST + 22,
	WINDOW_PROCESS_LIST = wxID_HIGHEST + 23,
	WINDOW_TREE = wxID_HIGHEST + 24,
	BTN_UPDATE_PROCESS_WINDOW = wxID_HIGHEST + 25,
	BTN_WINDOW_CONTROLS = wxID_HIGHEST + 26,
	BTN_UPDATE_WINDOW_CONTROLS = wxID_HIGHEST + 27,
	CONTROL_TREE = wxID_HIGHEST + 28,
	BTN_BACKTO_PROCESS = wxID_HIGHEST + 29,
	BTN_SAVE_PROCESS = wxID_HIGHEST + 30,
	BTN_SAVE_WINDOW = wxID_HIGHEST + 31,
	BTN_CONTROL_SAVE = wxID_HIGHEST + 32,
	BTN_MASK_WINDOW = wxID_HIGHEST + 33,
	BTN_CONTROL_BACKTO_WINDOW = wxID_HIGHEST + 34,
	BTN_CONTROL_BACKTO_PROCESS = wxID_HIGHEST + 35,
	BTN_MASK_SAVE = wxID_HIGHEST + 36,
	BTN_MASK_BACKTO_CONTROLS = wxID_HIGHEST + 37,
	BTN_MASK_BACKTO_WINDOW = wxID_HIGHEST + 38,
	BTN_MASK_BACKTO_PROCESS = wxID_HIGHEST + 39,
	BTN_SAVE_LAYOUT = wxID_HIGHEST + 67,
	CMB_LAYOUT_LIST = wxID_HIGHEST + 40,
	BTN_OPEN_SCREEN = wxID_HIGHEST + 41,
	BTN_NEW_MASK = wxID_HIGHEST + 42,
	CMB_MASK_LIST = wxID_HIGHEST + 43,
	BTN_SAVE_MASK = wxID_HIGHEST + 44,
	BTN_SAVE_IMAGE1 = wxID_HIGHEST + 45,
	BTN_SAVE_IMAGE2 = wxID_HIGHEST + 46,
	BTN_SAVE_IMAGE3 = wxID_HIGHEST + 47,
	BTN_SAVE_IMAGE4 = wxID_HIGHEST + 48,
	BTN_SAVE_IMAGE5 = wxID_HIGHEST + 49,
	BTN_SAVE_IMAGE6 = wxID_HIGHEST + 50,
	URL_SAVE = wxID_HIGHEST + 51,
	URL_CONTROL = wxID_HIGHEST + 52,
	URL_ISHOME = wxID_HIGHEST + 53,
	URL_IGNORE_EVENTS = wxID_HIGHEST + 54,
	URL_IGNORE_VIDEO = wxID_HIGHEST + 55,
	URL_CMB_LAYOUT_LIST = wxID_HIGHEST + 56,
	BTN_SAVE_URL_LAYOUT = wxID_HIGHEST + 57,
	BTN_MASK_URL_WINDOW = wxID_HIGHEST + 58,
	BTN_URL_CONTROL_SAVE = wxID_HIGHEST + 59,
	BTN_CONTROL_BACKTO_URL = wxID_HIGHEST + 60,
	BTN_UPDATE_URL_CONTROLS = wxID_HIGHEST + 61,
	URL_CONTROL_LIST = wxID_HIGHEST + 62,
	BTN_WINDOW_EXECUTE = wxID_HIGHEST + 63,
	BTN_COPY_URL_WINDOW = wxID_HIGHEST + 64,
	BTN_PASTE_URL_WINDOW = wxID_HIGHEST + 65,
	BTN_CLEAR_URL_WINDOW = wxID_HIGHEST + 66,
	BTN_CAPTURE = wxID_HIGHEST + 68,
	CAPTURE_URLNAME = wxID_HIGHEST + 69,
	CAPTURE_CONTROLNAME = wxID_HIGHEST + 70,
	CAPTURE_CONTROLTYPE = wxID_HIGHEST + 71,
	CAPTURE_CONTROLANNOTATION = wxID_HIGHEST + 72,
	CAPTURE_CONTROLLIST = wxID_HIGHEST + 73,
	CAPTURE_DONE = wxID_HIGHEST + 74,
	TIMER_ID = wxID_HIGHEST + 75,
	CAPTURE_SAVE = wxID_HIGHEST + 76,
	WINDOW_LIST = wxID_HIGHEST + 77,
	WINDOW_CONTROL_LIST = wxID_HIGHEST + 78,
	WIN_ISMAIN = wxID_HIGHEST + 79,
};
