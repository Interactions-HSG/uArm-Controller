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
    
    // get pin from corresponding profile
    uint8_t pin = registered_profiles[profile_id].driver.r_digital_generic.pin;
    //send debug msg
    send_msg(pin);
    // maybe add dynamic mode (INPUT/OUTPUT) changes
    digitalWrite(pin, (uint8_t) action.output);
    
}