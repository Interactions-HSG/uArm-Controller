#ifndef _COLOR_SENSOR_H_
#define _COLOR_SENSOR_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for color_sensor driver
    @return boolean if initialization was successful or not
*/
bool init_color_sensor(uint32_t profile_id, R_Color_Sensor profile);

/**************************************************************************/
/*!
    @brief  Action function for color_sensor ddriver
*/
void run_color_sensor(uint32_t profile_id, A_Color_Sensor action);

#endif
