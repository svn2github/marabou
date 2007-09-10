#!/usr/bin/tclsh

proc testArch {} {
	catch "exec uname -a" arch
	set arch [split $arch]
	set f [open "/etc/redhat-release" r]
	gets $f s
	close $f
	set s [split $s]
	set linux "xx"
	set rel "???"
	if {[lindex $s 0] == "Red"} {
		set linux "rh"
		set rel [lindex $s [expr [llength $s] - 2]]
	} elseif {[lindex $s 0] == "Fedora"} {
		set linux "fc"
		set rel [lindex $s [expr [llength $s] - 2]]
	} elseif {[lindex $s 0] == "Scientific"} {
		set linux "slc"
		set rel [lindex $s [expr [llength $s] - 2]]
	}
	set arch [lindex $arch [expr [llength $arch] - 2]]
	return [list $linux $rel $arch]
}

proc writeEnv {uEnv linux release arch admin} {
	global userEnv envFile env
	set uVar $userEnv(var)
	set f [open $envFile w]
	set clnpEnvs [split $userEnv($uEnv,cleanup,envs) ":"]
	if {[llength $clnpEnvs] > 0} {
		set clnpWhat [split $userEnv($uEnv,cleanup,what) ":"]
		foreach e $clnpEnvs {
			set newEnv {}
			set envVal [split $env($e) ":"]
			foreach v $envVal {
				foreach w $clnpWhat {
					if {![string match $w $v]} {
						set removeIt 0
					} else {
						set removeIt 1
						break
					}
				}
				if {$removeIt == 0} {
					if {[lsearch $newEnv $v] == -1} { lappend newEnv $v }
				}
			}
			puts $f "$e=[join $newEnv :]"
		}
	}
	puts $f "$userEnv($uEnv,action)"
	puts $f "export $uVar\_ENV=$uEnv"
	set a XX
	if {$arch == "i386"} { set a 32 }
	if {$arch == "x86_64"} { set a 64 }
	if {$admin} {
		set ua admin
		puts $f "if \[ -d \$$uVar/bin_$linux$release\_$a \]; then cd \$$uVar; rm -f bin; ln -s bin_$linux$release\_$a bin; cd -; fi"
		puts $f "if \[ -d \$$uVar/obj_$linux$release\_$a \]; then cd \$$uVar; rm -f obj; ln -s obj_$linux$release\_$a obj; cd -; fi"
		puts $f "if \[ -d \$$uVar/lib_$linux$release\_$a \]; then cd \$$uVar; rm -f lib; ln -s lib_$linux$release\_$a lib; cd -; fi"
	} else {
		set ua user
	}
	close $f
	if {$uEnv != "quit"} {
		puts "\[Setting environment \"$uEnv\" ($ua) for linux $linux$release on arch $arch\]"
	}
}

proc checkLock {isAdmin isWish} {
	global env userEnv
	if {$isAdmin} {
		set lock $env(HOME)/.$userEnv(name)_*.login
		set lofLocks [glob -nocomplain $lock]
		set n 0
		foreach l $lofLocks {
			if {[file exists $l]} {
				set f [open $l r]
				set wstr [list "----------------------------------------------------------" \
								"Warning: Another user is currently logged in!" ""]
				while {[gets $f line] != -1} { lappend wstr "  $line" }
				lappend wstr ""
				close $f
				if {$isWish} {
					label .wrn$n -text [join $wstr "\n"] -background red -borderwidth 2 -justify left -relief ridge -width 40
					pack .wrn$n -side top
					incr n
				} else {
					lappend wstr "----------------------------------------------------------"
					puts [join $wstr "\n"]
				}
			}
		}
	}	
}
	
proc createLock {uEnv isAdmin linux release arch} {
	global env userEnv
	if {$isAdmin && $uEnv != "quit"} {
		set lock $env(HOME)/.$userEnv(name)_$uEnv.login
		set f [open $lock w]
		puts $f "Host:   $env(HOSTNAME)"
		puts $f "Env:    $uEnv - $userEnv($uEnv,title)"
		puts $f "Date:   [exec date]"
		puts $f "Linux:  $linux$release"
		puts $f "Arch:   $arch"
		close $f
	}
}	
	
