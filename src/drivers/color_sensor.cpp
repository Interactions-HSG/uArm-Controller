#include "color_sensor.h"
#include "helper_files/Adafruit_TCS34725.h"

/*========================================================================*/
/*                          PRIVATE DEFINITIONS                           */
/*========================================================================*/

// decleration of color sensor class => for now only one is supported
// => TODO: adapt library to use multiple sensors with same address
Adafruit_TCS34725 tcs;

/* color variables */
uint16_t r;
uint16_t g;
uint16_t b;
// byte array to store one RGB value => R: data[0], G: data[1], B: data[2]
byte data[3];

/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    Initialization function for color_sensor
*/
bool init_color_sensor(uint32_t profile_id, R_Color_Sensor profile)
{
    r = 0;
    g = 0;
    b = 0;
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;

    tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_16X); //color sensors
    return tcs.begin();
}

/**************************************************************************/
/*!
    Action function for color_sensor
    TODO: implement event-handling => event on specific color (rgb) value
*/
void run_color_sensor(uint32_t profile_id, A_Color_Sensor action)
{
    /* read value */
    // TODO: check if /255 gives a valid resolution => why not use read8 ?
    r = tcs.read16(TCS34725_RDATAL) / 255; // => returns uint16_t value
    g = tcs.read16(TCS34725_GDATAL) / 255;
    b = tcs.read16(TCS34725_BDATAL) / 255;
    data[0] = (byte)constrain(r, 1, 255); // lower bound of 1 used for protobuf handling => no empty fields
    data[1] = (byte)constrain(b, 1, 255);
    data[2] = (byte)constrain(g, 1, 255);
    // data[0] = (byte)tcs.read8(TCS34725_RDATAL);
    // data[1] = (byte)tcs.read8(TCS34725_GDATAL);
    // data[2] = (byte)tcs.read8(TCS34725_BDATAL);

    send_data(profile_id, data, 3);
}
