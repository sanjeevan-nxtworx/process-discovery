#pragma once
#include <wx/wx.h>
#include <mutex>
#include <string>
#include "MMHeader.h"
using namespace std;

class PictureDisplay : public wxPanel
{
	wxPanel *pParent;
	struct ResizeScreenBuffer *pResizeBuffer;
	struct ResizeScreenBuffer *pThumbBuffer;
	int thumbXPos;
	mutex lockBitmap;
	string strAnnotation;
	string strNotes;
	bool	bShowAnnotation;

public:
	PictureDisplay(wxPanel * parent, wxWindowID id, const wxPoint & position, const wxSize & size);
	~PictureDisplay();
	struct ResizeScreenBuffer **LockBitmap();
	struct ResizeScreenBuffer **GetThumbBuffer();
	void SetThumbXPos(int xPos);
	void UnlockBitmap();
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);
	void SetAnnotation(string value);
	void SetNotes(string value);
	void SetShowSubTitle(bool bFlg);
	DECLARE_EVENT_TABLE()
};

