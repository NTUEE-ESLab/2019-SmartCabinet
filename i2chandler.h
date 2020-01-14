#ifndef i2chandler_h
#define i2chandler_h

#include <mbed.h>


I2CSlave _slave( NC, NC );


class i2c_handler{
public:
	i2c_handler(){};
	~i2c_handler(){};

	void capture( char* result ){
        _slave.address( 0x60 );
        _slave.frequency(100000);
		printf("Start\n");
		while (1) {
            string temp = "Apple";
            for(int i = 0; i < 6; i++){ result[i] = temp[i]; }
            return ;
       		int i = _slave.receive();

       		if(i == 1 ){
       			_slave.write( '1' );
       		}
           	else if( i == 2 ){
            	_slave.read(buf, 10);
                printf("Read G: %s\n", buf);
                for(int i = 0; i < 10; i++){ result[i] = buf[i]; }
                break;
            }
           	else if( i==3 ){
                _slave.read(buf, 10);
                printf("Read A: %s\n", buf);
                for(int i = 0; i < 10; i++) {result[i] = buf[i];}
                break;
            }
       		
       		for(int i = 0; i < 10; i++) {
       			buf[i] = 0;    // Clear buffer
       		}
   	 	}
	};

private:
	
	char buf[10] ;

};

#endif