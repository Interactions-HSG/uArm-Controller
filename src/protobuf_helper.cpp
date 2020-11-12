/**************************************************************************/
/*!
    @file     protobuf_helper.cpp
    @author   Jonas Brütsch

    The protobuf helper includes all additional functions and variables
    used to initialize and handle protobuf messages.

*/
/**************************************************************************/
#include "protobuf_helper.h"

/*========================================================================*/
/*                          PRIVATE DEFINITIONS                           */
/*========================================================================*/

/* Macros */
#define BAUDRATE 115200
#define TERMINATOR 0

/* Protobuf streams */
pb_istream_s pb_in;
pb_ostream_s pb_out;

// length of payload: number of bytes
uint32_t payload_length;

/**
    @brief  Callback function for bytes type encoding
*/
bool encode_bytes(pb_ostream_t *stream, const pb_field_t *field, void *const *arg);

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*
    Protobuf Initializer: Initializes istream + ostream of protobuf
*/
void protobuf_init()
{
    // init serial1
    Serial.begin(BAUDRATE);
    pb_in = as_pb_istream(Serial);
    pb_out = as_pb_ostream(Serial);
}

/**************************************************************************/
/*
    Protobuf Decoder: Decodes incoming messages (+ sends response on failure)
*/
void protobuf_decode(Request *req)
{

    // FIXME: works but returns not true => check why
    if (!pb_decode(&pb_in, Request_fields, req))
        send_error(404, "Decoding failed");
}

/*========================================================================*/
/*                FUNCTIONS USED TO SEND MESSAGES                         */
/*========================================================================*/
/*
    Functions used to send response to the gateway.

    For pb_callback_t message types:
        - use .arg [void*] to pass data to callback
        - use .funcs.encode to define callback function to encode
        - use .funcs.decode to define callback function to decode
    ! callback only working for simple messages (not oneof)
*/

/**************************************************************************/
/*
    Function used to send a simple message for debugging purpose.
*/
bool send_debug(const char *msg)
{
    // initiate Response msg
    Response response = {};

    // update data length for data field [bytes]
    payload_length = strlen(msg);

    /* add response fields */
    response.code = ResponseCode_DEBUG;
    response.payload.arg = (void *)msg;
    response.payload.funcs.encode = &encode_bytes;
    // encode protobuf message
    bool res = pb_encode(&pb_out, Response_fields, &response);
    // send termination
    Serial.write(TERMINATOR);
    return res;
}

/**************************************************************************/
/*
    Function used to send a simple error message to the gateway.
*/
bool send_error(uint32_t profile_id, const char *msg)
{
    // initiate Response msg
    Response response = {};

    // update data length for data field [bytes]
    payload_length = strlen(msg);

    /* add response fields */
    response.code = ResponseCode_ERROR;
    response.profile_id = profile_id;
    response.payload.arg = (void *)msg;
    response.payload.funcs.encode = &encode_bytes;
    // encode protobuf message
    bool res = pb_encode(&pb_out, Response_fields, &response);
    // send termination
    Serial.write(TERMINATOR);
    return res;
}

/**************************************************************************/
/*
    Function used to send a simple ACK message to the gateway.
*/
bool send_ack(uint32_t profile_id)
{
    // initiate Response msg
    Response response = {};

    /* add response fields */
    response.code = ResponseCode_ACK;
    response.profile_id = profile_id;
    // encode protobuf message
    bool res = pb_encode(&pb_out, Response_fields, &response);
    // send termination
    Serial.write(TERMINATOR);
    return res;
}

/**************************************************************************/
/*
    Function used to send a data message to the gateway.
    
    Param: data + length are optional => if not used, an empty data message will be sent
*/
bool send_data(uint32_t profile_id, void *data, uint32_t length)
{
    // initiate Response msg
    Response response = {};

    /* add response fields */
    response.code = ResponseCode_DATA;
    response.profile_id = profile_id;

    // check if argument data is used
    if (data != NULL)
    {
        // update data length for data field [bytes]
        payload_length = length;
        /* add response fields for payload */
        response.payload.arg = data;
        response.payload.funcs.encode = &encode_bytes;
    }
    // encode protobuf message
    bool res = pb_encode(&pb_out, Response_fields, &response);
    // send termination
    Serial.write(TERMINATOR);
    return res;
}

/*========================================================================*/
/*                          PRIVATE FUNCTIONS                             */
/*========================================================================*/

/**************************************************************************/
/*
    Callback funtion for encoding bytes types (only working for simple messages, non-oneof)
*/
bool encode_bytes(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    void *const *msg = (void *const *)*arg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t *)msg, payload_length);
}
