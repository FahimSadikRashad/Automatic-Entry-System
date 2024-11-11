#include "User.hpp"

/*!
* @brief Function to Set the RFID tag.
*/
void 
User::set_rfid(const String& rfid) 
{
    rfid_tag = rfid;
}


/*!
* @brief Function to Retrieve the RFID tag.
* @return The string to the stored rfid key.
*/
String 
User::get_rfid() const 
{
    return rfid_tag;
}

// TODO : implement user specific check during authentication.

/*!
* @brief Authenticate the user using RFID (to be implemented based on the logic).
*/
void User::authenticate() 
{
    // Logic for authenticating the user's RFID
}
