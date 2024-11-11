#include "AdminOperation.hpp"
#include "Screen.hpp"
#include "Database.hpp"
#include "AuthenticationService.hpp"
#include "RFIDreader.hpp"
#include "Door.hpp"

// Initialize static variables
//
AdminOperation* AdminOperation::instance = nullptr;
AdminOperation::State AdminOperation::currentState = WAIT_FOR_CLI;
String AdminOperation::username = "";
String AdminOperation::password = "";
bool AdminOperation::authenticated = false;
Database* db = nullptr;
Screen* screen = nullptr;


/*!
* @brief Singleton access method.
* @return The instance represent the singleton class.
*/
AdminOperation* 
AdminOperation::get_instance()
{
    if (instance == nullptr)
    {
        instance = new AdminOperation();
    }
    return instance;
}

// Set authentication service
/*!
* @brief  Set authentication service.
* @param[in] authService pointer to the global Authentication Service.
*/
void 
AdminOperation::set_authentication_service(AuthenticationService* authService)
{
    this->authService = authService;
}


/*!
* @brief Get authentication service.
* @return The Authentication Service object.
*/
AuthenticationService* 
AdminOperation::get_authentication_service()
{
    return this->authService;
}


/*!
* @brief Function to handle denied access.
*/
void 
AdminOperation::admin_access_denied()
{
    Serial.println("Access Denied!!!");
}


/*!
* @brief Function to handle granted access.
*/
void 
AdminOperation::admin_access_granted()
{
    db = Database::get_instance();
    String date = db->get_current_date();
    String time = db->get_current_time();

    screen = Screen::get_instance();
    screen->print_access_granted(date, time);
}


/*!
* @brief Function to authenticate admin.
* @param[in] admin_name string of admin name to authenticate.
* @param[in] admin_pass string of admin password to authenticate.
* @return The status if admin authenticated or not.
*/
bool 
AdminOperation::admin_auth(String admin_name, String admin_pass)
{
    // Use the stored AuthenticationService to authenticate
    //
    return authService->authenticate_admin(admin_name, admin_pass);
}


/*!
* @brief State transition when access is granted.
*/
void 
AdminOperation::admin_access_granted_state()
{
    db = Database::get_instance();
    String date = db->get_current_date();
    String time = db->get_current_time();

    screen = Screen::get_instance();
    screen->print_access_granted(date, time);
}


/*!
* @brief View users in the system.
*/
void 
AdminOperation::user_view()
{
    db = Database::get_instance();
    db->display_users();
}


/*!
* @brief  View admins in the system.
*/
void 
AdminOperation::admin_view()
{
    db = Database::get_instance();
    db->display_admins();
}


/*!
* @brief View logs in the system.
*/
void 
AdminOperation::view_log()
{
    db = Database::get_instance();
    db->display_user_logs();
}


/*!
* @brief Delete a user by empid.
* @param[in] empid string to the employee id.
*/
void 
AdminOperation::delete_user(String empid)
{
    db = Database::get_instance();
    db->delete_user(empid);
    
}


/*!
* @brief Register a new user.
* @param[in] name string to the user name.
* @param[in] uid RFID key to match against the user.
*/
void 
AdminOperation::register_user(String name, String uid)
{
    User user;
    user.set_name(name);
    user.set_rfid(uid);

    
    db = Database::get_instance();
    db->write_user(user);

    Serial.println("User Registered Successfully.");
}


