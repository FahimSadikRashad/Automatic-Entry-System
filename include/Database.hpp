/** @file Database.hpp
*
* @brief Defines the Database class, a singleton 
        responsible for managing data storage and 
        retrieval for users, admins, and 
        RFID log data in an embedded system (Arduino). 
        The class interfaces with an SD card, RTC module, 
        and maintains collections of user and 
        admin information to support authentication and logging functions.
*
* 
*/

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <SD.h>
#include <SPI.h>
#include <ArxContainer.h>
#include <Wire.h>
#include <RTClib.h>
#include <Arduino.h>
#include "Admin.hpp"
#include "User.hpp"

class Database 
{

public:

    // Singleton Usage Method
    //
    static Database *get_instance();
    void release_instance();

    // Initialization of SD card
    //
    void initSD(int);

    // Initialization of RTC module
    //
    void initRTC();
    
    // Admin Databse APIs
    //
    void load_admins();
    void update_admin_size(uint8_t);
    uint8_t get_admin_size();

    // User Databse APIs
    //
    void load_users();
    void update_user_size(uint8_t);
    uint8_t get_user_size();
    bool is_user_present(const String &);
    bool is_emp_present(const String &);
    void write_user(const User &);
    void delete_user(const String& );

    // RFID Databse APIs
    //
    void load_rfid_log_data();
    void load_rfid_user_log_data();
    void log_rfid_scan(const String & , const String &, const String &);

    
    // NOTE : Used to Debug values in the terminal,but later used in displaying in the terminal 

    // TODO : Separate functionality of displaying in other class
    
    // Display Database APIs
    //
    void print_working_hours();
    void display_logs();
    void display_users();
    void display_admins();
    void display_user_logs();
    
    // Access Methods for private data
    //
    const std::vector<Admin> & get_admins();
    const std::vector<User>  & get_users();

    std::map<String, String> const get_rfid_start_time();
    std::map<String, String> const get_rfid_end_time();
    
    // Time handling APIs using the RTC , provide support to other classes
    //
    String get_current_time();
    String get_current_date();

private:
    
    // For Singleton purpose 
    // 
    Database();     // Constructor
    ~Database();    // Destructor

    Database(const Database &) = delete;            // Constructor
    Database &operator=(const Database &) = delete; // Destructor

    // Singleton instance
    //
    static Database *instance;

    // RTC instance
    //
    RTC_DS3231 rtc;

    // Sizes of present Admins and Users
    //
    uint8_t admin_size;
    uint8_t user_size;

    // Warning : Check in the Database if the files are present or not 
    
    // Database files to be stored
    //
    const String admin_file         = "temp/admin.txt";
    const String user_file          = "temp/user.txt";
    const String rfid_log_file      = "temp/rfid_log.txt";
    const String person_size_file   = "temp/per_size.txt";

    // Variables regarding users and admins stored in runtime
    //
    static std::vector<Admin>       admins;
    static std::vector<User>        users;
    
    // Maps for the usage other classes
    //
    static std::map<String, bool>   rfid_map;
    static std::map<String, bool>   emp_map;
    static std::map<String, String> rfid_start_time;
    static std::map<String, String> rfid_end_time;
    
    // Delete functionality of user from the database
    //
    void delete_user_from_vector(const String & );
    void delete_user_database();
    void delete_user_from_rfid_map(const String &);
    void delete_user_from_emp_map(const String &);
    void delete_user_from_log_map(const String &);

    // Retrieve the User based on employee ID
    //
    User* get_key(const String &);

    // Update the map functionality accordingly
    //
    void update_start_and_end_time(const String &rfid, const String &date, const String &time);
    void update_rfid_map(const String &rfid);
    void update_emp_map(const String &emp);
    
    // TODO : Reduce the dependency between Database and RTC by separating these APIs

    // Helper functions for real time calculator
    //
    int time_to_seconds(const char *time_str);
    double calculate_working_hours(const char *start_time, const char *end_time);
    double calculate_working_minutes(const char *start_time, const char *end_time);

    // Helper function for Time Visualize
    //
    String convertToTimeFormat(double time);
    String addLeadingZeros(String time);
};

#endif  // DATABASE_HPP
