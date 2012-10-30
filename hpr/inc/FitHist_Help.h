#ifndef HPR_FITHISTHELP
#define HPR_FITHISTHELP
//_______________________________________________________________________________

const char Help_SaveCuts_text[]="\n\
2-dim windows (cuts) can be written to and read from\n\
an ordinary ASCII file.\n\
The format is as follows:\n\
Window1name\n\
npoints\n\
x1 y1\n\
x2 y2\n\
..\n\
Window2name\n\
\n\
etc.\n\
If a file contains 2-dim windows a commandline is\n\
added in the filelist to write out the windows.\n\
In the File popup menu of the HistPresenter main\n\
window a command is provided to read in the\n\
windows. The windows are put into memory and\n\
can be accesed in the normal way from  the \n\
popup menus provided for 2-dim histograms.";
//_______________________________________________________________________________

const char Help_Mark_text[]=
"Marks are used when expanding and fitting histograms.\n\
Marks are set pressing the middle mouse button in a \n\
histogram.\n\
If no marks are active the lower and upper edges of\n\
the histogram are used, if only one mark is active\n\
the lower or upper edge is used as the second mark\n\
depending on the position of the InOutSide switch.\n\
Marks may be set using keyboard input. (Set2Marks).\n\
They can be converted to windows. These windows\n\
may be written to a workfile to be used by other\n\
programs like cuts in 2dim histograms";
//_______________________________________________________________________________

const char Help_Fit_1dim_text[]=
"Fitting of functions to 1-dim histograms may be\n\
done in two ways:\n\
\n\
Predefined are polynmials upto degree 8, gaussians\n\
(max 3 peaks) plus a linear background and gaussians\n\
with low or high energy tails. In case with tail the\n\
parameter Ta_fract (tail fraction) is the contents\n\
of the tail part relative to the pure gaussian part.\n\
In case of more then one peak this fraction, the\n\
width of the tail and of the gaussian are the same\n\
to all peaks. Marks should be used to provide lower\n\
and upper limits and estimated positions of peaks if\n\
more than 1 gaussian is be fitted. The program\n\
estimates from this the start parameters of the fit\n\
which are presented and may be changed. Values may\n\
be fixed and bound. When using bounds please be\n\
beware of the warnings in the MINUIT manual on this.\n\
Fitted parameters may be kept as start for a new\n\
fit. As default Minos error analysis is not done but\n\
may be switched on.\n\
______________________________________________________\n\
\n\
A linear background determined from 2 regions in\n\
the spectrum outside the peaks can be incorporated\n\
in the following way:\n\
\n\
Define 4 marks to select the 2 regions. Then use the\n\
item 'Determine linear background' from the Fit\n\
popup menu to calculate the parameters const and\n\
slope of the linear background.  (This activates\n\
automatically the corresponding option  'Use linear\n\
background'.) In a subsequent fit the parameters\n\
Bg_Const and Bg_Slope will be  fixed to the above\n\
calculated values.  If needed these values can still\n\
be changed manually. Unfixing them of course will\n\
let MINUIT vary their values.\n\
______________________________________________________\n\
\n\
The quality of the start parameters of a fit determine\n\
the speed of convergence. Bad start parameters may lead\n\
MINUIT to find the expected minimum. Pressing 'Cancel'\n\
when the table with the start parameters is displayed\n\
will draw the fit function with these parameters\n\
without doing a fit.\n\
______________________________________________________\n\
\n\
The other, more general way is to edit a macro where\n\
all the functionality provided by the TFormula class\n\
may be used. Template macros serving as examples are\n\
provided to start with. This macro contains  2\n\
possibilities to provide the fit function: as\n\
formula or as normal C++ function.\n\
\n\
Currently 4 examples are provided: An exponential +\n\
a gaussian, 2 gaussians with common mean,  a Breit\n\
Wigner and 3 Landau functions.\n\
\n\
Use the command Edit User Fit Macro to edit the\n\
macro. If the macro given by the name exists this is\n\
taken otherwise a choice menu with the above 4\n\
examples is presented. Meaningful start values\n\
should be given to assure that MINUIT finds the\n\
correct minimum.\n\
";
//_______________________________________________________________________________

