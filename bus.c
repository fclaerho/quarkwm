// copyright (c) 2007-2014 fclaerhout.fr, released under the MIT license.

#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

typedef struct { I op; char text[50]; } OpTxt;
typedef struct { long port; I from,op,key,value; } Msg;
typedef struct { I port; I count; OpTxt ops[20]; } Desc;
typedef struct { long port; Desc desc; } DescMsg;

static Desc gDesc={.count=0}; // Module description.
static I gBid=-1; // (Private) Bus ID.

void handleSIGHUP(int i) { signal(SIGHUP,handleSIGHUP); }

I openBus(OpTxt* pOps,I count) {
	gDesc.port=getpid();
	key_t k=ftok("/",0xFC);
	if(k==-1) { gBid=-1;log("IPC key creation failed");return 0; }
	gBid=msgget(k,IPC_CREAT|0600);
	if(gBid<0) { log("IPC msg queue creation failed");return 0; }
	if(pOps) 
		gDesc.count=count<=20?count:20,
		memcpy(gDesc.ops,pOps,count*sizeof(OpTxt));
	signal(SIGHUP,handleSIGHUP); // Install handler.
	return 1;
}

void closeBus(void) { if(gBid) msgctl(gBid,IPC_RMID,0); }

I setMsg(I port,I op,I key,I value) { // Non-blocking.
	if(gBid<0) return 0;
	port || E("Illegal null port");
	Msg msg={.port=port,.from=gDesc.port,.op=op,.key=key,.value=value};
	return msgsnd(gBid,&msg,4*sizeof(I),IPC_NOWAIT)>=0
		&& !kill(port,SIGHUP);
}

static void setDescMsg(I port) {
	DescMsg msg={.port=port,.desc=gDesc};
	msgsnd(gBid,&msg,sizeof(Desc),IPC_NOWAIT)>=0 && !kill(port,SIGHUP);
}

I reply(Msg msg,I key,I data) { return setMsg(msg.from,42,key,data); }

I getMsg(Msg* pMsg,I wait) { // Blocking if wait is set.
	if(gBid<0) return 0;
	pMsg || E("Null-ptr");
	if(msgrcv(gBid,pMsg,4*sizeof(I),gDesc.port,wait?0:IPC_NOWAIT)>=0) {
		if(pMsg->op==~0) return setDescMsg(pMsg->from),1;
		if(pMsg->op==42) return 1;
		for(I i=0;i<gDesc.count;++i) // Only registered msgs are valid.
			if(pMsg->op==gDesc.ops[i].op) return 1;
	}
	return 0; // No msg or invalid msg (not registred in gDesc).
}

I getDescMsg(DescMsg* pMsg,I wait) { // Blocking if wait is set.
	if(gBid<0) return 0;
	pMsg || E("Null-ptr");
	return msgrcv(gBid,pMsg,sizeof(Desc),gDesc.port,wait?0:IPC_NOWAIT)>=0;
}
