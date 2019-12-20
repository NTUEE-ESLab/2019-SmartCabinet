
#include <mbed.h>

I2CSlave slave(D14, D15);

int main() {
   char buf[10];
   char msg[] = "Slave!";

   slave.address(0x60);
   printf( "Start!\n" );
   printf( "%d\n", strlen(msg) + 1 );
   int k = 0;
   while (1) {
       int i = slave.receive();
       if( i==I2CSlave::ReadAddressed ){
            int len = 2;
            slave.write(msg+k, len); // Includes null char
            k+=1;
            if( k>5 ){
                k=0;
            }
       }
       //printf( "%d\n",i );
       switch (i) {
           /* 
           case I2CSlave::ReadAddressed:
               slave.write(msg, strlen(msg) + 1); // Includes null char
               break;
            */
           case I2CSlave::WriteGeneral:
               slave.read(buf, 10);
               printf("Read G: %s\n", buf);
               break;
           case I2CSlave::WriteAddressed:
               slave.read(buf, 10);
               printf("Read A: %s\n", buf);
               break;
       }
       for(int i = 0; i < 10; i++) buf[i] = 0;    // Clear buffer
   }
}