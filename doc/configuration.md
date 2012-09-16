Quark3 Modules Configuration
============================

_30 Sep. 2007._

Communication Bus & qctl
------------------------

There is no configuration file for Quark, thus you can either modify
the source code to adapt the configuration to your needs (which is
rather deprecated but straightforward) or you can use the builtin
communication system: each Quark module is 'connected' to a common
message bus and can send commands or request data. A module will
only process messages identified by its pid. You can use qctl to
send messages from the command line (qctl allow you to use names
instead of raw codes which is bit more user friendly).

 E.g. to get the qwm version:

	$ ps -A|grep qwm
	5768 tty1     00:00:00 qwm
	$ qctl 5768 version
	quark3/ Version 3.4

NOTE:
'quark3/' is a common prefix for any module message and is not
related to the reply received by qctl in the above example.

Messages exchanged are of the form: `{op key value}` where each field
is an unsigned integer. They are only designed for discrete operations
(no massive data exchanges). To list the operations supported by a 
module, use `help`, example:

	$ qctl 5768 help
	quark3/ Module @5768, 11 ops:
	quark3/ 1 _ _ : Request version
	quark3/ 2 keyid keycode : Change keycode
	quark3/ 3 dockid reverse : Change dock reverse value
	quark3/ 4 dockid side : Change dock side, 0/L 1/R 2/T 3/B
	quark3/ 5 dockid size% : Change dock size%
	quark3/ 6 _ _ : Request key count
	quark3/ 7 _ _ : Request dock count
	quark3/ 8 keyid _ : Request keycode
	quark3/ 9 dockid _ : Request dock reverse value
	quark3/ 10 dockid _ : Request dock side
	quark3/ 11 dockid _ : Request dock size%

For instance the second line (1 _ _ : Request version) means that
a message with op=1, key=anything, value=anything, sent to qwm,
requests its version. The command `$ qctl 5768 version` (above) is
in fact (almost) equivalent to `$ qctl 5768 1 0 0` (or any value
instead of 0). To wait for a reply after a command using raw codes with
qctl, add `+` at the end, so `qctl 5768 version` really is equivalent
to `qctl 5768 1 0 0 +`. If you simply want to update a module
parameter, there is no reply to await.


Storing Your New Configuration
------------------------------

Once your list of qctl commands is ready (the sections below detail
the configurable parameters of the main modules), you have to modify
`~/.xinitrc` to take into account those modifications at startup. The
single subtlety is that you must know the process id of a module in
order to use qctl. Though it can be done using a bit of shell
programming (and the variable $!, which return the pid of the last
process run asynchronously), this is not straightforward. You should
instead use qinit, a tool provided with quark.

The syntax is the following:

	$ qinit -start (module name) -ctl (module name) (qctl cmd)

where you can use '-start' and '-ctl' multiple times, e.g.:

	$ qinit -start qbar -start qrun -ctl qrun 'setKeycode 0 28'

Qinit terminates when the 1st module launched terminates, so if you
have to reconfigure qwm, keep it in the first position. Example:

	export XTERM='xterm -bg black -fg lightgreen'
	export PLAYER='xmms -t'
	export MAILER='thunderbird'
	export BROWSER='firefox'
	/path/to/quark3/qbg 0& # Set a black background.
	/path/to/quark3/qinit\
		-start qwm -start qbar -start qrun\
		-ctl qbar 'setKeycode 0 28'\
		-ctl qrun 'setKeycode 0 42'\
		-ctl qwm 'setDockSize 0 20' # Not run asynchronously.

NOTES:
A `-ctl` flag can only be used on a module already started
with `-start`. Besides, the reconfiguration command must be
parsed as a single argument, so be sure to enclose it
between quotes.

 You don't have to use qinit for modules that are not reconfigured,
 remember to run qinit asynchronously in this case, e.g.:

	...
	/path/to/quark3/qinit -start qbar -ctl qbar 'setKeycode 0 28'&
	/path/to/quark3/qrun&
	/path/to/quark3/qwm

