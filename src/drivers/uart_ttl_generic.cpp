#include <drivers/uart_ttl_generic.h>
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    TODO: Description of initialization function for uart_ttl_generic
*/
void init_uart_ttl_generic(uint32_t profile_id, R_UART_TTL_Generic profile)
{
    if(profile.port == UART_Port_UART2){
        Serial2.begin(profile.baudrate);
    }
    else if(profile.port == UART_Port_UART3){
        Serial3.begin(profile.baudrate);
    }
    //else 
        //send_msg(404); // ERROR
}

/**************************************************************************/
/*!
    TODO: Description of initialization function for uart_ttl_generic
*/
void run_uart_ttl_generic(uint32_t profile_id, A_UART_TTL_Generic action){
    
    // TODO: implement action function
    
}