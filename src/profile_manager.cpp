/**************************************************************************/
/*!
    @file     profile_manager.cpp
    @author   Jonas Brütsch

    Contains all tasks related to the management of registered profiles.

    Following main tasks are included:
        - register: store all registered profiles in volatile registrations array
        - save/update registrations on SD card for backup => not implemented yet!
        - delete: delete the entry of a specific profile in the registered profile array
*/
/**************************************************************************/
#include <profile_manager.h>

ProfileManager::ProfileManager(void)
{
    // initialize all profiles
    for (int profile_id = 0; profile_id < 256; profile_id++)
    {
        memset(&profiles[profile_id], 0, sizeof(Registration));
        events[profile_id] = false;
    }
}

// Store profile in registered profiles list
void ProfileManager::register_profile(Registration registration)
{
    // store profile to registered_profiles
    profiles[registration.profile_id] = registration;
    // TODO: update profile on SD card => add new profile
}

// Delete corresponding profile field in registered profiles to avoid overlapping entries
void ProfileManager::delete_profile(uint8_t profile_id)
{
    // check if the Profile_ID is already registered:
    if (profiles[profile_id].profile_id != (uint32_t)0)
    {
        // clear entry in registered profiles
        memset(&profiles[profile_id], 0, sizeof(Registration));
        // TODO: update SD file => delete profile
    }
}
