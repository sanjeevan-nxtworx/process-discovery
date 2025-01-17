#pragma once


#ifndef __BASE_H 
// Make sure to only declare these classes once 
#define __BASE_H 
#include "resource.h"
class MainApp : public wxApp // MainApp is the class for our application 
{
	// MainApp just acts as a container for the window, or frame in MainFrame
public:
	virtual bool OnInit();
};

DECLARE_APP(MainApp)

#endif