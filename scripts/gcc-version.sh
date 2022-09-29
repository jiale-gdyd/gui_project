#!/bin/sh
#
# gcc-version gcc-command
#
# Print the gcc version of `gcc-command' in a 5 or 6-digit form
# such as `29503' for gcc-2.95.3, `30301' for gcc-3.3.1, etc.

if [ "$1" = "-p" ] ; then
	with_patchlevel=1;
	shift;
fi

compiler="$*"

if [ ${#compiler} -eq 0 ]; then
	echo "Error: No compiler specified."
	printf "Usage:\n\t$0 <gcc-command>\n"
	exit 1
fi

MAJOR=$(echo __GNUC__ | $compiler -E -x c - | tail -n 1)
MINOR=$(echo __GNUC_MINOR__ | $compiler -E -x c - | tail -n 1)
if [ "x$with_patchlevel" != "x" ] ; then
	PATCHLEVEL=$(echo __GNUC_PATCHLEVEL__ | $compiler -E -x c - | tail -n 1)
	printf "%02d%02d%02d\\n" $MAJOR $MINOR $PATCHLEVEL
else
	printf "%02d%02d\\n" $MAJOR $MINOR
fi