proc extractVersion {} {
	global env userEnv
	set odd 1
	foreach u [array get userEnv "*,name"] {
		if {$odd == 1} { set odd 0; continue }
		set odd 1
		set xt [string first %v $userEnv($u,title)]
		if {$xt != -1 && [info exists userEnv($u,version)]} {
			foreach p [split $userEnv($u,action)] {
				if {[string first ROOTSYS= $p] != -1} {
					set p [lindex [split $p =] 1]
					if {[catch {exec sed -n "s,.*ROOT_RELEASE *\\\"\\\(.*\\\)\\\".*,\\1,p" $p/include/RVersion.h} rvers] != 0} { set rvers "??" }
				}
				if {[string first MARABOU= $p] != -1} {
					set p [lindex [split $p =] 1]
					if {[catch {exec sed -n "s,.*MARABOU_RELEASE *\\\"\\\(.*\\\)\\\".*,\\1,p" $p/include/MVersion.h} mvers] != 0} { set mvers "??" }
				}
			}
			set xr [string first %r $userEnv($u,version)]
			if {$xr >= 0} { set userEnv($u,version) [string replace $userEnv($u,version) $xr [expr $xr + 1] $rvers] }
			set xm [string first %m $userEnv($u,version)]
			if {$xm >= 0} { set userEnv($u,version) [string replace $userEnv($u,version) $xm [expr $xm + 1] $mvers] }
		}
	}		
}

proc setTooltip {widget text} {
	if { $text != "" } {
		bind $widget <Any-Enter> [list after 1000 [list showTooltip %W $text]]
		bind $widget <Any-Leave> {destroy %W.tooltip}
	}
}

proc showTooltip {widget text} {
	if { [string match $widget* [winfo containing  [winfo pointerx .] [winfo pointery .]] ] == 0  } {
		return
	}

	catch { destroy $widget.tooltip }

	set tooltip [toplevel $widget.tooltip -bd 1 -bg black]
	wm overrideredirect $tooltip 1
	wm attributes $tooltip
	pack [label $tooltip.label -bg lightyellow -fg black -text $text -justify left]

	set width [winfo reqwidth $tooltip.label]
	set height [winfo reqheight $tooltip.label]

	set positionX [winfo pointerx .]
	set positionY [expr [winfo pointery .] + 25]

	wm geometry $tooltip [join  "$width x $height + $positionX + $positionY" {}]
	raise $tooltip
}

if {$argc > 1} {
	puts stderr "msetup: Wrong number of arguments - $argc"
	exit
}

if {$argc == 1} {
	set env(MSETUP_MODE) q
	set env(MARABOU_ENV) [lindex $argv 0]
}

if {[info exists env(MSETUP_DB)]} {
	set setupFile $env(MSETUP_DB)
} else {
	set setupFile $env(HOME)/marabou/tools/m.tcl
}

if {[info exists env(MSETUP_OUT)]} {
	set envFile $env(MSETUP_OUT)
} else {
	set envFile $env(HOME)/.msetup
}

exec rm -f $envFile
exec touch $envFile

set x [testArch]
set linux [lindex $x 0]
if {$linux != "rh" && $linux != "fc" && $linux != "slc"} {
	puts stderr "msetup: Unknown linux type - $linux (should be \"rh\", \"fc\", or \"sl\"). Giving up."
	exit
}
set release [lindex $x 1]
set arch [lindex $x 2]
if {$arch != "i386" && $arch != "x86_64"} {
	puts stderr "msetup: Unknown architecture - $linux (should be \"i386\" or \"x86_64\"). Giving up."
	exit
}

if {![info exists env(MSETUP_MODE)]} { set env(MSETUP_MODE) w }
if {![info exists env(MARABOU_ENV)]} { set env(MARABOU_ENV) pro }

