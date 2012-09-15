#!/bin/sh

MEDIUMFONT=-*-lucida-medium-r-*-*-12-*-*-*-*-*-*-*
BIGFONT=-*-lucida-bold-r-*-*-26-*-*-*-*-*-*-*
SMALLFONT=-*-snap-*-*-*-*-*-*-*-*-*-*-*-*
XLIBPATH=/usr/X11R6/lib

FLAGS="-L$XLIBPATH -lX11 -DSMALLFONT=\"$SMALLFONT\"\
	-DMEDIUMFONT=\"$MEDIUMFONT\" -DBIGFONT=\"$BIGFONT\"\
	-D_XOPEN_SOURCE" # _XOPEN_SOURCE requested by ipc.h.

while [ "$1" ]; do
	case "$1" in
		D) D='D';; # Development build (debug).
		V) V='V';; # Verbose mode.
		clean)
			rm -f qwm qbg qbar qctl qform qhelp qrun qinit
			exit 0;;
		*) if test -z "$SRC"; then SRC="$1"; fi
	esac
	shift 1
done

if test "$SRC"; then # Build a specific module.
	build.sh $D $V F "$LIBS $FLAGS $FONTS" "$SRC" && echo "Ok"
else # Build everything.
	build.sh $D $V F "$FLAGS" qwm.c \
	&& build.sh $D $V F "$FLAGS" qbg.c \
	&& build.sh $D $V F "$FLAGS" qbar.c \
	&& build.sh $D $V F "$FLAGS" qctl.c \
	&& build.sh $D $V F "$FLAGS" qform.c \
	&& build.sh $D $V F "$FLAGS" qhelp.c \
	&& build.sh $D $V F "$FLAGS" qrun.c \
	&& build.sh $D $V F "$FLAGS" qinit.c \
	&& echo "Ok"
fi
