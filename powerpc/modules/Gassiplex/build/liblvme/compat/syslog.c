static char *rcsId = "$Header: /misc/hadaq/cvsroot/compat/syslog.c,v 1.6 2003/11/24 12:47:52 hadaq Exp $";

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include </usr/include/syslog.h>

#define OUR_IDENT_LEN 256

static unsigned ourMaskpri = 0xffffffff;
static char ourIdent[OUR_IDENT_LEN] = "";
static unsigned ourLogopt = 0;
static unsigned ourFacility = LOG_USER;

void SYSLOG_syslog(int priority, const char *message, ...) {
	if (ourMaskpri & (1 << priority)) {
		va_list ap;
		static const char *pri = "PACEWNID";
		char idPri[OUR_IDENT_LEN + 3];

		sprintf(idPri, "%s<%c>", ourIdent, pri[priority]);
		if (ourLogopt & LOG_PERROR) {
			static const char *mode[] = {
				"\033[0;1m\033[43;31m",
				"\033[0;1m\033[43;39m",
				"\033[43;39m",
				"\033[0;1m\033[49;31m",
				"\033[0;1m",
				"\033[0;1m\033[49;32m",
				"",
				"",
				"\033[49;39m\033[0m"
			};
			fprintf(stderr, "%s", mode[priority]);
			fprintf(stderr, "%s: ", idPri);
			fprintf(stderr, "%s", mode[LOG_DEBUG + 1]);
			va_start(ap, message);
			vfprintf(stderr, message, ap);
			va_end(ap);
			fprintf(stderr, "%c", '\n');
		}

		openlog(idPri, ourLogopt & ~LOG_PERROR, ourFacility);
		va_start(ap, message);
		vsyslog(priority, message, ap);
		va_end(ap);
		closelog();
	}
}

void SYSLOG_openlog(const char *ident, int logopt, int facility) {
	strncpy(ourIdent, ident, OUR_IDENT_LEN-1);
	ourLogopt = logopt;
	ourFacility = facility;
}

void SYSLOG_closelog(void) {
}

int  SYSLOG_setlogmask(int maskpri) {
	ourMaskpri = maskpri;
	return setlogmask(maskpri);
}
