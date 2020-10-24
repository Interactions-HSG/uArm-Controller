#ifndef _ULTRASONIC_SENSOR_H_
#define _ULTRASONIC_SENSOR_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for ultrasonic_sensor driver
    @return boolean if initialization was successful or not
*/
bool init_ultrasonic_sensor(uint32_t profile_id, R_Ultrasonic_Sensor profile);

/**************************************************************************/
/*!
    @brief  Action function for ultrasonic_sensor ddriver
*/
void run_ultrasonic_sensor(uint32_t profile_id, A_Ultrasonic_Sensor action);

#endif
