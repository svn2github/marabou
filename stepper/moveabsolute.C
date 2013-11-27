#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

using namespace std;

#include "motor.h"

int main(int argc, char **argv)
{
  // Create new Motor Object, 
  //  Controller is connected to RS232 Interface ttyS0
  //  Controller Id is: 0
  //  Axes to use are X and Z
  
  int posX = 0;
  int posZ = 0;
  int speedX = 1000;
  int speedZ = 1000;

//define (0,0) of the coordinate system in mm
	int originX = 2320;
	int originZ = 3000;
  
  if (!((argc == 3) or (argc == 5))) {
    cerr << "Usage: moveabsolute posX posZ [speedX speedZ] (in mm with X=" << originX << " and Z= " << originZ << " as (0,0))" << endl;
    exit(EXIT_FAILURE);
  }

  Motor motor("/dev/ttyS0", 0, Motor::XZ);
  if (!motor.status()) abort();
  
  posX = -(80 * strtol(argv[1], NULL, 0)) + originX; //calibrated position X*80, Z*100 with X=2320,Z=3000 as (0,0)
  posZ = -(100 * strtol(argv[2], NULL, 0)) + originZ;
  
  if (argc == 5) {
    speedX = strtol(argv[3], NULL, 0);
    speedZ = strtol(argv[4], NULL, 0);
  }
  
  if (!motor.setSpeed(speedX, 0, speedZ)) {
    cerr << "Set Speed failed" << endl;
    exit(EXIT_FAILURE);
  }
  
  cerr << "Move to absolute position X = " << posX 
       << " and Z = " <<  posZ 
       << endl;
  
  if (!motor.moveAbsolute(posX, 0, posZ)) {
    cerr << "Set Position Failed" << endl;
    exit(EXIT_FAILURE);
  }
  cerr << "Current Absolute Positions after absolute move  : " << endl
	<< "  X = " << motor.posX() << " or " << -(motor.posX()-originX)/80 << " mm " << endl
	<< "  Z = " << motor.posZ() << " or " << -(motor.posZ()-originZ)/100 << " mm " << endl
	<< " with X=" << originX << " and Z= " << originZ << " as (0,0))" << endl;
  cerr << endl;
  
  motor.dump();

  exit(EXIT_SUCCESS);
}
