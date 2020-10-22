#ifndef _TEMPLATE_DRIVER_H_
#define _TEMPLATE_DRIVER_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for template_driver driver
    @return boolean if initialization was successful or not
*/
bool init_template_driver(uint32_t profile_id, R_Template_Driver profile);

/**************************************************************************/
/*!
    @brief  Action function for template_driver ddriver
*/
void run_template_driver(uint32_t profile_id, A_Template_Driver action);

#endif
