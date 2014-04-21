// Copyright (c) 2007 FC - Licenced under the GPLv3.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),exit(1),0);
typedef unsigned int I;

#include <stdlib.h>
#include <stdio.h>
#include "x11.c"

int main(int c,char** p) {
	if(c<2) E("Usage: %s [color value] (e.g. 0 for black)",p[0]);
	I color=atoi(p[1]);
	openXSession() || E("Cannot open X session");
	for(XID sid=0;sid<SNB;++sid)
		XSetWindowBackground(gpDisplay,RootWindow(gpDisplay,sid),color),
		XClearWindow(gpDisplay,RootWindow(gpDisplay,sid));
	closeXSession();
	return 0;
}
