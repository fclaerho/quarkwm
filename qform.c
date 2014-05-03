// copyright (c) 2007-2014 fclaerhout.fr, released under the MIT license.

#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
#define E(_...) (log("Error: " _),exit(1),0)
typedef unsigned int I;
#define or else if
typedef char* S;

#include <stdlib.h>
#include <stdio.h>
#include "form.c"

int main(int c,S* p) {
	I font,align,max;
	Form form=setupForm(0);
	Control control;
	for(I i=1;i<c;++i) { // Parse command line.
		if(p[i][0]!='-') {
			log("Floating value '%s', ignored",p[i]);
			continue;
		}
		if(p[i][1]=='l' && i+3<c) // -label
			font=p[++i][0]=='s'?0:(p[i][0]=='m'?1:2),
			align=p[++i][0]=='l'?0:(p[i][0]=='c'?1:2),
			control.label=getLabel(font,align,p[++i]),
			pushControl(&form,control);
		or(p[i][1]=='b') pushControl(&form,(Control){.bar=getBar(1)});
		or(p[i][1]=='-') pushControl(&form,(Control){.bar=getBar(0)});
		or(p[i][1]=='c' && i+4<c) // -cmd
			font=p[++i][0]=='s'?0:(p[i][0]=='m'?1:2),
			align=p[++i][0]=='l'?0:(p[i][0]=='c'?1:2),
			max=atoi(p[++i]),
			control.cmd=getCmd(form.wid,font,align,max,p[++i]),
			pushControl(&form,control);
		or(p[i][1]=='x' && i+1<c) // -x
			form.xAlign=p[++i][0]=='l'?0:(p[i][0]=='c'?1:(p[i][0]=='r'?2:3)),
			form.x=form.xAlign==3?atoi(p[i]):0;
		or(p[i][1]=='y' && i+1<c) // -y
			form.yAlign=p[++i][0]=='t'?0:(p[i][0]=='c'?1:(p[i][0]=='b'?2:3)),
			form.y=form.yAlign==3?atoi(p[i]):0;
	}
	if(showForm(&form)) // If submitted, dump inputs content.
		for(I i=0;i<form.count;++i)
			if(isInput(form.controls[i])) dumpInput(form.controls[i]);
	return 0;
}
