#ifndef _PROFILE_MANAGER_H_
#define _PROFILE_MANAGER_H_

#include "main.h"

class ProfileManager
{
public:
    ProfileManager();

    // Registration array to store information for all registered profiles
    Registration profiles[256];

    // array to store which profiles expect an event
    // => true means we expect an event for the corresponding profile
    bool events[256];

    void register_profile(Registration registration);
    void delete_profile(uint8_t profile_id);
    // function to initialize SD card
    // function to re-initialize all stored profiles/registrations
};

// instance of ProfileManager to handle all profiles
extern ProfileManager profile_manager;

#endif
