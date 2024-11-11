#include "Database.hpp"

// Initialize the static RFID time maps
//
std::map<String, String> Database::rfid_start_time;
std::map<String, String> Database::rfid_end_time;
std::map<String, bool> Database::rfid_map;
std::map<String, bool> Database::emp_map;

// Initialize the Admins
//
std::vector<Admin> Database::admins;

// Initialize the Users
//
std::vector<User> Database::users;

// Initialize the static instance pointer to nullptr
//
Database *Database::instance = nullptr;

/*!
* @brief Function to get the max length of the name and password all the admins present.
* @param[in] admins vector of admins which will be checked.
* @param[in] maxNameLength int of max name length which will be passed.
* @param[in] maxPasswordLength int of max password length which will be passed.
*/
void 
getMaxLengths(const std::vector<Admin>& admins, int& maxNameLength, int& maxPasswordLength) 
{
    maxNameLength = 0;
    maxPasswordLength = 0;
    
    for (const auto& admin : admins) 
    {
        maxNameLength = max(maxNameLength, (int)admin.get_name().length());
        maxPasswordLength = max(maxPasswordLength, (int)admin.get_password().length());
    }
}

/*!
* @brief Function to get the max length of the name and password all the users present.
* @param[in] users vector of users which will be checked.
* @param[in] maxNameLength int of max name length which will be passed.
* @param[in] maxEmpIDLength int of max employee id length which will be passed.
*/
void 
getMaxLengthsUsers(const std::vector<User>& users, int& maxNameLength, int& maxEmpIDLength) 
{
    maxNameLength = 0;
    maxEmpIDLength = 0;
    
    for (const auto& user : users) 
    {
        maxNameLength = max(maxNameLength, (int)user.get_name().length());
        maxEmpIDLength = max(maxEmpIDLength, (int)user.get_rfid().length());
    }
}

/*!
* @brief Constructor.
*/
Database::Database() {}

/*!
* @brief Destructor.
*/
Database::~Database() {}

/*!
* @brief Function to get the Singleton Instance.
* @return The databse instance.
*/
Database *
Database::get_instance() 
{
    if (instance == nullptr) 
    {
        instance = new Database();
    }
    return instance;
}

/*!
* @brief Function to release the Singleton Instance.
*/
void Database::release_instance() 
{
    if (instance != nullptr) 
    {
        delete instance;
        instance = nullptr;
    }
}

