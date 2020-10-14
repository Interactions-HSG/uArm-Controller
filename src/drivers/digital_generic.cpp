#include <drivers/digital_generic.h>

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/

/**************************************************************************/
/*
    Initialization of digital pin
*/
void init_digital_generic(uint32_t profile_id, R_Digital_Generic profile)
{
    // initialize pin 
    pinMode((uint8_t) profile.pin, (uint8_t) profile.mode);
    // TODO: update confuguration list with new pin allocation (pin_x = profile_id)
    //       + call profile_manager::delete(profile_id_x) for all profiles using pin_x
    
}

/**************************************************************************/
/*
    Write/(read) digital pin
*/
void run_digital_generic(uint32_t profile_id, A_Digital_Generic action){
    
    // TODO: implement INPUT mode + pin should be taken from registrations vector
    // maybe add dynamic mode (INPUT/OUTPUT) changes
    digitalWrite((uint8_t) action.pin, (uint8_t) action.output);
    
}