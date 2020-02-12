# JSON_OBJECT
JSON-style object written in C++
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ryan Toll, 2020
// This file lays out a provisional JSON library meant to create a bridge between the strong typing of C++ and the loose style of JSON.
// Ideally, this is used only minimally. However, it may be very useful when interfacing with web-style tools that use JSON.
// The objective is to create a reasonable approximation of a true JSON object within C++ with a decent user interface.
//
// JSON objects can contain: a single value, a single function, an array of JSON objects, or an object with 0+ properties, held as JSON objects.
// JSON objects can be composed recursively indefinitely, so arrays and objects can branch out into any of the four types.
// JSON objects must be dynamically composable at run-time.
// JSON objects lack compile-time type safety. They therefore need run-time checks and additional exception handling.
// 
// This implementation uses std::optionals to store data given that the types are known and limited and object could contain any one type.
// At any given time, at most one optional should be non-null. A completely null object is sensible and represents a null property.
// All single values are stored as strings for universal access in C++. They can be converted as needed form there.
// All functions take a JSON argument and return a JSON argument. Any function call should conceivably be convertable to this form.
// Arrays are stored using std::vector and contain JSON elements, which can take arbitrary form.
// Object-style elements are stored using std::map (enforcing default string sorting of properties). Properties are stored as JSON elements.
// 
// Getters are listed to give reference access to elements.
// Inserts can be done with push_back or AddProperty as appropriate.
// Overloaded operator[] also provides both string key and index access into internal map & vector respectively.
// Single elements can be constructed explicitly using the appropriate constructor.
// Unsupported operations can be done by directly accessing the map & vector references.
*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
