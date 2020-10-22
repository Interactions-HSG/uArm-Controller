#include "uart_ttl_generic.h"

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
    /* SECTION ONLY USED FOR TESTING WITHOUT UARM ROBOTS*/
    // for testing send message with command
    // the Gcode command field has a max. size of 40 char => plus additional 7 for 'DEBUG: '
    char str[100];
    snprintf(str, 100, "Gcode: '%s' is sent for profile: %lu", action.command, profile_id);
    send_debug(str);
    send_ack(profile_id);

    /* REAL IMPLEMENTATION => USED WITH UARM ROBOTS */
    // get corresponding port for profile
    // UartPort port = registered_profiles[profile_id].driver.r_uart_ttl_generic.port;
    // /* send received command to the serial output (USB-C: UART2 or UART3) */
    // if (port == UartPort_UART2)
    // {
    //     /* send command to corresponding Serial port */
    //     Serial2.write(action.command);
    //     Serial2.write("\n");
    //     /* Send ACK feedback with profile_id */
    //     // TODO:
    // }
    // else if (port == UartPort_UART3)
    // {
    //     // send command to corresponding Serial port
    //     Serial3.write(action.command);
    //     Serial3.write("\n");
    // }
    // else
    //     ;
}

/**************************************************************************/
/*!
    Handle UART TTL events:
    Listen on Serial2/3 if response as arrived => send response to gateway
*/
bool event_uart_ttl_generic(uint32_t profile_id)
{
    // TODO:
    // get which port
    // see if serial buffer is filled
    // send response with code: DATA

    return false;
}
