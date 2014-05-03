// copyright (c) 2007-2014 fclaerhout.fr, released under the MIT license.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),exit(1),0)
typedef unsigned int I;
#define or else if
typedef char* S;

#include <stdlib.h>
#include <stdio.h>
#include "form.c"

void pushShortcut(Form* pForm,I keycode,S text) {
	S s=XKeysymToString(XKeycodeToKeysym(gpDisplay,keycode,0));
	S buffer=malloc(50);
	sprintf(buffer,text,s); // Possible buffer overflow here.
	Control control={.label=getLabel(1,0,buffer)};
	pushControl(pForm,control);
}

void pushSection(Form* pForm,S s) {
	Control control={.bar=getBar(0)};
	pushControl(pForm,control);
	control.label=getLabel(0,2,s);
	pushControl(pForm,control);
	control.bar=getBar(1);
	pushControl(pForm,control);
}

int main(int c,S* p) {
	Form form=setupForm(0);
	form.eMargin=2;
	form.iMargin=2;
	Control control={.label=getLabel(2,1,"Quark3")};
	pushControl(&form,control);
	control.label=getLabel(0,1,"...Default Shortcuts");
	pushControl(&form,control);
	setup();
	pushSection(&form,"/qwm");
	pushShortcut(&form,24,"win+%s - Swap focus/primary window");
	pushShortcut(&form,10,"win+%s - Cycle primary");
	pushShortcut(&form,9,"win+%s - Hide all windows");
	pushShortcut(&form,41,"win+%s - Toggle focus fullscreen");
	pushShortcut(&form,23,"win+%s - Focus next window");
	pushShortcut(&form,54,"win+%s - Close focus window");
	pushShortcut(&form,53,"win+%s - Kill focus window");
	pushShortcut(&form,25,"win+%s - Focus primary");
	pushShortcut(&form,103,"win+%s - Exit");
	pushSection(&form,"/qbar");
	pushShortcut(&form,33,"win+%s - Media player");
	pushShortcut(&form,26,"win+%s - Terminal");
	pushShortcut(&form,56,"win+%s - Browser");
	pushShortcut(&form,47,"win+%s - Mailer");
	pushSection(&form,"/qrun");
	pushShortcut(&form,27,"win+%s - Run box");
	showForm(&form);
	return 0;
}
