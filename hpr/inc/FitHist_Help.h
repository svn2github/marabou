#ifndef HPR_FITHISTHELP
#define HPR_FITHISTHELP
//_______________________________________________________________________________

const char Help_SaveCuts_text[]="
2-dim windows (cuts) can be written to and read from
an ordinary ASCII file.
The format is as follows:
Window1name
npoints
x1 y1
x2 y2
..
Window2name

etc.
If a file contains 2-dim windows a commandline is
added in the filelist to write out the windows.
In the File popup menu of the HistPresenter main
window a command is provided to read in the
windows. The windows are put into memory and
can be accesed in the normal way from  the 
popup menus provided for 2-dim histograms.";
//_______________________________________________________________________________

const char Help_Mark_text[]=
"Marks are used when expanding and fitting histograms.
Marks are set pressing the middle mouse button in a 
histogram.
If no marks are active the lower and upper edges of
the histogram are used, if only one mark is active
the lower or upper edge is used as the second mark
depending on the position of the InOutSide switch.
Marks may be set using keyboard input. (Set2Marks).
They can be converted to windows. These windows
may be written to a workfile to be used by other
programs like cuts in 2dim histograms";
//_______________________________________________________________________________

const char Help_Fit_1dim_text[]=
"Fitting of functions to 1-dim histograms may be
done in two ways:

Predefined are polynmials upto degree 8, gaussians
(max 3 peaks) plus a linear background and gaussians
with low or high energy tails. In case with tail the
parameter Ta_fract (tail fraction) is the contents
of the tail part relative to the pure gaussian part.
In case of more then one peak this fraction, the
width of the tail and of the gaussian are the same
to all peaks. Marks should be used to provide lower
and upper limits and estimated positions of peaks if
more than 1 gaussian is be fitted. The program
estimates from this the start parameters of the fit
which are presented and may be changed. Values may
be fixed and bound. When using bounds please be
beware of the warnings in the MINUIT manual on this.
Fitted parameters may be kept as start for a new
fit. As default Minos error analysis is not done but
may be switched on.
______________________________________________________

A linear background determined from 2 regions in
the spectrum outside the peaks can be incorporated
in the following way:

