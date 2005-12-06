//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadColors.C
// Purpose:          Define VT100-like strings to output colored text
// Description:      VT100-like sequences to output colored text
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadColors.C,v 1.2 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	const char setblack[]	 =   "\033[30m";
	const char setred[] 	 =   "\033[31m";
	const char setgreen[]	 =   "\033[32m";
	const char setyellow[]   =   "\033[33m";
	const char setblue[]	 =   "\033[34m";
	const char setmagenta[]  =   "\033[35m";
	const char setcyan[]	 =   "\033[36m";
	const char bell[]		 =   "\007\007\007";
}
