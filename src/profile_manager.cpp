/**************************************************************************/
/*!
    @file     profile_manager.cpp
    @author   Jonas Brütsch

    Contains all tasks related to the management of registered profiles.

    Following main tasks are included:
        - store all registered profiles in volatile registrations array
        - save/update registrations on SD card for backup
        - manage configuration list/(class) for physical MCU-port management 
*/
/**************************************************************************/
#include <profile_manager.h>

/*========================================================================*/
/*                  DEFINITIONS                                           */
/*========================================================================*/
// initialize all profile_ids with 0 TODO: check if initialization is proper
// index corresponds to profile_id
Registration registered_profiles[256] = {0};

/*========================================================================*/
/*                  FUNCTION DEFINITIONS                                  */
/*========================================================================*/

/**************************************************************************/
/*
    Store profile in registered profiles list
*/
void store_profile(Registration registration)
{

    // store profile to registered_profiles
    registered_profiles[registration.profile_id] = registration;
    // TODO: update profile on SD card => add new profile
}

/**************************************************************************/
/*
    Delete corresponding profile field in registered profiles to avoid overlapping entries
*/
void delete_profile(uint8_t profile_id)
{
    // check if the Profile_ID is already registered:
    if (registered_profiles[profile_id].profile_id != (uint32_t)0)
    {
        // clear entry in registered profiles
        memset(&registered_profiles[profile_id], 0, sizeof(Registration));
        // TODO: update SD file => delete profile
    }
}
