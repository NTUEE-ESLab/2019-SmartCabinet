//*****************************************************************************
// httpserver_app.c
//
// camera application macro & APIs
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
//*****************************************************************************
//
//! \addtogroup Httpserverapp
//! @{
//
//*****************************************************************************

#include <string.h>
#include <stdlib.h>

// SimpleLink include
#include "cc3100_simplelink.h"
#include "cc3100_sl_common.h"

// Free-RTOS/TI-RTOS include
#include "osi.h"

// HTTP lib includes
#include "HttpCore.h"
#include "HttpRequest.h"

#include "websocketapp.h"
#include "httpserverapp.h"
#include "camera_app.h"

#include "ArduCAM.h"
#include "cli_uart.h"
#include "app_config.h"
#include "myBoardInit.h"
#include "HttpDebug.h"

using namespace mbed_cc3100;

cc3100 _cc3100_Mod(NC, NC, p9, p10, p8, SPI(p5, p6, p7));//LPC1768  irq, nHib, cs, mosi, miso, sck

ArduCAM CAM(OV5642, p14, SPI(p11, p12, p13), I2C(p28, p27));

/****************************************************************************/
/*				MACROS										*/
/****************************************************************************/


/****************************************************************************
                              Global variables
****************************************************************************/
char *g_Buffer;
UINT8 g_success = 0;
int g_close = 0;
UINT16 g_uConnection;
OsiTaskHandle g_iCameraTaskHdl = 0;


void WebSocketCloseSessionHandler()
{
	g_close = 1;
}

void CameraAppTask(void *param)
{
	UINT8 Opcode = 0x02;
	struct HttpBlob Write;
	
	  /* Reset cam */
//	  CAM.set_bit(ARDUCHIP_GPIO,GPIO_RESET_MASK);
//	  wait_ms(50);
//    CAM.clear_bit(ARDUCHIP_GPIO,GPIO_RESET_MASK);
//    wait(1);
    CAM.write_reg(ARDUCHIP_MODE, 0x00);
     
    
#ifdef ENABLE_JPEG
	InitCameraComponents(320, 240);
#else
    InitCameraComponents(240, 256);
#endif    
    
	while(1)
	{
		if(g_close == 0)
		{
			Write.uLength = StartCamera((char **)&Write.pData);
			if(!sl_WebSocketSend(g_uConnection, Write, Opcode))
			{
				Uart_Write((uint8_t*)"sl_WebSocketSend failed.\n\r");
				while(1);
			}
		}
	}

}


/*!
 * 	\brief 					This websocket Event is called when WebSocket Server receives data
 * 							from client.
 *
 *
 * 	\param[in]  uConnection	Websocket Client Id
 * 	\param[in] *ReadBuffer		Pointer to the buffer that holds the payload.
 *
 * 	\return					none.
 *     					
 */
void WebSocketRecvEventHandler(UINT16 uConnection, char *ReadBuffer)
{
	char *camera = "capture";

	/*
	 * UINT8 Opcode;
	 * struct HttpBlob Write;
	*/

	g_uConnection = uConnection;

	g_Buffer = ReadBuffer;
	g_close = 0;
	if (!strcmp(ReadBuffer,camera))
	{
		if(!g_iCameraTaskHdl)
		{
			osi_TaskCreate(CameraAppTask,
								   "CameraApp",
									1024,
									NULL,
									CAMERA_SERVICE_PRIORITY,
									&g_iCameraTaskHdl);
		}

	}
	//Free memory as we are not using anywhere later
	free(g_Buffer);
	g_Buffer = NULL;
	/* Enter websocket application code here */
	return;
}


/*!
 * 	\brief 						This websocket Event indicates successful handshake with client
 * 								Once this is called the server can start sending data packets over websocket using
 * 								the sl_WebSocketSend API.
 *
 *
 * 	\param[in] uConnection			Websocket Client Id
 *
 * 	\return						none
 */
void WebSocketHandshakeEventHandler(UINT16 uConnection)
{
	g_success = 1;
	g_uConnection = uConnection;
}


//****************************************************************************
//
//! Task function start the device and crete a TCP server showcasing the smart
//! plug
//!
//****************************************************************************


void HttpServerAppTask(void * param)
{
	int32_t lRetVal = -1;
	Uart_Write((uint8_t*)"Start SimpleLink in AP Mode \n\r");
	//Start SimpleLink in AP Mode
	lRetVal = _cc3100_Mod.Network_IF_InitDriver(ROLE_AP);
    if(lRetVal < 0)
    {
        Uart_Write((uint8_t*)"Start SimpleLink in AP Mode Failed \n\r");
        LOOP_FOREVER();
    }	

	//Stop Internal HTTP Server
	lRetVal = _cc3100_Mod._netapp.sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    if(lRetVal < 0)
    {
        Uart_Write((uint8_t*)"Stop Internal HTTP Server Failed \n\r");
        LOOP_FOREVER();
    }	
    Uart_Write((uint8_t*)"HttpServerAppTask \n\r");
	//Run APPS HTTP Server
	HttpServerInitAndRun(NULL);
	
    Uart_Write((uint8_t*)"HttpServerAppTask has returned for some reason\n\r");
	LOOP_FOREVER();

}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************




