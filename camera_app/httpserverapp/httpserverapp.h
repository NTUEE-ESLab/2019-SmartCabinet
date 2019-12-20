/******************************************************************************
*
*   Copyright (C) 2013 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/
/**
 * @defgroup Httpserverapp
 *
 * @{
 */

#ifndef __HTTP_APP_H__
#define __HTTP_APP_H__

#define DEVICE_NAME                   "httpserver"
#define TI_NAME                       "cc3200_"
#define mDNS_SERV                     "._device-info._tcp.local"
#define TTL_MDNS_SERV                 4500

#define MAX_DEV_NAME_SIZE                       23

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  unsigned char DevNameLen;
  unsigned char DevName[MAX_DEV_NAME_SIZE];
}t_DevName;

typedef struct
{
	char    mDNSServNameUnReg[64];
	unsigned char   mDNSServNameUnRegLen;

}t_mDNSService;


//extern void HttpServerAppTask(void *);
//extern void Init3200SimpleLink ( void );
//void InitCameraComponents(int width, int height);

/*!
 * 	\brief 					This websocket Event is called when WebSocket Server receives data
 * 							from client.
 *
 *
 * 	\param[in] puConnection	Websocket Client Id
 * 	\param[in] *ReadBuffer		Pointer to the buffer that holds the payload.
 *
 * 	\return					none.
 *     					
 */

void WebSocketRecvEventHandler(uint16_t uConnection, char *ReadBuffer);

/*!
 * 	\brief 							Callback function that indicates that handshake was a success
 * 									Once this is called the server can start sending data packets over websocket using
 * 									the sl_WebSocketSend API.
 *
 *
 * 	\param[in] uConnection				Websocket Client Id
 *
 * 	\return							void
 */

void WebSocketHandshakeEventHandler(uint16_t uConnection);

/*!
 * 	\brief 							Callback function that indicates that Websocket is closed
 * 									Once this is called the server acts as HTTP Server
 *
 *
 * 	\return							None
 */
void WebSocketCloseSessionHandler(void);
void WebSocketCloseSessionHandler(void);
void CameraAppTask(void *param);
void HttpServerAppTask(void * param);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif   //__HTTP_APP_H__


