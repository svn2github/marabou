/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#define __NO_VERSION__

#include <dda0816_P.h>



int dda0816_ioctl(struct inode* inode,struct file* file , unsigned int cmd, unsigned long arg )
{
  dda0816_no_par_t no_par_arg;
  dda0816_int_par_t int_par_arg;
  SetCurve_par_t Curvearg;

  DBGin("dda0816_ioctl");
  DBGprint(DBG_DATA,("cmd=%d",cmd));
  dda0816_errno = 0;
  
  switch(cmd){

        case SETPRESCALER:
              copy_from_user( &int_par_arg, (dda0816_int_par_t *) arg, sizeof(dda0816_int_par_t));
              int_par_arg.retval = dda0816_SetPreScaler( inode, int_par_arg.param );
              int_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_int_par_t *) arg, &int_par_arg, sizeof(dda0816_int_par_t));
              break;
      case SETPACERCLOCK:
              copy_from_user( &int_par_arg, (dda0816_int_par_t *) arg, sizeof(dda0816_int_par_t));
              int_par_arg.retval = dda0816_SetPacerClock( inode, int_par_arg.param );
              int_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_int_par_t *) arg, &int_par_arg, sizeof(dda0816_int_par_t));
              break;
      case SETOUTPUTCLOCK:
              copy_from_user( &int_par_arg, (dda0816_int_par_t *) arg, sizeof(dda0816_int_par_t));
              int_par_arg.retval = dda0816_SetOutputClock( inode, int_par_arg.param );
              int_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_int_par_t *) arg, &int_par_arg, sizeof(dda0816_int_par_t));
              break;
      case SETCURVE:
              copy_from_user( & Curvearg, (SetCurve_par_t *) arg, sizeof(SetCurve_par_t));
              Curvearg.retval = dda0816_SetCurve( inode, Curvearg.curve );
              Curvearg.error = dda0816_errno;
              copy_to_user( (SetCurve_par_t *) arg, &Curvearg, sizeof(SetCurve_par_t));
              break;
      case SETUPDATEGROUP:
              copy_from_user( &no_par_arg, (dda0816_no_par_t *) arg, sizeof(dda0816_no_par_t));
              no_par_arg.retval = dda0816_SetUpdateGroup( inode  );
              no_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_no_par_t *) arg, &no_par_arg, sizeof(dda0816_no_par_t));
              break;
      case CLEARUPDATEGROUP:
              copy_from_user( &no_par_arg, (dda0816_no_par_t *) arg, sizeof(dda0816_no_par_t));
              no_par_arg.retval = dda0816_ClearUpdateGroup( inode  );
              no_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_no_par_t *) arg, &no_par_arg, sizeof(dda0816_no_par_t));
              break;
      case SETCLOCKSOURCE:
              copy_from_user( &int_par_arg, (dda0816_int_par_t *) arg, sizeof(dda0816_int_par_t));
              int_par_arg.retval = dda0816_SetClockSource( inode, int_par_arg.param );
              int_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_int_par_t *) arg, &int_par_arg, sizeof(dda0816_int_par_t));
              break;
      case SETOUTPUTCLOCKGENERATION:
              copy_from_user( &no_par_arg, (dda0816_no_par_t *) arg, sizeof(dda0816_no_par_t));
              no_par_arg.retval = dda0816_SetOutputClockGeneration( inode  );
              no_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_no_par_t *) arg, &no_par_arg, sizeof(dda0816_no_par_t));
              break;
      case CLEAROUTPUTCLOCKGENERATION:
              copy_from_user( &no_par_arg, (dda0816_no_par_t *) arg, sizeof(dda0816_no_par_t));
              no_par_arg.retval = dda0816_ClearOutputClockGeneration( inode  );
              no_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_no_par_t *) arg, &no_par_arg, sizeof(dda0816_no_par_t));
              break;
      case STARTPACER:
              copy_from_user( &no_par_arg, (dda0816_no_par_t *) arg, sizeof(dda0816_no_par_t));
              no_par_arg.retval = dda0816_StartPacer( inode  );
              no_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_no_par_t *) arg, &no_par_arg, sizeof(dda0816_no_par_t));
              break;
      case STOPPACER:
              copy_from_user( &no_par_arg, (dda0816_no_par_t *) arg, sizeof(dda0816_no_par_t));
              no_par_arg.retval = dda0816_StopPacer( inode  );
              no_par_arg.error = dda0816_errno;
              copy_to_user( (dda0816_no_par_t *) arg, &no_par_arg, sizeof(dda0816_no_par_t));
              break;

  
  
  default:
    DBGout();	
    return -EINVAL;
  }
  
  

  DBGout();
  return 1;

}