Qwm Configuration
-----------------

### Keyboard Shortcuts

The default meta key is the 'windows' key. It cannot be changed dynamically.
If, however, you're using a keyboard without this key,
you'll have to modify the sources: simply change every instance of the
constant 'Mod4Mask' in x11.h and replace it by Mod1Mask (alt) for instance.
Remember to recompile the modules and to restart your X session.

Key Ids and associated default shortcuts:
- 0, **exit** (default: win+end)
- 1, **cycle focus** (default: win+tab)
- 2, **swap primary/focus** (default: win+a/q) a: Azerty kbd, q: Qwerty.
- 3, **focus primary** (default: win+z/w) z: Azerty kbd, w: Qwerty.
- 4, **kill focus** (default: win+x)
- 5, **close focus** (default: win+c)
- 6, set focus in **full screen** (default: win+f)
- 7, **hide all** (default: win+esc)
- 8, **cycle primary** (default: win+&) (used when docks are full)

To change a key, you must know the new keycode value first, to this
end you can use the program `xev` (provided with x11/xorg).

Option 1: Modify the source code
- edit qwm.c,
- jump to line 18: `I gKeys[]={103,23,24,25,53,54,41,9,10};`
- replace the value(s) (the order is given in the list above).
	Take care to not change the array size.
- recompile and restart your session.

Option 2: Use qctl
E.g. To set the keycode 42 for 'exit' (of keyid 0, see above):

	$ ps -A|grep qwm
	5768 tty1     00:00:00 qwm
	$ qctl 5768 setKeycode 0 42

NOTE:
`$ qctl 5768 setKeycode 0 42` is equivalent to `$ qctl 5768 2 0 42` as shown by `qctl 5768 help`.

### Docks

  A dock is a rectangular area anchored to a side of the screen (or
  of another dock if they are nested). Two docks are available by
  default, which should be sufficient. This number cannot be
  reconfigured dynamically, you have to modify the source code to
  add/remove a dock but the modification is trivial:
  - edit qwm.c,
  - jump to line 19: 'Dock gDocks[]={{0,0,30},{0,3,20}};'
  - add/remove an entry (see below for the meaning of each field),
  - recompile and restart your session.

  Docks are only used when needed: if only one window is opened, it
  uses the whole space (minus some margin), if you open another window,
  the primary window is resized and the second one is docked using the
  1st dock configuration. The second dock will be used when the first
  one is full, and so on.

  The default configuration is the following:
  - a first dock on the left, using 30% of the screen width,
  - a dock following on the bottom, using 20% of the screen height,

  An entry is composed of the following values: {reverse,side,size}
  - reverse=0 means from 'top to bottom' if side is left or right or
    'left to right' if side is top or bottom, reverse=1 for the
    opposite meanings.
  - side=0 for left, 1 for right, 2 for top, 3 for bottom.
  - size is a percentage of the screen width (if side is left
    or right) or height (if side is top or bottom).

  So the default configuration {{0,0,30},{0,3,20}} means:
  {0,0,30} -> A dock on the left (2nd 0), using 30% of the screen
              width from top to bottom (1st 0).
  {0,3,20} -> A dock on the bottom (3), using 20% of the screen
              height, from left to right (1st 0).

  For example, if you want to add a dock on the right, using 10% of
  the screen width and storing windows from the bottom to the top,
  add the following entry: {1,1,10}.
  You can also nest the docks, for instance, to get two successive
  docks on the left, using respectively 30% and 10% of the screen
  width: gDocks={{0,0,30},{0,0,10}}.

  If you simply want to update an existing dock parameter, you can
  use qctl, as for the keyboard. The 3 parameters can be changed.

  NOTE: Dock indexes start from 0 (so the second dock index is 1).

  E.g. 1/ To change the second dock size to 30%:
          $ ps -A|grep qwm
          5768 tty1     00:00:00 qwm
          $ qctl 5768 setDockSize 1 30
       2/ To change the second dock size order ('reverse' field):
          $ qctl 5768 setDockReverse 1 1
       3/ To change the second dock side (set it on top):
          $ qctl 5768 setDockReverse 1 2


