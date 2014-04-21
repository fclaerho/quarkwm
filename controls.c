// Copyright (c) 2007 FC - Licenced under the GPLv3.

#define DisabledColor 0x55555555
#define EnabledColor 0x77777777

#include <X11/Xutil.h>
#include <string.h>

typedef struct { I count; S s; } B; // String buffer.
typedef struct { I type,w,h; } Any;
typedef struct { I type,w,h,hidden; } Bar;
typedef struct { I type,w,h,font,align; B b; } Label;
typedef struct { I type,w,h; XID wid; } Input;
typedef struct { I type,w,h; XID wid; I font,align,c; B b; } Cmd;
typedef union { I type; Any any; Input input; // Interfaces.
	Label label; Bar bar; Cmd cmd; } Control;

static XID createWindow(XID pid,I w,I h) {
	XSetWindowAttributes xswa;
	xswa.event_mask=ExposureMask|KeyPressMask
		|ButtonPressMask|EnterWindowMask|FocusChangeMask;
	xswa.do_not_propagate_mask=KeyPressMask|ButtonPressMask;
	xswa.background_pixel=DisabledColor;
	xswa.border_pixel=0x88888888;
	xswa.override_redirect=1;
	XID wid=XCreateWindow(gpDisplay,pid,10000,10000,w,h,1,
		CopyFromParent,InputOutput,CopyFromParent,
		CWEventMask|CWDontPropagate|CWBackPixel
		|CWBorderPixel|CWOverrideRedirect,&xswa);
	XMapWindow(gpDisplay,wid);
	return wid;
}

static I textWidth(B b,I font) {
	if(font==0) return smallTextWidth(b.s,b.count);
	or(font==1) return mediumTextWidth(b.s,b.count);
	return bigTextWidth(b.s,b.count);
}

static I textHeight(I font) {
	if(font==0) return smallFontHeight();
	or(font==1) return mediumFontHeight();
	return bigFontHeight();
}

static void drawText(B b,I font,XID wid,GC gc,I x,I y) {
	if(font==0) drawSmallText(b.s,b.count,wid,gc,x,y);
	or(font==1) drawMediumText(b.s,b.count,wid,gc,x,y);
	else drawBigText(b.s,b.count,wid,gc,x,y);
}

Label getLabel(I font,I align,S s) {
	(font<3 && align<3 && s) || E("Ctor error");
	B b={.count=strlen(s),.s=s};
	return (Label){.type=0,.w=textWidth(b,font),
		.h=textHeight(font),.font=font,.align=align,.b=b};
}

static void renderLabel(Label label,XID wid,GC gc,I x,I y,I w) {
	if(label.align==1) x+=(w-label.w)/2;
	or(label.align==2) x+=(w-label.w);
	drawText(label.b,label.font,wid,gc,x,y);
}

Bar getBar(I hidden) { return (Bar){.type=1,.w=0,.h=7,.hidden=hidden}; }

static void renderBar(Bar bar,XID wid,GC gc,I x,I y,I w) {
	if(bar.hidden) return;
	XDrawLine(gpDisplay,wid,gc,x,y+3,x+w-1,y+3);
}

Cmd getCmd(XID pid,I font,I align,I max,S s) {
	(font<3 && align<3 && max) || E("Ctor error");
	B b={.count=max,.s=malloc(max+1)};
	for(I i=0;i<max;++i) b.s[i]='W'; // Allow to estimate max width.
	b.s[max]=0;
	Cmd input={.type=2,.font=font,.align=align,.b=b,.c=0};
	input.h=textHeight(font)+6;
	input.w=textWidth(b,font)+4;
	if(s) { // Default value.
		I length=strlen(s);
		memcpy(b.s,s,length>max?max:(length+1));
		input.c=length>max?max:length;
	}
	input.wid=createWindow(pid,input.w-2,input.h-4);
	return input;
}

static void renderCmdContent(Cmd input,GC gc) {
	XClearWindow(gpDisplay,input.wid);
	B b={.count=input.c,.s=input.b.s};
	drawText(b,input.font,input.wid,gc,1,1);
}

static void renderInputContent(Control control,GC gc) {
	if(control.type==2) renderCmdContent(control.cmd,gc);
}

static void renderCmd(Cmd input,XID wid,GC gc,I x,I y,I w) {
	if(input.align==1) x+=(w-input.w)/2;
	or(input.align==2) x+=(w-input.w);
	XMoveWindow(gpDisplay,input.wid,x,y+1);
	renderCmdContent(input,gc);
}

void renderControl(Control control,XID wid,GC gc,I x,I y,I w) {
	if(control.type==0) renderLabel(control.label,wid,gc,x,y,w);
	or(control.type==1) renderBar(control.bar,wid,gc,x,y,w);
	or(control.type==2) renderCmd(control.cmd,wid,gc,x,y,w);
}

static void handleCmd(Cmd* pInput,GC gc,XKeyEvent event) {
	if(event.keycode==107) {
		if(pInput->c) pInput->c=0,pInput->b.s[0]=0; else return;
	} or(event.keycode==22) {
		if(pInput->c) pInput->b.s[--pInput->c]=0; else return;
	} or(pInput->c<pInput->b.count) {
		I count=XLookupString(&event,pInput->b.s+pInput->c,
			pInput->b.count-pInput->c,0,0);
		pInput->c+=count;
	} else return;
	renderCmdContent(*pInput,gc);
}

void handleInput(Control* pControl,GC gc,XKeyEvent event) {
	if(pControl->type==2) handleCmd(&pControl->cmd,gc,event);
}

I isInput(Control control) { return control.type==2; }

void enableInput(Control control,GC gc) {
	isInput(control) || E("Expected input");
	XSetWindowBackground(gpDisplay,control.input.wid,EnabledColor);
	XSetWindowBorder(gpDisplay,control.input.wid,~0);
	renderInputContent(control,gc);
}

void disableInput(Control control,GC gc) {
	isInput(control) || E("Expected input");
	XSetWindowBackground(gpDisplay,control.input.wid,DisabledColor);
	XSetWindowBorder(gpDisplay,control.input.wid,0x88888888);
	renderInputContent(control,gc);
}

void dumpCmd(Cmd input) { fprintf(stdout,"%s\n",input.b.s); }

void dumpInput(Control control) {
	isInput(control) || E("Expected input");
	if(control.type==2) dumpCmd(control.cmd);
}