const char Help_Fit_2dim_text[]=
"For 2-dim histograms polynomials can be fitted to\n\
user defined marks or to the entries in regions of\n\
the histograms nomally defined with a graphical cut\n\
(see help on Cuts). In this case the entries inside\n\
the cut are averaged along the y-axis in slices of\n\
the  bins along the x-axis and the fit is done to\n\
the resulting 1-dim histogram using the  spread of\n\
the entries along  the y-axis as errors.\n\
\n\
Functions are named. They may be written on a root\n\
file and retrieved using their names. Parameters,\n\
errors etc. may be made persistent and reused in\n\
analysis programs e.g. as calibration.\n\
";
//_______________________________________________________________________________

const char Help_Cuts_text[]=
"The command InitCut allows to select an area\n\
enclosed  by a polygone to be used as a cut in\n\
expand and function fitting. Corners are set by\n\
clicking the left mouse button in the scatterplot.\n\
Successive clicks at the same position ends the\n\
input. Any number of cuts are possible.  This\n\
commands adds the defined cut to the active ones.\n\
\n\
Cuts are stored and retrieved by names. A default\n\
name is provided which can be changed during\n\
creation or via the popup menu for .\n\
\n\
Windows in 1-dim histograms can be defined with the\n\
command MarksToWindow. (See also Help on Marks)\n\
\n\
ListCut lists cuts currently in  memory. Active\n\
cuts are marked green. Pressing the select button\n\
toggles between active and passive. Pressing on the\n\
field with the name the values are displayed in a\n\
table an can be modified.\n\
\n\
Cuts and windows can be written to and read from a\n\
file (either root file or ASCII file) ";
//_______________________________________________________________________________

const char Help_Expand_text[]=
"Expand histgram using marks, if only one mark is\n\
active  the lower edge is used as the second mark.\n\
This builds a new histogram, under- and overflow\n\
bins will be refilled correctly.\n\
\n\
The command Entire shows the full range range again.\n\
\n\
To get only an expanded view move the mouse into the\n\
labels of the wanted axis, the cursor should turn\n\
into a hand with a forefinger, press the left mouse\n\
button and drag the mouse to the required position\n\
an release the button. To get back to the full\n\
picture use the command Entire from the Display\n\
popup menu";
//_______________________________________________________________________________

const char Help_RedefineAxis_text[]=
"The values of the axis labels may be redefined by\n\
providing new lower and upper limits for the axis. \n\
In this way the picture can reflect a calibrated\n\
spectrum.\n\
\n\
Note: This only changes the appearance of the picture\n\
The histogram remains unchanged.";
//_______________________________________________________________________________

const char Help_Project_text[]=
"The commands ProjectX, ProjectY produce projections\n\
on the X or Y axis for a 2-dim histogram. Active cuts\n\
will be applied.\n\
\n\
ProjectF will project the difference of the  Y -\n\
values to the values of a function previously\n\
generated by a fit (See Help on Fitting).  The result\n\
is the superposition of slices in X of the 2-dim\n\
histogram where each slice is shifted such that its Y\n\
- value equal to the function  value is moved to zero.\n\
So the resulting projection on the Y-axis will be\n\
centered at zero.\n\
\n\
This allows a banana shaped distribution to be\n\
straightened so the banana shows its real thickness in\n\
the Y-projection.";
//_______________________________________________________________________________

const char Help_Entire_text[]=
"Show full range of histogram";
//_______________________________________________________________________________

const char Help_GetRange[]=
"Show a histogram in same range as another one. A\n\
list of  histograms in memory is presented allowing\n\
a selection";
//_______________________________________________________________________________

const char Help_Superimpose_text[]=
"Superimpose a histogram on existing picture. A\n\
histogram can be selected from a histogram list by\n\
pressing the grey button left of the name. If none\n\
is selected a list of histograms in memory is\n\
presented from which selection can be done. The\n\
latter is needed to show histograms which result\n\
from an operation (scaled, summed etc.). histogram.\n\
For 1-dim the  color code is taken from its position\n\
+ 2 (position 1 gets red etc.) With this command the\n\
same scale is used as for the original histogram.\n\
\n\
A command Superimpose scaled is provided for 1-dim\n\
hists to scale the superimposed histogram to the\n\
maximum of the original one in the displayed region.";
//_______________________________________________________________________________

