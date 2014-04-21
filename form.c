// Copyright (c) 2007 FC - Licenced under the GPLv3.

#define AS(_) (sizeof(_)/sizeof(*_))

#include "x11.c"
#include "font.c"
#include "controls.c"
#include "nap.c"

typedef struct { Control controls[30]; XID sid,wid; GC gc;
	I x,y,xAlign,yAlign,iMargin,eMargin,transient,count; } Form;

static void renderForm(Form* pForm) {
	I x=pForm->eMargin,y=x,W=0,H=x+x;
	for(I i=0;i<pForm->count;++i) // 1st pass: width evaluation.
		(pForm->controls[i].any.w>W?W=pForm->controls[i].any.w:0),
		H+=pForm->controls[i].any.h;
	I X=pForm->xAlign==0?0:(pForm->xAlign==1?
		(DisplayWidth(gpDisplay,pForm->sid)-W-x-x)/2:(pForm->xAlign==2?
		DisplayWidth(gpDisplay,pForm->sid)-W-x-x-2:pForm->x));
	I Y=pForm->yAlign==0?0:(pForm->yAlign==1?
		(DisplayHeight(gpDisplay,pForm->sid)-H)/2:(pForm->yAlign==2?
		DisplayHeight(gpDisplay,pForm->sid)-H-2:pForm->y));
	XMoveResizeWindow(gpDisplay,pForm->wid,X,Y,W+x+x,H);
	XRaiseWindow(gpDisplay,pForm->wid);
	XClearWindow(gpDisplay,pForm->wid);
	for(I i=0;i<pForm->count;++i) // 2nd pass: rendering.
		renderControl(pForm->controls[i],pForm->wid,pForm->gc,x,y,W),
		y+=pForm->controls[i].any.h;
	XFlush(gpDisplay);
}

static void focusNextInput(Form* pForm) {
	XID wid,revert,found=0;
	XGetInputFocus(gpDisplay,&wid,(I*)&revert);
	if(wid==PointerRoot || wid==None) return;
	if(pForm->wid==wid) found=1;
	for(I i=0;i<pForm->count;++i)
		if(isInput(pForm->controls[i])) {
			if(found) { wid=pForm->controls[i].input.wid;goto focus; }
			or(pForm->controls[i].input.wid==wid) found=1;
		}
	wid=pForm->wid; // Fallback on form.
focus:
	XSetInputFocus(gpDisplay,wid,RPR,NOW);
}

static I getInput(Form* pForm,XID wid,I* pI) {
	for(I i=0;i<pForm->count;++i)
		if(isInput(pForm->controls[i])
		&& pForm->controls[i].input.wid==wid) { *pI=i; return 1; }
	return 0;
}

static I handleFormEvents(Form* pForm,I* pSubmitted) {
	I i;
	XEvent event;
	while(XPending(gpDisplay))
		if(XNextEvent(gpDisplay,&event),event.type==KeyPress) {
			if(event.xkey.keycode==9) return 0; // Esc.
			or(event.xkey.keycode==36) return *pSubmitted=1,0; // Ent.
			or(event.xkey.keycode==23) focusNextInput(pForm); // Tab.
			or(getInput(pForm,event.xkey.window,&i))
				handleInput(pForm->controls+i,pForm->gc,event.xkey);
		} or(event.type==EnterNotify)
			XSetInputFocus(gpDisplay,event.xcrossing.window,RPR,NOW);
		or(event.type==ButtonPress && event.xbutton.window==pForm->wid)
			return (*pSubmitted=event.xbutton.button==1),0;
		or(event.type==Expose && !event.xexpose.count) renderForm(pForm);
		or(event.type==FocusIn && getInput(pForm,event.xfocus.window,&i))
			enableInput(pForm->controls[i],pForm->gc);
		or(event.type==FocusOut) {
			if(getInput(pForm,event.xfocus.window,&i))
				disableInput(pForm->controls[i],pForm->gc);
			XID wid,revert;
			XGetInputFocus(gpDisplay,&wid,(I*)&revert); // Focus lost?
			if(!getInput(pForm,wid,&i) && wid!=pForm->wid) return 0;
		}
	return 1;
}

void setup(void) {
	static I ready=0;
	if(!ready)
		openXSession() || E("Cannot open X session"),
		loadFonts() || E("Cannot load Fonts"),
		ready=1;
}

Form setupForm(XID sid) {
	setup();
	XID rid=RootWindow(gpDisplay,sid);
	Form form={.x=0,.y=0,.xAlign=1,.yAlign=1,
		.iMargin=2,.eMargin=2,.transient=1,.count=0,.sid=sid,.wid=0};
	XSetWindowAttributes xswa;
	xswa.event_mask=ExposureMask
		|KeyPressMask|ButtonPressMask|FocusChangeMask;
	xswa.override_redirect=form.transient;
	xswa.background_pixel=0;
	xswa.border_pixel=~0;
	form.wid=XCreateWindow(gpDisplay,rid,10000,10000,
		1,1,1,CopyFromParent,InputOutput,CopyFromParent,
		CWEventMask|CWOverrideRedirect|CWBackPixel|CWBorderPixel,&xswa);
	form.gc=DefaultGC(gpDisplay,sid);
	XSetForeground(gpDisplay,form.gc,~0);
	return form;
}

I showForm(Form* pForm) { // Return 1 if submitted, 0 if cancelled.
	pForm || E("Null-ptr");
	I submitted=0;
	XMapWindow(gpDisplay,pForm->wid);
	renderForm(pForm);
	XSetInputFocus(gpDisplay,pForm->wid,RPR,NOW);
	/* FIXME */ focusNextInput(pForm); // Focus the 1st input.
	/* FIXME */ handleFormEvents(pForm,&submitted); // Enable the 1st input.
	Cursor cursor=XCreateFontCursor(gpDisplay,150);
	XGrabPointer(gpDisplay,pForm->wid,1,0,GMA,GMA,None,cursor,NOW);
	while(nap(0,0,XFD)==1?1:handleFormEvents(pForm,&submitted));
	XMoveWindow(gpDisplay,pForm->wid,10000,10000);
	XUnmapWindow(gpDisplay,pForm->wid);
	XUngrabPointer(gpDisplay,NOW);
	XFreeCursor(gpDisplay,cursor);
	return submitted;
}

I pushControl(Form* pForm,Control control) {
	pForm || E("Null-ptr");
	pForm->count<AS(pForm->controls) || E("Too many controls (%u)",pForm->count);
	pForm->controls[pForm->count]=control;
	return pForm->count++;
}
