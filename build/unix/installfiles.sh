#!/usr/bin/tclsh

# tcl script to install marabou files (libs or bins)
# without disturbing running applications

global argv

# get from unix arg list:
#   number of files to be installed
#   destination where to install files
#   list of file names to be installed

set nofFiles [expr [llength $argv] - 1]
set destDir [lindex $argv $nofFiles]
set lofFiles [lrange $argv 0 [expr $nofFiles - 1]]

# read current file version from file .version
# increment version number
# if version file doesn't exist start with version=1

if {[file exists $destDir/.version]} {
	set vf [open $destDir/.version r]
	gets $vf version
	set oldVersion $version
	incr version
	close $vf
} else {
	set oldVersion 0
	set version 1
}

# copy files to destination dir, append version number
# link main file name to new version
# remove old files - will actually be removed as soon as last consumer stops

foreach f $lofFiles {
	set fileName [file tail $f]
	exec cp $f $destDir/$fileName.$version 
	cd $destDir
	exec ln -fs $fileName.$version $fileName
	exec rm -f $fileName.$oldVersion
	puts "\[$f -> $destDir/$fileName.$version\]"
	cd ..
}

# save version number to file .version

set vf [open $destDir/.version w]
puts $vf $version
close $vf
