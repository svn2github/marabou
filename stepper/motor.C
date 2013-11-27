#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <climits>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <iostream>

#include "motor.h"

/* MaximumValues, do not change */
#define MIN_SPEED 30
#define MAX_SPEED 2000

// ----------------------------------------------------------------------

Motor::Motor(const string tty, int deviceNumber, AXES activeAxes)
{
  d_debug = false;
  d_semId = -1;  
  d_activeAxes = activeAxes;
  d_deviceNumber = deviceNumber;
  d_fd = -1;
  d_shmPtr = NULL;
  d_status = true;

  if (!init_semaphore()) {
    fprintf(stderr, "init_semaphore() failed\n");
    d_status = false;
    return;
  }
  
  d_fd = open(tty.c_str(), O_RDWR);
  if (d_fd == -1) {
    fprintf(stderr, "open_tty failed: %s\n", strerror(errno));
    d_status = false;
    return;
  }
  
  if (comm_init(d_fd) < 0) {
    fprintf(stderr, "comm_init failed: %s\n", strerror(errno));
    d_status = false;
    return;
  }
  
  if (!init_semaphore()) return;
  setMinPositions();
  setMaxPositions();
  setSpeed();
  if (!lockRS232()) {
    return;
  }
  if (!readPosition()) {
    d_status = false;
    unlockRS232();
    return;
  }
  if (!unlockRS232()) {
    d_status = false;
    return;
  }

  d_status = true;
}

Motor::~Motor()
{
  if (d_fd != -1) {
    close(d_fd);
    d_fd = -1;
  }
  
  if (d_shmPtr != NULL) {
    shmdt(d_shmPtr);
    d_shmPtr = NULL;
  }
}

bool Motor::setActiveAxes(AXES activeAxes)
{
  if (status() == false) return false;

  char command[32];
  snprintf(command, 32, "%d", d_activeAxes);
  
  if (!lockRS232()) return false;

  if (!sendCommand(command)) {
    fprintf(stderr, "setActiveAxes failed: %s\n", strError(d_error).c_str());
    unlockRS232();
    return false;
  }
  
  d_activeAxes = activeAxes;
  
  if (!readPosition()) {
    unlockRS232();
    return false;
  }
  
  if (!unlockRS232()) return false;
 
  return true;  
}

bool Motor::setSpeed(int x, int y, int z)
{
  if (status() == false) return false;
  
  if (((x > 0) && ((x < MIN_SPEED) || (x > MAX_SPEED)))) {
    fprintf(stderr, "setSpeed failed: invalid X Value: %d\n", x);
    return false;
  }
  if (((y > 0) && ((y < MIN_SPEED) || (y > MAX_SPEED)))) {
    fprintf(stderr, "setSpeed failed: invalid Y Value: %d\n", y);
    return false;
  }
  if (((z > 0) && ((z < MIN_SPEED) || (z > MAX_SPEED)))) {
    fprintf(stderr, "setSpeed failed: invalid Z Value: %d\n", z);
    return false;
  }

  if (x > 0) d_speedX = x;
  if (y > 0) d_speedY = y;
  if (z > 0) d_speedZ = z;
  
  return true;
}

bool Motor::setMinPositions(int x, int y, int z)
{
  if (status() == false) return false;
  
  d_minPosX = x;
  d_minPosY = y;
  d_minPosZ = z;
  
  return true;
}

bool Motor::setMaxPositions(int x, int y, int z)
{
  if (status() == false) return false;
  
  d_maxPosX = x;
  d_maxPosY = y;
  d_maxPosZ = z;

  return true;
}

bool Motor::referenceRun()
{
  if (status() == false) return false;
  
  char command[32];

  if (!lockRS232()) return false;

  snprintf(command, 32, "R%d", d_activeAxes);
  if (!sendCommand(command)) {
    fprintf(stderr, "referenceRun failed: %s\n", strError(d_error).c_str());
    unlockRS232();
    return false;
  }

  if (!readPosition()) {
    unlockRS232();
    return false;
  }
  
  if (!unlockRS232()) return false;

  return true;
}

