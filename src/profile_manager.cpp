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
// initialize all profile_ids with 0
Registration registered_profiles[256] = {0};

/*========================================================================*/
/*                  FUNCTION DEFINITIONS                                  */
/*========================================================================*/

/**************************************************************************/
/*
    TODO: Description
*/ 
void store_profile(Registration registration){

    // store profile to registered_profiles
    registered_profiles[registration.profile_id] = registration;
    // TODO: update profile on SD card => add new profile
    
}

/**************************************************************************/
/*
    TODO: Description
*/ 
void delete_profile(uint8_t profile_id){
    // check if the Profile_ID is already registered: 
    if(registered_profiles[profile_id].profile_id == (uint32_t) profile_id){
        // TODO: delete all old entries of config_list with Profile_ID
        // delete in registered_profiles => set profile_id = 0
        registered_profiles[profile_id].profile_id = 0;
        // TODO: update SD file => delete profile 
    }
}
