/** @file Screen.hpp
*
* @brief Define a Screen class to handle display 
         related functionality and APIs 
*
* 
*/

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Arduino.h>
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

// Singleton class representing the screen display
//
class Screen 
{
public:
    // Singleton class related functionality 
    //
    static Screen *get_instance();
    void release_instance();
    
    // Print functionlity APIs based on I2C display 
    //
    void print_line(uint8_t , uint8_t , const char *);
    void print_access_granted(const String& , const String& );
    void print_access_granted(const String&, const String& , const String& );
    void print_access_denied();
    void print_idle_state();
    void print_register_user_state(const String& );
    void display_menu_page(uint8_t );
    void clear_screen();
    void display_menu_page_console();
    
private:
    
    // Privatizaiton of the construtor and destructor for the Singleton class
    //
    Screen();
    ~Screen();
    Screen(const Screen &) = delete;
    Screen &operator=(const Screen &) = delete;

    // Sigleton class instance
    //
    static Screen *instance;
    
    // I2C display object
    //
    LiquidCrystal_I2C lcd;
};

#endif  // End of SCREEN_HPP
