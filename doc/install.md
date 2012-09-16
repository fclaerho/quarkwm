Quark3 Installation Procedure
=============================

_30 Sep. 2007._

NOTES:
- In the following '$' represents your shell prompt.
- You don't need root privileges to install Quark as, in fact,
  nothing is installed: binaries are left in the archive directory
  and not copied in the system directories by default so you can
  play with it without messing your system.


Quick Installation Procedure
----------------------------

1. Decompress the archive.
2. Update the content of `setup.sh` as needed:
   `MEDIUMFONT`, `SMALLFONT`, `BIGFONT` and `XLIBPATH`.
3. Compile: `$ ./setup.sh`
4. Modify your `.xinitrc`. If you plan to use qbar, you need those
   supplementary variables: `$XTERM`, `$PLAYER`, `$MAILER`, `$BROWSER`.
   (See below for an example of `.xinitrc`). 
5. Restart your session.

If you encounter any problem, please read the following detailed
procedure (which contain additional notes that may be useful).


Detailed Installation Procedure
-------------------------------

 1. Decompress the archive: `$ tar zxvf quark3_(version).tgz`

 2. Update the content of `setup.sh` to comply with your installation.
    The following variables may be modified:
    - `MEDIUMFONT`, `SMALLFONT`, `BIGFONT`:
      Use fonts that are installed on your system, or check the
      default values are indeed valid. You can use `xfontsel` or
      `xlsfonts` to choose other fonts.
    - `XLIBPATH`:
      Path to your X11 library.
      The script does not attempt to find the path to libX11.so.x.
      If the default value does not work, you have to modify it
      manually. Try `$ locate libX11.so|head -1|xargs dirname` and
      use this command result to set XLIBPATH. Depending on your
      system the locate database may not be up-to-date or even
      not available at all, so if this command fails try something
      else: for instance `$ which startx|xargs dirname` and replace
      'bin/' by 'lib/'. Once again results may vary depending on
      your system organization.

 3. Check that the two files `setup.sh` and `build.sh`
    are executable, if you're not sure, type:
	`$ chmod u+x setup.sh build.sh`

    NOTE: `build.sh` is a small script independent of any project which
          performs the real compilation job, it is called by `setup.sh`
          with the appropriate flags for the Quark modules.

 4. Compile the sources:
	`$ ./setup.sh`
    It should end with 'Ok';
    otherwise read the paragraph 'Compilation failure' below.

    NOTE1: By default `setup.sh` compiles all the modules. If you are
           already familiar with QuarkWM and only need some specific
           modules (only qwm for instance) then simply invoke `setup.sh`
           with the module source name, e.g. `$ ./setup.sh qwm.c`.

    **Compilation Failure**
     If the compilation fails, check `XLIBPATH` as explained above.
     If the library is not found you should get something like:
     `ld: cannot find -lX11`.
     If this is not the error cause, please post a message on
     the forum (or send it to me by mail, see readme.txt/contact)
     detailing the problem (remember to copy/paste the error
     using the verbose mode: `$ ./setup.sh V`). You can also try
     to debug the code yourself if you wish, recompile everything
     in development mode `$ ./setup.sh D` before starting.

    If the compilation succeeds, you get several binaries:
      - **qwm**, the window manager.
        NOTE: Please keep in mind that qwm is only the window manager
              and not an application launcher, you need an additional
              tool to launch your programs (qbar for instance, but
              any other tool will do fine).
      - **qbg**, a small tool to set your background color.
        It doesn't allow to display images for the moment.
      - **qbar**, a tool displaying an OSD clock and allowing to
        launch some applications through keyboard shortcuts.
      - **qctl**, a command line tool allowing to communicate with
        the modules (and mostly used to reconfigure them).
      - **qform** is a tool to dynamically build forms/textboxes,
        see qforms.txt for details.
      - **qrun** allows to invoke a text box to launch applications
        by typing the shell command.
      - **qhelp** displays a text box listing the default Quark
        shortcuts.
      - **qinit**, a tool to reconfigure the modules in .xinitrc,
        see `configuration.txt` for details.

 5. Check the fonts are ok, try `qhelp` for instance, you should
    see 3 types of font (small/medium/big). If one of them is
    not viewable or if the execution fails with an error message
    saying it cannot load a font, modify the corresponding value
    in `setup.sh` and restart from step 2. Also try `qbar`.

    NOTE: The font named `snap`, among other nice ones, can be
          downloaded there: http://artwizaleczapka.sourceforge.net/

 6. Modify your X startup file `~/.xinitrc` to specify QuarkWM as
    your new window manager and to start the other tools.

    Example of `.xinitrc`:

		export XTERM='xterm -bg black -fg lightgreen'
		export PLAYER='xmms -t'
		export MAILER='thunderbird'
		export BROWSER='firefox'
		/path/to/quark3/qbg 0& # Set a black background.
		/path/to/quark3/qbar&
		/path/to/quark3/qrun&
		/path/to/quark3/qwm

    This example is ok if the default configuration suits you,
    otherwise you will need additional lines to reconfigure the
    modules with qctl and qinit (see `configuration.txt`).

    Use whatever program you wish to setup your background image, qbg
    was just an example here. Other possibility:
    `display -window root yourFavoriteWallpaper.jpg` (`display` is not
    guaranteed to be installed on your system).

    NOTE1: If you set up an image background (with `display` for 
           instance as explained above) with an asynchronous command
           and use qbar, it is possible that qbar is executed faster:
           the clock is drawn but the image is rendered above so the
           clock will initially be hidden. To avoid this case, simply
           avoid to launch the 1st program asynchronously:

			display -window root wallpaper.jpg # No &
			/path/to/quark3/qbar&

           By using this command, qbar is launched only after the
           completion of the other command (display) so that first
           the wallpaper is rendered and then the clock is rendered
           without conflict.

    NOTE2: qbar uses the 4 environment variables `$XTERM`, `$PLAYER`,
           `$MAILER` and `$BROWSER` to launch the applications, so
           either setup those variables in your shell startup
           files (`.bashrc` if you're using bash for instance) or
           directly into the `.xinitrc` as shown above (better).

 7. Restart your X session.

 8. If you're using a desktop manager (gdm, kdm, etc.), remember
    to select 'Default X session' to use your `.xinitrc` instead
    or the system-wide startup file or the previous configuration.

FINAL NOTE: Be sure to have played a bit with QuarkWM and to know
            the basic shortcuts/limits before starting to work
            seriously with it.
            If something goes wrong (forgot the exit shortcut?),
            you can always use the sequence \<ctrl\>\<alt\>\<backspace\>
            to kill your X session (remember that your data won't
            be saved in this case as all the applications you
            started are simply killed).

