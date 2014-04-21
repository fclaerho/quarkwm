// Copyright (c) 2007 FC - Licenced under the GPLv3.

#include <sys/wait.h>
#include <unistd.h>

I extract(void) { return waitpid(0,0,WNOHANG)>0; } // Non-blocking.

void run(char* s) { !fork() && execl("/bin/sh","/bin/sh","-c",s,0); }
