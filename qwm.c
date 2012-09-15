// Copyright (c) 2007 FC - Licenced under the GPLv3.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),_exit(1),0)
#define AS(_) (sizeof(_)/sizeof(*_))
typedef unsigned int I;
#define or else if

#include <X11/Xutil.h>
#include <stdio.h>
#include "x11.c"
#include "bus.c"
#include "nap.c"

typedef struct { XID wids[50],count,left,fswid,tid; } Desk;
typedef struct { I reverse,side,size; } Dock;

I gKeys[]={103,23,24,25,53,54,41,9,10};
Dock gDocks[]={{0,0,30}};
OpTxt gOps[11]={
	{1,"_ _ : Request version"},
	{2,"keyid keycode : Change keycode"},
	{3,"dockid reverse : Change dock reverse value"},
	{4,"dockid side : Change dock side, 0/L 1/R 2/T 3/B"},
	{5,"dockid size% : Change dock size%"},
	{6,"_ _ : Request key count"},
	{7,"_ _ : Request dock count"},
	{8,"keyid _ : Request keycode"},
	{9,"dockid _ : Request dock reverse value"},
	{10,"dockid _ : Request dock side"},
	{11,"dockid _ : Request dock size%"}};

void adjustSize(XID wid,I* pW,I* pH) { // Use hints for adjustment.
	long mask;
	XSizeHints h;
	if(!XGetWMNormalHints(gpDisplay,wid,&h,&mask)) return;
	I incW=1,incH=1,ratioW=*pW,ratioH=*pH,maxW=*pW,maxH=*pH;
	if(h.flags&PResizeInc) incW=h.width_inc,incH=h.height_inc;
	if(h.flags&PMaxSize) maxW=h.max_width,maxH=h.max_height;
	if(h.flags&PAspect) ratioW=h.min_aspect.x,ratioH=h.min_aspect.y;
	if(ratioW>ratioH) { // Below: 2 corresponds to border width.
		if(maxW<*pW) *pW=maxW,*pH=(maxH<*pH)?maxH:*pH;
		else *pH=*pW*ratioH/ratioW,*pH-=(*pH-2)%incH-2,*pW-=(*pW-2)%incW-2;
	} else {
		if(maxH<*pH) *pH=maxH,*pW=(maxW<*pW)?maxW:*pW;
		else *pW=*pH*ratioW/ratioH,*pW-=(*pW-2)%incW-2,*pH-=(*pH-2)%incH-2;
	}
}

void renderWindow(XID wid,I x,I y,I* pW,I* pH,I side) {
	I w=*pW,h=*pH;
	*pW-=(side==4 || side<2)?10:6; // Set initial x margin.
	*pH-=(side>1)?10:6; // Set initial y margin.
	adjustSize(wid,pW,pH);
	x+=(side<2 || side==4)?(w-*pW)/2:6; // X centering if needed.
	y+=(side>1)?(h-*pH)/2:6; // Y centering if needed.
	XMoveResizeWindow(gpDisplay,wid,x,y,*pW,*pH);
	*pW+=(side==4 || side<2)?10:6; // Unset margin for final width.
	*pH+=(side>1)?10:6; // Unset margin for final height.
}

