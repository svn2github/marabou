AUTOMAKE_OPTIONS = foreign

include_HEADERS = @libgen_header@ @stdint_header@ @syslog_header@ @semaphore_header@ @pthread_header@
EXTRA_HEADERS = libgen.h stdint.h syslog.h semaphore.h pthread.h

lib_LIBRARIES = libcompat.a
libcompat_a_SOURCES =
EXTRA_libcompat_a_SOURCES = syslog.c semaphore.c pthread.c
libcompat_a_LIBADD = @syslog_object@ @semaphore_object@ @pthread_object@
libcompat_a_DEPENDENCIES = @syslog_object@ @semaphore_object@ @pthread_object@
