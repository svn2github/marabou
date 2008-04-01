#!/bin/bash
rootversion=`root-config --version | sed 's%/%.%'`
tarfile=marabou_$rootversion.tgz
echo $tarfile
tar -czf $tarfile bin data  icons  include  lib  macros  obj  powerpc  sounds  templates

