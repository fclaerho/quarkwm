#!/bin/sh

NAME=`pwd|xargs basename`

log() {
	OLD="[$NAME] $1"
	if test $VERBOSE; then
		echo $OLD
	else
		NEW=`echo $OLD|fold -w 70|head -1`
		if test "$NEW" = "$OLD"; then
			echo $NEW
		else
			echo "$NEW..."
		fi
	fi
}

try() { log "+ $1"; $1 || exit 1; }

die() { log "Error: $1"; exit 1; }

while test $1; do
	case "$1" in
		pack)
			cd .. # Store the archive in the parent directory.
			tar zcvf ${NAME}_`date +%Y%m%d%H%M`.tgz $NAME
			cd $NAME
			log 'Package ready'
			exit 0;;
		SLOC)
			wc -l *.c|sort -nr;
			exit 0;;
		F) shift 1;FLAGS="$1";;
		V) VERBOSE=1;;
		D) DEBUG=1;;
		clean)
			if test "$SRC" && test -e "$SRC"; then
				rm -f "$SRC"
			fi
			exit 0;;
		*)
			if test "$SRC"; then
				die "Unknown option '$1'"
			else
				SRC="$1"
			fi;;
	esac
	shift 1
done

if test -z "$SRC"; then die "No source file"; fi
FLAGS="$FLAGS -DNAME=\"$NAME\" -DPATH=\"`pwd`\""
TGT=`basename "$SRC" .c`
if test "$DEBUG"; then
	log "Development build:"
	try "gcc $SRC -std=c99 -g -Wall -Werror $FLAGS -o $TGT"
else
	try "gcc $SRC -std=c99 -O2 -s $FLAGS -o $TGT"
fi
