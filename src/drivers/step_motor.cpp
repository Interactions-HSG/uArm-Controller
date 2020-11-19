/**************************************************************************/
/*!
    @file     step_motor.cpp
    @author   Jonas Brütsch

    Driver for the spepping motors: belt conveyor and slider.

    TODO: Code was not tested with the corresponding hardware => probably not working!
    TODO: Event handling is done with an ISR using timer4 => differently than other event handling!
    => check advantages with using interrupts.

*/
/**************************************************************************/

#include "step_motor.h"
#include "helper_files/step_lowlevel.h"

/*========================================================================*/
/*                    PRIVATE DEFINITIONS                                 */
/*========================================================================*/

// define Motor Port => only one supported
#define STEP_PWM 7
#define MS3 A12
#define MS2 A11
#define MS1 A10

// can only be used if just one motor is supported per controller => change in the future
static uint32_t static_profile_id;

void response_callback();

/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    Initialization function for step_motor
    For now, no registration data has to be sent (in the future multiple ports may be supported).
*/
bool init_step_motor(uint32_t profile_id, R_Step_Motor profile)
{
    static_profile_id = profile_id;

    /* Init Belt */
    pinMode(MS3, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS1, OUTPUT);
    pinMode(STEP_PWM, OUTPUT);
    digitalWrite(MS1, LOW);
    digitalWrite(MS2, HIGH);
    digitalWrite(MS3, LOW);
    digitalWrite(STEP_PWM, HIGH);
    // Init the stepper driver in step_lowlevel
    return step_init_ll();
}

/**************************************************************************/
/*!
    Action function for step_motor:
        - Possible actions are: set speed or set steps
        - Events are handled by an ISR with timer4 (see .\helper_files\step_lowlevel.cpp)

*/
void run_step_motor(uint32_t profile_id, A_Step_Motor action)
{
    /* handle action to set the speed */
    if (action.which_mode == A_Step_Motor_direction_tag)
    {
        set_speed((int8_t)action.mode.direction, action.time_min_val, &response_callback, action.wait);
        if (!action.wait)
            send_ack(profile_id);
    }
    /* handle action to set the steps */
    else if (action.which_mode == A_Step_Motor_steps_tag)
    {
        set_steps((int8_t)action.mode.steps, action.time_min_val, &response_callback, action.wait);
        if (!action.wait)
            send_ack(profile_id);
    }
    else
        send_error(profile_id, "Step Motor: One of speed or steps has to be selected!");
}

/**************************************************************************/
/*!
    Handles callbacks of the step_lowlevel functions:
        - For simplicity send just an empty DATA msg (to be adapted in the future).
        - As the callback function has no
*/
void response_callback()
{
    // send received message to the gateway
    send_data(static_profile_id);
}
