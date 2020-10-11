#ifndef _DIGITAL_GENERIC_H_
#define _DIGITAL_GENERIC_H_

#include "main.h"

//=========== Public functions ===========//
void init_digital_generic(uint32_t profile_id, R_Digital_Generic profile);
void run_digital_generic(uint32_t profile_id, A_Digital_Generic action);
#endif