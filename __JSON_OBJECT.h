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

#ifndef __NETWORK_UTILITIES
#define __NETWORK_UTILITIES

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

class JSON_OBJECT {
public:
	JSON_OBJECT() = default;
	explicit JSON_OBJECT(std::string val) : singleValue(val) {};
	explicit JSON_OBJECT(std::function<JSON_OBJECT(JSON_OBJECT)> func) : function(func) {}
	
	bool IsArray() const { return valueArray.has_value(); }
	bool IsNull() const { return !singleValue.has_value() && !valueArray.has_value() && !data.has_value() && !function.has_value(); }
	void Reset() { singleValue.reset(); valueArray.reset(); data.reset(); function.reset(); }			// Resets all data to null-values
	std::string JSONObjectToString() const;
	JSON_OBJECT& StringToJSON(std::string&);

	JSON_OBJECT& push_back(const JSON_OBJECT&);
	JSON_OBJECT& AddProperty(const std::string&, const std::string&);
	JSON_OBJECT& AddProperty(const std::string&, const JSON_OBJECT&);
	JSON_OBJECT& operator[] (size_t index) { return (*valueArray)[index]; }					// Access element of indexed array
	const JSON_OBJECT& operator[] (size_t index) const { return (*valueArray)[index]; }		// Access element of indexed array
	JSON_OBJECT& operator[] (const std::string& key);
	JSON_OBJECT& at(const std::string key) { return (*data).at(key); }
	const JSON_OBJECT& at(const std::string key) const { return (*data).at(key); }
	// Overload operator() for function calls?

	// Getters. Each returns an optional that user is responsible for checking.
	std::optional<std::string>& Val() { return singleValue; }
	const std::optional<std::string>& Val() const { return singleValue; }
	std::optional<std::function<JSON_OBJECT(JSON_OBJECT)>>& Func() { return function; }
	const std::optional<std::function<JSON_OBJECT(JSON_OBJECT)>>& Func() const { return function; }
	std::optional<std::vector<JSON_OBJECT>>& Array() { return valueArray; }
	const std::optional<std::vector<JSON_OBJECT>>& Array() const { return valueArray; }
	std::optional<std::map<std::string, JSON_OBJECT>>& Map() { return data; }
	const std::optional<std::map<std::string, JSON_OBJECT>>& Map() const { return data; }
private:
	std::optional<std::string> singleValue;								// Object may be a single value
	std::optional<std::vector<JSON_OBJECT>> valueArray;					// ... or an array of values, which may in turn be objects themselves
	std::optional<std::map<std::string, JSON_OBJECT>> data;				// ... or an object with multiple properties
	std::optional<std::function<JSON_OBJECT(JSON_OBJECT)>> function;	// ... or a function object taking a JSON input and returning a JSON output
};

bool ClearEnclosingChars(const char, const char, std::string&);

#endif // !__NETWORK_UTILITIES
