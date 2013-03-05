#! /bin/sh

# Script to produce distribution of Marabou.
# Called by main Makefile.
#
# Author: Otto Schaile

TARFILE=marabou_$CERN_LEVEL.tgz

EXCLUDEBIN=`for i in bin/* ; do echo --exclude $i; done `
EXCLUDEGG=`for i in */src/G__* ; do echo --exclude $i; done `
EXCLUDEO=`for i in */src/*.o ; do echo --exclude $i; done `
EXCLUDED=`for i in */src/*.d ; do echo --exclude $i; done `
EXCLUDECVS=`for i in *; do find $i -name CVS -exec echo --exclude {} --exclude {}/* \;; done`

TARFILE=`root-config --version`
TARFILE=marabou-`dirname $TARFILE`.`basename $TARFILE`.tgz

echo "[Making MARaBOU distribution: $TARFILE]"

rm -f $TARFILE

tar --exclude *.so  \
    $EXCLUDEBIN $EXCLUDEGG $EXCLUDEO $EXCLUDED $EXCLUDECVS \
	--exclude test \
    --exclude *gz --exclude *.bck \
    --exclude G__* --exclude *.o  --exclude *.d --exclude *.tar \
-czf $TARFILE *

TF=`root-config --version`
TF=marabou-`dirname $TF`.`basename $TF`.tgz

TARFILE=`root-config --version`
TARFILE=marabou-html-`dirname $TARFILE`.`basename $TARFILE`.tgz

echo "[Making MARaBOU HTML docu: $TARFILE]"

tar -czf $TARFILE htmldoc

exit 0
