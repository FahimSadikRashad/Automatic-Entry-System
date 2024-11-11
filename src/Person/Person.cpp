#include "Person.hpp"


/*!
* @brief Function to set the name of the person.
* @param[in] person_name const String& of person name.
* @return The status if admin authenticated or not.
*/
void 
Person::set_name(const String& person_name) 
{
    name = person_name;
}


/*!
* @brief Function to retrieve the name of the person.
* @return The name that is stored.
*/
String 
Person::get_name() const 
{
    return name;
}
