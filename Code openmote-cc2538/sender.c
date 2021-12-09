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
#include "random.h"
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
/*---------------------------------------------------------------------------*/
PROCESS(openmote_demo_process, "OpenMote-CC2538 demo process");
AUTOSTART_PROCESSES(&openmote_demo_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_GREEN); 
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc_rx = { broadcast_recv };
static struct broadcast_conn bc;
static unsigned char sensors_data[13];
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(openmote_demo_process, ev, data)
{
  static struct etimer et;
  static uint16_t sht21_present;
  static int16_t temperature, humidity;

  PROCESS_EXITHANDLER(broadcast_close(&bc))

  PROCESS_BEGIN();
  //initialize the SHT21 sensor 
  sht21_present = SENSORS_ACTIVATE(sht21);
  if(sht21_present == SHT21_ERROR) {
    printf("SHT21 sensor is NOT present!\n");
    leds_on(LEDS_RED);
  }

  broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);

  printf("This is aresult from sender\n");
#if 1
  while(1) {
    etimer_set(&et, CLOCK_SECOND * 5);

    PROCESS_YIELD();
    

    if(ev == PROCESS_EVENT_TIMER) {
     if(sht21_present != SHT21_ERROR) {
        leds_on(LEDS_RED);
        temperature = sht21.value(SHT21_READ_TEMP);
   	    sensors_data[0] = (temperature / 100) + 25;
   	    sensors_data[4] = (uint16_t)(temperature);
    	  sensors_data[5] = (uint8_t)(temperature >> 8);
        printf("temperature: %d\n" , temperature);
        humidity = sht21.value(SHT21_READ_RHUM);
    	  sensors_data[5] = (humidity / 100) + 25;
        //printf("humidity: %d\n", humidity);
        packetbuf_copyfrom(sensors_data,10);
        packetbuf_copyfrom(&temperature,sizeof(temperature));
        broadcast_send(&bc);
        leds_off(LEDS_RED);
      }

      //printf("****************************************\n");
    }
        
    //packetbuf_copyfrom(sensors_data,10);
    //packetbuf_copyfrom(&temperature,sizeof(temperature));
    //broadcast_send(&bc);   // Transmission to Sink node
    //if(ev == sensors_event) {
    //  if(data == &button_sensor) {
    //    if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) ==
    //       BUTTON_SENSOR_PRESSED_LEVEL) {
    //      leds_toggle(LEDS_GREEN);
    //      packetbuf_copyfrom(&counter, sizeof(counter));
    //      broadcast_send(&bc);
    //    }
    // }
    //}
  }
#endif
  PROCESS_END();
} 

/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
