
#include "mbed.h"
#include "ov7670.h"



Timer t;
bool new_send = false;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);


//Camera
OV7670 camera
(
    I2C_SDA,I2C_SCL,            // SDA,SCL(I2C / SCCB)
    D8, A0,
    D6, A1,
    PortA, 0x07878000
); 
