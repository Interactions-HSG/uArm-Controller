#include "ultrasonic_sensor.h"

/*========================================================================*/
/*                          PRIVATE DEFINITIONS                           */
/*========================================================================*/

// buffer used to store measurements (distance) in byte array
byte buf[2] = {0};

/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    Initialization function for ultrasonic_sensor
*/
bool init_ultrasonic_sensor(uint32_t profile_id, R_Ultrasonic_Sensor profile)
{
    // nothing to do for now
    return true;
}

/**************************************************************************/
/*!
    Action function for ultrasonic_sensor: measure distance in cm and send to gateway
*/
void run_ultrasonic_sensor(uint32_t profile_id, A_Ultrasonic_Sensor action)
{
    uint32_t _pin = profile_manager.profiles[profile_id].driver.r_digital_generic.pin;

    /* read distance from sensor */
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(_pin, LOW);
    pinMode(_pin, INPUT);

    /*The measured distance from the range 0 to 400 Centimeters*/
    unsigned long duration = pulseIn(_pin, HIGH);
    uint16_t range_in_centimeters = (duration / 29 / 2);

    /* ensure no NULL byte is sent => use msb as flag*/
    buf[0] = ((byte)range_in_centimeters) | B10000000;
    buf[1] = ((byte)(range_in_centimeters >> 7)) | B10000000;

    //send_debug((const char *)buf);
    send_data(profile_id, buf, 2);
}
