#include "Admin.hpp"


/*!
* @brief Set the admin password.
* @param[in] admin_password string saving the admin password.
*/
void 
Admin::set_password(const String& admin_password) 
{
    password = admin_password;
}


/*!
* @brief Retrieve the password.
* @return The admin password.
*/
String 
Admin::get_password() const 
{
    return password;
}


/*!
* @brief  Authenticate the admin using pass (to be implemented based on the logic)
*/
void 
Admin::authenticate() 
{
    // TODO : Logic for authenticating the admin specific checks
}
