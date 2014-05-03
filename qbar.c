// copyright (c) 2007-2014 fclaerhout.fr, released under the MIT license.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),_exit(1),0)
typedef unsigned int I;
#define or else if
typedef char* S;

#include <stdio.h>
#include <time.h>
#include "x11.c"
#include "bus.c"
#include "font.c"
#include "nap.c"
#include "run.c"

I gOffset=1,gSide=3,gH,gKeys[]={26,33,47,56};
OpTxt gOps[6]={
	{1,"_ _ : Request version"},
	{2,"keyid keycode : Change keycode"},
	{6,"_ _ : Request key count"},
	{8,"keyid _ : Request keycode"},
	{12,"side offset% : Change position 0/L 1/R 2/T 3/B"},
	{14,"_ _ : Request position"}};
time_t gT;

void drawClock(I reset) {
	struct tm* pTm=localtime(&gT);
	char clock[6];
	I clockCount=strftime(clock,6,"%H:%M",pTm);
	clockCount || E("No clock value");
	I clockWidth=bigTextWidth(clock,clockCount);
	static char date[30];
	static I x=0,y=0,w=0,dateCount,dateWidth;
	XClearArea(gpDisplay,RID,x,y,w,gH,0); // Clear old area.
	if((!pTm->tm_hour && !pTm->tm_min) || reset) { // Updated once/day.
		dateCount=strftime(date,30,"%A %Y/%m/%d GMT%z",pTm);
		dateCount || E("No date value");
		dateWidth=smallTextWidth(date,dateCount);
		w=clockWidth>dateWidth?clockWidth:dateWidth;
		if(gSide==0) x=4,y=DSH*gOffset/100;
		or(gSide==1) x=DisplayWidth(gpDisplay,0)-w-4,y=DSH*gOffset/100;
		or(gSide==2) x=DSW*gOffset/100,y=4;
		or(gSide==3) x=DSW*gOffset/100,y=DisplayHeight(gpDisplay,0)-gH-4;
	}
	drawBigText(clock,clockCount,RID,RGC,x+(w-clockWidth)/2,y);
	drawSmallText(date,dateCount,RID,RGC,x,y+bigFontHeight());
	XFlush(gpDisplay);
}

void handleMsgs(void) {
	Msg msg;
	while(getMsg(&msg,0))
		if(msg.op==1) reply(msg,1,5); // Reply v1.5.
		or(msg.op==2 && msg.key<sizeof(gKeys)/sizeof(I))
			ungrabCombo(gKeys[msg.key]), // Cancel old combo.
			gKeys[msg.key]=msg.value,
			grabCombo(msg.value,1);
		or(msg.op==14) reply(msg,gSide,gOffset);
		or(msg.op==6) reply(msg,sizeof(gKeys)/sizeof(I),0);
		or(msg.op==8 && msg.key<sizeof(gKeys)/sizeof(I))
			reply(msg,gKeys[msg.key],0);
		or(msg.op==12 && msg.key<4 && msg.value<=100)
			gOffset=msg.value,
			gSide=msg.key,
			drawClock(1); // Force update.
}

void handleEvents(void) {
	XEvent event;
	while(XPending(gpDisplay))
		if(XNextEvent(gpDisplay,&event),event.type==KeyPress) {
			if(event.xkey.keycode==gKeys[0]) run("$XTERM");
			if(event.xkey.keycode==gKeys[1]) run("$PLAYER");
			if(event.xkey.keycode==gKeys[2]) run("$MAILER");
			if(event.xkey.keycode==gKeys[3]) run("$BROWSER");
		} or(event.type==Expose && !event.xexpose.count)
			drawClock(0);
	while(extract()); // Get back dead processes.
}

int main(int c,S* p) {
	openBus(gOps,6); // Bus failure is not fatal.
	openXSession() || E("Cannot open X session");
	(loadSmallFont() && loadBigFont()) || E("Cannot load fonts");
	XSelectInput(gpDisplay,RID,ExposureMask);
	XSetForeground(gpDisplay,RGC,~0); // Render text in white.
	for(I i=0;i<sizeof(gKeys)/sizeof(I);++i) grabCombo(gKeys[i],0);
	gH=bigFontHeight()+smallFontHeight();
	gT=time(0);
	drawClock(1); // Draw the clock once before the loop.
	handleMsgs(); // Handle possibly pending messages.
	for(;;)
		switch(nap(60-(gT=time(0))%60,0,XFD)) {
			case 0: gT+=60,drawClock(0);break;
			case 1: handleMsgs();break;
			case 2: handleEvents();break;
		}
	return 0;
}