if {[info vars tk_version] == ""} {
	set isWish 0
	if {$env(DISPLAY) != "" && $env(MSETUP_MODE) != "t" && $env(MSETUP_MODE) != "q"} {
		exec wish [info script]
		exit
	}
} else {
	set isWish 1
}

if {[file exists $setupFile]} {
	source $setupFile

	set uVar $userEnv(var)
	if {[lsearch -exact $lofUserEnvs quit] == -1} {
		set userEnv(quit,name)			quit
		set userEnv(quit,title)			"Don't set any environment"
		set userEnv(quit,version)		{}
		set userEnv(quit,cleanup,envs)	{}
		set userEnv(quit,cleanup,what)	{}
		set userEnv(quit,action) "
			unset ROOTSYS
			unset CERN_LEVEL
			unset $uVar
			unset $uVar\_INSTALL\_PATH
			export CWD=~"
		lappend lofUserEnvs quit
	}
	set userEnv(arch) "(Platform: Linux $linux$release on $arch)"
} else {
	puts stderr "msetup: Setup file \"$setupFile\" not found. Can't proceed."
	exit
}

checkLock $isAdmin $isWish

extractVersion

if {$env(MSETUP_MODE) == "q"} {
	set m $env(MARABOU_ENV)
	set x [lsearch $lofUserEnvs $m]
	if {$x == -1} {
		puts stderr "msetup: Env variable MARABOU_ENV=$m not set properly. Can't proceed."
	} else {
		writeEnv [lindex $lofUserEnvs $x] $linux $release $arch $isAdmin
		createLock [lindex $lofUserEnvs $x] $isAdmin $linux $release $arch;
	}	
	exit
} elseif {$isWish} {
	label .header -text $userEnv(title) -background blue -foreground white -borderwidth 2 -relief ridge -width 40
	pack .header -side top
	label .arch -text $userEnv(arch) -background blue -foreground white -borderwidth 2 -relief ridge -width 40
	pack .arch -side top
	
	foreach u $lofUserEnvs {
		button .b$u -command " writeEnv $u $linux $release $arch $isAdmin; \
								createLock $u $isAdmin $linux $release $arch; \
								exit"
		set title $userEnv($u,title)
		if {$title == ""} { set title $u } else { set title "$u: $title" }
		set xt [string first %v $title]
		if {$xt != -1} {
			set title [string replace $title $xt [expr $xt + 1]]
			setTooltip .b$u $userEnv($u,version)
		}			
		.b$u config -text $title
		pack .b$u -side top -fill x
	}
} else {
	set n 1
	puts "User environments available:"
	foreach u $lofUserEnvs {
		set title $userEnv($u,title)
		if {$title == ""} { set title $u } else { set title "$u: $title" }
		set xt [string first %v $title]
		if {$xt != -1} { set title [string replace $title $xt [expr $xt + 1] $userEnv($u,version)] }
		puts "\[$n\] $title"
		incr n
	}
	while {1} {
		set m $env(MARABOU_ENV)
		set x -1
		if {$m == ""} {
			puts -nonewline "Enter number or name to select environment: "
		} else {
			set x [lsearch $lofUserEnvs $m]
			if {$x == -1} {
				puts -nonewline "Enter number or name to select environment: "
			} else {
				incr x 1
				puts -nonewline "Enter number or name to select environment \[$m or $x]: "
			}
		}
		flush stdout
		gets stdin envIdx
		if {$envIdx == ""} { set envIdx $x }
		set l [llength $lofUserEnvs]
		if {$envIdx <= 0 || $envIdx > $l} {
			puts stderr "Illegal input - $envIdx (should be in \[1,$l\])"
		} else {
			incr envIdx -1
			writeEnv [lindex $lofUserEnvs $envIdx] $linux $release $arch $isAdmin
			createLock [lindex $lofUserEnvs $envIdx] $isAdmin $linux $release $arch;
			exit
		}
	}
}
