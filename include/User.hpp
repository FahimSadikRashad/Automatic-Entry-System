/** @file User.hpp
*
* @brief As an individual person categorizes user type,
*        information about the user stored here
* 
*/

#ifndef USER_HPP
#define USER_HPP

#include "Person.hpp"
#include <Arduino.h>  

// Class representing a user
//
class User : public Person 
{
public:
    void set_rfid(const String& rfid);      // Set the RFID tag
    String get_rfid() const;                // Retrieve the RFID tag
    void authenticate();                    // Authenticate the user using RFID

private:
    String rfid_tag;                        // RFID tag of the user
};

#endif  // End of USER_HPP
