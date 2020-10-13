#ifndef _Main_H_
#define _Main_H_

#include <Arduino.h>
#include <pb_arduino.h>
// include sub modules
#include <protobuf/line_protocol.pb.h>
#include <profile_manager.h>
// include drivers
#include <drivers/digital_generic.h>

/*========================================================================*/
/*                          PUBLIC VARIABLES                              */
/*========================================================================*/


/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/
/**
    @brief  Sends simple message to gateway  (for testing)
    @param  profile_id: Profile_id 
*/
bool send_msg(uint32_t profile_id);

#endif