void renderDesk(XID sid,Desk* pDesk) {
	if(!pDesk->count) return; // No window, nothing to do.
	I H=DisplayHeight(gpDisplay,sid),W=DisplayWidth(gpDisplay,sid);
	I X=0,Y=0,DW,DH,w,h,x,y,k=1,d=0; // k: window index, d: dock index.
	if(pDesk->count==1) goto renderPrimary;
setDock:
	if(d==AS(gDocks)) goto renderPrimary; // Docks full.
	DH=gDocks[d].size*DisplayHeight(gpDisplay,sid)/100; // Dock height%.
	DW=gDocks[d].size*DisplayWidth(gpDisplay,sid)/100; // Dock width%.
	if(gDocks[d].side<2) {
		if(DH>=200) DH*=0.8; // Geom. correction (screen is rectangular).
		y=gDocks[d].reverse?Y+H-DH:Y;
		W-=DW;
		x=gDocks[d].side==1?X+W:X;
		if(!gDocks[d].side) X+=DW;
	} else {
		x=gDocks[d].reverse?X+W-DW:X;
		H-=DH;
		y=gDocks[d].side==3?Y+H:Y;
		if(gDocks[d].side==2) Y+=DH;
	}
renderWindow:
	h=DH,w=DW;
	renderWindow(pDesk->wids[k++],x,y,&w,&h,gDocks[d].side);
	if(k==pDesk->count) goto renderPrimary;
	if(gDocks[d].side>1) x=gDocks[d].reverse?x-w:x+w;
	else y=gDocks[d].reverse?y-h:y+h;
	if((gDocks[d].reverse &&
		((gDocks[d].side<2 && y<Y) || (gDocks[d].side>1 && x<X)))
	|| ((gDocks[d].side<2 && y+DW*(DW>=200?0.6:1)>H+Y)
		|| (gDocks[d].side>1 && x+DH>W+X))) {++d;goto setDock;}
	goto renderWindow;
renderPrimary:
	if(k==1) X=Y=50,W-=100,H-=100; // If primary only, decrease margin.
	renderWindow(pDesk->wids[0],X,Y,&W,&H,4);
	pDesk->fswid=0; // Unset fullscreen window.
	pDesk->left=k;
	for(;k<pDesk->count;++k) // Discard remaining windows.
		XMoveWindow(gpDisplay,pDesk->wids[k],10000,10000);
}

void renderDesks(Desk* pDesk) {
	for(XID sid=0;sid<SNB;++sid) renderDesk(sid,pDesk+sid);
}

XID createToolbar(XID rid,XID sid) {
	char bitmap[]={0,0,0,0,0,0,0xF8,0x4F,0x15,0xFE,0,0,0,0x20,0x82,
	0,0,0x40,0,0x82,0,0,0,0xA0,0x83,0,0,0x40,0x80,0xFE,0,0,0,0xA0,
	0x20,0,0,0x40,0x80,0x20,0xFE,0x03,0x80,0xAA,0x3F,0,0,0,0,0};
	Pixmap pixmap=XCreatePixmapFromBitmapData(gpDisplay,
		rid,bitmap,40,10,~0,0,DefaultDepth(gpDisplay,sid));
	XSetWindowAttributes xswa;
	xswa.cursor=XCreateFontCursor(gpDisplay,90); // '+' shaped pointer.
	xswa.event_mask=ButtonPressMask;
	xswa.background_pixmap=pixmap;
	xswa.override_redirect=1;
	xswa.border_pixel=~0;
	XID wid=XCreateWindow(gpDisplay,rid,10000,10000,31,10,1,
		CopyFromParent,InputOutput,CopyFromParent,CWEventMask
		|CWBackPixmap|CWOverrideRedirect|CWBorderPixel|CWCursor,&xswa);
	XFreePixmap(gpDisplay,pixmap);
	XFreeCursor(gpDisplay,xswa.cursor);
	XMapWindow(gpDisplay,wid);
	return wid;
}

I setupWindow(XID wid,XID* pSid,Desk* pDesk,XID* pFS) {
	XWindowAttributes xwa={.root=0};
	if(XGetWindowAttributes(gpDisplay,wid,&xwa)==BadWindow
	|| xwa.class==InputOnly || xwa.override_redirect
	|| xwa.map_state==IsUnmapped || !xwa.root) return 0; // 0: failed.
	XSelectInput(gpDisplay,wid,EnterWindowMask|FocusChangeMask);
	XSetWindowBorderWidth(gpDisplay,wid,1);
	if(RootWindow(gpDisplay,*pSid)!=xwa.root) // Recompute sid if needed.
		for(*pSid=0;*pSid<SNB;++*pSid)
			if(RootWindow(gpDisplay,*pSid)==xwa.root) break;
	if(pDesk[*pSid].count>=50) { log("Too many windows");return 0; }
	pDesk[*pSid].wids[pDesk[*pSid].count++]=wid; // Registration.
	*pFS=xwa.width==DisplayWidth(gpDisplay,*pSid) 
		&& xwa.height==DisplayHeight(gpDisplay,*pSid); // 1 if fullscreen.
	return 1;
}

