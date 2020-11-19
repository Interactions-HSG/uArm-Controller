#include "mcu_driver.h"

/*========================================================================*/
/*                    PRIVATE DEFINITIONS                                 */
/*========================================================================*/

// Controller firmware version
#define VERSION "v1.0"

/*========================================================================*/
/*                          FUNCTION DEFINITIONS                          */
/*========================================================================*/

/**************************************************************************/
/*!
    TODO: Description of initialization function for mcu_driver
*/
bool init_mcu_driver(uint32_t profile_id, R_MCU_Driver profile)
{
    // nothing to do for now
    return true;
}

/**************************************************************************/
/*!
    Action handler for MCU driver: following actions are possible:
        - Version: return firmware version
        - RAM: get current RAM usage 
        - RESET: reset the MCU => not implemented yet TODO:
*/
void run_mcu_driver(uint32_t profile_id, A_MCU_Driver action)
{
    uint16_t free_ram = 0;
    byte data[4] = {0};

    switch (action.mcu_action)
    {
    case MCUAction_VERSION:
        send_data(profile_id, (char *)VERSION, strlen(VERSION));
        break;

    case MCUAction_RAM:
        extern int __heap_start, *__brkval;
        int v;
        // measure free ram TODO: check if correct
        free_ram = (uint16_t)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);

        /* ensure no NULL byte is sent => use msb as flag*/
        data[0] = ((byte)free_ram) | B10000000;
        data[1] = ((byte)(free_ram >> 7)) | B10000000;
        send_data(profile_id, data, 2);
        break;

    case MCUAction_RESET:
        /* TODO: code */
        break;

    default:
        break;
    }
}
