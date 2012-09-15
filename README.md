-- Quark3 Overview
-- 20070930/FC


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


Using Quark:

 Default Keyboard shortcuts:
  NOTE: Some shortcuts depend on the keyboard mapping, use qhelp to
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

 Toolbar usage:
  NOTE1: You can click anywhere in the toolbar, as the operation only
         depends on the button used, icons are just here to help
         remembering which button do what (left/middle/right).
  NOTE2: The toolbar is hidden in fullscreen mode.
  - left button: Swap docked/primary
  - middle button: Toggle fullscreen
  - right button: Close window


Problems, Requests, Feedback & Contact:

 For any remark/bug report/etc. don't hesitate to use the forum:
 http://sourceforge.net/projects/quarkwm/. Any feedback will be
 appreciated, particularly with any problem encountered on an unusual
 configuration or O/S: I can't fix something if I never heard about it.
 This release has been developped using a GNU/Linux distribution and
 only depends on the X11 library, the C standard library and a few
 posix calls; it should therefore works well on any unix-like system.
 You can also contact me directly through the sourceforge interface or
 by using this address: 0xfc.dev@mailnull.com.


Related Material:

 Quark has been designed to be small, it implies that you may miss
 some features (but still like the window management policy) in
 this case, check the following other window managers:
 - dwm, http://www.lab1702.net/fnurt/larswm/ 
 - wmii, http://www.suckless.org/wiki/wmii/
 - ion3, http://modeemi.fi/~tuomov/ion/
 - ratpoison, larswm, ...

 See www.xwinman.org for a more complete list of wm, and
 http://www.gilesorr.com/wm/table.html for reviews.

