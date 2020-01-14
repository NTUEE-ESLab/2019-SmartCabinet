#ifndef a4988stepper_h
#define a4988stepper_h

#include "mbed.h"
#include "Stepper.h"
#include <cstdlib>
#include <string>

class A4988_Driver{
public:
	A4988_Driver( PinName _pinstep, PinName _pinDir, double _delay = 0.02 )
	: Step( _pinstep ), Dir( _pinDir ), delay( _delay ), speed( 60/(delay*2*42) )
	{};
	~A4988_Driver(){};


	//////////////////need testing
	const int 	 up = 1;
	const int  	 down = 0;
	const double layer_round = 2;
	//////////////////need testing

	void rotate( double round, const int _Dir ){

		printf("calling rotate %.1f\n", round);
		if( _Dir > 1 || _Dir < 0 ){
			printf("Error : 4988 Dir un known %d\n", _Dir );
		}
		int stepping = round * 42;
		printf("stepping %d\n", stepping);
		
		for( int i=0 ; i<stepping; ++i ){
			Dir = _Dir;
			Step = 1;
			wait( delay );
			Step = 0;
			wait( delay );
		}
		printf("rotate %f\n", round);
	};

	void up_down( int _layer, const int _Dir ){
		if( _Dir > 1 || _Dir < 0 ){
			printf("Error : 4988 Dir un known %d\n", _Dir );
		}
		printf("up down %d layer\n", _layer );
		rotate( _layer*layer_round, _Dir );
	};

private:
	DigitalOut Step;
	DigitalOut Dir;
	double 	   delay = 0.05;// 60/delay*42 rpm is speed
	double	   speed = 60/(delay*42*2);

};

class ULN2003_Driver{
public:
	ULN2003_Driver( int step_one_round, PinName In1, PinName In2, PinName In3, PinName In4 )
	: In4(In4),In3(In3),In2(In2),In1(In1),step_one_round(step_one_round){
			myStepper = new Stepper( step_one_round,In4, In3, In2, In1 );
    	myStepper->setSpeed( 60 );
	};
	~ULN2003_Driver(){};

	void rotate_degree( double degree ){
		int steps = degree/360*step_one_round;
		for( int i=0; i< steps;++i ) {
        	myStepper->step( 1 );
        	wait( 0.008 );
    	}
	}
private:
	
   
    Stepper *myStepper;
    int step_one_round;
    PinName In4;
    PinName In3;
    PinName In2;
    PinName In1;
};

#endif