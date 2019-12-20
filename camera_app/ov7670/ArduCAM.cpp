/*
  ArduCAM.cpp - Arduino library support for CMOS Image Sensor
  Copyright (C)2011-2015 ArduCAM.com. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ArduCAM.com. You can find the latest version of the library at
  http://www.ArduCAM.com

  Now supported controllers:
        -   OV7670
        -   MT9D111
        -   OV7675
        -   OV2640
        -   OV3640
        -   OV5642
        -   OV7660
        -   OV7725
        - MT9M112           
        - MT9V111           
        - OV5640    
        - MT9M001
                        
    We will add support for many other sensors in next release.
        
  Supported MCU platform
        -   Theoretically support all Arduino families
        -   Arduino UNO R3          (Tested)
        -   Arduino MEGA2560 R3     (Tested)
        -   Arduino Leonardo R3     (Tested)
        -   Arduino Nano            (Tested)
        -   Arduino DUE             (Tested)
        - Arduino Yun               (Tested)
        -   Raspberry Pi            (Tested)
              
  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.ArduCAM.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*------------------------------------
    Revision History:
    2012/09/20  V1.0.0  by Lee  first release   
    2012/10/23  V1.0.1  by Lee  Resolved some timing issue for the Read/Write Register
    2012/11/29  V1.1.0  by Lee  Add support for MT9D111 sensor
    2012/12/13  V1.2.0  by Lee  Add support for OV7675 sensor
    2012/12/28  V1.3.0  by Lee  Add support for OV2640,OV3640,OV5642 sensors
    2013/02/26  V2.0.0  by Lee  New Rev.B shield hardware, add support for FIFO control 
                                                            and support Mega1280/2560 boards 
    2013/05/28  V2.1.0  by Lee  Add support all drawing functions derived from UTFT library         
    2013/08/24  V3.0.0  by Lee  Support ArudCAM shield Rev.C hardware, features SPI interface and low power mode.
                                Support almost all series of Arduino boards including DUE.
    2014/02/06  V3.0.1  by Lee  Minor change to the library, fixed some bugs, add self test code to the sketches for easy debugging.
    2014/03/09  V3.1.0  by Lee  Add the more impressive example sketches. 
                                Optimise the OV5642 settings, improve image quality.
                                Add live preview before JPEG capture.
                                Add play back photos one by one after BMP capture.  
    2014/05/01  V3.1.1  by Lee  Minor changes to add support Arduino IDE for linux distributions.               
    2014/09/30  V3.2.0  by Lee  Improvement on OV5642 camera dirver.            
    2014/10/06  V3.3.0  by Lee  Add OV7660,OV7725 camera support.   
    2015/02/27  V3.4.0  by Lee  Add the support for Arduino Yun board, update the latest UTFT library for ArduCAM.      
    2015/06/09  V3.4.1  by Lee  Minor changes and add some comments         
    2015/06/19  V3.4.2  by Lee  Add support for MT9M112 camera.                 
    2015/06/20  V3.4.3  by Lee  Add support for MT9V111 camera.     
    2015/06/22  V3.4.4  by Lee  Add support for OV5640 camera.                                      
    2015/06/22  V3.4.5  by Lee  Add support for MT9M001 camera.     
--------------------------------------*/

#include "ArduCAM.h"
#include "cli_uart.h"
#include "HttpDebug.h"
#include "memorysaver.h"

ArduCAM::ArduCAM(uint8_t model, PinName cam_cs, SPI cam_spi, I2C cam_i2c)
        :_cam_cs(cam_cs), _cam_spi(cam_spi), _cam_i2c(cam_i2c)
{ 
    _cam_spi.format(8,0);
    _cam_spi.frequency(8000000);
    _cam_i2c.frequency(100000);
    _cam_cs = 1;
    
//    flush_fifo();
    
    sensor_model=model;
    switch(sensor_model)
    {
        case OV7660:
        case OV7670:
        case OV7675:
        case OV7725:
            sensor_addr = 0x42;
            break;
        case MT9D111_A: //Standard MT9D111 module
            sensor_addr = 0xba;
            break;          
        case MT9D111_B: //Flex MT9D111 AF module
            sensor_addr = 0x90;
            break;
        case MT9M112:
            sensor_addr = 0x90; 
            break;      
        case MT9M001:
            sensor_addr = 0xba; 
            break;      
        case OV3640:
        case OV5640:
        case OV5642:
            sensor_addr = 0x78;
            break;
        case OV2640:
        case OV9650:
        case OV9655:
            sensor_addr = 0x60;
            break;
        case MT9V111:
            sensor_addr = 0xB8;
            break;      
        default:
            sensor_addr = 0x42;
            break;
    }
}

