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
    
    // get pin from corresponding profile
    uint8_t pin = registered_profiles[profile_id].driver.r_digital_generic.pin;

    // maybe add dynamic mode (INPUT/OUTPUT) changes
    digitalWrite((uint8_t) pin, (uint8_t) action.output);

    /* give feedback on action */
    send_feedback(profile_id, "Action on digital pin");
    
}
