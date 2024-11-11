#include "UserAccessControl.hpp"

// Initialize the static instance
//
UserAccessControl* UserAccessControl::instance = nullptr;

/*!
* @brief Function to Singleton access method.
* @return The pointer to the Singleton class.
*/
UserAccessControl* 
UserAccessControl::get_instance()
{
    if (instance == nullptr)
    {
        instance = new UserAccessControl();
    }
    return instance;
}

/*!
* @brief Function to represent access denied state if rfid failed to match in user space.
* @param[in] screen Screen* of Screen class.
*/
void 
UserAccessControl::access_denied(Screen *screen)
{
    
    screen->print_access_denied();
}

/*!
* @brief Function to set up the date and time at the time of rfid is scanned.
* @param[in] db Database * of the databsae class.
* @param[in] rfid_reader RFIDreader * of the RFIDreader class.
*/
void 
UserAccessControl::set_up_date_time(Database *db, RFIDreader *rfid_reader)
{
    // Retrieve current date and time from the database
    //    
    String date = db->get_current_date();
    String time = db->get_current_time();

    // Set date and time in the RFID reader
    //
    rfid_reader->set_current_date(date);
    rfid_reader->set_current_time(time);
}

/*!
* @brief Function to controle the user access to the system.
* @param[in] db Database * of the databsae class.
* @param[in] rfid_reader RFIDreader * of the RFIDreader class.
* @param[in] screen Screen * of the Screen class.
*/
void 
UserAccessControl::user_access_granted(Database *db, RFIDreader *rfid_reader, Screen *screen)
{
    // Get RFID reader instance and tag details
    //
    String tag = rfid_reader->get_tag();
    String date_rfid = db->get_current_date();
    String time_rfid = db->get_current_time();

    
    db->log_rfid_scan(tag, date_rfid, time_rfid);

    
    screen->print_access_granted(tag, date_rfid, time_rfid);
}

/*!
* @brief Function to controle the user access to the system.
* @param[in] db Database * of the databsae class.
* @param[in] rfid_reader RFIDreader * of the RFIDreader class.
* @param[in] auth  AuthenticationService & of the AuthenticationService class.
* @return The status if user is present in the system or not.
*/
bool 
UserAccessControl::user_auth_function(Database *db, RFIDreader *rfid_reader, Screen *screen, AuthenticationService & auth)
{
    // Retrieve the RFID tag
    //
    String rfid = rfid_reader->get_tag();
    
    // Reset the tag in the RFID reader after reading
    //
    rfid_reader->set_tag("");

    // Authenticate the user based on the RFID tag
    //
    if (auth.authenticate_user(rfid))
    {
        // DEBUG
        //
        // If authentication is successful, transition to UserAccessGrantedState
        // Serial.println("Authenticated");
        // Replace this with the actual state transition logic in your code
        // set_state(new UserAccessGrantedState());
        
        return true;
    }
    else 
    {
        //DEBUG
        //
        // If authentication fails, transition to AccessDeniedState
        // Serial.println("Not Authenticated");
        // Replace this with the actual state transition logic in your code
        // set_state(new AccessDeniedState());

        return false;
    }
}
