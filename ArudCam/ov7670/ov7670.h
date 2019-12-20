#include "mbed.h"
#include "OV7670reg.h"
 
#define OV7670_WRITE (0x42)
#define OV7670_READ  (0x43)
#define OV7670_WRITEWAIT (20)
#define OV7670_NOACK (0)
#define OV7670_REGMAX (201)
#define OV7670_I2CFREQ (50000)
 
//
// OV7670 Camera board test (no FIFO)
//

class OV7670 {
public:
    I2C camera;
    PinName xclkPin;
    PortIn data, inPort;
    int size;
        
    OV7670(
        PinName sda, // Camera I2C port
        PinName scl, // Camera I2C port
        PinName xclk,// Camera XCLK
        PinName pclkPin,// PCLK
        PinName vs,  // VSYNC
        PinName hr,  // HREF
        
        PortName port, // 8bit bus port
        int mask // 0000 0111 4000 0111 1000 0000 0000 0000 = 0x07878000
        );
 
    void Init();
 
    // write to camera
    void WriteReg(int addr, int data);
 
    // read from camera
    int ReadReg(int addr);
 
    void Reset(void);
    
    void InitQQVGA();
 
    void scopeTest(Serial *host);
 
    // Capture image to external SRAM. Returns number of bytes received
    int captureImage();
    
    virtual void sendImage(Serial *dest, int numBytes);
 
};