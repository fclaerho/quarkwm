// copyright (c) 2007-2014 fclaerhout.fr, released under the MIT license.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),exit(1),0)
typedef unsigned int I;
#define or else if
typedef char* S;

#include <stdlib.h>
#include <stdio.h>
#include "form.c"
#include "run.c"
#include "bus.c"

OpTxt gOps[3]={
	{1,"_ _ : Request version"},
	{2,"0 keycode : Change keycode"},
	{8,"0 _ : Request keycode"}};
I gKey=27; // Key to summon the box.

void handleMsgs(void) {
	Msg msg;
	while(getMsg(&msg,0))
		if(msg.op==1) reply(msg,1,2); // Reply v1.2.
		or(msg.op==8) reply(msg,gKey,0);
		or(msg.op==2)
			ungrabCombo(gKey),
			gKey=msg.value,
			grabCombo(gKey,1);
}

void handleEvents(Form* pForm) {
	XEvent event;
	while(XPending(gpDisplay))
		if(XNextEvent(gpDisplay,&event),event.type==KeyPress)
			showForm(pForm) && (run(pForm->controls[2].cmd.b.s),0);
	while(extract()); // Get back dead processes.
}

int main(int c,S* p) {
	openBus(gOps,3); // Bus failure is not fatal.
	setup();
	grabCombo(gKey,1);
	Form form=setupForm(0);
	form.yAlign=2; // Bottom.
	pushControl(&form,(Control){.label=getLabel(0,0,"Run:")});
	pushControl(&form,(Control){.bar=getBar(0)});
	pushControl(&form,(Control){.cmd=getCmd(form.wid,1,0,30,PATH "/qhelp")});
	handleMsgs(); // Handle possibly pending messages.
	for(;;) nap(0,0,XFD)==1?handleMsgs():handleEvents(&form);
	return 0;
}
