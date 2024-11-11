/** @file UserOperation.hpp
*
* @brief  This file defines the UserOperation class,
          a singleton responsible for managing user tasks 
          and handling a state machine for an user interface on an embedded 
          system (Arduino).
          It integrates user operaiton based on door control.
          
*
* 
*/

#ifndef USEROPERATION_HPP
#define USEROPERATION_HPP

#include "UserAccessControl.hpp"
#include "RFIDreader.hpp"
#include "Database.hpp"
#include "Screen.hpp"
#include "Door.hpp"
#include "AuthenticationService.hpp"

class UserOperation {

public:

    // Constructor to initialize most components except Door and AuthenticationService
    //
    UserOperation(UserAccessControl* , RFIDreader* , Database* , Screen*);

    // The function to run the user operation
    //
    void run();

    // Setter methods for Door and AuthenticationService (now static)
    //
    static void setDoor(Door* door_obj);
    static void setAuthenticationService(AuthenticationService* auth_service);

private:
    
    UserAccessControl* p_usr_acs_ctrl;
    RFIDreader* p_rfid;
    Database* p_db;
    Screen* p_screen;
    static AuthenticationService* p_auth;
    static Door* p_door;
    static String uid;
};

#endif // USEROPERATION_HPP
