/** Stepper Motor control library
 *  
 *  @class   StepperMotor
 *  @author  Tedd OKANO
 *  @version 0.51(27-Nov-2010)
 *  
 *  Copyright: 2010 Tedd OKANO, Tsukimidai Communications Syndicate - Crawl Design
 *  The library that controls stepper motor via motor driver chip: TA7774
 *  The TA7774 is a driver for a bipolar stepper motor. 
 *  With this library, mbed will generate 2 phase pulses to operate the motor. 
 */
 
#ifndef    MBED_STEPPERMOTOR
#define    MBED_STEPPERMOTOR

#include "mbed.h"

#define MAX_PPS   100   //  pulse per second

/** Stepper Motor control class
 *
 *  Example:
 *  @code
 *  #include "mbed.h"
 *  #include "StepperMotor.h"
 *
 *  StepperMotor m( p21, p22, p23, p24 );
 *
 *  int main() {
 *      m.set_steps_per_rotate( 480 );
 *      m.set_sync_mode( StepperMotor::SYNCHRONOUS );
 *      m.set_power_ctrl( true );
 *
 *      while( 1 ) {
 *          m.go_angle( 120 );
 *          wait( 0.5 );
 *     
 *          m.go_angle( 240 );
 *          wait( 0.5 );
 *   
 *          m.go_angle( 0 );
 *          wait( 0.5 ); 
 *
 *          m.go_angle( 240 );
 *          wait( 0.5 );
 *     
 *          m.go_angle( 120 );
 *          wait( 0.5 );
 *   
 *          m.go_angle( 0 );
 *          wait( 0.5 ); 
 *      }
 *  }
 *  @endcode
 */

class StepperMotor {
public:

    /** Constants for motor rotate mode */     
    typedef enum  {
        SHORTEST,                   /**< turn by shortest direction      */
        NO_WRAPAROUND,              /**< do not accross home position    */
        CLOCKWISE_ONLY,             /**< one-way: clockwise turn         */
        COUNTER_CLOCKWISE_ONLY      /**< one-way: counter clockwise turn */
    } RotMode;

    /** Constants for syncronization mode */     
    typedef enum  {
        ASYNCHRONOUS,               /**< program does wait motor turn completion     */
        SYNCHRONOUS                 /**< program doesn't wait motor turn completion  */
    } SyncMode;

    /** Constants for position detection edge polarity */     
    typedef enum  {
        RISING_EDGE,                /**< position detection done by rising  edge */
        FALLING_EDGE                /**< position detection done by falling edge */
    } PositionDetectPorarity;

    /** Create a stepper motor object connected to specified DigitalOut pins and a DigitalIn pin
     *
     *  @param out_A DigitalOut pin for motor pulse signal-A
     *  @param out_B DigitalOut pin for motor pulse signal-B
     *  @param out_PWR DigitalOut pin for TA7774's power control (option)
     *  @param position_detect DigitalIn pin for home position detection (option)
     */
    StepperMotor(
        PinName out_A   = p21,
        PinName out_B   = p22,
        PinName out_PWR = p23,
        PinName position_detect = p24
    ) ;

    /** Set the pulse width (i.e. motor turning speed)
     *
     *  @param v pulse per second : lower number makes the turn slower (default = 100)
     */
    float set_pps( float v );

    /** Set maximum PPS (= minimum pulse width) which will be used in finding home position
     *
     *  @param v maximum pulse per second : lower number makes the turn slower (default = 100)
     */
    void set_max_pps( float v );

    /** Find home position: rotate the motor until the detection edge comes.
     *
     *  Turns the motor until the home position detected. 
     *  The "home position" is a reference point for the step and angle. It will be step=0 and angle=0. 
     *  The detection signal edge can be defined by an argument. 
     *  It follows the rotate mode. 
     *  When the edge is detected, the motor will be stopped and it will be the new home position. 
     *  If no detection signal detected, no home position update done. 
     * 
     *  @param edge defines detection edge rise or fall
     */
    int find_home_position( PositionDetectPorarity edge );
    
    /** Update home position
     *
     *  Set the home position as current motor position.
     */     
    void set_home_position( void );
    
    /** Turn the motor to defined position (by steps from home position)
     *
     *  Make motor move to absolute position
     *
     *  @param v the position defined by steps from home position
     */     
    int go_position( int v );

    /** Turn the motor to defined position (by angle (degree)) from home position)
     *
     *  Make motor move to absolute position
     *
     *  @param v the position defined by steps from home position
     */     
    void go_angle( float angle );

    /** Turn the motor to defined position (by steps from current position)
     *
     *  Make motor move to defined position
     *
     *  @param v the position defined by steps from home position
     */     
    int move_steps( int s );
    
    /** Interface for motor rotate mode setting
     *
     *  Example:
     *  @code
     *  StepperMotor    m( p21, p22, p23, p24 );
     *  int main() {
     *      m.set_rot_mode( StepperMotor::NO_WRAPAROUND );
     *      ...
     *  @endcode
     *
     *  @param m motor rotate mode : SHORTEST (default), NO_WRAPAROUND, CLOCKWISE_ONLY or COUNTER_CLOCKWISE_ONLY
     */
    void set_rot_mode( RotMode m );

    /** Interface for syncronization mode setting
     *
     *  Example:
     *  @code
     *  StepperMotor    m( p21, p22, p23, p24 );
     *  int main() {
     *      m.set_sync_mode( StepperMotor::NO_WRAPAROUND );
     *      ...
     *  @endcode
     *
     *  @param m motor rotate mode : ASYNCHRONOUS (default) or SYNCHRONOUS
     */
    void set_sync_mode( SyncMode m );

    /** Check remaining distance that motor need to move
     *
     *  software can check if the motor action completed in asynchronous mode
     *  
     *  @return remaining steps that motor need to go
     */
    int distance( void );

    /** Pause/Resume the motor action
     *
     *  @param sw use "true" for pause, "false" (default) for resume
     */
    void set_pause( int sw );

    /** Auto power control enable
     *
     *  If the auto power control is enabled, the motor power will be turned-off when it stays same place
     *  
     *  @param sw use "true" for pause, "false" (default) for resume
     */
    void set_power_ctrl( int sw );

    /** Setting for steps/rotate
     *
     *  This parameter is required if program want to use the "go_angle()" interface. 
     *  The angle will be calculated from this parameter.
     *  
     *  @param steps per rotate
     */
    void set_steps_per_rotate( int steps );
    
private:

    Ticker      t;
    BusOut      motor_out;
    DigitalOut  pwr_out;
    DigitalIn   position_detect_pin;

    static const unsigned char  pat[ 4 ];  //  2 phase pulse pattern for motor control
    RotMode     rot_mode;
    SyncMode    sync_mode;
    int         max_pos;
    int         current_pos;
    int         pos_offset;
    int         target_pos;
    float       pps;
    float       max_pps;
    int         init_done;
    int         pause;
    int         power_ctrl;

    void set_target_pos( int p );  //  target position setting interface
    void motor_maintain( void );   //  this function is called periodically by Ticker
};


#endif