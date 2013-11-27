#ifndef MOTOR_H
#define MOTOR_H
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <climits>

using namespace std;

class Motor
{
public:
  typedef enum {
    X    = 1,
    XY   = 3,
    XZ   = 5,
    XYZ  = 7
  } AXES;
  
  static const key_t s_sem_key;
  
public:
  Motor(const string tty = "/dev/ttyS0", 
        int deviceNumber = 0, 
        AXES activeAxes = XZ
        );
        
  ~Motor();
  bool status() const;
  
  AXES activeAxes() const {return d_activeAxes;}
  int posX() const {if (status()) return *d_posX; else return INT_MIN;}
  int posY() const {if (status()) return *d_posY; else return INT_MIN;}
  int posZ() const {if (status()) return *d_posZ; else return INT_MIN;}
  int speedX() const {if (status()) return d_speedX; else return INT_MIN;}
  int speedY() const {if (status()) return d_speedY; else return INT_MIN;}
  int speedZ() const {if (status()) return d_speedZ; else return INT_MIN;}
  int minPosX() const {if (status()) return d_minPosX; else return INT_MIN;}
  int minPosY() const {if (status()) return d_minPosY; else return INT_MIN;}
  int minPosZ() const {if (status()) return d_minPosZ; else return INT_MIN;}
  int maxPosX() const {if (status()) return d_maxPosX; else return INT_MIN;}
  int maxPosY() const {if (status()) return d_maxPosY; else return INT_MIN;}
  int maxPosZ() const {if (status()) return d_maxPosZ; else return INT_MIN;}

  bool setSpeed(int x=100, int y=100, int z=100);
  
  bool moveAbsolute(int x, int y, int z);
  bool moveRelative(int dx, int dy, int dz);
  bool setMinPositions(int x=0, int y=0, int z=0);
  bool setMaxPositions(int x=6600, int y=0, int z=8000);
  bool setActiveAxes(AXES activeAxes);
  bool referenceRun();
  bool clearRAM();  
  void dump() const;
  
  int semId() const {return d_semId;}
  void setDebug(bool on) {d_debug = on;}

  static string strError(char error);

private:
  bool d_status;
  int d_deviceNumber;
  AXES d_activeAxes;
  int* d_posX;
  int* d_posY;
  int* d_posZ;
  int d_speedX;
  int d_speedY;
  int d_speedZ;
  int d_minPosX;
  int d_minPosY;
  int d_minPosZ;
  int d_maxPosX;
  int d_maxPosY;
  int d_maxPosZ;

  int d_fd;            
  char d_error;
  ssize_t d_bufferSize;
  char d_buffer[128];
  
  int d_semId;
  void* d_shmPtr;
  bool d_debug;

private:
  bool readPosition();
  bool sendCommand(const char *command);
  const char* Motor_strError(char error);    
  static string getAxesString(AXES d_activeAxes);
  bool comm_init(int chan);
  bool init_semaphore();
  bool lockRS232();
  bool unlockRS232();
};

#endif

