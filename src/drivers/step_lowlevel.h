#ifndef _STEP_LOWLEVEL_H_
#define _STEP_LOWLEVEL_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for step_lowlevel driver
    @return boolean if initialization was successful or not
*/
bool init_step_lowlevel(uint32_t profile_id, R_Step_Lowlevel profile);

/**************************************************************************/
/*!
    @brief  Action function for step_lowlevel ddriver
*/
void run_step_lowlevel(uint32_t profile_id, A_Step_Lowlevel action);

#endif
