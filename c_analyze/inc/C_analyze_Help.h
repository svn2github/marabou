const char Help_text[]= 
"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
Warning: This file is no longer updated. Please use\n\
         HelpBrowser instead.\n\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
\n\
The program C_analyze serves to control the analysis\n\
process of the MBS and ROOT based DAQ suite MARABOU.\n\
\n\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
Important note:  This program should not be run in\n\
background since it uses its xterm for (colored) logging\n\
messages.\n\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
\n\
The input to the analysis process (M_analyze) may be a\n\
generic MBS file (LMD format), a ROOT file or from an\n\
Online running  MBS system on a PowerPC or CVC connected\n\
by a TcpIp. In the latter case all events will be\n\
requested (Sync mode) .\n\
\n\
Histograms are saved in a memory mapped file which must\n\
reside on a local filesystem (e.g. /tmp). The size of\n\
this file should be adjusted to keep all histograms and\n\
not wasting too much memory. This file should be saved at\n\
the end of a run to a normal root file to compress it.\n\
This can optionally done automatically at end of a run.\n\
In this case the name of the file will be Hists_RUN.root.\n\
If the mapped file exists at start of a new run filling of\n\
histograms may continue on this file. The current contents\n\
is saved in the same directory (e.g. /tmp) with the\n\
name extended by _save.root.\n\
Use of a memory mapped file can be avoided giving it the\n\
name none . This saves memory otherwise needed for the\n\
memory mapped file.\n\
Also M_analyze runs without threads avoiding possible \n\
problems with them.No updates of the mapped file is needed\n\
saving some time\n\
\n\
However no further communication with M_analyze \n\
(like Pause, Stop) is possible. No Online monitoring is\n\
possible so its normally only useful for replaying\n\
data.\n\
\n\
If the user wants to reset individual histograms (while\n\
continue filling others) it has to this in the \n\
Analysis program. A good place for this is the \n\
xxxFilterMacro.C, it  c a n n o t  be done at the\n\
place where histograms are booked since restoring\n\
of the existing histograms must happen later.\n\
Another way is a piece of code at start of the \n\
xxx::Analyze() at start of new run. \n\
Note that a variable controlling this must be static. \n\
e.g.\n\
   static Int_t first_time = 0;\n\
   if(first_time == 0){\n\
      first_time = 1;\n\
      hxx->Reset(); \n\
      ....\n\
\n\
M_analyze can write events in ROOT format as a ROOT tree.\n\
This file can be analyzed by M_analyze itself, by a root \n\
macro or a special program like HistPresent.\n\
\n\
As input an ASCII file with a list of files can be given\n\
(select Input source: Filelist). This file has the \n\
following format:\n\
\n\
# input   first  number  param  histo       output\n\
# file    event  of evs  file   file        file\n\
#--------------------------------------------------------\n\
    \n\
run1.root  0        0   none    +           runs_1-6.root \n\
run2.root  0        0   -       +           +\n\
run3.root  0        0   -       h_1-3.root  +\n\
\n\
run5.root  0        0   -       +           +\n\
run6.root  0        0   -       h_5-6.root  +\n\
\n\
This list reads all events (0 means all) of each file. It\n\
puts histograms for run 1 - 3 on one file and runs 5,6 on\n\
a second one. All (selected) raw data are collected into\n\
runs_1-6.root. No parameter file is used.\n\
\n\
Note: Lines starting with # and empty lines are ignored.\n\
+ means: keep writing on previous file, in case of the\n\
histograms this means the memory mapped file. - means\n\
keep previous definition.\n\
\n\
Filenames may be automatically generated using the\n\
RunNumber which gets incremented when stopping a run. If\n\
the capital letters RUN appear in a file name field they\n\
get replaced by the RunNumber. RunNumbers are forced to\n\
have at least 3 digits (007). Intentionally RunNumbers\n\
are not used in the name of the memory mapped file to\n\
avoid accidental filling up of local discs. Filenames\n\
must not be called: none.\n\
\n\
An entry for a parameter file for the analyis process is\n\
provided. The name of this file is transfered as argument\n\
to M_analyze at startup. Rereading during Running (Status\n\
of run must be paused) may be triggered by pressing\n\
Reload.\n\
\n\
If a large nmumber of histograms get filled the time\n\
taken by . To save processing time about 2/3 are not\n\
filled for all events. The downscale factor may be\n\
adjusted before start of a run and when pausing.\n\
\n\
The  contents o selected histograms may be reset when\n\
pausing, * selects all.\n\
\n\
M_analyze fills a rate history of the last 5 minutes. On\n\
the display the 0 label corresponds to now.\n\
\n\
C_analyze remembers its parameters in an ASCII file\n\
C_analyze.def (backed up to C_analyze.def.bck).\n\
\n\
C_analyze controls also MBS running on Lynx processors.\n\
It uses for this rsh commands to start MBS and TcpIp\n\
socket connections when MBS is running. C_analyze can be\n\
used to compile the users readout  function on the Lynx\n\
processors.\n\
\n\
The MBS control files (e.g. node_list.txt) must reside on\n\
the users login directory on Lynx plus the path provided\n\
in the Mbs Directory input field. Logging output of MBS\n\
is written into file mbslog.l in this directory.\n\
\n\
The command Clear MBS executes via rsh the command\n\
/mbs/MbsVersion/script/mbsshutall.sc.\n\
\n\
Configure starts up on Lynx all the required processes\n\
(Prompter, Transport etc.).\n\
\n\
C_analyze communicates with M_analyze by sending\n\
messages. M_analyze writes its PID and status into a file\n\
/var/tmp/M_analyze.pid to avoid issueing ps commands.\n\
\n\
Start starts the M_analyze process on Linux and issues a\n\
start acquisition on the Lynx side.\n\
\n\
Pause stops acception of events on the Linux side. In\n\
synchronous mode (the normal one) data taking on the Lynx\n\
side will also stop when all buffers are filled.  It does\n\
n o t yet issue an stop acquisition on the Lynx side as\n\
it should be. This will be implemented eventually. This\n\
that after resume events may still in the buffers. If\n\
this is undisirable triggers should be inhibited by\n\
hardware before the Pause.\n\
\n\
Stop stops the current run, M_analyze closes output files\n\
and exits. On the Lynx side collector and tranport are\n\
restarted and the system is put in state Configured.\n\
\n\
M_analyze can be run in a debugger (gdb). This option \n\
is in the Parameter popup menu. Selecting value 1 justs\n\
runs M_analyze within gdb, 2 puts a breakpoint at main()\n\
i.e. start of program to allow more breakpoints to be set.\n\
Problems/features:\n\
\n\
With Pause data taking on Lynx should be interrupted.\n\
";









