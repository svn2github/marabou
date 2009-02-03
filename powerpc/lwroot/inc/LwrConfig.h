#ifndef __LwrConfig_h__
#define __LwrConfig_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrConfig.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: RConfig
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

/*---- machines --------------------------------------------------------------*/

#ifdef __hpux
#   define R__HPUX
#   define R__UNIX
#   define ANSICPP
#endif

#ifdef _AIX
#   define R__AIX
#   define R__UNIX
#   define ANSICPP
#   define NEED_STRCASECMP
#endif

#ifdef __alpha
#   define R__ALPHA
#   define ANSICPP
#   ifndef __VMS
#      define R__UNIX
#      define R__B64
#      define R__BYTESWAP
#      if __DECCXX_VER >= 60060002
#         define R__VECNEWDELETE /* supports overloading of new[] and delete[] */
#         define R__PLACEMENTDELETE /* supports overloading placement delete */
#      endif
#   else
#      define R__VMS
#      define cxxbug
#      define NEED_STRCASECMP
#      define G__NONSCALARFPOS
#   endif
#endif

#ifdef __sun
#   ifdef __SVR4
#      define R__SOLARIS
#      define ANSICPP
#      ifdef __i386
#         define R__I386
#         define R__BYTESWAP
#      endif
#   else
#      define R__SUN
#      include <stdlib.h>
#   endif
#   define R__UNIX
#   define NEED_STRING
#   define NEED_SIGJMP
#   if __SUNPRO_CC > 0x420
#      define R__ANSISTREAM
#   endif
#endif

#ifdef __sgi
#   define R__SGI
#   define R__UNIX
#   define ANSICPP
#   define NEED_STRING
#   define NEED_SIGJMP
#   ifdef IRIX64
#      define R__SGI64
#   endif
#endif

#if defined(__linux) && !defined(__powerpc)
#   define R__LINUX
#   define R__UNIX
#   define R__BYTESWAP
#   define NEED_SIGJMP
#endif

#if defined(__linux) && defined(__powerpc)
#   define R__MKLINUX
#   define R__LINUX
#   define R__UNIX
#   define NEED_SIGJMP
#   if __GNUC_MINOR__ >= 90   /* egcs */
#      define R__PPCEGCS
#   endif
#endif

#if defined(__FreeBSD__)
#   define R__FBSD
#   define R__UNIX
#   define R__BYTESWAP
#   define R__NOSTATS         /* problem using stats with FreeBSD malloc/free */
#endif

#ifdef _HIUX_SOURCE
#   define R__HIUX
#   define R__UNIX
#   define NEED_SIGJMP
#   define ANSICPP
#endif

#ifdef __GNUG__
#   define R__GNU
#   define ANSICPP
#   if __GNUC_MINOR__ >= 90    /* egcs 1.0.3 */
#      define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#      define R__PLACEMENTDELETE /* supports overloading placement delete */
#   endif
#   if __GNUC_MINOR__ >= 91    /* egcs 1.1 */
#      define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   endif
#endif

#ifdef __KCC
#   define R__KCC
#   define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#   define R__PLACEMENTDELETE /* supports overloading placement delete */
#   define ANSICPP
#endif

#ifdef R__ACC
#   define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#endif

#ifdef _WIN32
#ifndef WIN32
#   define WIN32
#endif
#endif

#ifdef BORLAND
#   define MSDOS        /* Windows will always stay MSDOS */
#   define ANSICPP
#   define R__INT16
#   define R__BYTESWAP
#endif

#ifdef __SC__
#   define SC
#   if defined(macintosh)
#      define R__MAC
#      define NEED_STRING
#      define ANSICPP
#   elif WIN32
#      define NEED_STRING
#      define NEED_STRCASECMP
#      define ANSICPP
#   else
#      define MSDOS
#      define NEED_STRCASECMP
#   endif
#endif

#ifdef WIN32
#   define NEED_STRING
#   define NEED_STRCASECMP
#   define ANSICPP
#   define R__BYTESWAP
#endif

#ifdef __MWERKS__
#   define R__MWERKS
#   define R__MAC
#   define ANSICPP
#   define NEED_STRING
#   define NEED_STRCASECMP
#endif


/*--- memory and object statistics -------------------------------------------*/

/* #define R__NOSTATS     */


/*--- cpp --------------------------------------------------------------------*/

#ifdef ANSICPP
   /* symbol concatenation operator */
#   define _NAME1_(name) name
#   define _NAME2_(name1,name2) name1##name2
#   define _NAME3_(name1,name2,name3) name1##name2##name3

   /* stringizing */
#   define _QUOTE_(name) #name

#else

#   define _NAME1_(name) name
#   define _NAME2_(name1,name2) _NAME1_(name1)name2
#   define _NAME3_(name1,name2,name3) _NAME2_(name1,name2)name3

#   define _QUOTE_(name) "name"

#endif


/*---- misc ------------------------------------------------------------------*/

#ifdef R__GNU
#   define SafeDelete(p) { if (p) { delete p; p = 0; } }
#else
#   define SafeDelete(p) { delete p; p = 0; }
#endif

#endif

