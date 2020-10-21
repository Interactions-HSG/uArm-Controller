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
    TODO: add header + tail defined in registration
*/
void run_uart_ttl_generic(uint32_t profile_id, A_UART_TTL_Generic action)
{

    /* send received command to the serial output (USB-C: UART2 or UART3) */
    // for testing send message with command
    // the Gcode command field has a max. size of 40 char => plus additional 7 for 'DEBUG: '
    char str[47];
    snprintf(str, 47, "DEBUG: %s", action.command);
    send_feedback(profile_id, str);
}
