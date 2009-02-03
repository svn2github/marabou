#ifndef __LwrVarargs_h__
#define __LwrVarargs_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrVarargs.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: Variable arg lists
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////


#if defined(sparc) && defined(__CC_ATT301__)
        extern "C" __builtin_va_arg_incr(...);
        typedef char *va_list;
#   define va_end(ap)
#   define va_start(ap, parmN) ap= (char*)(&parmN+1)
#   define va_arg(ap, mode) ((mode*) __builtin_va_arg_incr((mode*)ap))[0]
#   define va_(arg) __builtin_va_alist

#   include <stdio.h>
extern "C" {
        int vfprintf(FILE*, const char *fmt, va_list ap);
        char *vsprintf(char*, const char *fmt, va_list ap);
};

#else
#   include <stdarg.h>
#   if defined(sparc) && !defined(__GNUG__) && !defined(__CC_SUN21__) && !defined(__SVR4)
#       define va_(arg) __builtin_va_alist
#   else
#       define va_(arg) arg
#   endif
#endif

#endif
