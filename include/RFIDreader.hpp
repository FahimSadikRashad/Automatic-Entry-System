/** @file RFIDreader.hpp
*
* @brief Define a RFID reader class to handle rfid 
         scan related functionality and APIs 
*
* 
*/

#ifndef RFIDREADER_HPP
#define RFIDREADER_HPP

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Define RFID reader pins (make sure to define SS_PIN and RST_PIN in your code)
//
#define RST_PIN 5      // RST pin for RFID
#define SS_PIN 53      // Slave Select pin for RFID

class RFIDreader 
{

public:

    static RFIDreader *get_instance();    // Singleton pattern to get RFID reader instance
    void release_instance();              // Release RFID reader instance
    void set_tag(const char *tag);        // Set the RFID tag
    const char* get_tag();                // Retrieve the RFID tag
    void remove_tag();                    // Remove the RFID tag
    String scan_card();                   // Scan an RFID card

    void set_current_date(String&);  // Setter Current Date
    void set_current_time(String&);  // Setter Current Time

    String& get_current_date();      // Getter Current Date
    String& get_current_time();      // Setter Current Time
    String handleCardRead();         // Card read functionality
    
    void set_is_scan_card(bool);     // Setter for scan card
    bool get_is_scan_card();         // Getter for scan card

private:

    RFIDreader();                      // Private constructor
    ~RFIDreader();                     // Destructor

    RFIDreader(const RFIDreader &) = delete;             // Delete copy constructor
    RFIDreader &operator=(const RFIDreader &) = delete;  // Delete assignment operator

    void initializeRFID();
    
    // Helper functions
    //
    String readName(byte blockAddr);
    String readEmpID(byte blockAddr);

    static MFRC522 mfrc522;               // MFRC522 instance
    static MFRC522::MIFARE_Key key;       // MFRC522 key
    static RFIDreader *instance;          // Singleton instance
    static bool is_scan_card ;            // scanned or not 
    String rfid_tag;                      // RFID tag storage
    String current_date;                  // Current Date Storage
    String current_time;                  // Current Time Storage

};

#endif  // RFIDREADER_HPP
