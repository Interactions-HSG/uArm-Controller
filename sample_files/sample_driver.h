#ifndef _SAMPLE_DRIVER_H_
#define _SAMPLE_DRIVER_H_

#include "main.h"
/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for sample_driver
*/
void init_sample_driver(uint32_t profile_id, R_Sample_Driver profile);
/**************************************************************************/
/*!
    @brief  Action function for sample_driver 
*/
void run_sample_driver(uint32_t profile_id, A_Sample_Driver action);
#endif