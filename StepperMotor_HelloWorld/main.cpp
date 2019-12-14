#include "mbed.h"
#include "StepperMotor.h"

BusOut          leds( LED4, LED3, LED2, LED1 );
StepperMotor    m( p21, p22, p23, p24 );

int main() {
    m.set_sync_mode( StepperMotor::SYNCHRONOUS );
    m.set_power_ctrl( true );

    while ( 1 ) {
    
        leds    = 1;
        m.go_angle( 120 );
        wait( 0.5 );

        leds    = 2;
        m.go_angle( 240 );
        wait( 0.5 );

        leds    = 3;
        m.go_angle( 0 );
        wait( 0.5 );

        leds    = 4;
        m.go_angle( 240 );
        wait( 0.5 );

        leds    = 5;
        m.go_angle( 120 );
        wait( 0.5 );

        leds    = 6;
        m.go_angle( 0 );
        wait( 0.5 );
    }
}
