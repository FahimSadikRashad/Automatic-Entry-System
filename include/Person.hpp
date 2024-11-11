/** @file Person.hpp
*
* @brief Defining a person abstract class to hold person related common data
*
* 
*/

#ifndef PERSON_HPP
#define PERSON_HPP

#include <Arduino.h>

// Class representing a person
//
class Person 
{
public:
    virtual ~Person() {}
    virtual void authenticate() = 0;
    
    // Set the name of the person
    //
    virtual void set_name(const String& person_name);
    
    // Retrieve the name of the person
    //
    virtual String get_name() const;

protected:
    String name;  // Person's name
};

#endif  // End of PERSON_HPP
