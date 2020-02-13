#include "__JSON_OBJECT.h"
using std::map;
using std::string;
using std::vector;

// Returns a string representation of the JSON object
string JSON_OBJECT::JSONObjectToString() const {
	string output{ "{" };																			// Begin with an open-curly-brace
	if (singleValue.has_value()) { output = "\"" + *singleValue + "\""; return output; }			// Return single-value as string if it exists
	else if (IsArray()) { 																			// Print array if it is an array
		output = "[";																				// Open-square-bracket starts array
		for (auto element : *valueArray) { output += element.JSONObjectToString(); output += ","; }	// Iterate over elements, add each one, and deliniate with commas
		output.erase(output.size() - 1);															// Erase the trailing comma
		output += "]";																				// Close-square-bracket ends array
		return output;
	}
	else if (data.has_value()) {
		for (auto prop : *data) {																	// Print all properties if it's an object
			output += "\"" + prop.first + "\":";													// Property names are enclosed in quotes
			output += prop.second.JSONObjectToString();												// Recursively call string representation on each property
			output += ",";																			// Deliniate elements with commas
		}
		output.erase(output.size() - 1);															// Erase the trailing comma
	}
	else { return "null"; }																			// Print "null" to represent null values (or functions at present)

	output += "}";																					// End with close-curly-brace
	return output;
}

// Builds current object to match JSON-encoded string
JSON_OBJECT& JSON_OBJECT::StringToJSON(string& input) {
	Reset();																			// Start by clearing out any old data
	string key{ "" }, val{ "" };
	JSON_OBJECT tempJSON;
	if (ClearEnclosingChars('[', ']', input)) {											// Build array if enclosed in [ ]
		char delimiter{ '}' };															// Track if iterating through objects or single values
		auto n = input.find_first_of(delimiter);										// Select based upon object first
		if (n == string::npos) { delimiter = ','; input.find_first_of(delimiter); }		// Then try for single-values
		while (input.size() > 0) {														// Consume string as it is parsed. Done when string is empty
			val = input.substr(0, n + 1);												// Grab segment with enclosing chars
			tempJSON.StringToJSON(val);													// Recursively call build from string on each element
			push_back(tempJSON);														// Add newly build element into array
			n = input.find_first_of(delimiter);											// Get index for next segment
			delimiter == '}' ? input.erase(0, n + 2) : input.erase(0, n + 1);			// Erase read portion along with closing char and comma
		}
		return *this;
	}
	else if (ClearEnclosingChars('"', '"', input)) { singleValue.emplace(input); return *this; }		// Return single-value JSON
	else if (!ClearEnclosingChars('{', '}', input)) { singleValue.emplace(input);  return *this; }		// Erase {} on the two ends

	// Build out JSON object by iterating over properties
	size_t end{ 1 };
	while (end != string::npos) {
		end = input.find_first_of(":");						// Find the end of the property name
		key = input.substr(0, end);							// Grab the property name
		ClearEnclosingChars('"', '"', key);					// Strip off the quotes
		input.erase(0, end + 1);							// Erase read portion as well as colon

		end = input.find_first_of(",");						// Find the end of the value
		val = input.substr(0, end);							// Grab the value
		ClearEnclosingChars('"', '"', val);					// Clear quotes from string values
		AddProperty(key, val);								// Recursively build
		input.erase(0, end + 1);							// Erase read portion as well as comma
	}
	return *this;											// Return reference to self
}

// Tests for enclosing chars and clears them out. Returns bool indicating success/failure.
bool ClearEnclosingChars(const char c1, const char c2, string& s) {
	if (s[0] != c1 || s[s.size() - 1] != c2) { return false; }
	s.erase(0, 1);
	s.erase(s.size() - 1);
	return true;
}

// For arrays (and null objects)
// Adds value to back of array. Turns null-object into an array with one element.
// Throws if object is neither an array and nor null.
JSON_OBJECT& JSON_OBJECT::push_back(const JSON_OBJECT& val) {
	if (valueArray.has_value()) { valueArray->push_back(val); }
	else {
		if (!IsNull()) { std::logic_error e{ "Invalid JSON access [value array]. Only one non-null member may exist." }; throw e; }
		vector<JSON_OBJECT> tempVec{ val };
		valueArray.emplace(std::move(tempVec));
	}
	return *this;
}

// For object-type elements (and null objects)
// Adds a new property to the element. Turns null-objects into object-type elements.
// Throws if element is neither object-type nor null.
JSON_OBJECT& JSON_OBJECT::AddProperty(const std::string& key, const JSON_OBJECT& val) {
	if (data.has_value()) { data->emplace(key, val); }
	else { 
		if (!IsNull()) { std::logic_error e{ "Invalid JSON access [data map]. Only one non-null member may exist." }; throw e; }
		map<string, JSON_OBJECT> tempMap{ {key, val} };
		data.emplace(std::move(tempMap)); 
	}
	return *this;
}

// As above, except the element's value is the supplied string val
JSON_OBJECT& JSON_OBJECT::AddProperty(const std::string& key, const std::string& val) { return AddProperty(key, JSON_OBJECT(val)); }

// For object-type elements (and null objects)
// Accesses property denoted by the supplied key.
// Adds property with null JSON as value if property does not already exist (consistent with std::map).
// Transforms null objects into object-type elements.
// Throws if element is neither object-type nor null.
JSON_OBJECT& JSON_OBJECT::operator[] (const std::string& key) {
	try { return (*data)[key]; }
	catch (std::bad_optional_access) {
		if (!IsNull()) { std::logic_error e{ "Invalid JSON access [data map]. Only one non-null member may exist." }; throw e; }
		AddProperty(key, JSON_OBJECT())[key];
	}
}