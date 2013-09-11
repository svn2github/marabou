#include <syslog.h>

int main(int argc, char *argv[]) {
	openlog(argv[0], LOG_PERROR, LOG_LOCAL0);
	setlogmask(LOG_UPTO(LOG_INFO));
	syslog(LOG_INFO,  "Hello, World!");
	syslog(LOG_INFO,  "%d, %d, %d, %s", 1, 2, 3, "viele");
	syslog(LOG_DEBUG, "This is a debug message");
	syslog(LOG_INFO, "This is a info message");
	syslog(LOG_NOTICE, "This is a notice message");
	syslog(LOG_WARNING, "This is a warning message");
	syslog(LOG_ERR, "This is a err message");
	syslog(LOG_CRIT, "This is a crit message");
	syslog(LOG_ALERT, "This is a alert message");
	syslog(LOG_EMERG, "This is a emerg message");
	closelog();

	return 0;
}