ArduCAM::~ArduCAM()
{

}

//Assert CS signal
void ArduCAM::cs_low(void)
{
    _cam_cs = 0;
}

//Disable CS signal
void ArduCAM::cs_high(void)
{
    _cam_cs = 1;
}

//Set corresponding bit  
void ArduCAM::set_bit(uint8_t addr, uint8_t bit)
{
    uint8_t temp;
    temp = read_reg(addr);
    write_reg(addr, temp | bit);

}

//Clear corresponding bit 
void ArduCAM::clear_bit(uint8_t addr, uint8_t bit)
{
    uint8_t temp;
    temp = read_reg(addr);
    write_reg(addr, temp & (~bit));
}

//Get corresponding bit status
uint8_t ArduCAM::get_bit(uint8_t addr, uint8_t bit)
{
    uint8_t temp;
    temp = read_reg(addr);
    temp = temp & bit;
    return temp;
}

//Set ArduCAM working mode
//MCU2LCD_MODE: MCU writes the LCD screen GRAM
//CAM2LCD_MODE: Camera takes control of the LCD screen
//LCD2MCU_MODE: MCU read the LCD screen GRAM
void ArduCAM::set_mode(uint8_t mode)
{
    switch(mode)
    {
        case MCU2LCD_MODE:
            write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
            break;
        case CAM2LCD_MODE:
            write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);
            break;
        case LCD2MCU_MODE:
            write_reg(ARDUCHIP_MODE, LCD2MCU_MODE);
            break;
        default:
            write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
            break;
    }
}

//Low level SPI write operation
int ArduCAM::bus_write(int address, int value) {
  // take the SS pin low to select the chip:
  cs_low();
  //  send in the address and value via SPI:
  _cam_spi.write(address);
  _cam_spi.write(value);
  // take the SS pin high to de-select the chip:
  cs_high();
  
  return 0;
}

//Low level SPI read operation
uint8_t ArduCAM::bus_read(int address) {
  uint8_t value = 0;
  // take the SS pin low to select the chip:
  cs_low();
  //  send in the address and value via SPI:
//  printf("addr = 0x%x\r\n",address);
  _cam_spi.write(address);
  value = _cam_spi.write(0x00);
  // take the SS pin high to de-select the chip:
  cs_high();
  return value;
}

//Write ArduChip internal registers
void ArduCAM::write_reg(uint8_t addr, uint8_t data)
{
    bus_write(addr | 0x80, data);
}

//Read ArduChip internal registers
uint8_t ArduCAM::read_reg(uint8_t addr)
{
    uint8_t data;
    data = bus_read(addr);//(addr & 0x7F)
//    printf("len = 0x%x\r\n",data);
    return data;
}