const char Help_SetDrawMode_text[]="\n\
The following options are supported on all types\n\
SAME   : Superimpose on previous picture in the same pad\n\
CYL    : Use Cylindrical coordinates\n\
POL    : Use Polar coordinates\n\
SPH    : Use Spherical coordinates\n\
PSR    : Use PseudoRapidity/Phi coordinates\n\
LEGO   : Draw a lego plot with hidden line removal\n\
LEGO1  : Draw a lego plot with hidden surface removal\n\
LEGO2  : Draw a lego plot using colors to show the cell contents\n\
SURF   : Draw a surface plot with hidden line removal\n\
SURF1  : Draw a surface plot with hidden surface removal\n\
SURF2  : Draw a surface plot using colors to show the cell contents\n\
SURF3  : same as SURF with in addition a contour view drawn on the top\n\
SURF4  : Draw a surface using Gouraud shading\n\
\n\
The following options are supported for 1-D types:\n\
A      : Do not draw the axis labels and tick marks\n\
B      : Bar chart option\n\
C      : Draw a smooth Curve througth the histogram bins\n\
E      : Draw error bars\n\
E0     : Draw error bars including bins with o contents\n\
E1     : Draw error bars with perpendicular lines at the edges\n\
E2     : Draw error bars with rectangles\n\
E3     : Draw a fill area througth the end points of the vertical error bars\n\
E4     : Draw a smoothed filled area through the end points of the error bars\n\
L      : Draw a line througth the bin contents\n\
P      : Draw current marker at each bin\n\
*      : Draw a * at each bin\n\
HIST   : Default \n\
\n\
The following options are supported for 2-D types:\n\
ARR    : arrow mode. Shows gradient between adjacent cells\n\
BOX    : a box is drawn for each cell with surface proportional to contents\n\
COL    : a box is drawn for each cell with a color scale varying with contents\n\
CONT   : Draw a contour plot (same as CONT3)\n\
CONT0  : Draw a contour plot using colors to distinguish contours\n\
CONT1  : Draw a contour plot using line styles to distinguish contours\n\
CONT2  : Draw a contour plot using the same line style for all contours\n\
CONT3  : Draw a contour plot using fill area colors\n\
FB     : With LEGO or SURFACE, suppress the Front-Box\n\
BB     : With LEGO or SURFACE, suppress the Back-Box\n\
SCAT   : Draw a scatter-plot (default)";
//_______________________________________________________________________________

const char Help_Workfile_text[]=
"Opening of workfiles is triggered when output of an\n\
object (histogram, cut, function) is requested.\n\
A default name may be defined in .rootrc (See Help On\n\
Customize)";
//_______________________________________________________________________________

  const char Help_CloseWindows_text[]=
"Close current canvases containing histograms\n\
(i.e. clean up screen)\n\
The lists of histograms may be closed by the\n\
command Close HistLists";
//_______________________________________________________________________________

  const char Help_ListObjects_text[]=
"Display a list of objects (histograms , cuts)\n\
currently in memory. See also Help on \n\
Show Filelist ";
//_______________________________________________________________________________

  const char Help_ShowFileList_text[]=
"Display a list of root files (.map, .root,\n\
.histlist)  in current working directory. Ordering\n\
may be done alphabetically  (or reverse) or by\n\
modification time.\n\
\n\
Tighter selection can be made by defining a\n\
FileSelectionMask. Clicking on a file displays the\n\
contents (histograms, trees , cuts).The gray\n\
square button on the left side of the objects name\n\
is used to select the object for operations like\n\
Rebin, Add etc. Selecting a cut in this way makes\n\
it memory resident and can be used when displaying\n\
trees and 2-dim histograms. The command Clear Select\n\
resets all selections, currently this does not\n\
turn the green button to gray.\n\
\n\
When a file contains many histograms it can be\n\
usful to have lists with only a subset. Such a\n\
subset can be defined in a file e.g.\n\
mysubset.histlist. (Note: The Extension .histlist\n\
is compulsory.) It may contain the name of the\n\
file followed by the name of the histogram to be\n\
selected (one per line) separated by a space\n\
or only the name of the histogram.\n\
\n\
All histograms in such a list may be displayed\n\
in one single canvas allowing a quick overview\n\
on many histgrams.\n\
\n\
This list can be produced either by an editor or\n\
by selecting the subset with HistPresent itself\n\
(i.e. pressing the grey buttons left of the name)\n\
and then clicking on Compose list.\n\
//_______________________________________________________________________________\n\
\n\
";
 const char Help_Display_Tree_text[]=
