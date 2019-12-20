#include "OV7670.h"
#include <string> 
 
OV7670::OV7670(
    PinName sda, // Camera I2C port
    PinName scl, // Camera I2C port
    PinName xclk,// Camera XCLK
    PinName pclkPin,// PCLK
    PinName vs,  // VSYNC
    PinName hr,  // HREF
    
    PortName port, // 8bit bus port
    int mask // 0000 0111 4000 0111 1000 0000 0000 0000 = 0x07878000
    
    ) : camera(sda,scl), data(port, mask), inPort(PortB, 0x0000001C)
{
    camera.stop();
    camera.frequency(OV7670_I2CFREQ);
    xclkPin = xclk;
    size = 0;
}
 
void OV7670::Init() {
    double pclk_period_us = (1.0 / 16000000.0)*1000000;
    wait_ms(50);
}
 
void OV7670::WriteReg(int addr, int data) {
    // WRITE 0x42, ADDR, DATA
    camera.start();
    camera.write(OV7670_WRITE);
    wait_us(OV7670_WRITEWAIT);
    camera.write(addr);
    wait_us(OV7670_WRITEWAIT);
    camera.write(data);
    camera.stop();
}
 
int OV7670::ReadReg(int addr) {
    int data;
 
    // WRITE 0x42,ADDR
    camera.start();
    camera.write(OV7670_WRITE);
    wait_us(OV7670_WRITEWAIT);
    camera.write(addr);
    camera.stop();
    wait_us(OV7670_WRITEWAIT);    
 
    // WRITE 0x43,READ
    camera.start();
    camera.write(OV7670_READ);
    wait_us(OV7670_WRITEWAIT);
    data = camera.read(OV7670_NOACK);
    camera.stop();
 
    return data;
}
 
void OV7670::Reset(void) {    
    WriteReg(0x12,0x80) ; // RESET CAMERA
    wait_ms(200) ;
}
 
void OV7670::InitQQVGA() {
    // QQVGA RGB444
    WriteReg(REG_CLKRC,0x03);
    WriteReg(REG_COM11,0x0A) ;
    WriteReg(REG_TSLB,0x04);
    WriteReg(REG_COM7,0x04) ;
    
    WriteReg(REG_RGB444, 0x00);     // Disable RGB 444?
    WriteReg(REG_COM15, 0xD0);      // Set RGB 565?
    
    WriteReg(REG_HSTART,0x16) ;
    WriteReg(REG_HSTOP,0x04) ;
    WriteReg(REG_HREF,0x24) ;
    WriteReg(REG_VSTART,0x02) ;
    WriteReg(REG_VSTOP,0x7a) ;
    WriteReg(REG_VREF,0x0a) ;
    WriteReg(REG_COM10,0x02) ;
    WriteReg(REG_COM3, 0x04);
    WriteReg(REG_COM14, 0x1a);
    WriteReg(REG_MVFP,0x27) ;
    WriteReg(0x72, 0x22);
    WriteReg(0x73, 0xf2);
 
    // COLOR SETTING
    WriteReg(0x4f,0x80);
    WriteReg(0x50,0x80);
    WriteReg(0x51,0x00);
    WriteReg(0x52,0x22);
    WriteReg(0x53,0x5e);
    WriteReg(0x54,0x80);
    WriteReg(0x56,0x40);
    WriteReg(0x58,0x9e);
    WriteReg(0x59,0x88);
    WriteReg(0x5a,0x88);
    WriteReg(0x5b,0x44);
    WriteReg(0x5c,0x67);
    WriteReg(0x5d,0x49);
    WriteReg(0x5e,0x0e);
    WriteReg(0x69,0x00);
    WriteReg(0x6a,0x40);
    WriteReg(0x6b,0x0a);
    WriteReg(0x6c,0x0a);
    WriteReg(0x6d,0x55);
    WriteReg(0x6e,0x11);
    WriteReg(0x6f,0x9f);
 
    WriteReg(0xb0,0x84);
    size = 160 * 120 * 2;
}
 
void OV7670::scopeTest(Serial *host) {
    Timer t1, t2;
    int LED = 0x80000000;
    PortInOut status(PortA, LED);
    status = LED;
    int cycles = 1000000;
    while(inPort & 0x00000008) {};
    while(!(inPort & 0x00000008)) {};
    t1.start();
    while(cycles--) {
        while(!(inPort & 0x00000010)) {};
        while(inPort & 0x00000010) {};
    }
    status = 0;
    t1.stop();
    printf("One million cycles: %d\n\r", t1.read_us());
    //printf("Avg on, off = %d, %d\n\r", t1.read_us()/100, t2.read_us()/100);
}
 
int OV7670::captureImage() {
    int d, i=0;
    int byte = 0;
    Timer t1;
    string temping("");
    // Prepare SRAM output
    // ram.startWriteSequence();
    // Read in the first half of the image
    while(inPort & 0x00000008) {};    // Wait for VSYNC low
    while(!(inPort & 0x00000008)) {}; // Wait for VSYNC high
    //t1.start();
    while(inPort & 0x00000008) {      // While VSYNC high
        while((inPort & 0x00000008) && !(inPort & 0x00000004)) {};  // stall while VSYNC high and HREF low
        if(!(inPort & 0x00000008)) {                  // if VSYNC low
            printf("Interrupted!\n\r");
            break;
        }
        while (inPort & 0x00000004) {       // While HREF high
            while(!(inPort & 0x00000010)) {};  // Wait for PCLK high
            d = data;
            byte  = (d & 0x07800000) >> 19; // bit26 to bit7
            byte |= (d & 0x00078000) >> 15; // bit18 to bit3
            //arr1[i] = byte;
            //ram.writeSequence1(byte);
            temping += (char) byte;
            while(inPort & 0x00000010) {};        // Wait for PCLK low
            while(!(inPort & 0x00000010)) {};  // Wait for PCLK high
            d = data;
            byte  = (d & 0x07800000) >> 19; // bit26 to bit7
            byte |= (d & 0x00078000) >> 15; // bit18 to bit3\
            //ram.writeSequence1(byte);
            temping += (char) byte;
            //arr2[i++] = byte;
            while(inPort & 0x00000010) {}; 
            i++;
        }
    }
    //t1.stop();
    //ram.stopSequence();
    printf( "%s\n",temping.c_str() );
    printf("Image read: %d\n\r", t1.read_us());
    return i*2;
}
 
/* 
void OV7670::sendImage(Serial *dest, int numBytes) {
    Timer t;
    Serial host = *dest;
    // Write the image to the serial host
    ram.startReadSequence();
    t.start();
    for (int i = 0; i < numBytes; i++) {
        while(!(host.writeable())) {}
        host.putc(ram.readSequence1());
    }
    t.stop();
    printf("BT time: %d\n\r", t.read_us());
    ram.stopSequence();
}*/