void setupDesk(XID sid,Desk* pDesk) {
	XID rid=RootWindow(gpDisplay,sid),*pCid,pid,count,fs;
	XSelectInput(gpDisplay,rid,SubstructureNotifyMask);
	Cursor cursor=XCreateFontCursor(gpDisplay,38);
	XDefineCursor(gpDisplay,rid,cursor); // Dot shaped pointer.
	pDesk->tid=createToolbar(rid,sid);
	pDesk->count=0;
	if(XQueryTree(gpDisplay,rid,&rid,&pid,&pCid,(I*)&count))
		for(I i=0;i<count;++i) setupWindow(pCid[i],&sid,pDesk,&fs);
	renderDesk(sid,pDesk+sid); // Set pDesk->{fswid,left}.
	XFreeCursor(gpDisplay,cursor);
	XFree(pCid);
}

I getFocusWID(XID* pWid) {
	I revertTo;
	XGetInputFocus(gpDisplay,pWid,&revertTo);
	return (*pWid==None || *pWid==PointerRoot)?(*pWid=0):1;
}

I getWindowIndex(XID wid,Desk* pDesk,XID* pSid,XID* pI) { 
	for(XID sid=0;sid<SNB;++sid) // The window must be registered.
		for(I i=0;i<pDesk[sid].count;++i)
			if(pDesk[sid].wids[i]==wid) { *pSid=sid;*pI=i;return 1; }
	return 0; // Not found.
}

void cycleFocus(Desk* pDesk,I reset) {
	XID sid,i,wid=getFocusWID(&wid) && getWindowIndex(wid,pDesk,&sid,&i)?
		pDesk[sid].wids[reset?0:(++i%pDesk[sid].left)]:
		pDesk[0].count?pDesk[0].wids[0]:RID;
	XSetInputFocus(gpDisplay,wid,RPR,NOW);
}

void cyclePrimary(Desk* pDesk) {
	static I k=0;
	XID wid,sid,i;
	if(!getFocusWID(&wid) || !getWindowIndex(wid,pDesk,&sid,&i)
	|| pDesk[sid].left==pDesk[sid].count) return;
	k=(k>=pDesk[sid].left && k+1<pDesk[sid].count)?k+1:pDesk[sid].left;
	wid=pDesk[sid].wids[0];
	pDesk[sid].wids[0]=pDesk[sid].wids[k]; // Swap primary/wids[k].
	pDesk[sid].wids[k]=wid;
	renderDesk(sid,pDesk+sid);
	if(!i) XSetInputFocus(gpDisplay,pDesk[sid].wids[0],RPR,NOW);
}

I swapPrimary(Desk* pDesk) { // Swap focus/primary windows.
	XID wid,sid,i=0;
	if(getFocusWID(&wid) && getWindowIndex(wid,pDesk,&sid,&i) && i)
		pDesk[sid].wids[i]=pDesk[sid].wids[0],
		pDesk[sid].wids[0]=wid,
		renderDesk(sid,pDesk+sid);
	return wid && i;
}

void killFocus(XID wid) {
	if(!wid) getFocusWID(&wid);
	if(wid) XKillClient(gpDisplay,wid);
}

