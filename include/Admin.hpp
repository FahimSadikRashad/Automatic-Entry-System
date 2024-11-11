/** @file Admin.hpp
*
* @brief As an individual person categorizes admin type,
*        information about the admin stored here
* 
*/

#ifndef ADMIN_HPP
#define ADMIN_HPP

#include "Person.hpp"
#include <Arduino.h> 

// Class representing an admin
//
class Admin : public Person 
{

public:

    void    set_password(const String& admin_password);      // Set admin password
    String  get_password() const;                            // Retrieve admin password
    
    // TODO : Specific authenticaiton if required for admins
    
    void    authenticate();                                  // Authenticate admin credentials

private:

    String  password;                                       // Admin's password

};

#endif  // End of ADMIN_HPP
