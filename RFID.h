#ifndef rfid_h
#define rfid_h

#include "mbed.h"
#include "MFRC522.h"
#include <cstdlib>
#include <string>



class myRFID{
public:
	myRFID(){
		RfChip = new MFRC522(  PD_4, PD_3, PD_1, PD_2, PD_5 );
		RfChip->PCD_Init();
	};
	~myRFID(){};

	void Read_Card( char* Ustring ){
		while( true ){
			// Look for new cards
		    if ( ! RfChip->PICC_IsNewCardPresent()){
		      wait_ms(500);
		      continue;
		    }

		    // Select one of the cards
		    if ( ! RfChip->PICC_ReadCardSerial() ){
		      wait_ms(500);
		      continue;
		    }

		    // Print Card UID
		    printf("Card UID: \n");
		    for (uint8_t i = 0; i < RfChip->uid.size; i++){
		      printf(" %X02\n", RfChip->uid.uidByte[i] );
              
		      Ustring[ i ] = RfChip->uid.uidByte[i];
              std::sprintf( &Ustring[i] , "%X02", RfChip->uid.uidByte[i] );
              printf("%X02\n", Ustring[ i ] );
              if( i==RfChip->uid.size-1 ){
                  printf( "%d\n", i );
                  return;
              }
		    }

		}
	}

private:
	MFRC522 *RfChip;
};

#endif