void closeFocus(void) {
	XID wid,count;
	XClientMessageEvent event;
	if(!getFocusWID(&wid)) return; // No focus window.
	Atom* pAtom=0;
	if(XGetWMProtocols(gpDisplay,wid,&pAtom,(I*)&count))
		for(I i=0;i<count;++i)
			if(pAtom[i]==XInternAtom(gpDisplay,"WM_DELETE_WINDOW",0))
				goto close;
	killFocus(wid); // Alternative if WM_DELETE is not supported.
	goto out;
close: // Send a proper close request.
	event.type=ClientMessage;
	event.send_event=1;
	event.display=gpDisplay;
	event.window=wid;
	event.message_type=XInternAtom(gpDisplay,"WM_PROTOCOLS",0);
	event.format=32;
	event.data.l[0]=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",0);
	XSendEvent(gpDisplay,wid,0,NoEventMask,(XEvent*)&event);
out:
	if(pAtom) XFree(pAtom);
}

void toggleFocusFullScreen(Desk* pDesk) {
	XID wid,sid,i;
	if(!getFocusWID(&wid) || !getWindowIndex(wid,pDesk,&sid,&i)) return;
	if(pDesk[sid].fswid) { renderDesk(sid,pDesk+sid);return; } // Off.
	XMoveResizeWindow(gpDisplay,wid,-1,-1, // -1,-1 to hide the border.
		DisplayWidth(gpDisplay,sid),DisplayHeight(gpDisplay,sid));
	XRaiseWindow(gpDisplay,wid);
	pDesk[sid].fswid=wid; // Tag as fullscreen window.
}

void hideWindows(Desk* pDesk) { // Or show all if already hidden.
	static I hidden=0;
	if(hidden)
		for(XID sid=0;sid<SNB;++sid) renderDesk(sid,pDesk+sid); // Show.
	else
		for(XID sid=0;sid<SNB;++sid) // Hide.
			for(I i=0;i<pDesk[sid].count;++i)
				XMoveWindow(gpDisplay,pDesk[sid].wids[i],10000,10000); 
	hidden=!hidden;
}

void anchorToolbar(XID wid,Desk* pDesk) { // Except in fullscreen.
	XID sid,i;
	if(!getWindowIndex(wid,pDesk,&sid,&i)) return;
	XID tid=pDesk[sid].tid;
	if(pDesk[sid].fswid) { XLowerWindow(gpDisplay,tid);return; }
	XWindowAttributes xwa;
	XGetWindowAttributes(gpDisplay,wid,&xwa);
	(!i && pDesk[sid].left<pDesk[sid].count)?
		XMoveResizeWindow(gpDisplay,tid,xwa.x+xwa.width-41,xwa.y,41,10):
		XMoveResizeWindow(gpDisplay,tid,xwa.x+xwa.width-31,xwa.y,31,10);
	XRaiseWindow(gpDisplay,tid);
}

void handleKeyPress(I keycode,Desk* pDesk,I* pResume) {
	if(keycode==gKeys[1]) cycleFocus(pDesk,0);
	if(keycode==gKeys[2]) swapPrimary(pDesk);
	if(keycode==gKeys[3]) cycleFocus(pDesk,1); // Reset focus.
	if(keycode==gKeys[4]) killFocus(0);
	if(keycode==gKeys[5]) closeFocus();
	if(keycode==gKeys[6]) toggleFocusFullScreen(pDesk);
	if(keycode==gKeys[7]) hideWindows(pDesk);
	if(keycode==gKeys[8]) cyclePrimary(pDesk);
	*pResume=keycode!=gKeys[0];
}

void handleMapNotify(XID wid,Desk* pDesk) {
	XID sid=0,fs;
	if(!setupWindow(wid,&sid,pDesk,&fs)) return;
	XSetInputFocus(gpDisplay,wid,RPR,NOW);
	if(fs) toggleFocusFullScreen(pDesk);
	or(!swapPrimary(pDesk)) renderDesk(sid,pDesk+sid);
}

