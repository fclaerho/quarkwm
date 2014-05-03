// copyright (c) 2007-2014 fclaerhout.fr, released under the MIT license.

#include <sys/wait.h>
#include <unistd.h>

I extract(void) { return waitpid(0,0,WNOHANG)>0; } // Non-blocking.

void run(char* s) { !fork() && execl("/bin/sh","/bin/sh","-c",s,0); }
