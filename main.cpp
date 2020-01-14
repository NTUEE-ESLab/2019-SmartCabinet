#include "mbed.h"
#include "a4988stepper.h"
#include "RFID.h"
#include <cstdlib>
#include <string>
#include "Wifi.h"
#include "blehandle.h"
#include "i2chandler.h"

A4988_Driver  UpMotor( D9, D8 );
ULN2003_Driver L1Motor( 2048, D3, D2, D1, D0 );
ULN2003_Driver L2Motor( 2048, D7, D6, D5, D4 );

void PlaceItem( int target ){
	if ( target < 3 ){
    		switch( target%3 ){
    			case 0:
    				UpMotor.up_down( 2, UpMotor.down );
    				wait( 0.5 );
    				L1Motor.rotate_degree( -90 );
    				wait( 0.5 );
    				UpMotor.up_down( 2, UpMotor.up );
    				break;
    			case 1:
    				UpMotor.up_down( 2, UpMotor.down );
    				wait( 0.5 );
    				L1Motor.rotate_degree( 180 );
    				wait( 0.5 );
    				UpMotor.up_down( 2, UpMotor.up );
    				break;
    			case 2:
    				UpMotor.up_down( 2, UpMotor.down );
    				wait( 0.5 );
    				L1Motor.rotate_degree( 90 );
    				wait( 0.5 );
    				UpMotor.up_down( 2, UpMotor.up );
    				break;
    		}
    	}
    	else{
    		switch( target%3 ){
    			case 0:
    				UpMotor.up_down( 1, UpMotor.down );
    				wait( 0.5 );
    				L2Motor.rotate_degree( -90 );
    				wait( 0.5 );
    				UpMotor.up_down( 1, UpMotor.up );
    				break;
    			case 1:
    				UpMotor.up_down( 1, UpMotor.down );
    				wait( 0.5 );
    				L2Motor.rotate_degree( 180 );
    				wait( 0.5 );
    				UpMotor.up_down( 1, UpMotor.up );
    				break;
    			case 2:
    				UpMotor.up_down( 1, UpMotor.down );
    				wait( 0.5 );
    				L2Motor.rotate_degree( +90 );
    				wait( 0.5 );
    				UpMotor.up_down( 1, UpMotor.up );
    				break;
    		}
    	}
}

void FechItem( int target ){
	if ( target < 3 ){
    		switch( target%3 ){
    			case 0:
    				L1Motor.rotate_degree( 90 );
    				wait( 0.5 );
    				UpMotor.up_down( 2, UpMotor.up );
    				break;
    			case 1:
    				L1Motor.rotate_degree( 180 );
    				wait( 0.5 );
    				UpMotor.up_down( 2, UpMotor.up );
    				break;
    			case 2:
    				L1Motor.rotate_degree( -90 );
    				wait( 0.5 );
    				UpMotor.up_down( 2, UpMotor.up );
    				break;
    		}
    	}
    	else{
    		switch( target%3 ){
    			case 0:
    				L2Motor.rotate_degree( 90 );
    				wait( 0.5 );
    				UpMotor.up_down( 1, UpMotor.up );
    				break;
    			case 1:
    				L2Motor.rotate_degree( 180 );
    				wait( 0.5 );
    				UpMotor.up_down( 1, UpMotor.up );
    				break;
    			case 2:
    				L2Motor.rotate_degree( -90 );
    				wait( 0.5 );
    				UpMotor.up_down( 1, UpMotor.up );
    				break;
    		}
    	}
}

Thread* BLE_detect = new Thread; 

void BLE_detect_p( ble_handler* _ble ){
	_ble->server_on();
}

Thread* RFID_detect = new Thread;

myRFID rfid_handle;
void RFID_detect_p( char* UID  ){
	rfid_handle.Read_Card( UID );
}



int main()
{
    while(true){
        L1Motor.rotate_degree(360);
    }

	/* 

	char MAC[19] = {0};
    ble_handler _ble( MAC );

    char UID[19] = {0};

    char item[10] = {0};
    i2c_handler _i2c;

    wifihandler _wifi;

    string getting_type;

    int target=0;
    char PASS[19] = {0};
    DigitalIn Item_placed( D10 );

    RFID_detect->start( callback( &RFID_detect_p, UID ) );
	BLE_detect->start(  callback( &BLE_detect_p, &_ble )  );
	while( true ){
        L1Motor.rotate_degree( 90 );
    	if( MAC[0]!=char(0) || UID[6]!=char(0) ){
    		if( MAC[0]!= char(0) ){
    			getting_type = "mac";
    			for( int i=0; i<19; ++i ) PASS[i] = MAC[i];
    			printf("pass by MAC, %s\n", MAC );
    			for( int i=0; i<19; ++i ) MAC[i] = char(0);
    			for( int i=0; i<19; ++i ) UID[i] = char(0);
    		}
    		else if( UID[0]!=char(0) ){
    			getting_type = "rfid";
    			for( int i=0; i<19; ++i ) PASS[i] = UID[i];
    			printf("pass by RFID, %s\n", UID );
    			for( int i=0; i<19; ++i ) MAC[i] = char(0);
    			for( int i=0; i<19; ++i ) UID[i] = char(0);
    		}
    		BLE_detect->terminate();
    		RFID_detect->terminate();

    		std::string stemp = "user_item?";
    		stemp += getting_type + "=" + PASS;
    		target = _wifi.Get_request( stemp.c_str() );
	    	
    		FechItem( target );
            printf( "Waiting for item placed...\n" );
    		while(true){ 
    			if(Item_placed.read()) 
    				break;
    			else
    				wait_ms( 5 );
    		}

	    	_i2c.capture( item );
	    	std::string s_stemp = "update_item?";
	    	s_stemp += getting_type + "=" + PASS + "&item=" + item;
	    	target = _wifi.Get_request( s_stemp.c_str() );

	    	PlaceItem( target );
	    	target = 0;
	    	for( int i=0; i<10; ++i ) item[i] = char(0);
	    	for( int i=0; i<19; ++i ) PASS[i] = char(0);

    	    delete BLE_detect;
    	    delete RFID_detect;
    	    Thread* BLE_detect = new Thread; 
		    Thread* RFID_detect = new Thread;
            RFID_detect->start( callback( &RFID_detect_p, UID ) );
	        BLE_detect->start(  callback( &BLE_detect_p, &_ble )  );
    	}
        else{
            //printf( "%s",MAC );
            //printf( "%s", UID );
            continue;
        }

	}
    */



    //ULN2003_Driver upMotor( 2048, D3, D2, D1, D0 );
    //upMotor.rotate_degree(360);
    
    ///rfid_handle
    //print_memory_info();
    /*
    wait_ms(5);
    myRFID rfid_handle;
    char uid[10];
    rfid_handle.Read_Card( uid );
    print_memory_info();
    printf( "from class :\n");
    printf( "%s\n" ,uid );
	*/

 	/* 
	std::string stemp("user_item?name=justin");
	wifihandler _wifi;
    int result = _wifi.Get_request( stemp.c_str() );
    printf( "from class : %d\n", result );
	*/
	/*
    char peerAddr[19] = {0};
    ble_handler _ble( peerAddr );
    //peerAddr = new char[19] ;
    _ble.server_on( );
    printf("from class : %s\n", peerAddr );

    char capture[10] = {0};
    i2c_handler _i2c;
    _i2c.capture( capture );
	*/

}
