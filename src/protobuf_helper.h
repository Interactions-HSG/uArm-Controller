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
    @brief  Sends simple debug message to the gateway
    @param  msg: feedback message for debugging purpose 
*/
bool send_debug(const char *msg);

/**
    @brief  Sends simple error message to the gateway
    @param  profile_id: Profile_id
    @param  msg: feedback message for debugging purpose 
*/
bool send_error(uint32_t profile_id, const char *msg);

/**
    @brief  Sends simple acknowledgement message to the gateway
    @param  profile_id: Profile_id
*/
bool send_ack(uint32_t profile_id);

/**
    @brief  Sends simple DONE message to the gateway
    @param  profile_id: Profile_id
*/
bool send_done(uint32_t profile_id);

/**
    @brief  Sends data message to the gateway
    @param  profile_id: Profile_id
    @param  data: void pointer to raw data
*/
bool send_data(uint32_t profile_id, void *data);

#endif
