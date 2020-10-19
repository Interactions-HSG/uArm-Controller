#include "digital_generic.h"

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*
    Initialization of digital pin
*/
bool init_digital_generic(uint32_t profile_id, R_Digital_Generic profile)
{
    // initialize pin 
    pinMode((uint8_t) profile.pin, (uint8_t) profile.mode);
    
    return true;
    
}

/**************************************************************************/
/*
    Write/(read) digital pin
*/
void run_digital_generic(uint32_t profile_id, A_Digital_Generic action){
    
    // TODO: implement INPUT mode + pin should be taken from registrations vector
    // maybe add dynamic mode (INPUT/OUTPUT) changes
    digitalWrite((uint8_t) action.pin, (uint8_t) action.output);

    /* give feedback on action */
    send_feedback(profile_id, "Action on digital pin");
    
}