void handleUnmapNotify(XID wid,Desk* pDesk) {
	XID sid,i;
	if(!getWindowIndex(wid,pDesk,&sid,&i)) return;
	pDesk[sid].wids[i]=pDesk[sid].wids[--pDesk[sid].count];
	if(!getFocusWID(&wid))
		XMoveWindow(gpDisplay,pDesk[sid].tid,10000,10000),
		cycleFocus(pDesk,1);
	renderDesk(sid,pDesk+sid);
}

void handleMsgs(Desk* pDesk) {
	Msg msg;
	while(getMsg(&msg,0))
		if(msg.op==1) reply(msg,3,9); // Reply v3.9.
		or(msg.op==2 && msg.key<AS(gKeys))
			ungrabCombo(gKeys[msg.key]), // Cancel old combo.
			gKeys[msg.key]=msg.value,
			grabCombo(msg.value,1);
		or(msg.op==3 && msg.key<AS(gDocks))
			gDocks[msg.key].reverse=msg.value,
			renderDesks(pDesk);
		or(msg.op==4 && msg.value<4 && msg.key<AS(gDocks))
			gDocks[msg.key].side=msg.value,
			renderDesks(pDesk);
		or(msg.op==5 && msg.key<AS(gDocks))
			gDocks[msg.key].size=msg.value,
			renderDesks(pDesk);
		or(msg.op==6) reply(msg,AS(gKeys),0);
		or(msg.op==7) reply(msg,AS(gDocks),0);
		or(msg.op==8 && msg.key<AS(gKeys))
			reply(msg,gKeys[msg.key],0);
		or(msg.op==9 && msg.key<AS(gDocks))
			reply(msg,gDocks[msg.key].reverse,0);
		or(msg.op==10 && msg.key<AS(gDocks))
			reply(msg,gDocks[msg.key].side,0);
		or(msg.op==11 && msg.key<AS(gDocks))
			reply(msg,gDocks[msg.key].size,0);
}

void handleEvents(Desk* pDesk,I* pResume) {
	XID wid;
	XEvent event;
	while(*pResume && XPending(gpDisplay))
		if(XNextEvent(gpDisplay,&event),event.type==KeyPress)
			handleKeyPress(event.xkey.keycode,pDesk,pResume);
		or(event.type==ButtonPress)
			event.xbutton.button==1?swapPrimary(pDesk):
			event.xbutton.button==2?toggleFocusFullScreen(pDesk):
			event.xbutton.button==3?closeFocus():cyclePrimary(pDesk);
		or(event.type==ConfigureNotify && getFocusWID(&wid)
			&& wid==event.xconfigure.window) anchorToolbar(wid,pDesk);
		or(event.type==MapNotify && !event.xmap.override_redirect)
			handleMapNotify(event.xmap.window,pDesk);
		or(event.type==UnmapNotify)
			handleUnmapNotify(event.xmap.window,pDesk);
		or(event.type==EnterNotify)
			XSetInputFocus(gpDisplay,event.xcrossing.window,RPR,NOW);
		or(event.type==FocusIn && event.xfocus.detail!=NotifyPointer)
			XSetWindowBorder(gpDisplay,event.xfocus.window,~0),
			anchorToolbar(event.xfocus.window,pDesk);
		or(event.type==FocusOut && event.xfocus.detail!=NotifyPointer)
			XSetWindowBorder(gpDisplay,event.xfocus.window,0x88888888);
}

int main(int c,char** p) {
	openBus(gOps,11); // Bus failure is not fatal.
	openXSession() || E("Cannot open X session");
	Desk desks[SNB]; // Allocate desks (1 desk=1 screen).
	for(I sid=0;sid<SNB;++sid) setupDesk(sid,desks+sid);
	for(I i=0;i<AS(gKeys);++i) grabCombo(gKeys[i],0);
	cycleFocus(desks,1); // Reset focus.
	handleMsgs(desks); // Handle possibly pending messages.
	for(c=1;c;nap(0,0,XFD)==1?handleMsgs(desks):handleEvents(desks,&c));
	closeXSession();
	closeBus();
	return 0;
}