bool Motor::moveAbsolute(int x, int y, int z)
{
  if (status() == false) return false;
  
  char command[128];
  
  if (!lockRS232()) return false;
  
  if (!readPosition()) {
    unlockRS232();
    return false;
  }

  if ((x < d_minPosX) || (x > d_maxPosX)) {
    fprintf(stderr, " X position %d would be out of Range\n", x);
    unlockRS232();
    return false;
  }
  if ((y < d_minPosY) || (y > d_maxPosY)) {
    fprintf(stderr, " Y position %d would be out of Range\n", y); 
    unlockRS232();
    return false;
  } 
  if ((z < d_minPosZ) || (z > d_maxPosZ)) {
    fprintf(stderr, " Z position %d would be out of Range\n", z);
    unlockRS232();
    return false;
  }
  
  switch(d_activeAxes) {
  case X:
    snprintf(command, 128, "M %d,%d", 
             x, d_speedX);
    break; 
    
  case XY:
    snprintf(command, 128, "M %d,%d,%d,%d", 
             x, d_speedX, y, d_speedY);
    break;
    
  case XZ:
    snprintf(command, 128, "M %d,%d,%d,%d,%d,%d", 
             x, d_speedX, z, d_speedZ, 0, 50);
    break;
    
  case XYZ:
    snprintf(command, 128, "M %d,%d,%d,%d,%d,%d,%d,%d", 
             x, d_speedX, y, d_speedY, z, d_speedZ, 0, 50);
    
  default:
    unlockRS232();
    return false;
  }
  
  if (!sendCommand(command)) {
    fprintf(stderr, "moveAbsolute: sendCommand failed: %s\n", 
            strError(d_error).c_str());
    unlockRS232();
    return false;
  }

  if (!readPosition()) {
    unlockRS232();
    return false;
  }

  if (!unlockRS232()) return false;

  return true;
}

bool Motor::moveRelative(int dx, int dy, int dz)
{
  if (status() == false) return false;
  
  char command[128];

  if (!lockRS232()) return false;  

  if (!readPosition()) {
    unlockRS232();
    return false;
  }
  
  if ((posX() + dx < d_minPosX) || (posX() + dx > d_maxPosX)) {
    fprintf(stderr, " X position %d would be out of Range\n", posX() + dx);
    unlockRS232();
    return false;
  }
  if ((posY() + dy < d_minPosY) || (posY() + dy > d_maxPosY)) {
    fprintf(stderr, " Y position %d would be out of Range\n", posY() + dy); 
    unlockRS232();
    return false;
  } 
  if ((posZ() + dz < d_minPosZ) || (posZ() + dz > d_maxPosZ)) {
    fprintf(stderr, " Z position %d would be out of Range\n", posZ() + dz);
    unlockRS232();
    return false;
  }
  
  switch(d_activeAxes) {
  case X:
    snprintf(command, 128, "A %d,%d", 
             dx, d_speedX);
    break; 
    
  case XY:
    snprintf(command, 128, "A %d,%d,%d,%d", 
             dx, d_speedX, dy, d_speedY);
    break;
    
  case XZ:
    snprintf(command, 128, "A %d,%d,%d,%d,%d,%d", 
             dx, d_speedX, dz, d_speedZ, 0, 50);
    break;
    
  case XYZ:
    snprintf(command, 128, "A %d,%d,%d,%d,%d,%d,%d,%d", 
             dx, d_speedX, dy, d_speedY, dz, d_speedZ, 0, 50);
    
  default:
    unlockRS232();
    return false;
  }

  if (!sendCommand(command)) {
    fprintf(stderr, "moveRelative: %s\n", strError(d_error).c_str());
    unlockRS232();
    return false;
  }

  if (!readPosition()) {
    unlockRS232();
    return false;
  }

  if (!unlockRS232()) return false;

  return true;
}

bool Motor::readPosition()
{
  if (status() == false) return false;
  
  int* pos[3] = {
    d_posX,
    d_posY,
    d_posZ
  };
  char hexNumber[7]="FFFFFF";
  unsigned int i;

  for (i = 0; i < 3; i++) {
    *pos[i] = INT_MIN;
  }

  if (!sendCommand("P")) {
    fprintf(stderr, "readPosition failed: %s\n", 
            strError(d_error).c_str());
    return false;
  }
 
  for (i = 0; i < 3; i++) {
    int32_t position;
    hexNumber[6] = '\0';
    strncpy(hexNumber, d_buffer + 1 + i * 6, 6);
    position = strtol(hexNumber, NULL, 16);  
    if (((position >> 23) & 0x01) == 1 ) {
      // Negative value, 2'complement
      position |= 0xff000000;
    }
    *pos[i] = position;
  }

  return true;
}

