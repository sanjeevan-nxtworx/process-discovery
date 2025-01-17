#include "stdafx.h"
#include "LineConnectionInfo.h"


LineConnectionInfo::LineConnectionInfo()
{
	ZeroMemory(&fromRect, sizeof(RECT));
	ZeroMemory(&toRect, sizeof(RECT));
	ZeroMemory(&wndRect, sizeof(RECT));
}


LineConnectionInfo::~LineConnectionInfo()
{
}


void LineConnectionInfo::SetFromRect(LPRECT value)
{
	fromRect.left = value->left;
	fromRect.right = value->right;
	fromRect.top = value->top;
	fromRect.bottom = value->bottom;
}

void LineConnectionInfo::SetToRect(LPRECT value)
{
	toRect.left = value->left;
	toRect.right = value->right;
	toRect.top = value->top;
	toRect.bottom = value->bottom;
}

void LineConnectionInfo::SetWndRect(LPRECT value)
{
	wndRect.left = value->left;
	wndRect.right = value->right;
	wndRect.top = value->top;
	wndRect.bottom = value->bottom;
}

LPRECT LineConnectionInfo::GetFromRect()
{
	return &fromRect;
}

LPRECT LineConnectionInfo::GetToRect()
{
	return &toRect;
}

LPRECT LineConnectionInfo::GetWndRect()
{
	return &wndRect;
}