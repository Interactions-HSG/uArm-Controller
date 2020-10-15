/**************************************************************************/
/*!
    @file     protobuf_helper.cpp
    @author   Jonas Brütsch

    The protobuf helper includes all additional functions and variables
    used to initialize and handle protobuf messages.

*/
/**************************************************************************/
#include <protobuf_helper.h>


/*========================================================================*/
/*                          PRIVATE DEFINITIONS                           */
/*========================================================================*/

/* Macros */
#define BAUDRATE 115200
#define TERMINATOR 0

/* Protobuf streams */
pb_istream_s pb_in;
pb_ostream_s pb_out;

/* Function prototypes */
/**
    @brief  Callback function for string type handling
    @param  TODO:
*/
bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

/**
    @brief  Callback function for uint8_t type handling
    @param  TODO:
*/
bool encode_uint8(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);


/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*
    Protobuf Initializer: Initializes istream + ostream of protobuf
*/ 
void protobuf_init(){
    // init serial1 
    Serial.begin(BAUDRATE);
    pb_in = as_pb_istream(Serial);
    pb_out = as_pb_ostream(Serial);
}

/**************************************************************************/
/*
    Protobuf Decoder: Decodes incoming messages + sends feedback on failure
*/ 
void protobuf_decode(Request *req){
    // FIXME: works but returns not true => check why
    if(!pb_decode(&pb_in, Request_fields, req))
        ;//TODO: send error message: decoding failed

}

/**************************************************************************/
/*
    Function used to send feedback to the gateway.

    For pb_callback_t message types:
        - use .arg [void*] to pass data to callback
        - use .funcs.encode to define callback function to encode
        - use .funcs.decode to define callback function to decode
*/ 
bool send_feedback(uint32_t profile_id, const char* msg)
{ 
  //uint8_t test_id = 40;
  //uint8_t* point_id = &test_id;

  Feedback feedback = {};
  feedback.profile_id = profile_id;
  feedback.message.arg = (void*) msg;
  feedback.message.funcs.encode = &encode_string;
  //feedback.data.arg = (void*) point_id;
  //feedback.data.funcs.encode = &encode_uint8;
  bool res = pb_encode(&pb_out, Feedback_fields, &feedback);
  Serial.write(TERMINATOR);
  return res;
}


/*========================================================================*/
/*                          PRIVATE FUNCTIONS                             */
/*========================================================================*/

/**************************************************************************/
/*
    Callback funtion for string types
*/ 
bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    void * const *msg = (void* const*)*arg;
    const char* str = (const char*) msg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}

/**************************************************************************/
/*
    Callback funtion for uint8_t types => FIXME: not working correctly 
*/ 
bool encode_uint8(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    uint8_t* data = (uint8_t*) *arg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    
    return pb_encode_string(stream, (uint8_t*)data, 1);
}