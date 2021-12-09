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
#include "math.h"
/*---------------------------------------------------------------------------*/
#define BROADCAST_CHANNEL   129
/*---------------------------------------------------------------------------*/
PROCESS(send_process, "Send process");
AUTOSTART_PROCESSES(&send_process);

/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_GREEN);
  printf("Received %u bytes: '%d'\n", packetbuf_datalen(),
         *(uint16_t *)packetbuf_dataptr());
}

/*---------------------------------------------------------------------------*/

static const struct broadcast_callbacks bc_rx = { broadcast_recv };
static struct broadcast_conn bc;
int T1=209.5;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(send_process, ev, data)
{
  static struct etimer et;
  //static uint16_t sht21_present;
  PROCESS_EXITHANDLER(broadcast_close(&bc))
  static uint16_t adxl346_present,x,y,z,i,difference,tempx,tempy,tempz;
  tempx =0;
  tempy =0;
  tempz =0;
   PROCESS_BEGIN();
   adxl346_present = SENSORS_ACTIVATE(adxl346);
  if(adxl346_present == ADXL346_ERROR) {
    printf("ADXL346 sensor is NOT present!\n");
    leds_on(LEDS_YELLOW);
  } else {
    adxl346.configure(ADXL346_CALIB_OFFSET, 0);
  }
   broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);
    while(1) {
      etimer_set(&et, 2*CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); 
     // current value
     x =adxl346.value(ADXL346_READ_X_mG);
     printf("The value of X: %u \n", x);
     y =adxl346.value(ADXL346_READ_Y_mG);
     printf("The value of Y: %u \n", y);
     z =adxl346.value(ADXL346_READ_Z_mG);
     printf("The value of Z: %u \n", z);
     //if(i>0){
     difference =(sqrt(x -tempx)+sqrt(y-tempy)+sqrt(z-tempz));
     //}
    //previous value
     tempx = x;
     tempy = y;
     tempz =z;
     if(difference>209){
       leds_on(LEDS_ALL);
    }
     packetbuf_copyfrom(difference , 11);
     broadcast_send(&bc);
     leds_toggle(LEDS_ORANGE);
   }
   PROCESS_END();    
} 
/*---------------------------------------------------------------------------*/