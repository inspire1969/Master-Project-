/*
 * Copyright (c) 2014, OpenMote Technologies, S.L.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup openmote-cc2538
 * @{
 *
 * \defgroup openmote-examples OpenMote-CC2538 Example Projects
 * @{
 *
 * Example project demonstrating the OpenMote-CC2538 functionality
 *
 * @{
 *
 * \file
 * Example demonstrating the OpenMote-CC2538 platform
 * \author
 * Pere Tuset <peretuset@openmote.com>
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/button-sensor.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "net/rime/broadcast.h"

#include "dev/adxl346.h"
#include "dev/max44009.h"
#include "dev/sht21.h"

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define BROADCAST_CHANNEL   129
#define Rx packetbuf_dataptr()

#define FALSE 0
#define TRUE  1

#define NODE_1 0


/*---------------------------------------------------------------------------*/
PROCESS(openmote_demo_process, "OpenMote-CC2538 demo process");
AUTOSTART_PROCESSES(&openmote_demo_process);
/*---------------------------------------------------------------------------*/
uint8_t Rx_Data[20] = {1,2,3,4,5,10,20,30,40,100};
uint8_t Rx_Data2[10] = {0x00};
uint8_t Rx_Data3[10] = {0x00};
uint8_t Rx_Data4[10] = {0x00};
uint8_t Rx_Data5[10] = {0x00};
uint8_t Rx_Data6[10] = {0x00};
uint8_t Nodes_Tab[12] = {0x00};
uint8_t RfRxStatus[10] = {0x00};
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_GREEN);
  
#if 1
  if((from->u8[0] == Nodes_Tab[0]) && (from->u8[1] == Nodes_Tab[1]))
  {
     RfRxStatus[NODE_1] = TRUE;
     memcpy(Rx_Data, (uint8_t *)packetbuf_dataptr(), packetbuf_datalen());
     
  }
#endif
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc_rx = { broadcast_recv };
static struct broadcast_conn bc;
static uint8_t NodeNumber = NODE_1;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(openmote_demo_process, ev, data)
{
  static struct etimer et;
  

  PROCESS_EXITHANDLER(broadcast_close(&bc))

  PROCESS_BEGIN();

  broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);

  printf("****************************************\n");
  while(1) {
    etimer_set(&et, CLOCK_SECOND * 1);

    

    PROCESS_YIELD();

    if(ev == PROCESS_EVENT_TIMER) 
    {
	switch(NodeNumber)
	{
	case NODE_1:
	{
	   if(RfRxStatus[NODE_1])
	   {
		Rx_Data[5] =NODE_1 + 1; 
		// Tx to PC
                printf("The value received of acceleration x, y,z = %d\n",
                Rx_Data[0],Rx_Data[1],Rx_Data[2],Rx_Data[3],Rx_Data[4],Rx_Data[5]); 
		RfRxStatus[NODE_1] = TRUE;
	
	   NodeNumber = NODE_1;
	  break;
	}
	default:
	{
  break;
	}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
