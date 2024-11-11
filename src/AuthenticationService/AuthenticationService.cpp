#include "AuthenticationService.hpp"

/*!
* @brief Constructor.
*/
AuthenticationService::AuthenticationService() {
    database = Database::get_instance();  // Initialize database instance
}


/*!
* @brief Destructor.
*/
AuthenticationService::~AuthenticationService() {
    
}


/*!
* @brief Function to authenticate a user by RFID key eg.(name,employee_Id).
* @param[in] rfid string of key to authenticate.
* @return The status if user authenticated or not.
*/
bool 
AuthenticationService::authenticate_user(const String &rfid) {
    
    if (database->is_user_present(rfid)) 
    {
        // DEBUG
        // Serial.println("User authenticated successfully.");
        
        return true;
    } 
    else 
    {
        // DEBUG
        // Serial.println("Authentication failed: User not found.");
        return false;
    }

}

/*!
* @brief Function to authenticate admin.
* @param[in] username string of admin name to authenticate.
* @param[in] pass string of admin password to authenticate.
* @return The status if admin authenticated or not.
*/
bool 
AuthenticationService::authenticate_admin(const String &username, const String &password) 
{       
    std::vector<Admin> admins=database->get_admins();  // Load admins from the database    
    
    // Loop through all admins and authenticate
    //
    for (uint8_t i = 0; i < admins.size(); i++) {
        if (admins[i].get_name() == username && admins[i].get_password() == password) {
            // database->release_instance();  // Release database instance
            Serial.println("Admin authenticated successfully.");
            return true;
        }
    }
    

    Serial.println("Authentication failed: Invalid credentials.");
    return false;

}
