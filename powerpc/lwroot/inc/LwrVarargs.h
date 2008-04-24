#ifndef __NR_Varargs_h__
#define __NR_Varargs_h__


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
