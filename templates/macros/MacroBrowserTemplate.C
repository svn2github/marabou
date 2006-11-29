//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             %%MACRO_FILE%%
// Purpose:          %%MACRO_TITLE%%
%%BeginOfCode%%
%ARG1%// Syntax:           .x $macroFile($argType $argName$argDel
%ARGN%//                   $argSpace$argType $argName$argDel
%%EndOfCode%%
%%MACRO_SYNTAX%%
%%BeginOfCode%%
%ARG1%// Arguments:        $argType $argName$argSpace -- $argTitle
%ARGN%//                   $argType $argName$argSpace -- $argTitle
%%EndOfCode%%
%%MACRO_ARGUMENTS%%
// Description:      %%MACRO_TITLE%%
// Author:           %%AUTHOR%%
// Mail:             %%MAILADDR%%
// URL:              %%URL%%
// Revision:         $Id: MacroBrowserTemplate.C,v 1.2 2006-11-29 15:11:24 Rudolf.Lutter Exp $
// Date:             %%CREATION_DATE%%
%%BeginOfCode%%
//                   $envName:$envSpace$envVal
%%EndOfCode%%
%%MACRO_EXEC%%
//////////////////////////////////////////////////////////////////////////////

%%BeginOfCode%%
%NEW%#include <iostream>
%NEW%#include <iomanip>
%%EndOfCode%%
%%MACRO_INCLUDE_H_FILES%%

%%BeginOfCode%%
enum	{ $enumName = $enumVal };
%%EndOfCode%%
%%MACRO_ENUMS%%

%%BeginOfCode%%
%ARG1%$macroType $macroName($argType $argName$argDel
%ARG1D%$macroType $macroName($argType $argName = $argDefault$argDel
%ARG1DC%$macroType $macroName($argType $argName = "$argDefault"$argDel
%ARGN%$argSpace$argType $argName$argDel
%ARGND%$argSpace$argType $argName = $argDefault$argDel
%ARGNDC%$argSpace$argType $argName = "$argDefault"$argDel
%NEW%//>>_________________________________________________(do not change this line)
%NEW%//
%NEW%{
%NEW%// ... user code goes here ...
%NEW%}
%%EndOfCode%%
%%MACRO_CODE%%