"Some simple tools are provided to histogram the\n\
variables contained in a root tree (in the simplest\n\
case just a ntuple). Clicking on tree produces a\n\
list showing the variables of the tree. Just\n\
clicking into a name displays a 1-dim histogram with\n\
a default binning, the range is calculated from the\n\
first few 1000 values. One can however provide a\n\
user defined histogram by pressing Use selected\n\
hist.  Exactly one histogram must be provided with\n\
the correct dimensions. A scatter plot is produced\n\
by selecting 2 variables by pressing the grey button\n\
left of the name (which turns into green) and then\n\
clicking Show leafs. Pressing the green button\n\
deselects it.\n\
\n\
One can provide a user defined histogram by pressing\n\
Use selected hist. Such a histogram may be defined\n\
from  the amin menu. Exactly one histogram must be\n\
provided with the correct dimensions.\n\
\n\
To display not just the value itself but an\n\
expression of it press Edit expression. An\n\
expression can contain the variables names\n\
explicitly (e.g. sqrt(var1) ) or the name can be\n\
taken from a selection. This is done by putting $1\n\
as a placeholder. (e.g. Defining an expression:\n\
$1&&0xfff and then clicking on a variables name will\n\
select only the lower 12 bit of the variables value.\n\
For 2-dim hists expressions like sqrt($1):$2&&0xfff\n\
may be used. Similiarly formula cuts may be applied\n\
by Edit formula cut. (e.g. $1 > 1000 & $1 <2000).\n\
Applying graphical cuts (TCutG) are possible by\n\
giving the name of a memory resident TCutG and the 2\n\
variables names used for the cut. Detailed\n\
information can be found in the class descriptions\n\
of TTree and TCutG in ROOTs WWW site, the ROOT\n\
HOWTOS contain useful examples.";
//_______________________________________________________________________________

const char Help_SelectionMask_text[]=
"Selection mask may be defined in 3 different ways:\n\
\n\
If option 'Use Regexp' is active:\n\
\n\
Use regular expression as Selection Mask.\n\
 \n\
Examples: ^M:    all  starting with M\n\
          w$:    all  ending with w\n\
          [0-9]  all containing a number\n\
\n\
If option 'Use Regexp' is n o t active:\n\
Use wild card expression:\n\
\n\
Examples: *mxy*  all containing mxy\n\
          *mxy*1  all containing mxy and ending with 1\n\
\n\
If the expression contains:\n\
 & (read AND) or | (read OR) or ! (read NOT) the\n\
two expressions before and after the operator are\n\
used and the operator is applied.\n\
\n\
Examples: xxx & yyy  true if name contains xxx AND yyy\n\
          xxx | yyy  true if name contains xxx OR yy\n\
          xxx ! yyy  true if name contains xxx but not yyy\n\
          !zzz       true if name does not contain zzz\n\
";
//_______________________________________________________________________________

const char Help_ShowSelected_text[]=
"This command displays up to 16 selected\n\
histograms in one single canvas. Selection is\n\
done by pressing the grey button in a histogram\n\
list which should turn green. Selection may\n\
be done from different lists.\n\
\n\
In this canvas only the following operations are\n\
possible (From the Display pupup menu):\n\
\n\
'Show all as selected (Range only) or (Range,\n\
Min, Max)':\n\
\n\
First select a histgram with the middle mouse,\n\
then adjust the desired range by moving the left\n\
mouse pressed in the axis scale.\n\
\n\
'Rebin all':\n\
\n\
Rebin all histograms with the same rebin value.\n\
\n\
'Activate automatic update':\n\
\n\
This is useful in an online environment where\n\
histograms are written to a memory mapped file.\n\
This option allows the refresh the display of the\n\
histograms in regular intervals. The refresh rate\n\
(AutoUpdateDelay) may be choosen from the Option\n\
'HistPresent numerical options' or in the .rootrc\n\
file.  ";
//_______________________________________________________________________________

  const char Help_Macro_text[]=
