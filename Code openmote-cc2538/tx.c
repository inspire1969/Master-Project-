
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
#include "net/rime/unicast.h"
#include "dev/sht21.h"
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define BROADCAST_CHANNEL   129
/*---------------------------------------------------------------------------*/
PROCESS(demo_process, "demo process");
AUTOSTART_PROCESSES(&demo_process);
/*---------------------------------------------------------------------------*/
//float A[16] = {0.429, -0.235, -0.389, -0.098, -0.138, 0.795, -0.160, 0.062,
//		-0.396, -0.190, 0.584, -0.066, 0.672, 0.372, 0.596, 0.951};
//float B[4] = { 0, 0, 0, 0 };
//float U = 0;
//float H[4] = {1, 0, 0, 0};
//float Q[16] = {0.3, -0.016, 0.001, -0.001, -0.002, 0.298, 0.001, 0.002,
//		0.001, 0.001, 0.301, -0.001, -0.001, 0.002, -0.001, 0.299};
//float R = 0.1;
//float M[4] = {0, 0, 0, 0};
//float X0[4] = {0, 0, 0, 0};
//float P0[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//float Th = 0.5;
//float count = 0;
//float rate = 0;
//float bits = 0;
//float y = 0;
//float yp = 0;
//float Rec = 0;
//float X[4] = { 0, 0, 0, 0 };
//float P[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  //printf("%u \n",*(int16_t *)packetbuf_dataptr());
  printf("%u  %u ",*(int16_t *)packetbuf_dataptr(),*((int16_t *)packetbuf_dataptr()+1));
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  printf("unicast message  sent to %d.%d: status %d num_tx %d\n",dest->u8[0], dest->u8[1], status, num_tx);
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(demo_process, ev, data)
{
  static struct etimer et;
  static uint16_t sht21_present;
  static int16_t temperature;
  //static   temp;
  static int16_t   humidity;
  static int16_t tx[2]={};
  //static int16_t rec;
  //int16_t i;
  //static float temp;

  PROCESS_EXITHANDLER(unicast_close(&uc));

  PROCESS_BEGIN();
  unicast_open(&uc, 146, &unicast_callbacks);

  /* Initialize the SHT21 sensor */
  sht21_present = SENSORS_ACTIVATE(sht21);
  if(sht21_present == SHT21_ERROR) {
    printf("SHT21 sensor is NOT present!\n");
    leds_on(LEDS_RED);
  }

  //unicast_open(&uc, 146, &unicast_callbacks);
  while(1) {
    etimer_set(&et, 5*CLOCK_SECOND);
    linkaddr_t addr;
    if(sht21_present != SHT21_ERROR) {
        leds_on(LEDS_RED);
        temperature = sht21.value(SHT21_READ_TEMP);
        humidity = sht21.value(SHT21_READ_RHUM);
        leds_off(LEDS_RED);
      }
    //float temp=(float)temperature;
    //temp=(float)temperature/100;
    //temp=temp*10;
    //tx=(int16_t)temp;
    //pkf(A, B, U, H, Q, R, M, temp, X0, P0, Th, &count, X, P, &y, &yp, &Rec);
    //memcpy(X0, X, sizeof(float) * 4);
    //memcpy(P0, P, sizeof(float) * 16);
    //Rec=Rec*100;
    //rec=(int16_t)Rec;
    //int16_t tx=(int16_t)temp;
    tx[0]=temperature;
    tx[1]=humidity;
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    //packetbuf_copyfrom(&rec, sizeof(rec));
    packetbuf_copyfrom(tx, sizeof(tx));
    addr.u8[0] = 155;
    addr.u8[1] = 124;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }
  } 
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
