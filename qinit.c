// Copyright (c) 2007 FC - Licenced under the GPLv3.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),_exit(1),0);
typedef unsigned int I;
typedef char* S;

#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

struct { S s; pid_t pid; } gModules[5];
char buffer[100];
I gCount=0; // Number of modules loaded.

void start(S s) {
	if(gCount>=5) { log("Too many modules");return; }
	gModules[gCount].s=s;
	if((gModules[gCount++].pid=fork())) { log("%s started",s);return; }
	if(strlen(PATH)+strlen(s)+1>100) { log("Buffer too small");return; }
	sprintf(buffer,PATH "/%s",s);
	execl(buffer,buffer,0);
}

void ctl(S s,S op) {
	for(I i=0;i<gCount;++i)
		if(!strcmp(gModules[i].s,s)) {
			if(strlen(PATH)+strlen(op)>70) return;
			sprintf(buffer,PATH "/qctl %u %s",gModules[i].pid,op);
			log("Reconfigure %s: %s",s,buffer);
			!fork() && execl("/bin/sh","/bin/sh","-c",buffer,0);
			return;
		}
	log("Module %s not started, use '-start %s'",s,s);
}

int main(int c,char** p) {
	for(I i=1;i<c;++i) // Parse command line.
		if(!strcmp(p[i],"-start") && i+1<c) start(p[++i]);
		else if(!strcmp(p[i],"-ctl") && i+2<c) ctl(p[i+1],p[i+2]),i+=2;
		else log("Unknown option '%s', ignored",p[i]);
	if(gCount)
		waitpid(gModules[0].pid,0,0); // Wait for the 1st module to end.
	return 0;
}
