#ifndef _TEMPLATE_DRIVER_H_
#define _TEMPLATE_DRIVER_H_

#include "main.h"
/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for template_driver
*/
void init_template_driver(uint32_t profile_id, R_Template_Driver profile);
/**************************************************************************/
/*!
    @brief  Action function for template_driver 
*/
void run_template_driver(uint32_t profile_id, A_Template_Driver action);
#endif