//Reset the FIFO pointer to ZERO        
void ArduCAM::flush_fifo(void)
{
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

//Send capture command
void ArduCAM::start_capture(void)
{
    write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

//Clear FIFO Complete flag
void ArduCAM::clear_fifo_flag(void)
{
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

//Read FIFO single      
uint8_t ArduCAM::read_fifo(void)
{
    uint8_t data;
    data = bus_read(SINGLE_FIFO_READ);
    return data;
}

//Read Write FIFO length
//Support ArduCAM Mini only
uint32_t ArduCAM::read_fifo_length(void)
{
    uint32_t len1,len2,len3=0;
    uint32_t length=0;
    len1 = read_reg(FIFO_SIZE1);    
    len2 = read_reg(FIFO_SIZE2);    
    len3 = read_reg(FIFO_SIZE3) & 0x07;
    length = ((len3 << 16) | (len2 << 8) | len1);// & 0x07ffff;
    return length;
}

//Send read fifo burst command
//Support ArduCAM Mini only
void ArduCAM::set_fifo_burst()
{
//    _cam_cs = 0;
    _cam_spi.write(BURST_FIFO_READ);
//    _cam_cs = 1;
}

//I2C Write 8bit address, 8bit data
uint8_t ArduCAM::wrSensorReg8_8(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID;
    buff[1] = regDat;
    _cam_i2c.write(sensor_addr,(char*)buff,2);

//    wait_ms(1);
  return(1);
}

//I2C Read 8bit address, 8bit data
uint8_t ArduCAM::rdSensorReg8_8(uint8_t regID, uint8_t* regDat)
{
    uint8_t buff[20];
    buff[0] = regID;
    
    _cam_i2c.write(sensor_addr,(char*)buff,1);
    _cam_i2c.read(sensor_addr+1,(char*)buff,1);
    *regDat = buff[0];

 //   wait_ms(1);
  return(1);
}

//I2C Write 8bit address, 16bit data
uint8_t ArduCAM::wrSensorReg8_16(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID;
    buff[1] = regDat >> 8;
    buff[2] = regDat & 0x00FF;
    _cam_i2c.write(sensor_addr,(char*)buff,3);

//    wait_ms(1);
    return(1);
}

//I2C Read 8bit address, 16bit data
uint8_t ArduCAM::rdSensorReg8_16(uint8_t regID, uint16_t* regDat)
{
//    uint8_t temp;
    uint8_t buff[20];
    buff[0] = regID;
    
    _cam_i2c.write(sensor_addr,(char*)buff,1);
    _cam_i2c.read(sensor_addr+1,(char*)buff, 2);
    *regDat = (buff[0] << 8) | buff[1];

//    wait_ms(1);
    return(1);
}

//I2C Write 16bit address, 8bit data
uint8_t ArduCAM::wrSensorReg16_8(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    buff[2] = regDat;
    _cam_i2c.write(sensor_addr,(char*)buff,3);

//    wait_ms(1);
  return(1);
}

//I2C Read 16bit address, 8bit data
uint8_t ArduCAM::rdSensorReg16_8(uint16_t regID, uint8_t* regDat)
{
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    _cam_i2c.write(sensor_addr,(char*)buff,2);
    _cam_i2c.read(sensor_addr+1,(char*)buff, 1);
    *regDat = buff[0];

//    wait_ms(1);
  return(1);
}

//I2C Write 16bit address, 16bit data
uint8_t ArduCAM::wrSensorReg16_16(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    buff[2] = regDat >> 8;
    buff[3] = regID & 0x00FF;
    _cam_i2c.write(sensor_addr,(char*)buff,4);

//    wait_ms(1);
  return(1);
}

//I2C Read 16bit address, 16bit data
uint8_t ArduCAM::rdSensorReg16_16(uint16_t regID, uint16_t* regDat)
{
//    uint8_t temp;
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    _cam_i2c.write(sensor_addr,(char*)buff,2);
    _cam_i2c.read(sensor_addr+1,(char*)buff, 1);
    *regDat = (buff[0] << 8) | buff[1];

//    wait_ms(1);
  return(1);
}

//I2C Array Write 8bit address, 8bit data
int ArduCAM::wrSensorRegs8_8(const struct sensor_reg reglist[])
{
//    int err = 0;
    uint16_t reg_addr = 0;
    uint16_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xff) | (reg_val != 0xff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg8_8(reg_addr, reg_val);
    next++;
        
    } 
    
    return 1;
}

//I2C Array Write 8bit address, 16bit data
int ArduCAM::wrSensorRegs8_16(const struct sensor_reg reglist[])
{
//    int err = 0;
    
    uint32_t reg_addr = 0;
    uint32_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xff) | (reg_val != 0xffff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg8_16(reg_addr, reg_val);
        //  if (!err)
        //return err;
        next++;
    }  
    
    return 1;
}

//I2C Array Write 16bit address, 8bit data
int ArduCAM::wrSensorRegs16_8(const struct sensor_reg reglist[])
{
//    int err = 0;
    
    uint32_t reg_addr = 0;
    uint8_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xffff) | (reg_val != 0xff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg16_8(reg_addr, reg_val);
        //if (!err)
        //return err;
       next++;
    } 
    
    return 1;
}

