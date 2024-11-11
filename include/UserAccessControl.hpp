/** @file UserAccessControl.hpp
*
* @brief  This file defines the UserAccessControl class,
          a singleton responsible for managing user tasks 
          and handling the access system that handles the 
          authentication part in part of Screen and RFID.
*
* 
*/

#ifndef USER_ACCESS_CONTROL_HPP
#define USER_ACCESS_CONTROL_HPP

#include <Arduino.h>
#include "Database.hpp"
#include "RFIDreader.hpp"
#include "Screen.hpp"
#include "AuthenticationService.hpp"

class UserAccessControl
{
public:
    static UserAccessControl* get_instance();  // Singleton pattern for global access
    
    // Function to access denied
    // 
    void access_denied(Screen *screen);

    // Function to set up date and time in the RFID reader
    //
    void set_up_date_time(Database *db, RFIDreader *rfid_reader);

    // Function to handle user access when authenticated
    //
    void user_access_granted(Database *db, RFIDreader *rfid_reader, Screen *screen);

    // Function to authenticate a user based on the RFID tag
    //
    bool user_auth_function(Database *db, RFIDreader *rfid_reader, Screen *screen ,AuthenticationService & auth);

private:
    
    UserAccessControl() {}               // Private constructor for singleton

    static UserAccessControl* instance;  // Static instance for singleton
};

#endif // USER_ACCESS_CONTROL_HPP
