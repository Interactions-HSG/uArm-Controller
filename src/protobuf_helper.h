#ifndef _PROTOBUF_HELPER_H_
#define _PROTOBUF_HELPER_H_

#include "main.h"


/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**
    @brief  Initializes protobuf streams 
*/
void protobuf_init();

/**
    @brief  Decodes incoming protobuf message, sends error msg on failure
    @param  req: Request message to decode 
*/
void protobuf_decode(Request *req);


/**
    @brief  Sends simple message to gateway
    @param  profile_id: Profile_id
    @param  msg: feedback message for debugging purpose 
*/
bool send_feedback(uint32_t profile_id, const char* msg);


#endif