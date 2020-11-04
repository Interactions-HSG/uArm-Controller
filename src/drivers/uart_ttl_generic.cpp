#include "uart_ttl_generic.h"

/*========================================================================*/
/*                          PRIVATE DEFINITIONS                           */
/*========================================================================*/
// used to receive and send feedback to gateway
void feedback_handler(uint32_t profile_id, HardwareSerial *Serialref);

/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    TODO: Description of initialization function for uart_ttl_generic
*/
bool init_uart_ttl_generic(uint32_t profile_id, R_UART_TTL_Generic profile)
{

    if (profile.port == UartPort_UART2)
    {
        Serial2.begin(profile.baudrate);
        while (!Serial2)
        { // wait until it's ready
            ;
        }
        return true;
    }
    else if (profile.port == UartPort_UART3)
    {
        Serial3.begin(profile.baudrate);
        while (!Serial3)
        { // wait until it's ready
            ;
        }
        return true;
    }
    else
        return false;
}

/**************************************************************************/
/*!
    Send received command to the serial output (USB-C: UART2 or UART3).
    Send ACK feedback with profile_id
    Start event-listening for uArm response
    TODO: add header + tail defined in registration
*/
void run_uart_ttl_generic(uint32_t profile_id, A_UART_TTL_Generic action)
{
    /* get corresponding port for profile to select correct Serial port */
    UartPort port = profile_manager.profiles[profile_id].driver.r_uart_ttl_generic.port;
    HardwareSerial *Serialref = NULL;
    if (port == UartPort_UART2)
        Serialref = &Serial2;
    else
        Serialref = &Serial3;

    /* send received command to the serial output (USB-C: UART2 or UART3) */
    Serialref->write(action.command);
    Serialref->write("\n");

    /* non-blocking action => send ACK */
    if (action.event_triggered)
    {
        // set event flag for profile to true => starts event listening
        profile_manager.events[profile_id] = true;
        /* Send ACK feedback with profile_id */
        send_ack(profile_id);
    }
    /* blocking action => send directly response DATA*/
    else
    {
        /* wait until response is available */
        while (!Serialref->available())
            ;
        // receive feedback and send to gateway as DATA message
        feedback_handler(profile_id, Serialref);
    }
}

/**************************************************************************/
/*!
    Handle UART TTL events:
    Listen on Serial2/3 if response as arrived => send response to gateway
*/
bool event_uart_ttl_generic(uint32_t profile_id)
{
    /* get corresponding port for profile to select correct Serial port */
    UartPort port = profile_manager.profiles[profile_id].driver.r_uart_ttl_generic.port;
    HardwareSerial *Serialref = NULL;
    if (port == UartPort_UART2)
        Serialref = &Serial2;
    else
        Serialref = &Serial3;

    // if serial buffer is filled
    if (Serialref->available())
    {
        // receive feedback and send to gateway as DATA message
        feedback_handler(profile_id, Serialref);
        // set event flag for profile to false => stop event listening
        profile_manager.events[profile_id] = false;
        return true;
    }
    /* event did not occure */
    else
        return false;
}

/**************************************************************************/
/*!
    Handle feedback messages:
    Receive data on serial port and send DATA to the gateway
*/
void feedback_handler(uint32_t profile_id, HardwareSerial *Serialref)
{
    // response char array (max. length same as command defined in .proto file)
    char response[40] = {0};

    uint8_t response_index = 0;

    /* Process serial data */
    while (true)
    {
        response[response_index] = Serialref->read();
        response_index++;
        /* handle overflow of index */
        if (response_index > 40)
        {
            send_error(profile_id, "Overflow Error: Response index is bigger than max. response size.");
            break;
        }
        // break if terminating char is received
        if (response[response_index - 1] == '\n')
            break;
        // wait until next char is available
        while (!Serialref->available())
            ;
    }

    // send received message to the gateway
    send_data(profile_id, response, response_index);
}