Define 4 marks to select the 2 regions. Then use the
item 'Determine linear background' from the Fit
popup menu to calculate the parameters const and
slope of the linear background.  (This activates
automatically the corresponding option  'Use linear
background'.) In a subsequent fit the parameters
Bg_Const and Bg_Slope will be  fixed to the above
calculated values.  If needed these values can still
be changed manually. Unfixing them of course will
let MINUIT vary their values.
______________________________________________________

The quality of the start parameters of a fit determine
the speed of convergence. Bad start parameters may lead
MINUIT to find the expected minimum. Pressing 'Cancel'
when the table with the start parameters is displayed
will draw the fit function with these parameters
without doing a fit.
______________________________________________________

The other, more general way is to edit a macro where
all the functionality provided by the TFormula class
may be used. Template macros serving as examples are
provided to start with. This macro contains  2
possibilities to provide the fit function: as
formula or as normal C++ function.

Currently 4 examples are provided: An exponential +
a gaussian, 2 gaussians with common mean,  a Breit
Wigner and 3 Landau functions.

Use the command Edit User Fit Macro to edit the
macro. If the macro given by the name exists this is
taken otherwise a choice menu with the above 4
examples is presented. Meaningful start values
should be given to assure that MINUIT finds the
correct minimum.
";
//_______________________________________________________________________________

const char Help_Fit_2dim_text[]=
"
For 2-dim histograms polynomials can be fitted to
user defined marks or to the entries in regions of
the histograms nomally defined with a graphical cut
(see help on Cuts). In this case the entries inside
the cut are averaged along the y-axis in slices of
the  bins along the x-axis and the fit is done to
the resulting 1-dim histogram using the  spread of
the entries along  the y-axis as errors.

Functions are named. They may be written on a root
file and retrieved using their names. Parameters,
errors etc. may be made persistent and reused in
analysis programs e.g. as calibration.
";
//_______________________________________________________________________________

const char Help_Cuts_text[]=
"
The command InitCut allows to select an area
enclosed  by a polygone to be used as a cut in
expand and function fitting. Corners are set by
clicking the left mouse button in the scatterplot.
Successive clicks at the same position ends the
input. Any number of cuts are possible.  This
commands adds the defined cut to the active ones.

Cuts are stored and retrieved by names. A default
name is provided which can be changed during
creation or via the popup menu for .

Windows in 1-dim histograms can be defined with the
command MarksToWindow. (See also Help on Marks)

ListCut lists cuts currently in  memory. Active
cuts are marked green. Pressing the select button
toggles between active and passive. Pressing on the
field with the name the values are displayed in a
table an can be modified.

Cuts and windows can be written to and read from a
file (either root file or ASCII file) ";
//_______________________________________________________________________________

const char Help_Expand_text[]=
"Expand histgram using marks, if only one mark is
active  the lower edge is used as the second mark.
This builds a new histogram, under- and overflow
bins will be refilled correctly.

The command Entire shows the full range range again.

To get only an expanded view move the mouse into the
labels of the wanted axis, the cursor should turn
into a hand with a forefinger, press the left mouse
button and drag the mouse to the required position
an release the button. To get back to the full
picture use the command Entire from the Display
popup menu";
//_______________________________________________________________________________

const char Help_RedefineAxis_text[]=
"The values of the axis labels may be redefined by
providing new lower and upper limits for the axis. 
In this way the picture can reflect a calibrated
spectrum.

Note: This only changes the appearance of the picture
The histogram remains unchanged.";
//_______________________________________________________________________________

const char Help_Project_text[]=
"The commands ProjectX, ProjectY produce projections
on the X or Y axis for a 2-dim histogram. Active cuts
will be applied.

ProjectF will project the difference of the  Y -
values to the values of a function previously
generated by a fit (See Help on Fitting).  The result
is the superposition of slices in X of the 2-dim
histogram where each slice is shifted such that its Y
- value equal to the function  value is moved to zero.
So the resulting projection on the Y-axis will be
centered at zero.

This allows a banana shaped distribution to be
straightened so the banana shows its real thickness in
the Y-projection.";
//_______________________________________________________________________________

const char Help_Entire_text[]=
"Show full range of histogram";
//_______________________________________________________________________________

const char Help_GetRange[]=
"Show a histogram in same range as another one. A
list of  histograms in memory is presented allowing
a selection";
//_______________________________________________________________________________

const char Help_Superimpose_text[]=
"Superimpose a histogram on existing picture. A
histogram can be selected from a histogram list by
pressing the grey button left of the name. If none
is selected a list of histograms in memory is
presented from which selection can be done. The
latter is needed to show histograms which result
from an operation (scaled, summed etc.). histogram.
For 1-dim the  color code is taken from its position
+ 2 (position 1 gets red etc.) With this command the
same scale is used as for the original histogram.

A command Superimpose scaled is provided for 1-dim
hists to scale the superimposed histogram to the
maximum of the original one in the displayed region.";
//_______________________________________________________________________________

const char Help_SetDrawMode_text[]="\
The following options are supported on all types
SAME   : Superimpose on previous picture in the same pad
CYL    : Use Cylindrical coordinates
POL    : Use Polar coordinates
SPH    : Use Spherical coordinates
PSR    : Use PseudoRapidity/Phi coordinates
LEGO   : Draw a lego plot with hidden line removal
LEGO1  : Draw a lego plot with hidden surface removal
LEGO2  : Draw a lego plot using colors to show the cell contents
SURF   : Draw a surface plot with hidden line removal
SURF1  : Draw a surface plot with hidden surface removal
SURF2  : Draw a surface plot using colors to show the cell contents
SURF3  : same as SURF with in addition a contour view drawn on the top
SURF4  : Draw a surface using Gouraud shading

The following options are supported for 1-D types:
A      : Do not draw the axis labels and tick marks
B      : Bar chart option
C      : Draw a smooth Curve througth the histogram bins
E      : Draw error bars
E0     : Draw error bars including bins with o contents
E1     : Draw error bars with perpendicular lines at the edges
E2     : Draw error bars with rectangles
E3     : Draw a fill area througth the end points of the vertical error bars
E4     : Draw a smoothed filled area through the end points of the error bars
L      : Draw a line througth the bin contents
P      : Draw current marker at each bin
*      : Draw a * at each bin
HIST   : Default 

The following options are supported for 2-D types:
ARR    : arrow mode. Shows gradient between adjacent cells
BOX    : a box is drawn for each cell with surface proportional to contents
COL    : a box is drawn for each cell with a color scale varying with contents
CONT   : Draw a contour plot (same as CONT3)
CONT0  : Draw a contour plot using colors to distinguish contours
CONT1  : Draw a contour plot using line styles to distinguish contours
CONT2  : Draw a contour plot using the same line style for all contours
CONT3  : Draw a contour plot using fill area colors
FB     : With LEGO or SURFACE, suppress the Front-Box
BB     : With LEGO or SURFACE, suppress the Back-Box
SCAT   : Draw a scatter-plot (default)";
//_______________________________________________________________________________

const char Help_Workfile_text[]=
"Opening of workfiles is triggered when output of an
object (histogram, cut, function) is requested.
A default name may be defined in .rootrc (See Help On
Customize)";
//_______________________________________________________________________________

  const char Help_CloseWindows_text[]=
"Close current canvases containing histograms
(i.e. clean up screen)
The lists of histograms may be closed by the
command Close HistLists";
//_______________________________________________________________________________

  const char Help_ListObjects_text[]=
"Display a list of objects (histograms , cuts)
currently in memory. See also Help on 
Show Filelist ";
//_______________________________________________________________________________

  const char Help_ShowFileList_text[]=
"Display a list of root files (.map, .root,
.histlist)  in current working directory. Ordering
may be done alphabetically  (or reverse) or by
modification time.