bool Motor::clearRAM()
{
  if (status() == false) return false;
  
  if (!lockRS232()) return false;
  
  if (!sendCommand("k")) {
    fprintf(stderr, "clearRAM failed: %s\n", 
            strError(d_error).c_str());
    unlockRS232();
    return false;
  }
  
  if (!readPosition()) {
    unlockRS232();
    return false;
  }
  
  if (!unlockRS232()) return false;
  
  return true;
}

void Motor::dump() const
{
  printf("\n");
  printf("device#     :  %d\n", d_deviceNumber);
  printf("activeAxes  :  %d = %s\n", 
         d_activeAxes, getAxesString(d_activeAxes).c_str());
  printf("error       :  %c = %s\n", d_error, strError(d_error).c_str());  
  printf("status      :  %s\n\n", d_status == true ? "true" : "false");
  printf("posX        :  %d\n", posX());
  printf("posY        :  %d\n", posY());
  printf("posZ        :  %d\n", posZ());
  printf("speedX      :  %d\n", d_speedX);
  printf("speedY      :  %d\n", d_speedY);
  printf("speedZ      :  %d\n", d_speedZ);
  printf("minPosX     :  %d\n", d_minPosX);
  printf("minPosY     :  %d\n", d_minPosY);
  printf("minPosZ     :  %d\n", d_minPosZ);
  printf("maxPosX     :  %d\n", d_maxPosX);
  printf("maxPosY     :  %d\n", d_maxPosY);
  printf("maxPosZ     :  %d\n", d_maxPosZ);
  printf("\n");
}

string Motor::getAxesString(AXES d_activeAxes)
{
  switch(d_activeAxes) {
  case X:
    return "X";
  case XY:
    return "XY";
  case XZ:
    return "XZ";
  case XYZ:
    return "XYZ";
  default:
    return "UKNOWN";
  }
}

string Motor::strError(char error)
{
  switch (error) {
  case '0':
    return string("kein Fehler");
  case '1':
    return string("unzulaessige Zahlenangabe");
  case '2':
    return string("Not-Aus oder Endschalter");
  case '3':
    return string("unzulaessige Achsenangabe");
  case '4':
    return string("keine Achsen definiert");
  case '5':
    return string("Syntax-Fehler");
  case '6':
    return string("Speicherende");
  case '7':
    return string("unzulaessige Parameterzahl");
  case '8':
    return string("unzulaessiger Speicherbefehl");
  case 'A':
    return string("Impulsfehler");
  case 'B':
    return string("unzulaessige Verzweigung");
  case 'C':
    return string("Schleifenfehler");
  case 'D':
    return string("unzulaessige Geschwindigkeit");
  case 'E':
    return string("Schleifenfehler");
  case 'F':
    return string("Benutzer Stop aktiv");
  case 'G':
    return string("keine Daten fuer Operation vorhanden");
  case 'Z':
    return string("interner SW-Fehler");
  case 'P':
    return string("interner SW-Fehler");
    
  default:
    return string("fehler nicht bekannt");
  } 
}

bool Motor::sendCommand(const char* command)
{
  if (status() == false) return false;
  
  int count = 0;
  ssize_t size;
  char commandBuffer[128];
  d_error = '0';
  
  if (command == NULL) {
    fprintf(stderr, "empty command\n");   
    return false;
  }
  snprintf(commandBuffer, 128, "@%d%s\r", d_deviceNumber, command);
  
  size = strlen(commandBuffer);

  if (d_debug) {
    fprintf(stderr, "Sending command: %s\n", commandBuffer);
  }

  if (write(d_fd, (void*)commandBuffer, size) != size) {
    fprintf(stderr, "error writing to tty: %s\n", strerror(errno));
    return -1;
  }
  
  d_bufferSize = 0;
  memset(d_buffer, '\0', 128);
  count = read(d_fd, d_buffer, 128);
  if (count == -1) {
    fprintf(stderr, "error reading tty: %s\n", strerror(errno));
    return false;
  }
  
  if ((count != 1) and (count != 19)) {
    fprintf(stderr, "tty returned invalid value %d\n", (int)count);
    return false;
  }
  d_bufferSize = count;
  d_error = d_buffer[0];
  
  return d_buffer[0] == '0' ? true : false;
}

