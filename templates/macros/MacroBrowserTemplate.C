//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             %%MACRO_FILE%%
// Purpose:          %%MACRO_TITLE%%
%%BeginOfCode%%
%ARGGUI%// Syntax:           .L $macroFile
%ARGGUI%//                   $macroName(TGMrbMacroFrame * GuiPtr)
%ARG1%// Syntax:           .L $macroFile
%ARG1%//                   $macroName($argType $argName$argDel
%ARGN%//                   $argSpace$argType $argName$argDel
%%EndOfCode%%
%%MACRO_SYNTAX%%
%%BeginOfCode%%
%ARGGUI%// Arguments:        TGMrbMacroFrame * GuiPtr -- pointer to MacroBrowser GUI
%ARG1%// Arguments:        $argType $argName$argSpace -- $argTitle
%ARGN%//                   $argType $argName$argSpace -- $argTitle
%%EndOfCode%%
%%MACRO_ARGUMENTS%%
// Description:      %%MACRO_TITLE%%
// Author:           %%AUTHOR%%
// Mail:             %%MAILADDR%%
// URL:              %%URL%%
// Revision:         $Id: MacroBrowserTemplate.C,v 1.6 2007-09-26 07:43:05 Rudolf.Lutter Exp $
// Date:             %%CREATION_DATE%%
%%BeginOfCode%%
//                   $envName:$envSpace$envVal
%%EndOfCode%%
%%MACRO_EXEC%%
//////////////////////////////////////////////////////////////////////////////

%%BeginOfCode%%
%B%//__________________________________________________________[C++ HEADER FILES]
%B%//////////////////////////////////////////////////////////////////////////////
%B%//+IncludeFiles
%NEW%#include <iostream>
%NEW%#include <iomanip>
%GUI%#include "TGMrbMacroFrame.h"
%E%//-IncludeFiles
%%EndOfCode%%
%%MACRO_INCLUDE_H_FILES%%

%%BeginOfCode%%
%B%//______________________________________________________[C++ ENUM DEFINITIONS]
%B%//////////////////////////////////////////////////////////////////////////////
%B%// Name:           E<MacroName>ArgNums
%B%// Format:         kArg<ArgName> = <ArgNumber>
%B%// Description:    Enums to address macro arguments by their numbers
%B%//////////////////////////////////////////////////////////////////////////////
%B%//+ArgNums
%B%
%B%enum E$macroNameArgNums {
%N% 		$enumName = $enumVal,
%E% 	};
%E%//-ArgNums
%%EndOfCode%%
%%MACRO_ARG_ENUMS%%

%%BeginOfCode%%
%B%//______________________________________________________[C++ ENUM DEFINITIONS]
%B%//////////////////////////////////////////////////////////////////////////////
%B%// Name:           E<MacroName>Enums
%B%// Format:         k<ArgName><ArgState> = <StateValue>
%B%// Description:    Enums defining possible states of radio/check buttons
%B%//////////////////////////////////////////////////////////////////////////////
%B%//+Enums
%B%
%B%enum E$macroNameEnums {
%N% 		$enumName = $enumVal,
%E% 	};
%E%//-Enums
%%EndOfCode%%
%%MACRO_ENUMS%%

%%BeginOfCode%%
%B%//+CallingSequence
%ARGGUI%$macroType $macroName(TGMrbMacroFrame * GuiPtr)
%ARG1%$macroType $macroName($argType $argName$argDel
%ARG1D%$macroType $macroName($argType $argName = $argDefault$argDel
%ARG1DC%$macroType $macroName($argType $argName = "$argDefault"$argDel
%ARGN%$argSpace$argType $argName$argDel
%ARGND%$argSpace$argType $argName = $argDefault$argDel
%ARGNDC%$argSpace$argType $argName = "$argDefault"$argDel
%ARGNCHK%$argSpaceBool_t $argNameCheck$argDel
%ARGNCHKD%$argSpaceBool_t $argNameCheck = kFALSE$argDel
%E%//-CallingSequence
%NEW%//
%NEW%{
%NEW%// ... user code goes here ...
%NEW%}
%%EndOfCode%%
%%MACRO_CODE%%
