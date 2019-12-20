#include "mbed.h"
#include "AsyncStepper/AsyncStepper.hpp"

// main() runs in its own thread in the OS
/* 

int main()
{
    AsyncStepper mystepper(D0, D2, D3, 20, FULL, NC, NC, NC, 2048, stopMode_e::FREE);

    for( int i=0;i<2048;++i ){
        mystepper.Rotate( direction_e::NEGATIVE, 1);
        printf( "steps\n" );
    }
    printf( "loop\n" );

}
*/

// simple A4988 sample
// jumper reset and sleep together
// connect  VDD to mbed VOUT
// connect  GND to mbed GND
// connect  1A and 1B to stepper coil 1 (GREEN and BLACK wire)
// connect 2A and 2B to stepper coil 2 (BLUE and RED wire)
// connect VMOT to power source (to AC adapter)
// connect GRD to power source (to AC adapter)
// connect STEP to mbed p22
// connect DIR to mbed p21
 
 
DigitalOut A4988STEP(D2);
DigitalOut A4988DIR(D3);
int a=0;
 
int main() {
    A4988DIR = 0;
    A4988STEP = 0;
 
    while(1) {
      if (a <  2048)  //sweep 200 step in dir 1
       {
        a++;
        A4988STEP = 1;
        wait(0.0008);
        A4988STEP = 0;
        wait(0.0008);
       }
      else 
       {
        A4988DIR = 1;
        a++;
        A4988STEP = 1;
        wait(0.0008);
        A4988STEP = 0;
        wait(0.0008);
        
        if (a>4096)    //sweep 200 in dir 2
         {
          a = 0;
          A4988DIR = 0;
         }
        }
    }
}