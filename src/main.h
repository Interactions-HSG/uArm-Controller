#ifndef _Main_H_
#define _Main_H_

#include <Arduino.h>
#include <pb_arduino.h>
// include sub modules
#include <protobuf/line_protocol.pb.h>
#include <profile_manager.h>
#include <protobuf_helper.h>
// include drivers
#include <drivers/digital_generic.h>
#include <drivers/uart_ttl_generic.h>
#include <drivers/color_sensor.h>
#include <drivers/ultrasonic_sensor.h>
#include <drivers/step_lowlevel.h>
// ADI-MAIN-Include: Label for automatic driver initialization (Do not move!)

/*========================================================================*/
/*                          PUBLIC VARIABLES                              */
/*========================================================================*/

/* Decleration of the event state list */
// array to store which profiles expect an event
// => true means we expect an event for the corresponding profile
extern bool profiles_with_event[256];

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

#endif