Tighter selection can be made by defining a
FileSelectionMask. Clicking on a file displays the
contents (histograms, trees , cuts).The gray
square button on the left side of the objects name
is used to select the object for operations like
Rebin, Add etc. Selecting a cut in this way makes
it memory resident and can be used when displaying
trees and 2-dim histograms. The command Clear Select
resets all selections, currently this does not
turn the green button to gray.

When a file contains many histograms it can be
usful to have lists with only a subset. Such a
subset can be defined in a file e.g.
mysubset.histlist. (Note: The Extension .histlist
is compulsory.) It may contain the name of the
file followed by the name of the histogram to be
selected (one per line) separated by a space
or only the name of the histogram.

All histograms in such a list may be displayed
in one single canvas allowing a quick overview
on many histgrams.

This list can be produced either by an editor or
by selecting the subset with HistPresent itself
(i.e. pressing the grey buttons left of the name)
and then clicking on Compose list.
//_______________________________________________________________________________

";
 const char Help_Display_Tree_text[]=
"Some simple tools are provided to histogram the
variables contained in a root tree (in the simplest
case just a ntuple). Clicking on tree produces a
list showing the variables of the tree. Just
clicking into a name displays a 1-dim histogram with
a default binning, the range is calculated from the
first few 1000 values. One can however provide a
user defined histogram by pressing Use selected
hist.  Exactly one histogram must be provided with
the correct dimensions. A scatter plot is produced
by selecting 2 variables by pressing the grey button
left of the name (which turns into green) and then
clicking Show leafs. Pressing the green button
deselects it.

One can provide a user defined histogram by pressing
Use selected hist. Such a histogram may be defined
from  the amin menu. Exactly one histogram must be
provided with the correct dimensions.

To display not just the value itself but an
expression of it press Edit expression. An
expression can contain the variables names
explicitly (e.g. sqrt(var1) ) or the name can be
taken from a selection. This is done by putting $1
as a placeholder. (e.g. Defining an expression:
$1&&0xfff and then clicking on a variables name will
select only the lower 12 bit of the variables value.
For 2-dim hists expressions like sqrt($1):$2&&0xfff
may be used. Similiarly formula cuts may be applied
by Edit formula cut. (e.g. $1 > 1000 & $1 <2000).
Applying graphical cuts (TCutG) are possible by
giving the name of a memory resident TCutG and the 2
variables names used for the cut. Detailed
information can be found in the class descriptions
of TTree and TCutG in ROOTs WWW site, the ROOT
HOWTOS contain useful examples.";
//_______________________________________________________________________________

const char Help_SelectionMask_text[]=
"Selection mask may be defined in 3 different ways:

If option 'Use regular expression syntax' is active:

Use a  regular expression as Selection Mask.
 
Examples: ^M:    all  starting with M
          w$:    all  ending with w
          [0-9]  all containing a number

If option 'Use regular expression syntax' 
is n o t active:
Use wild card expression:

Examples: *mxy*  all containing mxy
          *mxy*1  all containing mxy and ending with 1

If the expression contains:
 & (read AND) or | (read OR) or ! (read NOT) the
two expressions before and after the operator are
used and the operator is applied.

Examples: xxx & yyy  true if name contains xxx AND yyy
          xxx | yyy  true if name contains xxx OR yy
          xxx ! yyy  true if name contains xxx but not yyy
          !zzz       true if name does not contain zzz
";
//_______________________________________________________________________________

const char Help_ShowSelected_text[]=
"This command displays up to 16 selected
histograms in one single canvas. Selection is
done by pressing the grey button in a histogram
list which should turn green. Selection may
be done from different lists.

In this canvas only the following operations are
possible (From the Display pupup menu):

'Show all as selected (Range only) or (Range,
Min, Max)':

