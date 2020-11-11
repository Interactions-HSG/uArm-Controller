#ifndef _UART_TTL_GENERIC_H_
#define _UART_TTL_GENERIC_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for uart_ttl_generic driver
    @return boolean if initialization was successful or not
*/
bool init_uart_ttl_generic(uint32_t profile_id, R_UART_TTL_Generic profile);

/**************************************************************************/
/*!
    @brief  Action function for uart_ttl_generic driver
*/
void run_uart_ttl_generic(uint32_t profile_id, A_UART_TTL_Generic action);

/**************************************************************************/
/*!
    @brief  Event function for uart_ttl_generic driver
    @return boolean if event for specific profile occured
*/
bool event_uart_ttl_generic(uint32_t profile_id);

#endif
