/** @file AdminOperation.hpp
*
* @brief  This file defines the AdminOperation class,
          a singleton responsible for managing administrative tasks 
          and handling a state machine for an admin interface on an embedded 
          system (Arduino).
          It integrates authentication, user management,
          and menu-driven interactions within a secure environment.
*
* 
*/

#ifndef ADMIN_OPERATION_HPP
#define ADMIN_OPERATION_HPP

#include <Arduino.h>
#include "Screen.hpp"
#include "Database.hpp"
#include "AuthenticationService.hpp"

class AdminOperation
{

private:
    
    static AdminOperation* instance;            // Singleton instance
    AuthenticationService* authService;         // Pointer to AuthenticationService

    
    AdminOperation() : authService(nullptr) {}  // Constructor

public:
    
    // Enum representing the state machine states
    //
    enum State 
    {
        WAIT_FOR_CLI,
        NAME_PROCESS,
        PASS_PROCESS,
        AUTHENTICATE,
        SHOW_MENU,
        WAIT_OPTION,
        VIEW_USER,
        VIEW_ADMIN,
        VIEW_LOG,
        WAIT_INPUT,
        REGISTER_USER,
        SAVE_USER,
        DELELTE_USER,
        MAIN_MENU
    };

    // Static variables for state, username, password, and authentication status
    //
    static State    currentState;
    static String   username;
    static String   password;
    static bool     authenticated;

    // Singleton access method
    //
    static AdminOperation* get_instance();

    // Main run method to handle state transitions and operations
    //
    void run();

    // Additional functionalities
    //
    void admin_access_denied();
    void admin_access_granted();
    bool admin_auth(String admin_name, String admin_pass);
    void admin_access_granted_state();
    void user_view();
    void admin_view();
    void view_log();
    void delete_user(String empid);
    void register_user(String name, String uid);

    // Set and get authentication service
    //
    void set_authentication_service(AuthenticationService* authService);
    AuthenticationService* get_authentication_service();
};

#endif // ADMIN_OPERATION_HPP
