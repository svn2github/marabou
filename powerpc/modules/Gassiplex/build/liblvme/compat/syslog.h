/* $Header: /misc/hadaq/cvsroot/compat/syslog.h,v 1.6 2005/03/10 14:53:04 hadaq Exp $ */
#ifndef OUR_SYSLOG_H
#define OUR_SYSLOG_H

#ifdef HAVE_LIBCOMPAT

#include </usr/include/syslog.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(LOG_PERROR)
#define LOG_PERROR  0x20    /* log to stderr as well */
#endif

void SYSLOG_openlog(const char *ident, int logopt, int facility);
void SYSLOG_syslog(int priority, const char *message, ...);
void SYSLOG_closelog();
int SYSLOG_setlogmask(int maskpri);

#define syslog     SYSLOG_syslog
#define openlog    SYSLOG_openlog
#define closelog   SYSLOG_closelog
#define setlogmask SYSLOG_setlogmask

#ifdef __cplusplus
}
#endif

#else
#include </usr/include/syslog.h>
#endif

#endif
