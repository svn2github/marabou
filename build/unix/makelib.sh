#! /bin/sh

# Script to generate a shared library.
# Called by main Makefile.
#
# Author: Fons Rademakers, 29/2/2000

if [ "$1" = "-v" ] ; then
   MAJOR=$2
   MINOR=$3
   REVIS=$4
   shift
   shift
   shift
   shift
fi

if [ "$1" = "-x" ] ; then
   EXPLICIT="yes"
   shift
fi

PLATFORM=$1
LD=$2
LDFLAGS=$3
SOFLAGS=$4
SONAME=$5
LIB=$6
OBJS=$7
EXTRA=$8

rm -f $LIB

if [ $PLATFORM = "macosx" ]; then
   soext="dylib"
elif [ $PLATFORM = "aix" ] || [ $PLATFORM = "aix5" ]; then
   soext="a"
else
   soext="so"
fi

LIBVERS=
VERSION=
EXPLLNKCORE=
if [ "x$EXPLICIT" = "xyes" ]; then
   if test $LIB != "lib/libCint.$soext" && test $LIB != "lib/libReflex.$soext" ; then
      NEEDREFLEX=""
      if [ "$(bin/root-config --dicttype)" != "cint" ]; then
          NEEDREFLEX="-lReflex"
          if [ $LIB != "lib/libCintex.$soext" ]; then
              NEEDREFLEX="-lCintex $NEEDREFLEX"
          fi
      fi
      if [ $LIB != "lib/libCore.$soext" ]; then
         EXPLLNKCORE="-Llib $NEEDREFLEX -lCore -lCint"
      else
         EXPLLNKCORE="-Llib -lCint"
      fi
   fi
fi

if [ $PLATFORM = "aix" ] || [ $PLATFORM = "aix5" ]; then
   if [ $PLATFORM = "aix" ]; then
      makeshared="/usr/ibmcxx/bin/makeC++SharedLib"
   fi
   if [ $PLATFORM = "aix5" ]; then
      makeshared="/usr/vacpp/bin/makeC++SharedLib"
   fi
   if [ $LD = "xlC" ]; then
      cmd="$makeshared -o $LIB -p 0 $OBJS $EXTRA $EXPLLNKCORE"
      echo $cmd
      $cmd
   fi
   if [ $LD = "g++" ]; then
      cmd="$LD $SOFLAGS $LDFLAGS -o $LIB $OBJS $EXTRA $EXPLLNKCORE"
      echo $cmd
      $cmd
   fi
elif [ $PLATFORM = "alphaegcs" ] || [ $PLATFORM = "hpux" ] || \
     [ $PLATFORM = "solaris" ]   || [ $PLATFORM = "sgi" ]; then
   cmd="$LD $SOFLAGS $LDFLAGS -o $LIB $OBJS $EXTRA $EXPLLNKCORE"
   echo $cmd
   $cmd
elif [ $PLATFORM = "lynxos" ]; then
   cmd="ar rv $LIB $OBJS $EXTRA"
   echo $cmd
   $cmd
elif [ $PLATFORM = "fbsd" ] || [ $PLATFORM = "obsd" ]; then
   if [ "x$MAJOR" = "x" ] ; then
      cmd="$LD $SOFLAGS$SONAME $LDFLAGS -o $LIB \
         `lorder $OBJS | tsort -q` $EXTRA $EXPLLNKCORE"
   else
      cmd="$LD $SOFLAGS$SONAME.$MAJOR.$MINOR $LDFLAGS -o $LIB.$MAJOR.$MINOR \
         `lorder $OBJS | tsort -q` $EXTRA $EXPLLNKCORE"
   fi
   echo $cmd
   $cmd
