#include <syslog.h>

/* 
   "name" has to be the name of the module, which is the standard for blubb_lib's
*/

#define print_info(my_text) syslog(LOG_INFO, my_text)
#define print_info1(my_text,my_1) syslog(LOG_INFO, my_text,my_1)
#define print_info2(my_text,my_1,my_2) syslog(LOG_INFO, my_text,my_1,my_2)
#define print_info3(my_text,my_1,my_2,my_3) syslog(LOG_INFO, my_text,my_1,my_2,my_3)

#define print_debug(my_text) syslog(LOG_DEBUG, my_text)
#define print_debug1(my_text,my_1) syslog(LOG_DEBUG, my_text,my_1)
#define print_debug2(my_text,my_1,my_2) syslog(LOG_DEBUG, my_text,my_1,my_2)
#define print_debug3(my_text,my_1,my_2,my_3) syslog(LOG_DEBUG, my_text,my_1,my_2,my_3)
#define print_debug4(my_text,my_1,my_2,my_3,my_4) syslog(LOG_DEBUG, my_text,my_1,my_2,my_3,my_4)

#define print_end(my_text) syslog(LOG_DEBUG, "END[%s]: %s",name,my_text)


/* syntax for info and debug: like printf: (but not \n)
   eg.:
   print_debug2("Variable is %u at %x", variable,address);
*/

#define print_warning(my_text) syslog(LOG_WARNING, "WARNING[%s]: %s",name,my_text)
#define print_warning1(my_text,my_1) syslog(LOG_WARNING, "WARNING[%s]: %s %s",name,my_text,my_1)

#define print_return(my_text) {syslog(LOG_ERR, "FATAL[%s]: %s",name,my_text);return -1;}
#define print_return1(my_text,my_1) {syslog(LOG_ERR, "FATAL[%s]: %s %s",name,my_text,my_1);return -1;}

#define print_fatal(my_text) {syslog(LOG_ERR, "FATAL[%s]: %s",name,my_text);}
#define print_fatal1(my_text,my_1) {syslog(LOG_ERR, "FATAL[%s]: %s %s",name,my_text,my_1);}

#define print_success(my_text)  syslog(LOG_NOTICE, "SUCCESS[%s]: %s",name,my_text)

/* syntax for warning and fatals: only strings: (and no %s)
   print_fatal("Problem!");
   print_fatal1("File not found: ",filename);
   
   print_fatal should be used for missing parameters and the bus error!!!!!

   if you need other formats, sprintf it!
   warnings are in bold font, fatals are in red and leave the program.


   print_success is in green:
   print_success("Reset done.");

   please use print_success only for "nodes" of the system.

*/



char *print_open(char *name);
