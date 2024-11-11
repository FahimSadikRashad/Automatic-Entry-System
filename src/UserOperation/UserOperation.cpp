#include "UserOperation.hpp"
#include <Arduino.h>

// Static member definitions
//
Door* UserOperation::p_door = nullptr;
AuthenticationService* UserOperation::p_auth = nullptr;
String UserOperation::uid = "";

/*!
* @brief Constructor to initialize most components except Door and AuthenticationService.
* @param[in] usrAcsCtrl UserAccessControl* of the UserAccessControl class .
* @param[in] rfid RFIDreader* of the RFIDreader class.
* @param[in] screen Screen* of the Screen class.
*/
UserOperation::UserOperation(UserAccessControl* usrAcsCtrl, RFIDreader* rfid, Database* db, Screen* screen)
    : p_usr_acs_ctrl(usrAcsCtrl), p_rfid(rfid), p_db(db), p_screen(screen) {}


/*!
* @brief Function to setter for Door.
*/
void 
UserOperation::setDoor(Door* door_obj) 
{
    p_door = door_obj;
}

/*!
* @brief Function to setter for AuthenticationService.
*/
void 
UserOperation::setAuthenticationService(AuthenticationService* auth_service) 
{
    p_auth = auth_service;
}

/*!
* @brief Function to run the user operation.
*/
void 
UserOperation::run() 
{
    p_screen->print_idle_state();
    delay(50);
    
    // Check if a card scan is pending or already done
    //
    if (!p_rfid->get_is_scan_card()) 
    {
        uid = p_rfid->handleCardRead(); // Scan for a new card
    }
    else 
    {
        uid = p_rfid->get_tag();  // Retrieve the UID of the already scanned card

        //DEBUG
        //
        // Log the scanned UID
        // Serial.print("The UID scanned: ");
        // Serial.println(uid);    

        uid.trim();  // Remove any extra whitespace from the UID string

        
        // Check if the UID is valid
        //
        if (uid != "") 
        {
            // DEBUG
            //
            // Serial.println("Processing valid UID...");
            // Set up the date and time using the user access control

            // Check if the user is present in the database
            //
            bool is_present = p_usr_acs_ctrl->user_auth_function(p_db, p_rfid, p_screen, (*p_auth));
            if (is_present) 
            {
                // Grant access if the user is authenticated
                //
                p_usr_acs_ctrl->set_up_date_time(p_db, p_rfid);
                p_rfid->set_tag(uid.c_str());
                
                p_usr_acs_ctrl->user_access_granted(p_db, p_rfid, p_screen);
                p_rfid->remove_tag();
                if (p_door) 
                {
                    p_door->open();  // Open the door if it's set
                }

            }
            else 
            {
                // Deny access if the user is not authenticated
                //
                p_usr_acs_ctrl->access_denied(p_screen);
            }
        } 
        else 
        {
            //DEBUG
            //
            // Serial.println("Invalid UID, denying access...");
            // Deny access if the UID is invalid or empty
            
            p_usr_acs_ctrl->access_denied(p_screen);
        }

        // Reset for the next scan
        //
        uid = "";  // Clear the UID
        p_rfid->set_is_scan_card(false);  // Mark the scan as complete and reset
    }
}