//I2C Array Write 16bit address, 16bit data
int ArduCAM::wrSensorRegs16_16(const struct sensor_reg reglist[])
{
//    int err = 0;
    
    uint32_t reg_addr = 0;
    uint32_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xffff) | (reg_val != 0xffff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg16_16(reg_addr, reg_val);
        //if (!err)
        //   return err;
       next++;
    } 
    
    return 1;
}

void ArduCAM::OV5642_set_JPEG_size(uint8_t size)
{
    #if defined OV5642_CAM
//    uint8_t reg_val;
    wrSensorRegs16_8(ov5642_dvp_fmt_global_init); 
    wait_ms(100); 
    switch(size)
    {
        case OV5642_320x240:    
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_qvga);
            wrSensorReg16_8(0x4407,0x04);
            wrSensorReg16_8(0x3818, 0xA8); 
            wrSensorReg16_8(0x3621, 0x10); 
            wrSensorReg16_8(0x3801 , 0x98); 
            break;
        case OV5642_640x480:    
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_vga);
            wrSensorReg16_8(0x3818, 0xA8); 
            wrSensorReg16_8(0x3621, 0x10); 
            wrSensorReg16_8(0x3801 , 0x98);  
            break;
        case OV5642_1280x720:
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_qvga);
            wrSensorRegs16_8(ov5642_res_720P);
            wrSensorReg16_8(0x3818, 0xA8); 
            wrSensorReg16_8(0x3621, 0x10); 
            wrSensorReg16_8(0x3801 , 0x98);
            break;
        case OV5642_1920x1080:
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_qvga);
            wrSensorRegs16_8(ov5642_res_1080P);
            wrSensorReg16_8(0x3818, 0xA8); 
            wrSensorReg16_8(0x3621, 0x10); 
            wrSensorReg16_8(0x3801 , 0x98);
            break;
        case OV5642_2048x1563:
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_qxga);
            wrSensorReg16_8(0x3818, 0xA8); 
            wrSensorReg16_8(0x3621, 0x10); 
            wrSensorReg16_8(0x3801 , 0x98); 
            break;
        case OV5642_2592x1944:
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_5M);
            wrSensorReg16_8(0x4407,0x08); 
            wrSensorReg16_8(0x3818, 0xA8); 
            wrSensorReg16_8(0x3621, 0x10); 
            wrSensorReg16_8(0x3801 , 0x98);  
            break;
        default:
            wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_qvga);
            break;
    }

    #endif
}

void ArduCAM::set_format(uint8_t fmt)
{
    if(fmt == BMP){
        m_fmt = BMP;
        printf("Format = BMP\r\n");
    }else{
        m_fmt = JPEG;
        printf("Format = JPEG\r\n");
    }    
}
            
