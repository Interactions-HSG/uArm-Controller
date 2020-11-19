#ifndef _STEP_MOTOR_H_
#define _STEP_MOTOR_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for step_motor driver
    @return boolean if initialization was successful or not
*/
bool init_step_motor(uint32_t profile_id, R_Step_Motor profile);

/**************************************************************************/
/*!
    @brief  Action function for step_motor ddriver
*/
void run_step_motor(uint32_t profile_id, A_Step_Motor action);

#endif
