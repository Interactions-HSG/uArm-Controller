#ifndef _DIGITAL_GENERIC_H_
#define _DIGITAL_GENERIC_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Initialization function for generic driver for digital I/O
    @return boolean if initialization was successful or not
*/
bool init_digital_generic(uint32_t profile_id, R_Digital_Generic profile);

/**************************************************************************/
/*!
    @brief  Action function for generic driver for digital I/O
*/
void run_digital_generic(uint32_t profile_id, A_Digital_Generic action);
#endif