/*!
* @brief Function to initialise the SD card module.
* @param[in] cs_in int to the CS pin in arduino board.
*/
void 
Database::initSD(int cs_pin) 
{
    if (!SD.begin(cs_pin)) 
    {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");
}

/*!
* @brief Function to initialise the RTC module.
*/
void 
Database::initRTC() 
{
    if (!rtc.begin()) 
    {
        Serial.println("Couldn't find RTC!");
        while (1);
    }
    if (rtc.lostPower()) 
    {
        Serial.println("RTC lost power, setting the time!");
        
        // Uncomment to set the time to the time when the sketch is compiled
        // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

/*!
* @brief Function to load the admins stored inside SD card module.
*/
void 
Database::load_admins() 
{
    File file = SD.open(admin_file.c_str());
    if (!file) 
    {
        Serial.println("Error: Could not open the admin file!");
        return;
    }
    while (file.available()) 
    {
        String line = file.readStringUntil('\n');
        int comma_pos = line.indexOf(',');
        if (comma_pos != -1) 
        {
            String name = line.substring(0, comma_pos);
            String password = line.substring(comma_pos + 1);

            // DEBUG
            //
            // Serial.print("Admin: ");
            // Serial.print(name);
            // Serial.print(", Password: ");
            // Serial.println(password);

            name.trim();
            password.trim();
            
            Admin admin;
            admin.set_name(name);
            admin.set_password(password);

            admins.push_back(admin);

        }
    }
    file.close();
}

/*!
* @brief Function to update the admins size when new admin is added in SD card module.
* @param[in] new_size uint8_t to the new size of the admins.
*/
void 
Database::update_admin_size(uint8_t new_size)
{
    File input_file = SD.open(person_size_file.c_str(), FILE_READ);

    if (!input_file) 
    {
        Serial.println("Error: Could not open the person size file!");
        return;
    }

    String updated_content = "";
    bool found = false;
    while (input_file.available()) 
    {
        String line = input_file.readStringUntil('\n');
        if (!found && line.startsWith("admin_size:")) 
        {
            line = "admin_size:" + String(new_size);
            found = true;
        }
        updated_content += line + "\n";
    }
    input_file.close();

    if (!found) 
    {
        Serial.println("Error: admin_size not found!");
        return;
    }

    File output_file = SD.open(person_size_file.c_str(), FILE_WRITE);
    output_file.print(updated_content);
    output_file.close();

    Serial.print("Admin size updated to: ");
    Serial.println(new_size);
}

/*!
* @brief Function to get the admin size.
* @return The admin size.
*/
uint8_t 
Database::get_admin_size()
{
    return admin_size;
}

/*!
* @brief Function to load the users stored inside SD card module.
*/
void 
Database::load_users() 
{
    
    File file = SD.open(user_file.c_str());
    
    if (!file) 
    {
        Serial.println("Error: Could not open the user file!");
        return;
    }
    
    while (file.available()) 
    {

        String line = file.readStringUntil('\n');
        int comma_pos = line.indexOf(',');
        if (comma_pos != -1) {
            String name = line.substring(0, comma_pos);
            String rfid = line.substring(comma_pos + 1);
            
            name.trim();
            rfid.trim();

            update_emp_map(rfid);

            User user;
            user.set_name(name);
            user.set_rfid(rfid);

            users.push_back(user);

            // DEBUB
            //
            // Serial.print("User: ");
            // Serial.print(name);
            // Serial.print(", RFID: ");
            // Serial.println(rfid);
           
            update_rfid_map(rfid);
            
        }
        line.trim();
        update_rfid_map(line);
    }
    
    file.close();

}

/*!
* @brief Function to update the users size when new user is added in SD card module.
* @param[in] new_size uint8_t to the new size of the users.
*/
void 
Database::update_user_size(uint8_t new_size)
{
    File input_file = SD.open(person_size_file.c_str(), FILE_READ);

    if (!input_file) 
    {
        Serial.println("Error: Could not open the person size file!");
        return;
    }

    String updated_content = "";
    bool found = false;
    while (input_file.available()) 
    {
        String line = input_file.readStringUntil('\n');
        if (!found && line.startsWith("user_size:")) 
        {
            line = "user_size:" + String(new_size);
            found = true;
        }
        updated_content += line + "\n";
    }
    input_file.close();

    if (!found) 
    {
        Serial.println("Error: user_size not found!");
        return;
    }

    File output_file = SD.open(person_size_file.c_str(), FILE_WRITE);
    output_file.print(updated_content);
    output_file.close();

    Serial.print("User size updated to: ");
    Serial.println(new_size);
}

/*!
* @brief Function to get the user size.
* @return The user size.
*/
uint8_t 
Database::get_user_size()
{
    return user_size;
}

/*!
* @brief Function to check if user is present based on key(name,uuid).
* @param[in] rfid string to the rfid present.
* @return The status if present or not.
*/
bool 
Database::is_user_present(const String &rfid)
{
    return rfid_map[rfid];
}

/*!
* @brief Function to check if employee is present based on key(name,employeeid) 
         which is embedded inside rfid card by separate program.
* @param[in] emp string to the rfid present.
* @return The status if present or not.
*/
bool 
Database::is_emp_present(const String &emp)
{
    return emp_map[emp];
}

/*!
* @brief Function to write the user in the system.
* @param[in] user User to the user which needs to be inserted.
*/
void 
Database::write_user(const User &user) 
{
    
    String name = user.get_name();
    String rfid = user.get_rfid();

    name.trim();
    rfid.trim();
    String key=name+","+rfid;
    update_emp_map(rfid);
    update_rfid_map(key);
    update_rfid_map(rfid);

    File file = SD.open(user_file.c_str(), FILE_WRITE);
    if (!file) 
    {
        Serial.println("Error: Could not open the user file!");
        return;
    }
    file.println(name + "," + rfid);
    file.close();

    users.push_back(user);
}

/*!
* @brief Function to log user access information.
* @param[in] rfid string of key(name,uuid) which is scanned from the rfid.
* @param[in] date string of current date.
* @param[in] time string of current time.
*/
void 
Database::log_rfid_scan(const String &rfid ,  const String &date, const String &time) 
{
    
    File log_file = SD.open(rfid_log_file.c_str(), FILE_WRITE);
    
    if (!log_file) 
    {
        Serial.println("Error: Could not open RFID log file!");
        return;
    }

    String current_date = date;
    String current_time = time;
    log_file.println(rfid + "," + current_date + "," + current_time);
    log_file.close();
    
    update_start_and_end_time(rfid, current_date, current_time);
    
    // DEBUG
    //
    // Serial.print("Logged RFID: ");
    // Serial.print(rfid);
    // Serial.print(", Date: ");
    // Serial.print(current_date);
    // Serial.print(", Time: ");
    // Serial.println(current_time);
}


/*!
* @brief Function to load the rfid access data stored inside SD card module.
*/
void 
Database::load_rfid_log_data() 
{
    File file = SD.open(rfid_log_file.c_str());

    if (!file) 
    {
        Serial.println("Error loading files");
        return;
    }
    
    String line;
    while (file.available()) 
    {
        line = file.readStringUntil('\n');
        int comma_pos = line.indexOf(',');
        String rfid = line.substring(0, comma_pos);
        String second_sep = line.substring(comma_pos + 1);

        comma_pos = second_sep.indexOf(',');
        String date = second_sep.substring(0, comma_pos);
        String time = second_sep.substring(comma_pos + 1);

        rfid.trim();
        date.trim();
        time.trim();
        
        update_start_and_end_time(rfid, date, time);
    }
    
    file.close();

}

/*!
* @brief Function to load the rfid access data stored inside SD card module changed key.
*/
void 
Database::load_rfid_user_log_data()
{
    File file = SD.open(rfid_log_file.c_str());

    if (!file) 
    {
        Serial.println("Error loading files");
        return;
    }
    
    String line;
    
    while (file.available()) 
    {
        
        line = file.readStringUntil('\n');
        int comma_pos = line.indexOf(',');
        String rfid = line.substring(0, comma_pos);
        String second_sep = line.substring(comma_pos + 1);
        
        rfid+=",";
        

        comma_pos = second_sep.indexOf(',');
        String id = second_sep.substring(0, comma_pos);
        
        id.trim();
        rfid+=id;

        String date = second_sep.substring(comma_pos + 1);
        
        comma_pos = date.indexOf(',');
        String time = date.substring(comma_pos + 1);
        date = date.substring(0,comma_pos);        
        
        rfid.trim();
        date.trim();
        time.trim();

        // DEBUG
        //    
        // Serial.print("Rfid = ");
        // Serial.println(rfid);

        // Serial.print("Date = ");
        // Serial.println(date);

        // Serial.print("Time = ");
        // Serial.println(time);
        
        update_start_and_end_time(rfid, date, time);
    }

    file.close();
}

/*!
* @brief Function to print the working hours stored inside map and calculated.
*/
void 
Database::print_working_hours() 
{
   
    Serial.println("Printing working hours:");
   
    for (const auto& entry : rfid_start_time) 
    {
        const String& key = entry.first;
        const String& start_time = entry.second;
        const String& end_time = rfid_end_time[key];

        double hours = calculate_working_hours(start_time.c_str(), end_time.c_str());
        double minutes = calculate_working_minutes(start_time.c_str(), end_time.c_str());

        Serial.print("ID: ");
        Serial.print(key.substring(0, key.indexOf(',')));
        Serial.print(" working hours: ");
        Serial.println(hours, 2);
        Serial.print("ID: ");
        Serial.print(key.substring(0, key.indexOf(',')));
        Serial.print(" working minutes: ");
        Serial.println(minutes, 2);
    }
}

/*!
* @brief Function to get all the admins present in the system.
* @return admins vector of all the admins.
*/
const 
std::vector<Admin> & Database::get_admins()
{
    return admins;
}

/*!
* @brief Function to get all the users present in the system.
* @return admins vector of all the users.
*/
const 
std::vector<User> & Database::get_users()
{
    return users;
}

/*!
* @brief Function to get the start time based on rfid log of the users.
* @return rfid_start_time map of start time when the user scan into the system.
*/
std::map<String, String>
const Database::get_rfid_start_time()
{
    return rfid_start_time;
}

/*!
* @brief Function to get the end time based on rfid log of the users.
* @return rfid_start_time map of end time when the user scan into the system.
*/
std::map<String, String> 
const Database::get_rfid_end_time()
{
    return rfid_end_time;
}

/*!
* @brief Function to update the map for start and end time based on logged in information.
* @param[in] rfid string of key(name,uuid) which is scanned from the rfid.
* @param[in] date string of current date.
* @param[in] time string of current time.
*/
void 
Database::update_start_and_end_time(const String &rfid, const String &date, const String &time) 
{
    String key = rfid + "," + date;
    
    if (rfid_start_time.find(key) == rfid_start_time.end()) 
    {
        rfid_start_time[key] = time;
    }
    
    rfid_end_time[key] = time;
}

/*!
* @brief Function to update the map rfid based on specific key.
* @param[in] rfid string of key(name,uuid) which is scanned from the rfid.
*/
void 
Database::update_rfid_map(const String &rfid)
{
    rfid_map[rfid]=true;
}

/*!
* @brief Function to update the map fo start and end time based on logged in information.
* @param[in] rfid string of key(name,empID) which is scanned from the rfid.
*/
void 
Database::update_emp_map(const String &emp)
{
    emp_map[emp]=true;
}

/*!
* @brief Function to get the current time.
* @return The current time based on rtc .
*/
String 
Database::get_current_time() 
{
    DateTime now = rtc.now();
    return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

/*!
* @brief Function to get the current date.
* @return The current date based on rtc .
*/
String 
Database::get_current_date() 
{
    DateTime now = rtc.now();
    return String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
}

/*!
* @brief Function to get the time in seconds.
* @param[in] time_str pointer to char which represent time in the format (hh/mm/ss).
* @return The parameter time in seconds.
*/
int 
Database::time_to_seconds(const char *time_str) 
{
    int hours = String(time_str).substring(0, 2).toInt();
    int minutes = String(time_str).substring(3, 5).toInt();
    int seconds = String(time_str).substring(6, 8).toInt();
    return hours * 3600 + minutes * 60 + seconds;
}

/*!
* @brief Function to calculate the working hours based on start and end time.
* @param[in] start_time pointer to char start time in the format (hh/mm/ss).
* @param[in] end_time pointer to char of end time in the format (hh/mm/ss).
* @return The working hour calculated using start and end time.
*/
double 
Database::calculate_working_hours(const char *start_time, const char *end_time) 
{
    int start_seconds = time_to_seconds(start_time);
    int end_seconds = time_to_seconds(end_time);
    return (end_seconds - start_seconds) / 3600.0;
}

/*!
* @brief Function to calculate the working minutes based on start and end time.
* @param[in] start_time pointer to char start time in the format (hh/mm/ss).
* @param[in] end_time pointer to char of end time in the format (hh/mm/ss).
* @return The working hour calculated using start and end time.
*/
double 
Database::calculate_working_minutes(const char *start_time, const char *end_time) 
{
    int start_seconds = time_to_seconds(start_time);
    int end_seconds = time_to_seconds(end_time);
    return (end_seconds - start_seconds) / 60.0;
}

/*!
* @brief Function to convert the time format as string.
* @param[in] time double to the actual time taken.
* @return The string represting the time(HH/MM).
*/
String 
Database::convertToTimeFormat(double time)
{
  int hours = (int) time;                    // Extract the hours (integer part)
  int minutes = (int)((time - hours) * 60);  // Convert the fractional part to minutes
  
  // Format hours and minutes to always be two digits
  //
  char buffer[6];          // HH:MM\0 (5 characters + null terminator)
  sprintf(buffer, "%02d:%02d", hours, minutes);
  
  return String(buffer);  // Return the formatted time as a String
}

/*!
* @brief Function to insert leading zeros if one digit number into the time format as string.
* @param[in] time double to the actual time taken.
* @return The string represting the time(HH/MM).
*/
String 
Database::addLeadingZeros(String time)
{

  String result = "";
  int lastIndex = 0;
  int index = 0;
  
  // Split the string based on colon ':'
  //
  while ((index = time.indexOf(':', lastIndex)) != -1) 
  {
    
    String part = time.substring(lastIndex, index); // Get the part before the colon
  
    if (part.length() == 1) 
    {
      result += "0" + part; // Add leading zero if single-digit
    } 
    else 
    {
      result += part;
    }
    
    result += ":"; // Add back the colon
    lastIndex = index + 1;
  }
  
  // Process the last part after the last colon
  //
  String part = time.substring(lastIndex);
  if (part.length() == 1) 
  {
    result += "0" + part;
  } 
  else 
  {
    result += part;
  }
  
  return result;
}

/*!
* @brief Function to display rfid logs inside the system in the format visualize in the terminal.
*/
void 
Database::display_logs() 
{
    // Create iterators for the start and end time maps
    //
    std::map<String, String>::const_iterator it_start = rfid_start_time.begin();
    std::map<String, String>::const_iterator it_end = rfid_end_time.begin();

    // Print the header
    //
    Serial.println("ID    Date        Start Time   End Time   Working mins");

    // Iterate through the maps and display the values
    //
    while (it_start != rfid_start_time.end() && it_end != rfid_end_time.end()) 
    {
        String start_time = it_start->second;
        String end_time = it_end->second;

        // Extract the date and RFID from the key
        //
        String full_key = it_start->first;
        String date = full_key.substring(full_key.indexOf(',') + 1);
        String rfid = full_key.substring(0, full_key.indexOf(','));

        // Calculate working minutes using the Database object
        //
        double working_minutes = calculate_working_minutes(start_time.c_str(), end_time.c_str());
    
        // Display the values
        //
        Serial.print(rfid);
        Serial.print("   ");
        Serial.print(date);
        Serial.print("   ");
        Serial.print(start_time);
        Serial.print("   ");
        Serial.print(end_time);
        Serial.print("   ");
        Serial.println(working_minutes);

        it_start++;
        it_end++;
    }
}

/*!
* @brief Function to display users in the format visualize in the terminal.
*/
void 
Database::display_users()
{
    // Get the maximum lengths
    //
    int maxNameLength = 0;
    int maxEmpIDLength = 0;
    getMaxLengthsUsers(users, maxNameLength, maxEmpIDLength);

    Serial.print("NAME");
    Serial.print("           ");
    Serial.println("EMP ID");
    Serial.println("----------------------------------");
    
    
    for (uint8_t i = 0; i < users.size(); i++) 
    {
        
        Serial.print(users[i].get_name());
        
        int nameSpaces = maxNameLength - users[i].get_name().length() + 10; // +10 for extra padding
        for (int j = 0; j < nameSpaces; j++) 
        {
            Serial.print(" ");
        }
        
        Serial.println(users[i].get_rfid());
    }
}

/*!
* @brief Function to display admins in the format visualize in the terminal.
*/
void 
Database::display_admins()
{
    // Get the maximum lengths
    //
    int maxNameLength = 0;
    int maxPasswordLength = 0;
    getMaxLengths(admins, maxNameLength, maxPasswordLength);

    Serial.print("Name");
    Serial.print("          ");
    Serial.println("Password");
    Serial.println("----------------------------------");

    for (uint8_t i = 0; i < admins.size(); i++) 
    {
        Serial.print(admins[i].get_name());
        
        // Calculate the spaces needed for consistent formatting
        //
        int nameSpaces = maxNameLength - admins[i].get_name().length() + 10; // +10 for extra padding
        for (int j = 0; j < nameSpaces; j++) {
            Serial.print(" ");
        }

        Serial.println(admins[i].get_password());
    }
}

/*!
* @brief Function to display user logs in different format in the format visualize in the terminal.
*/
void 
Database::display_user_logs()
{
    // Create iterators for the start and end time maps
    //
    std::map<String, String>::const_iterator it_start = rfid_start_time.begin();
    std::map<String, String>::const_iterator it_end = rfid_end_time.begin();

    // Display the headers
    //
    Serial.print("NAME");
    Serial.print("   ");
    Serial.print("EMPID");
    Serial.print("   ");
    Serial.print("DATE");
    Serial.print("     ");
    Serial.print("START TIME");
    Serial.print("  ");
    Serial.print("END TIME");

    // TODO : If admin wants the format to be in working hours
    //
    // Serial.print("  ");
    // Serial.print("WORKING MINS");
    
    Serial.print("  ");
    Serial.println("WORKING HOURS");
    
    Serial.println("--------------------------------------------------------");
    
    // Iterate through the maps and display the values
    //
    while (it_start != rfid_start_time.end() && it_end != rfid_end_time.end()) 
    {
        String start_time = it_start->second;
        String end_time = it_end->second;

        // Extract the date and RFID from the key
        //
        String full_key = it_start->first;
        String id = full_key.substring(full_key.indexOf(',') + 1);
        String date = id.substring(id.indexOf(',') + 1);
        String name = full_key.substring(0, full_key.indexOf(','));
        id = id.substring(0, id.indexOf(','));
        
        // Calculate working minutes
        // 
        double working_minutes = calculate_working_minutes(start_time.c_str(), end_time.c_str());

        // Calculate working hours
        // 
        double working_hours = calculate_working_hours(start_time.c_str(), end_time.c_str());


        // Display the values
        //
        Serial.print(name);
        Serial.print("   ");
        Serial.print(id);
        Serial.print("   ");
        Serial.print(date);
        Serial.print("   ");

        
        String modified_start_time = addLeadingZeros(start_time);
        String modified_end_time = addLeadingZeros(end_time);
        
        Serial.print(modified_start_time);
        Serial.print("   ");
        Serial.print(modified_end_time);
        Serial.print("   ");
        
        // TODO : If admins wants working minutes
        // 
        // Serial.println(working_minutes);
        // Serial.print("          ");
        // Serial.println(working_hours);
        
        String modified_working_hour = convertToTimeFormat(working_hours);

        Serial.println(modified_working_hour);

        it_start++;
        it_end++;
    }

}

/*!
* @brief Function to delete user from the vector using only employee id.
* @param[in] empid String  a key to the employee id which is embedded inside rfid.
*/
void 
Database::delete_user_from_vector(const String &empid)
{
    std::vector<User>::iterator it;

    for (it = users.begin(); it != users.end(); ) 
    {
        if (it->get_rfid() == empid) 
        {
            it = users.erase(it);  
        } 
        else 
        {
            ++it;  
        }
    }



}

/*!
* @brief Function to delete user from the database.
*/
void 
Database::delete_user_database()
{
    String filepath = "TEMP/user.txt";
    
    // Delete the existing file if it exists
    //
    if (SD.exists(filepath)) 
    {
        if (SD.remove(filepath)) 
        {
            // DEBUG 
            //
            // Serial.print("Deleted old file: ");
            // Serial.println(filepath);
        } 
        else 
        {
            Serial.print("Failed to delete file: ");
            Serial.println(filepath);
            return; 
        }
    } 
    else 
    {
        Serial.println("File does not exist, creating a new one.");
    }

    // Create a new file for users
    //
    File userFile = SD.open(filepath, FILE_WRITE);
    if (!userFile) 
    {
        Serial.println("Failed to create new user file.");
        return; // Exit if file creation fails
    }

    
    for (const auto& user : users) 
    { 
        userFile.println(user.get_name() + "," + user.get_rfid());
    }

    userFile.close();
    
    // DEBUG
    // 
    // Serial.print("Created new user file with users: ");
    // Serial.println(filepath);
}

/*!
* @brief Function to delete user from the rfid map using employee id key(name,id).
* @param[in] empid String  a key to the employee id which is embedded inside rfid.
*/
void 
Database::delete_user_from_rfid_map(const String &empid) 
{
    
    
    
    User* user = get_key(empid);
    
    

    if (user == nullptr) 
    {
        Serial.println("User not found.");
        return; // User not found, exit the function
    }

    // Construct keys
    //
    String key1 = user->get_name() + "," + user->get_rfid(); // Name, RFID
    String key2 = user->get_rfid(); // RFID

    // Delete from rfid_map
    //
    auto it1 = rfid_map.find(key1);
    if (it1 != rfid_map.end()) 
    {
        rfid_map.erase(it1); // Remove using key1
        
        //DEBUG
        //
        // Serial.print("Deleted from rfid_map using key1: ");
        // Serial.println(key1);

    }
    else 
    {
        Serial.print("Key1 not found in rfid_map: ");
        Serial.println(key1);
    }

    auto it2 = rfid_map.find(key2);
    if (it2 != rfid_map.end()) 
    {
        rfid_map.erase(it2); // Remove using key2
        
        // DEBUG
        //
        // Serial.print("Deleted from rfid_map using key2: ");
        // Serial.println(key2);

    } 
    else 
    {
        Serial.print("Key2 not found in rfid_map: ");
        Serial.println(key2);
    }
}

/*!
* @brief Function to delete user from the employee map using employee id key(name,id).
* @param[in] empid String  a key to the employee id which is embedded inside rfid.
*/
void 
Database::delete_user_from_emp_map(const String &empid) 
{
    User* user = get_key(empid);
    
    if (user == nullptr) 
    {
        Serial.println("User not found.");
        return; // User not found, exit the function
    }

    // Construct the key using the user's RFID
    //
    String key = user->get_rfid(); // Key: RFID

    // Delete from emp_map
    //
    auto it = emp_map.find(key);
    if (it != emp_map.end()) 
    {
        emp_map.erase(it); // Remove the entry using the key

        // DEBUG
        //  
        // Serial.print("Deleted from emp_map using key (RFID): ");
        // Serial.println(key);
    
    } 
    else 
    {
        Serial.print("Key not found in emp_map: ");
        Serial.println(key);
    }
}

/*!
* @brief Function to delete user from the map using employee id key(name,id).
* @param[in] empid const String  a key to the employee id which is embedded inside rfid.
*/
void 
Database::delete_user_from_log_map(const String &empid) 
{
    

    User* user = get_key(empid);
    
    
    
    if (user == nullptr) 
    {
        Serial.println("User not found.");
        return; // User not found, exit the function
    }

    // Construct the key using the user's name and RFID
    //
    String key = user->get_name() + "," + user->get_rfid(); // Key: name, RFID

    // Delete from rfid_start_time map
    //
    for (auto it = rfid_start_time.begin(); it != rfid_start_time.end();) 
    {
        if (it->first.indexOf(key) != -1) 
        { 
            // DEBUG
            //
            // Check if key is a substring of the map key
            // Serial.print("Deleting from rfid_start_time: ");
            // Serial.println(it->first);
      
            it = rfid_start_time.erase(it); // Remove entry and update iterator
        } 
        else 
        {
            ++it; // Move to the next entry
        }
    }

    // Delete from rfid_end_time map
    //
    for (auto it = rfid_end_time.begin(); it != rfid_end_time.end();) 
    {
        if (it->first.indexOf(key) != -1) 
        { 
            // Check if key is a substring of the map key
            // Serial.print("Deleting from rfid_end_time: ");
            // Serial.println(it->first);
            it = rfid_end_time.erase(it); // Remove entry and update iterator
        } 
        else 
        {
            ++it; // Move to the next entry
        }
    }
}

/*!
* @brief Function to delete user from the map using employee id key(name,id).
* @return The pointer to the User based on the employee id key.
*/
User* 
Database::get_key(const String & empid)
{
    std::vector<User>::iterator it;
    User* user=nullptr;
    for (it = users.begin(); it != users.end(); it++ ) 
    {
        if (it->get_rfid() == empid) 
        {
            user=&(*it);
            break;
        } 

    }
    return user;
}

/*!
* @brief Function to delete user from the system using employee id key(name,id).
*/
void Database::delete_user(const String& empid)
{
    delete_user_from_rfid_map(empid);
    delete_user_from_emp_map(empid);
    delete_user_from_log_map(empid);
    delete_user_from_vector(empid);
    delete_user_database();
}
