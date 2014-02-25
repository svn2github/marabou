#! /bin/sh

# Script to produce distribution of Marabou.
# Called by main Makefile.
#
# Author: Otto Schaile

ROOTVERS=`root-config --version | sed -e 's/\//\./'`
TYPE=`root-config --arch`
if [ "x`root-config --platform`" = "xmacosx" ]; then
   TYPE=$TYPE-`sw_vers -productVersion | cut -d . -f1 -f2`
   TYPE=$TYPE-`uname -p`
   # /usr/bin/tar on OSX is BSDTAR which is for our purposes GNU tar compatible
   TAR=/usr/bin/tar
fi
if [ "x`root-config --platform`" = "xsolaris" ]; then
   TYPE=$TYPE-`uname -r`
   TYPE=$TYPE-`uname -p`
fi

# debug build?
DEBUG=
BUILDOPT=`grep ROOTBUILD config/Makefile.config`
if [ "x$BUILDOPT" != "x" ]; then
   if echo $BUILDOPT | grep debug > /dev/null 2>& 1 ; then
      DEBUG=".debug"
   fi
else
   if echo $ROOTBUILD | grep debug > /dev/null 2>& 1 ; then
      DEBUG=".debug"
   fi
fi

# MSI?
if [ "x$1" = "x-msi" ]; then
   MSI=1
   shift
fi

# compiler specified?
COMPILER=$1
if [ "x${COMPILER}" != "x" ]; then
   COMPILER="-${COMPILER}"
fi

TARFILE=marabou_v${ROOTVERS}.${TYPE}${COMPILER}${DEBUG}
TARFILE=$TARFILE.tgz

# make a symlink in order to prepend version number to all files

rm -f ../$ROOTVERS
ln -s `basename $PWD` ../$ROOTVERS

#construct dir list

DIRLIST=""
for i in bin data doc icons include lib macros obj powerpc sounds templates
do
	DIRLIST="$DIRLIST ../$ROOTVERS/$i"
done

echo "Making MARaBOU binary distribution: $TARFILE"
echo Directories:
echo $DIRLIST

rm -f $TARFILE

tar -czf $TARFILE --exclude-vcs $DIRLIST

exit 0