const key_t Motor::s_sem_key = 0x4d4f544f;  //MOTO

bool Motor::init_semaphore()
{
  /* Get / Create semaphore */
  if ((d_semId = semget(s_sem_key, 1,
                        IPC_CREAT | IPC_EXCL |
                        S_IRUSR | S_IWUSR |
                        S_IRGRP | S_IWGRP |
                        S_IROTH | S_IWOTH)) == -1) {
    if (errno == EEXIST) {
      /* Semaphore already exist, so just open it */
      if ((d_semId = semget(s_sem_key, 1, 0)) == -1) {
        return false;
      }
    } else {
      /* Fatal error, see errno */
      return false;
    }
  } else {
    /* Initialize newly created  semaphore, i.e set value to 1 */
    if (semctl(d_semId, 0, SETVAL, 1) == -1) {
      return false;
    }
  }

  const key_t ipckey = 0xdeadbeef; 
  const int perm = 0666;
  const size_t shmSize = 3 * sizeof(int);
  
  int shmId = shmget(ipckey, shmSize, IPC_CREAT|perm);
  if (shmId ==-1) {
    perror("Motor::Motor Error> shmget()");
    return false;
  }
  
  d_shmPtr = shmat(shmId, NULL, 0);
  if (d_shmPtr == (void *) -1) {
    perror("Motor::Motor Error> shmat()");
    d_shmPtr = NULL;
    return false;
  }
  
  d_posX = static_cast<int*>(d_shmPtr);
  d_posY = static_cast<int*>(d_shmPtr) + 1;
  d_posZ = static_cast<int*>(d_shmPtr) + 2;
  
  return true;
}

bool Motor::lockRS232()
{
  struct sembuf sops = {
    0,           /* sem_num: We only use one track                        */
    -1,          /* sem_op: decrement semaphore by 1, i.e. lock it        */
    SEM_UNDO     /* sem_flg: remove lock if process gets killed           */
  };
  
  /* Wait for semaphore and lock it */
  if (semop(d_semId, &sops, 1) == -1) {
    perror("semaphore lock");
    return false;
  }

  return true;
}

bool Motor::unlockRS232()
{
  struct sembuf sops = {
    0,              /* sem_num: We only use one track                    */
    1,              /* sem_op: increment semaphore by 1, i.e. unlock it  */
    SEM_UNDO        /* sem_flg: remove lock if process gets killed       */
  };

  /* Release semaphore */
  if (semop(d_semId, &sops, 1) == -1) {
    perror("semaphore unlock");
    return false;
  }

  return true;
}

bool Motor::comm_init(int chan)
{
  if (status() == false) return false;

  struct termios t;

  if (tcgetattr(chan, &t) != 0) return false;
  t.c_cc[VMIN] = 32;
  t.c_cc[VTIME] = 1;
  t.c_iflag &= ~(BRKINT
                 | IGNPAR | PARMRK 
                 | INPCK 
                 | ISTRIP 
                 | INLCR | IGNCR | ICRNL
                 | IXON);
  t.c_iflag |= IGNBRK | IXOFF;
  t.c_oflag &= ~(OPOST);
  t.c_lflag &= ~(
                 ECHO|ECHOE|ECHOK|ECHONL|
                 ICANON |
                 ISIG  |
                 NOFLSH |
                 TOSTOP);
  t.c_cflag &= (
                CSIZE |
                CSTOPB |
                HUPCL |
                PARENB);
  t.c_cflag |= CLOCAL | CREAD | CS8;
  if (cfsetispeed(&t, B9600) == -1) return false;
  if (cfsetospeed(&t, B9600) == -1) return false;
  if (tcflush(chan, TCIFLUSH) == -1) return false;
  if (tcsetattr(chan,TCSANOW, &t) == -1) return false;
  
  return true;
}

bool Motor::status() const
{
  if (d_status == false) {
    cerr << "Status of Object is invalid, check semaphore and shared " 
	 << "memory, aborting operation..." 
	 << endl;
    return false;
  }
  
  return true;
}