/*!
* @brief Main run method with state machine handling all admin operations.
*/
void 
AdminOperation::run()
{
    switch (currentState) {
        case WAIT_FOR_CLI:
            if (Serial.available()) 
            {
                String cliInput = Serial.readStringUntil('\n');
                if (cliInput.equalsIgnoreCase("cli")) {
                    currentState = NAME_PROCESS;
                    Serial.println();
                    Serial.print("Enter Admin Username:");
                }
            }
            break;

        case NAME_PROCESS:
            if (Serial.available()) 
            {
                username = Serial.readStringUntil('\n');
                Serial.println(username);
                currentState = PASS_PROCESS;
                Serial.print("Enter Admin Password:");
            }
            break;

        case PASS_PROCESS:
            if (Serial.available()) 
            {
                password = Serial.readStringUntil('\n');
                Serial.println(password);
                currentState = AUTHENTICATE;
            }
            break;

        case AUTHENTICATE:
            // Use the stored AuthenticationService instance to authenticate
            Serial.println();
            if (admin_auth(username, password)) 
            {
                authenticated = true;
                admin_access_granted();
                currentState = SHOW_MENU;
            } 
            else 
            {
                authenticated = false;
                admin_access_denied();
                Serial.println("Exit. Enter CLI:");
                currentState = WAIT_FOR_CLI;
            }
            break;

       case DELELTE_USER:
            
            if (Serial.available())
            {
                String uid = Serial.readStringUntil('\n');
                Serial.println(uid);
                Serial.println();
                db = Database::get_instance();
                
                if(db->is_emp_present(uid) == false)
                {
                    Serial.println("Provide valid employee id");
                    Serial.println("Enter employee id :");
                    currentState = DELELTE_USER;
                }
                else
                {
                    delete_user(uid);
                    currentState = SHOW_MENU;
                } 
            }
            break;


        case SHOW_MENU:
            if (authenticated) 
            {

                Serial.println();
                Serial.println("1. View User");
                Serial.println("2. View Admin");
                Serial.println("3. View Log");
                Serial.println("4. Register User");
                Serial.println("5. Delete User");
                Serial.println("6. Exit");
                currentState = WAIT_OPTION;
            }
            break;
        case WAIT_INPUT:
            if (Serial.available()) 
            {
                String input = Serial.readStringUntil('\n');
                if(input[0]=='m')
                {
                    currentState = SHOW_MENU;
                }
                else 
                {
                    Serial.println("Invalid Input");
                    currentState = WAIT_INPUT;
                }
            }
            break;
        case WAIT_OPTION:
            if (Serial.available()) 
            {
                Serial.println();
                int option = Serial.parseInt();
                switch (option) {
                    case 1:
                        Serial.println();
                        user_view();
                        Serial.println();
                        Serial.println("Enter \"m\" to show the Menu");
                        currentState = WAIT_INPUT;
                        break;
                    case 2:
                        admin_view();
                        Serial.println();
                        Serial.println("Enter \"m\" to show the Menu");
                        currentState = WAIT_INPUT;
                        break;
                    case 3:
                        Serial.println();
                        view_log();
                        Serial.println();
                        Serial.println("Enter \"m\" to show menu");
                        currentState = WAIT_INPUT;
                        break;
                    case 4:
                        currentState = REGISTER_USER;
                        Serial.println("Provide valid username less than 16 character and only letters ");
                        Serial.print("Enter Employee Username:");
                        break;
                    case 5:
                        Serial.print("Enter employee id :");
                        currentState = DELELTE_USER;
                        break;
                    case 6:
                        authenticated = false;
                        Serial.println("Exit. Enter CLI:");
                        currentState = WAIT_FOR_CLI;
                        break;
                    default:
                        Serial.println("Invalid Option. Try Again:");
                        currentState = SHOW_MENU;
                        break;
                }
            }
            break;

        case REGISTER_USER:
            if (Serial.available()) 
            {
                // TODO: check for null input for various cases in terminal.

                // NOTE : null input is checked and working based on serial input terminal handling.
                
                bool okay = true;
                username = "-1";

                username = Serial.readStringUntil('\n');
                Serial.println(username);

                if(username=="-1" || username.length()==0)
                {
                    Serial.println("Provide valid username without null");
                    okay=false;    
                }

                if(username.length()>=16)
                {
                    Serial.println("Provide valid username less than 16 character");
                    okay=false;
                }

                String check_string = username;
                check_string.toLowerCase();
                uint8_t n = check_string.length();
                
                // Serial.println(check_string);

                for(uint8_t i=0;i<n;i++)
                {
                    if(check_string[i]<'a' or check_string[i]>'z')
                    {
                        Serial.println("Provide valid username using letters");
                        okay=false;
                        break;
                        
                    }
                }

                if(okay == true)
                {
                    
                    Serial.print("Enter Employee ID:");
                    currentState = SAVE_USER;
                }
                else
                {
                    Serial.println();
                    Serial.print("Enter Employee Username:");
                    currentState = REGISTER_USER;
                }
            }
            break;

        case SAVE_USER:
            if (Serial.available()) 
            {
                bool okay = true;
                
                String uid = "-1";
                uid = Serial.readStringUntil('\n');
                Serial.println(uid);

                if(uid=="-1" || uid.length()==0)
                {
                    Serial.println("Provide valid employee id without null");
                    okay=false;    
                }
                

                if(username.length()>=16)
                {
                    Serial.println("Provide valid employee id less than 16 character");
                    okay=false;
                }

                String check_string = uid;
                uint8_t n = check_string.length();
                
                // Serial.println(check_string);

                for(uint8_t i=0;i<n;i++)
                {
                    if(check_string[i]<'1' or check_string[i]>'9')
                    {
                        Serial.println("Provide valid employee id using numbers");
                        okay=false;
                        break;
                        
                    }
                }

                if(db->is_emp_present(uid))
                {
                    Serial.println("Provide unique employee id ");
                    okay = false;
                }

                if(okay == true)
                {
                    Serial.println();
                    register_user(username, uid);
                    currentState = SHOW_MENU;
                }
                else
                {
                    Serial.print("Enter Employee ID:");
                    currentState = SAVE_USER;
                }
                
            }
            break;

        default:
            break;
    }
}
