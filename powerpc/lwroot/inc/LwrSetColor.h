#ifndef __SetColor_h__
#define __SetColor_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			SetColor.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: Text colors
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

static const char setblack[]	=	"\033[39m"; 	// set "default" color - may be other than "black"
static const char setred[]		=	"\033[31m";
static const char setgreen[]	=	"\033[32m";
static const char setyellow[]	=	"\033[33m";
static const char setblue[] 	=	"\033[34m";
static const char setmagenta[]	=	"\033[35m";
static const char setcyan[] 	=	"\033[36m";
static const char bell[] 		=	"\007\007\007";

#endif
