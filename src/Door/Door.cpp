#include "Door.hpp"

/*!
* @brief Function to initialize the stepper motor.
*/
void 
Door::setup() 
{
  myStepper.setSpeed(15);  // Set motor speed to 15 RPM
}


/*!
* @brief Function to open the door.
*/
void
 Door::open() 
{
  if (doorstate == CLOSED) 
  {
    doorstate = OPENING;
  }
}

/*!
* @brief Function to close the door.
*/
void 
Door::close() 
{
  if (doorstate == IDLE) 
  {
    doorstate = CLOSING;
  }
}

/*!
* @brief Function to run the door state machine.
*/
void 
Door::run() 
{
  switch (doorstate) 
  {
    case OPENING:
      myStepper.step(100);  // Open the door
      current_position += 100;  // Update the current position
      
      if (current_position >= max_steps_per_revolution) 
      {
        current_position = max_steps_per_revolution; // Set to max
        doorstate = IDLE;  // Move to idle state
        start_time = millis();  // Record start time for idle
      }
      break;

    case IDLE:
      // Check if idle time has passed
      //
      if (millis() - start_time >= idle_time) 
      {  
  
        close();  // Transition to closing state
      }
      break;

    case CLOSING:
      myStepper.step(-100);  // Close the door
      current_position -= 100;  // Update the current position
      if (current_position <= 0) 
      {
        current_position = 0;  // Reset position to 0
        doorstate = CLOSED;  // Move to closed state
      }
      break;

    case CLOSED:
      // Door is closed; no action needed
      break;
  }
}
