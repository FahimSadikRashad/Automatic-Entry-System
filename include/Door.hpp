/** @file Door.hpp
*
* @brief Defines the Door class, which manages door operations for an embedded system (Arduino) 
         using a stepper motor. It includes states and
         methods for controlling the door's position and idle timing.
*
* 
*/

#ifndef DOOR_HPP
#define DOOR_HPP

#include <Arduino.h>
#include <Stepper.h>

const uint16_t max_steps_per_revolution = 1024;  // Number of steps per revolution
const uint32_t idle_time = 5000;                 // Idle time in milliseconds (5 seconds)

// Enum for door state
//
enum door_state 
{
    OPENING,
    IDLE,
    CLOSING,
    CLOSED
};

// Door class to handle door operations
//
class Door {

private:

    door_state doorstate = CLOSED;      // Initialize door state to CLOSED
    uint32_t start_time = 0;            // Start time for the idle state
    int current_position = 0;           // Track current position of the door
    
    Stepper myStepper = Stepper(max_steps_per_revolution, 6, 8, 7, 9);

public:
    
    // Door APIs for manupulating the motor
    //
    void setup();
    void run();
    void open();
    void close();
};

#endif // DOOR_HPP
