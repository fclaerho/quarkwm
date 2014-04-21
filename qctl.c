// Copyright (c) 2007 FC - Licenced under the GPLv3.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),_exit(1),0)
typedef unsigned int I;
#define or else if

#include <stdlib.h>
#include <stdio.h>
#include "bus.c"

int main(int c,char** p) {
	openBus(0,0) || E("Cannot open bus");
	c>=3 || E("Usage: %s port op [key value [+]]",p[0]);
	I port=0,op=0,key=0,value=0,reply=0;
	port=atoi(p[1]);
	if(!strcmp(p[2],"help")) { op=~0;reply=1;goto send; } // Reserved.
	or(!strcmp(p[2],"version")) { op=1;reply=1;goto send; } // Reserved.
	or(!strcmp(p[2],"setKeycode")) op=2; 
	or(!strcmp(p[2],"setDockReverse")) op=3;
	or(!strcmp(p[2],"setDockSide")) op=4;
	or(!strcmp(p[2],"setDockSize")) op=5;
	or(!strcmp(p[2],"keyCount")) { op=6;reply=1;goto send; }
	or(!strcmp(p[2],"dockCount")) { op=7;reply=1;goto send; }
	or(!strcmp(p[2],"keycode")) { op=8;reply=1;goto send; }
	or(!strcmp(p[2],"dockReverse")) op=9;
	or(!strcmp(p[2],"dockSide")) op=10;
	or(!strcmp(p[2],"dockSize")) op=11;
	or(!strcmp(p[2],"setPosition")) op=12;
	or(!strcmp(p[2],"setSize")) op=13;
	or(!strcmp(p[2],"position")) { op=14;reply=1;goto send; }
	or(!strcmp(p[2],"size")) { op=15;reply=1;goto send; }
	or(!strcmp(p[2],"close")) { op=16;goto send; }
	or(!strcmp(p[2],"render")) { op=17;goto send; }
	or(!strcmp(p[2],"restart")) { op=18;goto send; }
	else op=atoi(p[2]);
	c>=5 || E("Need a key and value.");
	key=atoi(p[3]);
	value=atoi(p[4]);
	if(c>=6 && !strcmp(p[5],"+")) reply=1;
send:
	setMsg(port,op,key,value) || E("Emission error");
	if(reply && op==~0) {
		DescMsg descMsg;
		if(!getDescMsg(&descMsg,1)) E("Reception error");
		log("Module @%u, %u ops:",descMsg.desc.port,descMsg.desc.count);
		for(I i=0;i<descMsg.desc.count;++i)
			log("%u %s",descMsg.desc.ops[i].op,descMsg.desc.ops[i].text);
	} or(reply) {
		Msg msg;
		if(!getMsg(&msg,1)) E("Reception error");
		if(op==1) log("Version %u.%u",msg.key,msg.value);
		else log("%u(%u:%u)",msg.op,msg.key,msg.value);
	}
	return 0;
}
