#include "digital_generic.h"

/*========================================================================*/
/*                          PRIVATE VARIABLES                             */
/*========================================================================*/

/* Definitions used for event handling */
// indicates if corresponding profile has trigger on HIGH or LOW (default = true)
bool event_trigger[256] = {1};

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
    pinMode((uint8_t)profile.pin, (uint8_t)profile.mode);

    return true;
}

/**************************************************************************/
/*
    Action handling function for digital pin.

    Following actions are implemented:
        - write digital pin (HiGH/LOW)
        - read digital pin in blocking mode: return value on request
        - read digital pin in non-blocking mode: return value on event (HIGH/LOW/CHANGE?)
*/
void run_digital_generic(uint32_t profile_id, A_Digital_Generic action)
{
    // get registration profile
    R_Digital_Generic profile = registered_profiles[profile_id].driver.r_digital_generic;

    /* action: write digital pin */
    if (profile.mode == DigitalMode_OUTPUT)
    {
        digitalWrite((uint8_t)profile.pin, (uint8_t)action.output);
        /* give feedback on action */
        send_done(profile_id);
    }
    /* action: read digital pin in blocking mode */
    else if (profile.mode != DigitalMode_OUTPUT && !(action.event_triggered))
    {
        int result = digitalRead((uint8_t)profile.pin);
        send_data(profile_id, &result, 1);
    }
    /* action: read digital pin in non-blocking mode: start event listening */
    else if (profile.mode != DigitalMode_OUTPUT && action.event_triggered)
    {
        // set event flag for profile to true => starts event listening
        profiles_with_event[profile_id] = true;
        // set trigger for event
        event_trigger[profile_id] = action.output;
        // acknowledge start of event listening
        send_ack(profile_id);
    }
    /* ERROR */
    else
        send_error(profile_id, "Action for digital generic failed.");
}

/**************************************************************************/
/*!
    Handle digital generic events.
*/
bool event_digital_generic(uint32_t profile_id)
{
    int result = digitalRead((uint8_t)registered_profiles[profile_id].driver.r_digital_generic.pin);

    /* event occured: pin has trigger value */
    if (result == event_trigger[profile_id])
    {
        ++result; // to avoid empty byte field => cannot be parsed otherwise
        send_data(profile_id, &result, 1);
        send_debug("Event for digital pin occured!");
        // set event flag for profile to true => starts event listening
        profiles_with_event[profile_id] = false;
        return true;
    }
    /* event did not occure */
    else
        return false;
}
