/** Stepper Motor control library
 *  
 *  @class   StepperMotor
 *  @author  Tedd OKANO
 *  @version 0.6 (13-Sep-2017)
 *  
 *  Copyright: 2010 Tedd OKANO, Tsukimidai Communications Syndicate - Crawl Design
 *  The library that controls stepper motor via motor driver chip: TA7774
 *  The TA7774 is a driver for a bipolar stepper motor. 
 *  With this library, mbed will generate 2 phase pulses to operate the motor. 
 *  
 *  version 0.51  (27-Nov-2010)
 *  version 0.6   (13-Sep-2017) //  fixed to keep last position while power-control disabled
 */
 
#include "mbed.h"
#include "StepperMotor.h"

StepperMotor::StepperMotor(
    PinName out_A,
    PinName out_B,
    PinName out_PWR,
    PinName position_detect
) :
        motor_out( out_A, out_B ),
        pwr_out( out_PWR ),
        position_detect_pin( position_detect ),
        rot_mode( SHORTEST ),
        sync_mode( ASYNCHRONOUS ),
        max_pos( 480 ),
        current_pos( 0 ),
        pos_offset( 0 ),
        target_pos( 0 ),
        max_pps( MAX_PPS ),
        init_done( false ),
        pause( false ),
        power_ctrl( false ) {

    pps     = max_pps;
    t.attach( this, &StepperMotor::motor_maintain, 1.0 / (float)pps );
}

float StepperMotor::set_pps( float v ) {
    float   p;

    p   = pps;
    pps = v;
    t.detach();
    t.attach( this, &StepperMotor::motor_maintain, 1.0 / (float)pps );
    return ( p );
}

void StepperMotor::set_max_pps( float v ) {
    max_pps     = v;
}

int StepperMotor::find_home_position( PositionDetectPorarity edge ) {
    RotMode     prev_rot;
    SyncMode    prev_sync;
    float       prev_pps;
    int         prev_det_pin;
    int         direction;

    prev_pps    = set_pps( max_pps );
    prev_rot    = rot_mode;
    prev_sync   = sync_mode;
    set_sync_mode( SYNCHRONOUS );
    set_rot_mode( SHORTEST );

    prev_det_pin    = position_detect_pin;

    if ( prev_rot == COUNTER_CLOCKWISE_ONLY )
        direction   = -1;
    else
        direction   = 1;

    if ( prev_rot == NO_WRAPAROUND ) {

        for ( int i = 0; i < (max_pos >> 1); i++ ) {
            move_steps( -1 );
            if ( ((edge == RISING_EDGE) && !prev_det_pin && position_detect_pin)
                    || ((edge == FALLING_EDGE) && prev_det_pin && !position_detect_pin) ) {
                set_home_position();
                init_done   = true;
                break;
            }
            prev_det_pin    = position_detect_pin;
        }
    }

    for ( int i = 0; i < ((prev_rot == NO_WRAPAROUND) ? (max_pos - 1) : (max_pos + 10)); i++ ) {
        move_steps( direction );
        if ( ((edge == RISING_EDGE) && !prev_det_pin && position_detect_pin)
                || ((edge == FALLING_EDGE) && prev_det_pin && !position_detect_pin) ) {
            set_home_position();
            init_done   = true;
            break;
        }
        prev_det_pin    = position_detect_pin;
    }

    go_position( 0 );
    set_pps( prev_pps );
    set_rot_mode( prev_rot );
    set_sync_mode( prev_sync );

    return ( init_done );
}

void StepperMotor::set_home_position( void ) {
    set_pause( true );
    pos_offset  = current_pos & 0x3;
    current_pos = 0;
    set_target_pos( 0 );
    set_pause( false );
}

int StepperMotor::go_position( int v ) {
    set_target_pos( v );
    return ( current_pos );
}

void StepperMotor::go_angle( float angle ) {
    go_position( (int)((angle / 360.0) * (float)max_pos) );
}

int StepperMotor::move_steps( int s ) {
    set_target_pos( current_pos + s );
    return ( current_pos );
}

void StepperMotor::set_rot_mode( RotMode m ) {
    rot_mode    = m;
}

void StepperMotor::set_sync_mode( SyncMode m ) {
    sync_mode    = m;
}

int StepperMotor::distance( void ) {
    return( target_pos - current_pos );
}

void StepperMotor::set_pause( int sw ) {
    pause   = sw;
}

void StepperMotor::set_target_pos( int p ) {
    p           = p % max_pos;
    target_pos  = (p < 0) ? (p + max_pos) : p;
     
    if (sync_mode == SYNCHRONOUS)
        while ( distance() )
            wait( 0 );
}

void StepperMotor::set_power_ctrl( int sw ) {
    power_ctrl  = sw;
}

void StepperMotor::set_steps_per_rotate( int steps ) {
    max_pos = steps;
}

void StepperMotor::motor_maintain( void ) {

    int     diff;
    int     direction;

    if ( pause )
        return;

    diff    = target_pos - current_pos;

    if ( !diff ) {
        if ( power_ctrl )
            pwr_out = 0;
            
        return;
    }

    pwr_out = 1;

    diff = (diff + max_pos) % max_pos;

    if ( diff > (max_pos >> 1) )
        diff -= max_pos;

    switch ( rot_mode ) {
        case NO_WRAPAROUND :
            direction   = ( (target_pos - current_pos) < 0 ) ? -1 : 1;
            break;
        case CLOCKWISE_ONLY :
            direction   = 1;
            break;
        case COUNTER_CLOCKWISE_ONLY :
            direction   = -1;
            break;
        default : // SHORTEST :
            direction   = ( diff < 0 ) ? -1 : 1;
            break;
    }


    current_pos = ((current_pos + max_pos) + direction) % max_pos;

//    printf( "pos=%d  ofst=%d  puls=%d\r\n", current_pos, pos_offset, (current_pos + pos_offset) );
    motor_out   = pat[ (current_pos + pos_offset) & 0x3 ];
};

const unsigned char StepperMotor::pat[ 4 ]  = { 0, 1, 3, 2 };
