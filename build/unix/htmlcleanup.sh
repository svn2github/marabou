#! /bin/sh

# Script to remove unnecessary files from html dir
# Called by main Makefile.
#
# Author: R. Lutter

echo
echo "[htmlcleanup.sh: Remove unnecessary files from html dir]"
echo
for f in html/*
do
	FILE=`echo $f | awk -F. '{print $1}'`
	if [ ! -d $FILE -a ! -f $FILE.h ]
	then
		FILE=`basename $FILE`
		case $FILE in
			ClassIndex)	;;
			TMrb*) 		;;
			TGMrb*)		;;
			TUsr*) 		;;
			TSnk*) 		;;
			TPol*) 		;;
			*)			rm -f $f;;
		esac
	fi
done
