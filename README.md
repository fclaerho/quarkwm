FOREWORD
========

_15 Sep. 2012._

Time flies.
I resurrected this old project in order to archive it here in my git repository but it is **no longer maintained**.
The coding style is highly debatable and shall neither be considered as an example for anyone nor representative of my current style.
That being said, it was a prototype of a minimalist coding style, and I'm still a minimalist.
A few gadgets apart (component-oriented architecture, runtime configuration...),
this remains a usable skeleton project for anyone looking to start a new homebrew window manager for X11.
The documentation is pretty extensive, so dig in for details.
I've refreshed the documentation syntax (which is now markdown) to make it more readable on github, its content is mostly unchanged.
Unfortunately this project was not handled by a RCS, thus there is no revision history per se;
You'll find earlier versions as tarballs in the repository.

NOTE: this project is still hosted on sourceforge, where it was first released, see the feedback section below.

* * *

Quark3 Overview
===============

_30 Sep. 2007._

Quark[WM] is a small (15k) and fast window manager designed to
increase your productivity by:
- organizing your windows (position and size),
- letting you optimize this window organization policy,
- avoid multiple obscure configuration files (there's none),
- allow a dynamic reconfiguration,
- allow to work only with the keyboard (as long as your programs
  themselves can be driven with the keyboard),
- allow to work with the mouse if you need it.

The main goal here is to minimize the usual causes of time wasting
(endless tweaking of multiple options, windows organization, etc.).

On each screen, windows are divided in two sets:
- the primary window (generally only one window),
- the docked windows (everything else).

The primary window is the application on which you are working,
whereas docked windows are support windows or background tasks.
However, docked windows are not iconified windows (and docks
should not be confused with taskbars): they are just reduced and
still operational.

This abstraction is justified by the fact you are rarely actively
working on several windows at the same time. Therefore Quark optimizes
the screen space for the primary window and reduce the others. You
can switch of primary window anytime, and the windows are reorganized
accordingly.

A dock is the abstraction of the space dynamically allocated to the
other windows. Typically those secondary windows are divided between
the support windows (often used and switched with the primary window)
and the less-important applications (mailer, etc.). Your docks will
probably be organized so that the most important secondary windows
have more space and the others are much more reduced. The default
Quark configuration (detailed later) illustrates this idea. When the
docks are full, the remaining windows are simply stacked under the
primary window, you are informed of this situation by a supplementary
icon in the toolbar.

Default Keyboard Shortcuts
--------------------------

Some shortcuts depend on the keyboard mapping, use qhelp to
know the exact shortcuts on your system. In the following
1 means an azerty keyboard, 2 means a qwerty keyboard.

- qwm, Exit: win+end
- qwm, Focus the primary window: win+{1/z, 2/w}
- qwm, Focus the next window: win+tab
- qwm, Swap a docked window and the primary window: win+{1/a, 2/q}
- qwm, Close the focus window: win+c
- qwm, Kill the focus window: win+x
- qwm, Cycle the primary (when docks are full): win+&
- qwm, Toggle the focus window in full screen: win+f
- qwm, Hide/Show all windows: win+esc
- qbar, Launch terminal: win+e
- qbar, Launch player: win+p
- qbar, Launch mailer: win+{1/m, 2/,}
- qbar, Launch browser: win+b
- qrun, Open run box: win+r

Toolbar Usage
-------------

You can click anywhere in the toolbar, as the operation only
depends on the button used, icons are just here to help
remembering which button do what (left/middle/right).

The toolbar is hidden in fullscreen mode.

- left button: Swap docked/primary
- middle button: Toggle fullscreen
- right button: Close window


Problems, Requests, Feedback & Contact
--------------------------------------

For any remark/bug report/etc. don't hesitate to use the forum:
http://sourceforge.net/projects/quarkwm/. Any feedback will be
appreciated, particularly with any problem encountered on an unusual
configuration or O/S: I can't fix something if I never heard about it.
This release has been developped using a GNU/Linux distribution and
only depends on the X11 library, the C standard library and a few
posix calls; it should therefore works well on any unix-like system.
You can also contact me directly through the sourceforge interface.

EDIT: I removed the email which is no longer valid.

Related Material
----------------

Quark has been designed to be small, it implies that you may miss
some features (but still like the window management policy) in
this case, check the following other window managers:
- dwm, http://www.lab1702.net/fnurt/larswm/ 
- wmii, http://www.suckless.org/wiki/wmii/
- ion3, http://modeemi.fi/~tuomov/ion/
- ...

See www.xwinman.org for a more complete list of wm, and
http://www.gilesorr.com/wm/table.html for reviews.

