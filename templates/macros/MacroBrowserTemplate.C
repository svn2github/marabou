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
// Revision:         $Id: MacroBrowserTemplate.C,v 1.4 2007-09-02 06:11:55 Rudolf.Lutter Exp $
// Date:             %%CREATION_DATE%%
%%BeginOfCode%%
//                   $envName:$envSpace$envVal
%%EndOfCode%%
%%MACRO_EXEC%%
//////////////////////////////////////////////////////////////////////////////

%%BeginOfCode%%
%NEW%#include <iostream>
%NEW%#include <iomanip>
%GUI%#include "TGMrbMacroFrame.h"
%%EndOfCode%%
%%MACRO_INCLUDE_H_FILES%%

%%BeginOfCode%%
%B%enum E$macroNameArgNums {
%N% 		$enumName = $enumVal,
%E% 	}
%%EndOfCode%%
%%MACRO_ARG_ENUMS%%

%%BeginOfCode%%
%B%enum E$macroNameEnums {
%N% 		$enumName = $enumVal,
%E% 	}
%%EndOfCode%%
%%MACRO_ENUMS%%

%%BeginOfCode%%
%ARGGUI%$macroType $macroName(TGMrbMacroFrame * GuiPtr)
%ARG1%$macroType $macroName($argType $argName$argDel
%ARG1D%$macroType $macroName($argType $argName = $argDefault$argDel
%ARG1DC%$macroType $macroName($argType $argName = "$argDefault"$argDel
%ARGN%$argSpace$argType $argName$argDel
%ARGND%$argSpace$argType $argName = $argDefault$argDel
%ARGNDC%$argSpace$argType $argName = "$argDefault"$argDel
%ARGNCHK%$argSpaceBool_t $argNameCheck$argDel
%ARGNCHKD%$argSpaceBool_t $argNameCheck = kFALSE$argDel
%NEW%//>>_________________________________________________(do not change this line)
%NEW%//
%NEW%{
%NEW%// ... user code goes here ...
%NEW%}
%%EndOfCode%%
%%MACRO_CODE%%
