#include "Screen.hpp"

// Initialize the static instance pointer to nullptr
//
Screen *Screen::instance = nullptr;


/*!
* @brief Constructor of the Screen class, setting the lcd display.
*/
Screen::Screen() : lcd(0x27, 16, 2) 
{
    lcd.init();         
    lcd.backlight();    
}

/*!
* @brief Destructor of the Screen class.
*/
Screen::~Screen() {}

/*!
* @brief Function to get the sigleton Screen class.
* @return The instance of the sigleton Screen class.
*/
Screen *
Screen::get_instance() 
{
    if (instance == nullptr) 
    {
        instance = new Screen();
    }
    return instance;
}

/*!
* @brief Function to release the instance.
*/
void 
Screen::release_instance() 
{
    if (instance != nullptr) 
    {
        delete instance;
        instance = nullptr;
    }
}

// TODO : change it to terminal only or delete the method

/*!
* @brief Function to print the line in the terminal.
* @param[in] col uint8_t of col of the lcd display.
* @param[in] row uint8_t of row of the lcd display.
* @param[in] content const char * the content to display.
*/
void 
Screen::print_line(uint8_t col, uint8_t row, const char *content) 
{
    Serial.println(content);
}

/*!
* @brief Function to print granted message to the lcd display.
* @param[in] date const String& which date to show in the lcd display.
* @param[in] time const String& which time to show in the lcd display.
*/
void 
Screen::print_access_granted(const String& date, const String& time) 
{
    String display_content;

    // Only update the screen when content changes
    //
    display_content = "Access Granted";
    lcd.setCursor(0,0);
    lcd.print( display_content.c_str());

    // Use millis() to create non-blocking delays
    //
    unsigned long startMillis = millis();
    while (millis() - startMillis < 1000);  // Wait for 1 second (non-blocking)

    display_content = "Date: " + date;
    lcd.setCursor(0,0);
    lcd.print( display_content.c_str());
    display_content = "Time: " + time;
    lcd.setCursor(0,1);
    lcd.print(display_content.c_str());
    
    startMillis = millis();
    while (millis() - startMillis < 1000);  // Wait for 1 second (non-blocking)
}

/*!
* @brief Function to print denied message to the lcd display.
*/
void 
Screen::print_access_denied() 
{
    clear_screen();
    lcd.setCursor(0,0);
    lcd.print("Access Denied !!!");
    lcd.setCursor(5,1);
    lcd.print("Cloudly");
    delay(250);
}

/*!
* @brief Function to print ideal message to the lcd display.
*/
void 
Screen::print_idle_state() 
{
    clear_screen();
    lcd.setCursor(0,0);
    lcd.print("Scan Your Card>>");
    lcd.setCursor(5,1);
    lcd.print("Cloudly");
}

/*!
* @brief Function to print the user name message to the lcd display, when a user registered.
*/
void 
Screen::print_register_user_state(const String& name) 
{
    String display_content = "Hello! " + name;
    print_line(0, 0, display_content.c_str());
    print_line(0, 1, "Scan Your Card>>");
}

/*!
* @brief Function to print the menu to the lcd display, when blocking state is performed.
*/
void 
Screen::display_menu_page(uint8_t page) 
{
    // Clear screen only when the page changes
    //
    static uint8_t lastPage = 0;
    if (lastPage != page) 
    {
        lcd.clear();
        lastPage = page;
    }

    if (page == 1) 
    {
        print_line(0, 0, "1: View Logs");
        print_line(0, 1, "2: View Users");
    } 
    else if (page == 2) 
    {
        print_line(0, 0, "3: View Admin");
        print_line(0, 1, "4: Reg. User");
    } 
    else if (page == 3) 
    {
        print_line(0, 0, "5: Exit");
    }
}

/*!
* @brief Function to print the menu to the terminal.
*/
void 
Screen::display_menu_page_console()
{
    
    
    Serial.println("1: View Logs");
    Serial.println("2: View Users");
    
    Serial.println( "3: View Admin");
    Serial.println( "4: Reg. User");
    
    Serial.println( "5: Exit");
    

}

/*!
* @brief Function to clear the lcd display.
*/
void 
Screen::clear_screen()
{
    lcd.clear();
}

/*!
* @brief Function to print granted message to the lcd display (overloaded).
* @param[in] name const String& which name to show in the lcd display.
* @param[in] date const String& which date to show in the lcd display.
* @param[in] time const String& which time to show in the lcd display.
*/
void 
Screen::print_access_granted(const String &name,const String& date, const String& time)
{
    String display_content;

    // Only update the screen when content changes
    //
    display_content = "Access Granted";
    lcd.setCursor(0,0);
    lcd.print( display_content.c_str());
    display_content = "Cloudly";
    lcd.setCursor(5,1);
    lcd.print(display_content);

    // Use millis() to create non-blocking delays
    //
    unsigned long startMillis = millis();
    while (millis() - startMillis < 1000);  // Wait for 1 second (non-blocking)

    clear_screen();
    String modified_name = name.substring(0,name.indexOf(','));
    display_content = modified_name;
    lcd.setCursor(0,0);
    lcd.print( display_content.c_str());
    String modified_time = time.substring(0,5);
    String modified_date = date.substring(0,4);
    display_content = "T/D: " + modified_time +" " + modified_date;
    lcd.setCursor(0,1);
    lcd.print(display_content.c_str());
    
    startMillis = millis();
    while (millis() - startMillis < 1000);  // Wait for 1 second (non-blocking)
}