"Present a list of root macros (files ending .C) which\n\
have a first line starting with //Hint: , The hint will\n\
appear in the ToolTipBox when the cursor in moved on\n\
the button.\n\
Pressing the button will execute the macro";
//_______________________________________________________________________________

 const char Help_Rebin_text[]=
"Rebin a selected histogram using the selected\n\
(green) value. The new bin content may the sum or\n\
average of the original\n\
bins";
//_______________________________________________________________________________

  const char Help_Operators_text[]=
"Operate on selected histgrams \n\
If two histograms (or more for Add)\n\
are selected one can\n\
perform the following operations: \n\
+ Add: hist1 + factor*hist2\n\
- Sub: hist1 - factor*hist2\n\
* Mul: hist1 * factor*hist2\n\
* Div: hist1 / (factor*hist2)\n\
\n\
The resulting histograms are shown and stored in\n\
memory with new names. Note that in this case  the\n\
order of selection is important.\n\
\n\
If only 1 histogram is selected with operation\n\
multiply then this gets scaled\n\
\n\
For operation Add any number of selections are\n\
possible";
//_______________________________________________________________________________

  const char Help_EditAttr_text[]=
"A root macro FH_defaults.C can be used to customize\n\
the attributes of a histogram and canvas. If existing\n\
it is called before the picture is painted. The macro\n\
is called with the histogram and canvas name. In this\n\
way it possible to set parameters  like ranges lin-log\n\
scale, fill colors etc. depending on the  name of a\n\
histogram. If FH_defaults.C does not exist yet a\n\
template is provided.\n\
\n\
This can also be used to get and superimpose other\n\
histograms, please read the provided template\n\
FH_defaults.C for more information";
//_______________________________________________________________________________

  const char Help_DefineHist_text[]=
"A new histogram normally to be used when\n\
displaying a root tree (ntuple) can be\n\
defined by the command Define new histogram\n\
If NBinsY = 0 a 1-dim histogram is assumed";
//_______________________________________________________________________________

  const char Help_Customize_text[]=
"The following resources can be put into\n\
a .rootrc file either in the current or \n\
in the home directory. Units for Window \n\
parameters are in pixel. (N.b. Normally\n\
a screen has 1280 x 1024 pixels.)\n\
\n\
HistPresent.PaperSizeX:         20\n\
HistPresent.PaperSizeY:         26\n\
HistPresent.WindowX:            530\n\
HistPresent.WindowY:              5\n\
HistPresent.WindowYShift:        30\n\
HistPresent.WindowXShift:        0\n\
HistPresent.WindowXWidth_1dim:  750\n\
HistPresent.WindowYWidth_1dim:  550\n\
HistPresent.WindowXWidth_2dim:  750\n\
HistPresent.WindowYWidth_2dim:  650\n\
HistPresent.WindowXWidth_many:  800\n\
HistPresent.WindowYWidth_many:  800\n\
HistPresent.WindowArrange_many:  ontop\n\
HistPresent.WindowArrange_many:  sidebyside\n\
\n\
HistPresent.WorkFileName:  q3dparams.root\n\
HistPresent.PrintCommand:  lpr -Paul\n\
HistPresent.LastSettingsName: defaults/Last\n\
\n\
With WindowXShift is > 0 one can arrange the \n\
pictures as tiles. In this case one should set\n\
WindowXShift = WindowXWidth_1dim + 4 ,\n\
WindowYShift = WindowYWidth_1dim + 30,\n\
\n\
This will cause an arrangment like this:\n\
--------------------------\n\
| window1    |  window2  |\n\
--------------------------\n\
| window3    |  window4  |\n\
--------------------------\n\
etc.\n\
\n\
If the path LastSettingsName is defined \n\
ranges and scales (lin - log) of histograms are\n\
remembered during and between sessions in files\n\
with names defaults/Last_histname.def. \n\
It is convenient to use an own subdirectory \n\
(in this example $CWD/defaults) for this.\n\
LastSettingsName may be any legal pathname\n\
e.g. /tmp/$(USER)/\n\
Note $(USER) instead of just $USER for compatibility\n\
with NT.\n\
";
#endif
