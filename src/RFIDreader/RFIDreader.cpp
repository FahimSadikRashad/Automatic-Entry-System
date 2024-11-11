#include "RFIDreader.hpp"

// Initialize the static instance pointer
//
RFIDreader *RFIDreader::instance = nullptr;  

// Initialize the scan card
//
bool RFIDreader::is_scan_card = false;

// Set up RFID reader (define SS_PIN and RST_PIN elsewhere)
//
MFRC522 RFIDreader::mfrc522(SS_PIN, RST_PIN);

// Set up RFID reader key
//
MFRC522::MIFARE_Key RFIDreader::key;


/*!
* @brief Private constructor.
*/
RFIDreader::RFIDreader() : rfid_tag("") 
{
    SPI.begin();         // Initialize SPI
    mfrc522.PCD_Init();  // Initialize RFID module
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF; // Default key
    }
}


/*!
* @brief Destructor.
*/
RFIDreader::~RFIDreader() 
{
    SPI.end();  // Clean up SPI
}


/*!
* @brief Function to Get the singleton instance.
* @return The pointer to Singleton instance.
*/
RFIDreader *
RFIDreader::get_instance() 
{
    if (instance == nullptr) 
    {
        instance = new RFIDreader();
    }
    return instance;
}

/*!
* @brief Function to Release the singleton instance.
*/
void 
RFIDreader::release_instance() 
{
    if (instance != nullptr) 
    {
        delete instance;
        instance = nullptr;
    }
}

/*!
* @brief Function to Set the RFID tag.
*/
void 
RFIDreader::set_tag(const char *tag) 
{
    rfid_tag = String(tag);
}

/*!
* @brief Function to Get the RFID tag.
* @return The pointer to RFID tag.
*/
const char *
RFIDreader::get_tag() 
{
    return rfid_tag.c_str();
}

/*!
* @brief Function to remove the RFID tag.
*/
void 
RFIDreader::remove_tag()
{
    rfid_tag="";
}

/*!
* @brief Function to Scan the RFID tag.
* @return The string representing the scanned uid.
*/
String 
RFIDreader::scan_card() 
{
    // Check if a new card is present
    //
    if (!mfrc522.PICC_IsNewCardPresent()) 
    {
        return "";  // No card present, return empty string
    }

    // Select one of the cards
    //
    if (!mfrc522.PICC_ReadCardSerial()) 
    {
        return "";  // Unable to read card, return empty string
    }

    // Convert the UID into a string format
    //
    String rfid_uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
        if (mfrc522.uid.uidByte[i] < 0x10) 
        {
            rfid_uid += "0";  // Add leading zero for single hex digits
        }
        rfid_uid += String(mfrc522.uid.uidByte[i], HEX);  // Convert to hexadecimal string
    }

    rfid_uid.toUpperCase();  // Convert UID to uppercase (optional)
    rfid_uid.trim();
    rfid_tag = rfid_uid;  // Assigining rfid value

    // Halt the PICC so that it can be read again later
    //
    mfrc522.PICC_HaltA();

    return rfid_uid;
}

/*!
* @brief Function to set the current date.
* @param[in] curr_date String& of current date.
*/
void 
RFIDreader::set_current_date(String& curr_date)
{
    current_date = curr_date;
}

/*!
* @brief Function to set the current time.
* @param[in] curr_time String& of current time.
*/
void 
RFIDreader::set_current_time(String& curr_time)
{
    current_time = curr_time;
}

/*!
* @brief Function to Get the current date.
* @return The string representing the current date.
*/
String& 
RFIDreader::get_current_date()
{
    return current_date;
}

/*!
* @brief Function to Get the current time.
* @return The string representing the current time.
*/
String& 
RFIDreader::get_current_time()
{
    return current_time;
}

/*!
* @brief Function to read name from a specified block.
* @return The string representing the name.
*/
String 
RFIDreader::readName(byte blockAddr) 
{
    byte buffer[18];
    byte size = sizeof(buffer);
    String name = "";
    
    MFRC522::StatusCode status;
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return "";
    } 
    else 
    {
        for (byte i = 0; i < 16; i++) 
        {
            if (buffer[i] != 0) 
            {
                name += (char)buffer[i];
            }
        }
    }
    
    String trimmedName = name;   // Create a new String for the trimmed value
    trimmedName.trim();          // Trim the name
    return trimmedName;          // Return trimmed name
}

/*!
* @brief Function to read employee ID from a specified block.
* @return The string representing the employee ID.
*/
String 
RFIDreader::readEmpID(byte blockAddr) 
{
    byte buffer[18];
    byte size = sizeof(buffer);
    String empid = "";
    
    MFRC522::StatusCode status;
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return "";
    } 
    else 
    {
        for (byte i = 0; i < 16; i++) 
        {
            if (buffer[i] != 0) 
            {
                empid += (char)buffer[i];
            }
        }
    }

    String trimmedEmpID = empid;   // Create a new String for the trimmed value
    trimmedEmpID.trim();           // Trim the empid
    return trimmedEmpID;           // Return trimmed empid
}

/*!
* @brief  Function to handle card reading.
* @return The string representing scanned key embedded inside the card.
*/
String 
RFIDreader::handleCardRead() 
{
    // Reset the loop if no new card present on the sensor/reader.
    //
    if (!mfrc522.PICC_IsNewCardPresent()) return "";

    // Select one of the cards
    //
    if (!mfrc522.PICC_ReadCardSerial()) return "";

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    
    if (piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
    {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return "";
    }

    // Authenticate using key A for sector 2
    //
    byte trailerBlock = 11;        // Trailer block of sector 2
    MFRC522::StatusCode status;
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return "";
    }

    
    // Read name from block 8 and employee ID from block 9
    //
    String name = readName(8);
    String empid = readEmpID(9);

    is_scan_card=true;
    // Check if name and empid are not empty
    //
    if (name != "" && empid != "") 
    {
        String result = name + "," + empid; // Concatenate name and empid
        // Halt PICC
        //
        result.trim();
        
        set_tag(result.c_str());
        
        mfrc522.PICC_HaltA();
        
        // Stop encryption on PCD
        //
        mfrc522.PCD_StopCrypto1();
        return result; // Return the result
    } 
    else 
    {
        // Halt PICC if data is missing
        //
        mfrc522.PICC_HaltA();
        
        // Stop encryption on PCD
        //
        
        mfrc522.PCD_StopCrypto1();
        
        return ""; // Return empty if either is missing
    }
}

/*!
* @brief Function to set the card is scanned or not.
* @param[in] is_scan bool of scanned variable.
*/
void 
RFIDreader::set_is_scan_card(bool is_scan)
{
    is_scan_card = is_scan;
}

/*!
* @brief Function to get the card is scanned or not.
* @return The status if the card is scanned or not.
*/
bool 
RFIDreader::get_is_scan_card()
{
    return is_scan_card;
}