elif [ $PLATFORM = "macosx" ]; then
   macosx_minor=`sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2`
   # Look for a fink installation
   FINKDIR=`which fink 2>&1 | sed -ne "s/\/bin\/fink//p"`
   export DYLD_LIBRARY_PATH=`pwd`/lib:$DYLD_LIBRARY_PATH
   if [ $macosx_minor -ge 3 ]; then
      unset LD_PREBIND
      export MACOSX_DEPLOYMENT_TARGET=10.$macosx_minor
   fi
   # check if in 64 bit mode
   m64=
   if [ "x`echo $LDFLAGS | grep -- '-m64'`" != "x" ]; then
      m64=-m64
   fi
   # We need two library files: a .dylib to link to and a .so to load
   BUNDLE=`echo $LIB | sed s/.dylib/.so/`
   # Add versioning information to shared library if available
   if [ "x$MAJOR" != "x" ]; then
      VERSION="-compatibility_version ${MAJOR} -current_version ${MAJOR}.${MINOR}.${REVIS}"
      SONAME=`echo $SONAME | sed "s/\(.*\)\.dylib/\1.${MAJOR}.dylib/"`
      LIB=`echo $LIB | sed "s/\(\/*.*\/.*\)\.dylib/\1.${MAJOR}.${MINOR}.dylib/"`
      LIBVERS=$LIB
   fi
   if [ $macosx_minor -ge 4 ]; then
      cmd="$LD $SOFLAGS$SONAME $m64 -o $LIB $OBJS \
           -ldl $EXTRA $EXPLLNKCORE $VERSION"
   else
      cmd="$LD $SOFLAGS$SONAME $m64 -o $LIB $OBJS \
           `[ -d ${FINKDIR}/lib ] && echo -L${FINKDIR}/lib` \
           -ldl $EXTRA $EXPLLNKCORE $VERSION"
   fi
   echo $cmd
   $cmd
   linkstat=$?
   if [ $linkstat -ne 0 ]; then
      exit $linkstat
   fi
   if [ "x`echo $SOFLAGS | grep -- '-g'`" != "x" ]; then
      opt=-g
   else
      opt=-O
   fi
   if [ $LIB != $BUNDLE ]; then
       if [ $macosx_minor -ge 4 ]; then
	   cmd="ln -fs `basename $LIB` $BUNDLE"
       elif [ $macosx_minor -ge 3 ]; then
	   cmd="$LD $opt $m64 -bundle -undefined dynamic_lookup -o \
                $BUNDLE $OBJS `[ -d ${FINKDIR}/lib ] && echo -L${FINKDIR}/lib` \
                -ldl $EXTRA $EXPLLNKCORE"
       else
	   cmd="$LD $opt -bundle -undefined suppress -o $BUNDLE \
	        $OBJS `[ -d ${FINKDIR}/lib ] && echo -L${FINKDIR}/lib` \
                -ldl $EXTRA $EXPLLNKCORE"
       fi
       echo $cmd
       $cmd
   fi
elif [ $LD = "KCC" ]; then
   cmd="$LD $LDFLAGS -o $LIB $OBJS $EXTRA $EXPLLNKCORE"
   echo $cmd
   $cmd
elif [ $LD = "build/unix/wingcc_ld.sh" ]; then
   EXPLLNKCORE=
   if [ $SONAME != "libCint.dll" ]; then
      if [ $SONAME != "libCore.dll" ]; then
         EXPLLNKCORE="-Llib -lCore -lCint"
      else
         EXPLLNKCORE="-Llib -lCint"
      fi
   fi
   if [ "x$MAJOR" != "x" ] ; then
      MINORVERSION=`echo ${MINOR}$REVIS | sed 's,^0,,'`
      VERSION="-Wl,--major-image-version,$MAJOR -Wl,--minor-image-version,$MINORVERSION"
      SONAME=`echo $SONAME | sed "s/.*\./&${MAJOR}./"`
      LIB=`echo $LIB | sed "s/\/*.*\/.*\./&${MAJOR}.${MINOR}./"`
      LIBVERS=$LIB
   fi
   cmd="$LD $VERSION $SOFLAGS$SONAME $LDFLAGS -o $LIB $OBJS $EXTRA $EXPLLNKCORE"
   echo $cmd
   $cmd
else
   if [ "x$MAJOR" = "x" ] ; then
      cmd="$LD $SOFLAGS$SONAME $LDFLAGS -o $LIB $OBJS $EXTRA $EXPLLNKCORE"
      echo $cmd
      $cmd
   else
      cmd="$LD $SOFLAGS$SONAME.$MAJOR.$MINOR $LDFLAGS \
           -o $LIB.$MAJOR.$MINOR $OBJS $EXTRA $EXPLLNKCORE"
      echo $cmd
      $cmd
   fi
fi

linkstat=$?
if [ $linkstat -ne 0 ]; then
   exit $linkstat
fi


if [ "x$MAJOR" != "x" ]; then
    if [ -f $LIB.$MAJOR.$MINOR ]; then
        # Versioned library has format foo.so.3.05
	ln -fs $SONAME.$MAJOR.$MINOR $LIB.$MAJOR
	ln -fs $SONAME.$MAJOR        $LIB
    elif [ -f $LIBVERS ]; then
	# Versioned library has format foo.3.05.so
	source_file=`echo $SONAME | sed "s/.*\./&${MINOR}./"`
	LIBNOMAJORMINOR=`echo $LIB|sed 's,\.'$MAJOR'\.'$MINOR',,'`
	if [ $LIB != $LIBNOMAJORMINOR ]; then
	    LIBNOMINOR=`echo $LIB|sed 's,\.'$MINOR',,'`
	    ln -fs $source_file $LIBNOMINOR
	    ln -fs $source_file $LIBNOMAJORMINOR
	fi
    fi
fi

if [ $PLATFORM = "hpux" ]; then
   chmod 555 $LIB
fi

echo "==> $LIB done"

exit 0
