/** @file AuthenticationService.hpp
*
* @brief Defines the AuthenticationService class,
         responsible for authenticating users and
         admins within an embedded system (Arduino).
         This class interfaces with the Database to verify credentials and
         manage secure access for both user and admin functionalities.
*
* 
*/

#ifndef AUTHENTICATIONSERVICE_HPP
#define AUTHENTICATIONSERVICE_HPP

#include<Arduino.h>
#include "Database.hpp"

// Class handling authentication for users and admins
//
class AuthenticationService 
{

private:

    Database *database;          // Pointer to the database

public:

    AuthenticationService();     // Constructor
    ~AuthenticationService();    // Destructor

    // Authenticate a user
    //
    bool authenticate_user(const String &rfid);  

    // Authenticate an admin
    //
    bool authenticate_admin(const String &username, const String &password);  

};

#endif  // End of AUTHENTICATIONSERVICE_HPP
