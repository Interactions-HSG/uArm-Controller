#include <drivers/uart_ttl_generic.h>
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    TODO: Description of initialization function for uart_ttl_generic
*/
bool init_uart_ttl_generic(uint32_t profile_id, R_UART_TTL_Generic profile)
{
    if(profile.port == UART_Port_UART2){
        Serial2.begin(profile.baudrate);
        return true;
    }
    else if(profile.port == UART_Port_UART3){
        Serial3.begin(profile.baudrate);
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
void run_uart_ttl_generic(uint32_t profile_id, A_UART_TTL_Generic action){
    
    // send received command to the serial output (USB-C: UART2 or UART3)
    //for testing send message with command
    char str[37];
    snprintf(str,37,"DEBUG: %s",action.command);
    send_feedback(profile_id, str);

    
}