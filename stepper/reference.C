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
      
  // After a reset of controller you have to call this once:
  motor.setActiveAxes(Motor::XZ);
  motor.referenceRun();
  motor.setSpeed(1000, 0, 1000);
  motor.dump();
  
  exit(EXIT_SUCCESS);
}