void ArduCAM::InitCAM()
{
    uint8_t rtn = 0;
    uint8_t reg_val;
    switch(sensor_model)
    {
        case OV7660:
        {
            #if defined OV7660_CAM
            wrSensorReg8_8(0x12, 0x80);
            wait_ms(100);
            rtn = wrSensorRegs8_8(OV7660_QVGA);
            #endif
            break;
        }
        case OV7725:
        {
            #if defined OV7725_CAM
            wrSensorReg8_8(0x12, 0x80);
            wait_ms(100);
            rtn = wrSensorRegs8_8(OV7725_QVGA);
            rdSensorReg8_8(0x15,&reg_val);
            wrSensorReg8_8(0x15, (reg_val | 0x02));
            #endif
            break;
        }
        case OV7670:
        {
            #if defined OV7670_CAM
            wrSensorReg8_8(0x12, 0x80);
            wait_ms(100);
            rtn = wrSensorRegs8_8(OV7670_QVGA);
            #endif
            break;
        }
        case OV7675:
        {
            #if defined OV7675_CAM
            wrSensorReg8_8(0x12, 0x80);
            wait_ms(100);
            rtn = wrSensorRegs8_8(OV7675_QVGA);
            
            #endif
            break;
        }
        case MT9D111_A:
        case MT9D111_B:
        {
            #if defined MT9D111_CAM
            wrSensorRegs8_16(MT9D111_QVGA_30fps);
            wait_ms(1000);
            wrSensorReg8_16(0x97, 0x0020);
            wrSensorReg8_16(0xf0, 0x00);
            wrSensorReg8_16(0x21, 0x8403); //Mirror Column
            wrSensorReg8_16(0xC6, 0xA103);//SEQ_CMD
            wrSensorReg8_16(0xC8, 0x0005); //SEQ_CMD
      #endif
            break;

        }  
        case OV5642:
        {
            #if defined OV5642_CAM
            wrSensorReg16_8(0x3008, 0x80);

            wait_ms(100);
            if(m_fmt == JPEG)
            {
                wrSensorRegs16_8(ov5642_dvp_fmt_global_init); 
                wait_ms(100);
//                OV5642_set_JPEG_size(OV5642_640x480); 
                OV5642_set_JPEG_size(OV5642_320x240); 
//                wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_vga);
//                wrSensorRegs16_8(ov5642_dvp_fmt_jpeg_qvga); 
                wrSensorReg16_8(0x4407,0x0C);
                
            }
            else
            {
                wrSensorRegs16_8(OV5642_RGB_QVGA);
                rdSensorReg16_8(0x3818,&reg_val);
                wrSensorReg16_8(0x3818, (reg_val | 0x60) & 0xff);
                rdSensorReg16_8(0x3621,&reg_val);
                wrSensorReg16_8(0x3621, reg_val & 0xdf);
            }
            
            #endif
            break;
        }
        case OV3640:
        {
            #if defined OV3640_CAM
            rtn = wrSensorRegs16_8(OV3640_QVGA);
            #endif
            break;
        }
        case OV2640:
        {
            #if defined OV2640_CAM
            wrSensorReg8_8(0xff, 0x01);
            wrSensorReg8_8(0x12, 0x80);
            wait_ms(100);
            if(m_fmt == JPEG)
            {
                wrSensorRegs8_8(OV2640_JPEG_INIT);
                wrSensorRegs8_8(OV2640_YUV422);
                wrSensorRegs8_8(OV2640_JPEG);
                wrSensorReg8_8(0xff, 0x01);
                wrSensorReg8_8(0x15, 0x00);
                wrSensorRegs8_8(OV2640_320x240_JPEG);
                //wrSensorReg8_8(0xff, 0x00);
                //wrSensorReg8_8(0x44, 0x32);
            }
            else
            {
                wrSensorRegs8_8(OV2640_QVGA);           
            }
            #endif
            break;
        }
        case OV9655:
        {
            
            break;
        }
        case MT9M112:
        {
            #if defined MT9M112_CAM
            wrSensorRegs8_16(MT9M112_QVGA);
            #endif  
            break;
        }
        case MT9V111:
        {
            #if defined MT9V111_CAM
            //Reset sensor core
            wrSensorReg8_16(0x01, 0x04);
            wrSensorReg8_16(0x0D, 0x01);
            wrSensorReg8_16(0x0D, 0x00);
            //Reset IFP 
            wrSensorReg8_16(0x01, 0x01);
            wrSensorReg8_16(0x07, 0x01);
            wrSensorReg8_16(0x07, 0x00);
            wait_ms(100);
            wrSensorRegs8_16(MT9V111_QVGA);
            //wait_ms(1000);
            wrSensorReg8_16(0x97, 0x0020);
            wrSensorReg8_16(0xf0, 0x00);
            wrSensorReg8_16(0x21, 0x8403); //Mirror Column
            wrSensorReg8_16(0xC6, 0xA103);//SEQ_CMD
      wrSensorReg8_16(0xC8, 0x0005); //SEQ_CMD
      #endif    
            break;
        }
        case OV5640:
        {
            #if defined OV5640_CAM
            wrSensorReg16_8(0x3008, 0x80);
            wait_ms(100);
            wrSensorRegs16_8(OV5640YUV_Sensor_Dvp_Init);
            wrSensorRegs16_8(ov5640_vga_preview);
            wrSensorRegs16_8(OV5640_RGB_QVGA);
            #endif
            break;
        }
        case MT9M001:
        {
            #if defined MT9M001_CAM
            wrSensorRegs8_16(MT9M001_QVGA_30fps);       
            #endif  
            break;
        }
        default:
            
            break;  
    }
}
