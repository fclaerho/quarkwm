// Copyright (c) 2007 FC - Licenced under the GPLv3.

#include <X11/Xlib.h>

#define XFD ConnectionNumber(gpDisplay)
#define DSH DisplayHeight(gpDisplay,0)
#define DSW DisplayWidth(gpDisplay,0)
#define RID RootWindow(gpDisplay,0)
#define RGC DefaultGC(gpDisplay,0)
#define SNB ScreenCount(gpDisplay)
#define RPR RevertToPointerRoot
#define GMA GrabModeAsync
#define NOW CurrentTime

Display* gpDisplay=0;

int handleXError(Display* pDisplay,XErrorEvent* pEvent) { return 0; }

I openXSession(void) {
	XSetErrorHandler(handleXError);
	gpDisplay=XOpenDisplay(0);
	return gpDisplay!=0;
}

void closeXSession(void) { if(gpDisplay) XCloseDisplay(gpDisplay); }

void ungrabCombo(I keycode) {
	XUngrabKey(gpDisplay,keycode,Mod4Mask|Mod2Mask,RID);
	XUngrabKey(gpDisplay,keycode,Mod4Mask,RID);
}

void grabCombo(I keycode,I flush) {
	XGrabKey(gpDisplay,keycode,Mod4Mask|Mod2Mask,RID,1,GMA,GMA);
	XGrabKey(gpDisplay,keycode,Mod4Mask,RID,1,GMA,GMA);
	if(flush) XFlush(gpDisplay);
}
