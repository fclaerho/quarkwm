// Copyright (c) 2007 FC - Licenced under the GPLv3.

#include <sys/select.h>

I nap(I s,I ms,I fd) { // Return 0: timeout, 1: int., 2: event.
	struct timeval tv={.tv_sec=s,.tv_usec=ms*1000};
	fd_set fdSet;
	FD_ZERO(&fdSet);
	if(fd) FD_SET(fd,&fdSet);
	int r=select(fd?fd+1:0,&fdSet,0,0,(s+ms)?&tv:0);
	return r<0?1:((!r)?0:2); // [r==-1 && errno==EINTR]: Interruption.
}
