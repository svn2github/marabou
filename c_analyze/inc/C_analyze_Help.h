const char Help_text[]= "
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Warning: This file is no longer updated. Please use
         HelpBrowser instead.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

The program C_analyze serves to control the analysis
process of the MBS and ROOT based DAQ suite MARABOU.

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Important note:  This program should not be run in
background since it uses its xterm for (colored) logging
messages.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

The input to the analysis process (M_analyze) may be a
generic MBS file (LMD format), a ROOT file or from an
Online running  MBS system on a PowerPC or CVC connected
by a TcpIp. In the latter case all events will be
requested (Sync mode) .

Histograms are saved in a memory mapped file which must
reside on a local filesystem (e.g. /tmp). The size of
this file should be adjusted to keep all histograms and
not wasting too much memory. This file should be saved at
the end of a run to a normal root file to compress it.
This can optionally done automatically at end of a run.
In this case the name of the file will be Hists_RUN.root.
If the mapped file exists at start of a new run filling of
histograms may continue on this file. The current contents
is saved in the same directory (e.g. /tmp) with the
name extended by _save.root.
Use of a memory mapped file can be avoided giving it the
name none . This saves memory otherwise needed for the
memory mapped file.
Also M_analyze runs without threads avoiding possible 
problems with them.No updates of the mapped file is needed
saving some time

However no further communication with M_analyze 
(like Pause, Stop) is possible. No Online monitoring is
possible so its normally only useful for replaying
data.

If the user wants to reset individual histograms (while
continue filling others) it has to this in the 
Analysis program. A good place for this is the 
xxxFilterMacro.C, it  c a n n o t  be done at the
place where histograms are booked since restoring
of the existing histograms must happen later.
Another way is a piece of code at start of the 
xxx::Analyze() at start of new run. 
Note that a variable controlling this must be static. 
e.g.
   static Int_t first_time = 0;
   if(first_time == 0){
      first_time = 1;
      hxx->Reset(); 
      ....

M_analyze can write events in ROOT format as a ROOT tree.
This file can be analyzed by M_analyze itself, by a root 
macro or a special program like HistPresent.

As input an ASCII file with a list of files can be given
(select Input source: Filelist). This file has the 
following format:

# input   first  number  param  histo       output
# file    event  of evs  file   file        file
#--------------------------------------------------------
    
run1.root  0        0   none    +           runs_1-6.root 
run2.root  0        0   -       +           +
run3.root  0        0   -       h_1-3.root  +

run5.root  0        0   -       +           +
run6.root  0        0   -       h_5-6.root  +

This list reads all events (0 means all) of each file. It
puts histograms for run 1 - 3 on one file and runs 5,6 on
a second one. All (selected) raw data are collected into
runs_1-6.root. No parameter file is used.

Note: Lines starting with # and empty lines are ignored.
+ means: keep writing on previous file, in case of the
histograms this means the memory mapped file. - means
keep previous definition.

Filenames may be automatically generated using the
RunNumber which gets incremented when stopping a run. If
the capital letters RUN appear in a file name field they
get replaced by the RunNumber. RunNumbers are forced to
have at least 3 digits (007). Intentionally RunNumbers
are not used in the name of the memory mapped file to
avoid accidental filling up of local discs. Filenames
must not be called: none.

An entry for a parameter file for the analyis process is
provided. The name of this file is transfered as argument
to M_analyze at startup. Rereading during Running (Status
of run must be paused) may be triggered by pressing
Reload.

If a large nmumber of histograms get filled the time
taken by . To save processing time about 2/3 are not
filled for all events. The downscale factor may be
adjusted before start of a run and when pausing.

The  contents o selected histograms may be reset when
pausing, * selects all.

M_analyze fills a rate history of the last 5 minutes. On
the display the 0 label corresponds to now.

C_analyze remembers its parameters in an ASCII file
C_analyze.def (backed up to C_analyze.def.bck).

C_analyze controls also MBS running on Lynx processors.
It uses for this rsh commands to start MBS and TcpIp
socket connections when MBS is running. C_analyze can be
used to compile the users readout  function on the Lynx
processors.

The MBS control files (e.g. node_list.txt) must reside on
the users login directory on Lynx plus the path provided
in the Mbs Directory input field. Logging output of MBS
is written into file mbslog.l in this directory.

The command Clear MBS executes via rsh the command
/mbs/MbsVersion/script/mbsshutall.sc.

Configure starts up on Lynx all the required processes
(Prompter, Transport etc.).

C_analyze communicates with M_analyze by sending
messages. M_analyze writes its PID and status into a file
/var/tmp/M_analyze.pid to avoid issueing ps commands.

Start starts the M_analyze process on Linux and issues a
start acquisition on the Lynx side.

Pause stops acception of events on the Linux side. In
synchronous mode (the normal one) data taking on the Lynx
side will also stop when all buffers are filled.  It does
n o t yet issue an stop acquisition on the Lynx side as
it should be. This will be implemented eventually. This
that after resume events may still in the buffers. If
this is undisirable triggers should be inhibited by
hardware before the Pause.

Stop stops the current run, M_analyze closes output files
and exits. On the Lynx side collector and tranport are
restarted and the system is put in state Configured.

M_analyze can be run in a debugger (gdb). This option 
is in the Parameter popup menu. Selecting value 1 justs
runs M_analyze within gdb, 2 puts a breakpoint at main()
i.e. start of program to allow more breakpoints to be set.
Problems/features:

With Pause data taking on Lynx should be interrupted.
";









