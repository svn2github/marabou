#!/bin/sh

SUITE=suite.c

if test -e $SUITE ; then
	rm -f $SUITE
fi

cat begin.c >> $SUITE

for i in */*.h ; do
	echo "#include \"$i\"" >> $SUITE
done

cat middle.c >> $SUITE

for i in */*.c ; do
	echo "	errors -= $(basename $i \.c)(\"$(basename $i \.c)\");" >> $SUITE
	echo "	tests++;" >> $SUITE
done

cat end.c >> $SUITE

