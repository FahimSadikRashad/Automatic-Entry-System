#include <Arduino.h>
#include "Database.hpp"
#include "AuthenticationService.hpp"
#include "RFIDreader.hpp"
#include "UserAccessControl.hpp"
#include "Screen.hpp"
#include "AdminOperation.hpp"
#include "Door.hpp"
#include "UserOperation.hpp"




Database *p_db;
RFIDreader *p_rfid;
Screen* p_screen;
AuthenticationService auth ;
UserAccessControl* p_usr_acs_ctrl;
AdminOperation* p_admin_op;
UserOperation* p_user_operation;
Door door;


const uint8_t buttonPin = 2;

bool button_pressed=false;

// ISR to open the door when button is pressed
//
static void openDoorISR() {
  
  // Change Button state in the pressed state
  //
  if(button_pressed == false)
  {
    button_pressed=true;
  }
  
}


void setup()
{
    Serial.begin(9600);
    
    // NOTE    : Delay is performed to setup the serial monitor
    
    // WARNING : If delay is not assingned then arduino will setup 
    //           twice 
    
    // REFER   : https://support.arduino.cc/hc/en-us/articles/4839084114460-If-your-board-runs-the-sketch-twice
    
    delay(2000);
    
    // Setup the buttion pin for ISR
    //
    pinMode(buttonPin, INPUT_PULLUP);  
    attachInterrupt(digitalPinToInterrupt(buttonPin), openDoorISR, RISING);

    // Setup the database
    //
    p_db = Database::get_instance();
    p_db->initSD(10);                     // CS pin for SD card
    p_db->initRTC();                      // Initialize RTC

    // Load admins and users from SD card
    //
    p_db->load_admins();
    p_db->load_users();
    p_db->load_rfid_user_log_data();

    // Setup the RFID reader
    //
    p_rfid = RFIDreader::get_instance();

    // Setup the Display
    //
    p_screen= Screen::get_instance();
    
    // Setup the User Access Control
    //
    p_usr_acs_ctrl= UserAccessControl ::get_instance();
    
    // Setup the Admin Operation
    //
    p_admin_op=AdminOperation::get_instance();

    // Setup the Admin Operation
    //
    p_admin_op->set_authentication_service(&auth);

    // NOTE    : Door is an alias for the motor

    // Setup the Door
    //
    door.setup();   
     
    // Initialise the User Operation
    //
    p_user_operation= new UserOperation(p_usr_acs_ctrl,p_rfid,p_db,p_screen);

    // NOTE    : Only one copy(auth and door) is used across the program 

    // Assign the Services
    //
    p_user_operation->setAuthenticationService(&auth);
    p_user_operation->setDoor(&door);

    // First Print to the terminal 
    //
    Serial.println("Enter CLI");
}


void loop() 
{
  
  // Start the User operation  
  //
  p_user_operation->run();

  // Start the Admin operation  
  //
  p_admin_op->run();

  // Check the button states
  //
  if(button_pressed)
  {
    
    // If button is pressed , open the Door
    //
    door.open();

    // Change back the Button state, initial state
    //
    button_pressed=false;
  }
  
  // According to the situaiton check perform door operations
  //
  door.run();

}
