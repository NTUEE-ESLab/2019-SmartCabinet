//*****************************************************************************
// camera_app.h
//
// camera application macro & API's prototypes
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __CAMERA_APP_H__
#define __CAMERA_APP_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define UART_COMMAND_IMG_CAPTURE                    ('x')
#define LOWER_TO_UPPER_CASE                         (32)

#define DISABLE                           (0)
#define ENABLE                            (1)
#define SL_VERSION_LENGTH                 (11)

extern int PIXELS_IN_X_AXIS; 
extern int PIXELS_IN_X_AXIS;
extern int FRAME_SIZE_IN_BYTES;

#define NUM_OF_1KB_BUFFERS      10


#define BYTES_PER_PIXEL             (2)       // RGB 565 

#define ONE_KB                      (1024)
#define IMAGE_BUF_SIZE              (ONE_KB * NUM_OF_1KB_BUFFERS)

//#define NUM_OF_4B_CHUNKS            ((IMAGE_BUF_SIZE)/(sizeof(unsigned int)))
#define NUM_OF_4B_CHUNKS            ((IMAGE_BUF_SIZE - ONE_KB)/(sizeof(unsigned int)))
#define NUM_OF_1KB_CHUNKS           (IMAGE_BUF_SIZE/ONE_KB)
#define NUM_OF_4B_CHUNKS_IN_1KB     (ONE_KB/(sizeof(unsigned int)))

#define MAX_EMAIL_ID_LENGTH         34
#define SMTP_BUF_LEN                1024

//#define CAM_BT_CORRECT_EN   0x00001000

typedef enum opcd{
    START_CAPTURE = 1,
    STOP_CAPTURE,
    IMG_FMT,
    IMG_SIZE,
    EXIT
}e_opcode;

typedef struct cmd_struct{
    int    opcode;
    char    email_id[MAX_EMAIL_ID_LENGTH];
}s_cmd_struct;

//******************************************************************************
// APIs
//******************************************************************************

void getCamId(void);

unsigned short StartCamera(char **WriteBuffer);
int SetCameraResolution(int width, int height);
void InitCameraComponents(int width, int height);

static void CamControllerInit(void);
static void CameraIntHandler(void);
uint16_t CaptureImage(char **WriteBuffer);
uint8_t read_fifo_burst(void);

/****************************************************************************/
/*                      LOCAL FUNCTION PROTOTYPES                           */
/****************************************************************************/
//#ifdef ENABLE_JPEG
static int CreateJpegHeader(char *header, int width, int height,
                            int format, int restart_int, int qscale);
static int DefineRestartIntervalMarker(char *pbuf, int ri);
static int DefineHuffmanTableMarkerAC(char *pbuf, unsigned int *htable, int class_id);
static int DefineHuffmanTableMarkerDC(char *pbuf, unsigned int *htable, int class_id);
static int DefineQuantizationTableMarker (unsigned char *pbuf, int qscale, int format);
static int ScanHeaderMarker(char *pbuf, int format);
static int FrameHeaderMarker(char *pbuf, int width, int height, int format);
static int JfifApp0Marker(char *pbuf);
//#endif

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __CAMERA_APP_H__ */

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************