Qbar configuration
------------------

 First the list of operations:

	$ ps -A|grep qbar
	5767 tty1     00:00:00 qbar
	$ qctl 5767 help
	quark3/ Module @5767, 6 ops:
	quark3/ 1 _ _ : Request version
	quark3/ 2 keyid keycode : Change keycode
	quark3/ 12 side offset% : Change position 0/L 1/R 2/T 3/B
	quark3/ 14 _ _ : Request position
	quark3/ 6 _ _ : Request key count
	quark3/ 8 keyid _ : Request keycode

### Keyboard Shortcuts

  Key Ids and associated default shortcuts:
  - 0, terminal (default: win+e)
  - 1, media player (default: win+p)
  - 2, mailer (default: win+m/,) m: Azery kbd, ,: Qwerty.
  - 3, browser (default: win+b)

  Proceed as for qwm, e.g. to change the terminal shortcut to 'win+t':
  type '$ qctl 5767 setKeycode 0 28'.

  If you prefer to edit directly the source code:
  - edit qbar.c
  - jump to line 17: 'I gOffset=1,gSide=3,gH,gKeys[]={26,33,47,56};'
  - replace the value(s) for gKeys (the order is given in the list
    above). Take care to not change the array size.
  - recompile and restart qbar.

### Clock Position

  Parameters of the clock:
  - side (like for docks: 0 for left, 1 right, 2 top, 3 bottom).
  - offset (% from top or left of the screen depending on side).

  Option 1: Modify the source code
  - edit qbar.c
  - jump to line 17: 'I gOffset=1,gSide=3,gH,gKeys[]={26,33,47,56};'
  - change the value of gOffset and gSide,
  - recompile and restart qbar.

  Option 2: Use qctl
   E.g. To set the bar on the top (2) with an offset of 20%:
	$ ps -A|grep qbar
	5767 tty1     00:00:00 qbar
	$ qctl 5767 setPosition 2 20


QRun configuration
------------------

 There is only one parameter than can be reconfigured for Qrun, the
 shortcut to summon the text box, proceed as for qwm or qbar:
 either edit qrun.c, change the value of gKey (line 19), recompile
 and restart, or use qctl: $ qctl (qrun pid) setKeycode 0 (keycode).


Communication Bus, Additional Notes
-----------------------------------

### Implementation

  The communication bus is based on an IPC message queue coupled with
  a signal (SIGHUP) to allow events multiplexing with select (nap.c).
  The message queue used can be viewed using the command 'ipcs' (and
  deleted with ipcrm -q (qid) where (qid) is the queue id returned by
  'ipcs'). Only one module is allowed to call closeBus() (as it
  deletes the list), this is done by qwm.

### Bus Usage: Minimal Code Sample

	#define log(_...) (fprintf(stderr,NAME "/ " _),fprintf(stderr,"\n"))
	#define E(_...) (log("Error: " _),_exit(1),0)
	typedef unsigned int I;

	#include <stdio.h>
	#include "bus.c"
	#include "nap.c"

	OpTxt gOps[]={
		{1,"_ _ : Example 1"},
 		{2,"_ _ : Example 2"}};

	void processMsg(void) {
		Msg msg;
		while(getMsg(&msg,0))
		if(msg.op==1) log("QTest: Reply 1"),reply(msg,42,666);
		else if(msg.op==2) log("QTest: Reply 2"),reply(msg,42,666);
	}

	int main(int c,char** p) {
		openBus(gOps,2) || E("Cannot open bus");
		while(1) if(nap(0,0,0)==1) processMsg();
		return 0;
	}

 Once compiled (copy the source code in a file, say test.c and type
 ./setup.sh test.c) and run, this program can interact with qctl:
 - qctl (pid) help
 - qctl (pid) 1 0 0 +
 - qctl (pid) 2 0 0 +

