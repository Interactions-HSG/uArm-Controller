#ifndef _MCU_DRIVER_H_
#define _MCU_DRIVER_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for mcu_driver driver
    @return boolean if initialization was successful or not
*/
bool init_mcu_driver(uint32_t profile_id, R_MCU_Driver profile);

/**************************************************************************/
/*!
    @brief  Action function for mcu_driver driver
*/
void run_mcu_driver(uint32_t profile_id, A_MCU_Driver action);

#endif