First select a histgram with the middle mouse,
then adjust the desired range by moving the left
mouse pressed in the axis scale.

'Rebin all':

Rebin all histograms with the same rebin value.

'Activate automatic update':

This is useful in an online environment where
histograms are written to a memory mapped file.
This option allows the refresh the display of the
histograms in regular intervals. The refresh rate
(AutoUpdateDelay) may be choosen from the Option
'HistPresent numerical options' or in the .rootrc
file.  ";
//_______________________________________________________________________________

  const char Help_Macro_text[]=
"Present a list of root macros (files ending .C) which
have a first line starting with //Hint: , The hint will
appear in the ToolTipBox when the cursor in moved on
the button.
Pressing the button will execute the macro";
//_______________________________________________________________________________

 const char Help_Rebin_text[]=
"Rebin a selected histogram using the selected
(green) value. The new bin content may the sum or
average of the original
bins";
//_______________________________________________________________________________

  const char Help_Operators_text[]=
"Operate on selected histgrams 
If two histograms (or more for Add)
are selected one can
perform the following operations: 
+ Add: hist1 + factor*hist2
- Sub: hist1 - factor*hist2
* Mul: hist1 * factor*hist2
* Div: hist1 / (factor*hist2)

The resulting histograms are shown and stored in
memory with new names. Note that in this case  the
order of selection is important.

If only 1 histogram is selected with operation
multiply then this gets scaled

For operation Add any number of selections are
possible";
//_______________________________________________________________________________

  const char Help_EditAttr_text[]=
"A root macro FH_defaults.C can be used to customize
the attributes of a histogram and canvas. If existing
it is called before the picture is painted. The macro
is called with the histogram and canvas name. In this
way it possible to set parameters  like ranges lin-log
scale, fill colors etc. depending on the  name of a
histogram. If FH_defaults.C does not exist yet a
template is provided.

This can also be used to get and superimpose other
histograms, please read the provided template
FH_defaults.C for more information";
//_______________________________________________________________________________

  const char Help_DefineHist_text[]=
"A new histogram normally to be used when
displaying a root tree (ntuple) can be
defined by the command Define new histogram
If NBinsY = 0 a 1-dim histogram is assumed";
//_______________________________________________________________________________

  const char Help_Customize_text[]=
"The following resources can be put into
a .rootrc file either in the current or 
in the home directory. Units for Window 
parameters are in pixel. (N.b. Normally
a screen has 1280 x 1024 pixels.)

HistPresent.PaperSizeX:         20
HistPresent.PaperSizeY:         26
HistPresent.WindowX:            530
HistPresent.WindowY:              5
HistPresent.WindowYShift:        30
HistPresent.WindowXShift:        0
HistPresent.WindowXWidth_1dim:  750
HistPresent.WindowYWidth_1dim:  550
HistPresent.WindowXWidth_2dim:  750
HistPresent.WindowYWidth_2dim:  650
HistPresent.WindowXWidth_many:  800
HistPresent.WindowYWidth_many:  800
HistPresent.WindowArrange_many:  ontop
HistPresent.WindowArrange_many:  sidebyside

HistPresent.WorkFileName:  q3dparams.root
HistPresent.PrintCommand:  lpr -Paul
HistPresent.LastSettingsName: defaults/Last

With WindowXShift is > 0 one can arrange the 
pictures as tiles. In this case one should set
WindowXShift = WindowXWidth_1dim + 4 ,
WindowYShift = WindowYWidth_1dim + 30,

This will cause an arrangment like this:
--------------------------
| window1    |  window2  |
--------------------------
| window3    |  window4  |
--------------------------
etc.

If the path LastSettingsName is defined 
ranges and scales (lin - log) of histograms are
remembered during and between sessions in files
with names defaults/Last_histname.def. 
It is convenient to use an own subdirectory 
(in this example $CWD/defaults) for this.
LastSettingsName may be any legal pathname
e.g. /tmp/$(USER)/
Note $(USER) instead of just $USER for compatibility
with NT.
";
#endif
