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
  
  cerr << "Current Positions is: " 
       << "  X = " << motor.posX()
       << "  Z = " << motor.posZ() 
       << endl;
  cerr << endl;
 
  cerr << "Device Status: " << endl; 
  motor.dump();

  exit(EXIT_SUCCESS);
}
