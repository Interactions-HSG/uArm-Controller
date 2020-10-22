#ifndef _PROFILE_MANAGER_H_
#define _PROFILE_MANAGER_H_

#include "main.h"

/*========================================================================*/
/*                          PUBLIC DEFINITIONS                            */
/*========================================================================*/

// Registration array to store all registered profiles
// Index == Profile_ID for efficient lookup
//IDEA: use first bit of 1-byte Profile_ID to indicate which controller => less space needed, easier for gateway
extern Registration registered_profiles[256]; 

/*========================================================================*/
/*                          PUBLIC FUNCTIONS                              */
/*========================================================================*/
/**************************************************************************/
/*!
    @brief  Store registration in registered_profiles + update SD card
*/
void store_profile(Registration registration);

/**************************************************************************/
/*!
    @brief  Delete profile with corresponding ID on registered_profiles and on SD card
*/
void delete_profile(uint8_t profile_id);

// function to initialize SD card
// function to re-initialize all stored profiles/registrations

#endif