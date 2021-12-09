
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/i2c.h"
#include "dev/sht21.h"
#include "lib/sensors.h"
#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(sensor_temperature_process, "Sensor Temperature");
AUTOSTART_PROCESSES(&sensor_temperature_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from node b which is Ikram %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_temperature_process, ev, data)
{
  static struct etimer et;
  static uint16_t sht21_present;
  static signed   temperature;
  
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)


  PROCESS_BEGIN();
  
  sht21_present = SENSORS_ACTIVATE(sht21);
  if(sht21_present == SHT21_ERROR) {
    printf("SHT21 sensor is NOT present!\n");
    leds_on(LEDS_RED);
  }
    broadcast_open(&broadcast, 129, &broadcast_call);
 while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 10 + random_rand() % (CLOCK_SECOND * 5));
    if(sht21_present != SHT21_ERROR) {
        leds_on(LEDS_RED);
        temperature = sht21.value(SHT21_READ_TEMP);
        printf("Temperature:  %i.%iC\n", temperature / 100, temperature % 100);
        packetbuf_copyfrom(&temperature, sizeof(temperature));
        broadcast_send(&broadcast);
       printf("This is a sender\n");

    }
 }    
 PROCESS_END();
}