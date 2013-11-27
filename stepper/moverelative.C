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
  
  if (!((argc == 3) or (argc == 5))) {
    cerr << "Usage: moverelative posX posZ [speedX speedZ]" << endl;
    exit(EXIT_FAILURE);
  }
  
  Motor motor("/dev/ttyS0", 0, Motor::XZ);
  if (!motor.status()) abort();  

  posX = strtol(argv[1], NULL, 0);
  posZ = strtol(argv[2], NULL, 0);
  
  if (argc == 5) {
    speedX = strtol(argv[3], NULL, 0);
    speedZ = strtol(argv[4], NULL, 0);
  }
  motor.setSpeed(speedX, 0, speedZ);
  cerr << "Move relative by DeltaX = " << posX 
       << " and DeltaZ = " <<  posZ 
       << endl;
  motor.moveRelative(posX, 0, posZ);
  cerr << "Current Absolute Positions after relative move  : " 
       << "  X = " << motor.posX()
       << "  Z = " << motor.posZ() 
       << endl;
  cerr << endl;
  
  motor.dump();
	
/*//playground
	sleep(5);
  motor.moveRelative(500, 0, 500);
  cerr << "Current Positions after rel. Move      : "
       << "  X = " << motor.posX() //tells actual X position from controller
       << "  Z = " << motor.posZ() //tells actual y position from controller
       << endl;
  cerr << endl;

  motor.dump();
*/// playground


  exit(EXIT_SUCCESS);
}
