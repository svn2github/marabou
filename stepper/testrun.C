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
  Motor motor("/dev/ttyS0", 0, Motor::XZ);
  if (!motor.status()) abort();

  // In case you want some debug infos about commands sent to the controller:
  //motor.setDebug(true);
  
  // After a reset of controller you have to call this once:
  //motor.setActiveAxes(Motor::XZ);
  //motor.referenceRun();

  // Set speed X = 50 and Z = 150, Y = keep current setting
  //motor.setSpeed(1000, 0, 1000);
  
  // Give some information about current status
  motor.dump();

  exit(1);
  
  cerr << "Move to absolute position X=300 and Z=50" << endl;
  motor.moveAbsolute(300, 0, 200);
  cerr << "Current Positions after absolute move  : " 
       << "  X = " << motor.posX()
       << "  Z = " << motor.posZ() 
       << endl;
  cerr << endl;

  sleep(2);

  cerr << "Do a relative move from current position to X + 100 and Z - 150" 
       << endl;
  motor.moveRelative(100, 0, -150);
  cerr << "Current Positions after rel. Move      : "
       << "  X = " << motor.posX() //tells actual X position from controller
       << "  Z = " << motor.posZ() //tells actual y position from controller
       << endl;
  cerr << endl;
  
  
  exit(EXIT_SUCCESS);
}
