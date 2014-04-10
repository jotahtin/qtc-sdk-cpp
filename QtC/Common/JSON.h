/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/JSON.h
*/

#ifndef QTC_COMMON_JSON_H
#define QTC_COMMON_JSON_H

#include <iostream>
#include <map>
#include <vector>
#include <stack>

namespace QtC {
    
    namespace JSON {
        
        /** Possible JSON type of a value (array, object, bool, ...). */
        enum ValueType {
            INT,        // JSON's int
            FLOAT,      // JSON's float 3.14 12e-10
            BOOL,       // JSON's boolean (true, false)
            STRING,     // JSON's string " ... " or (not really JSON) ' ... '
            OBJECT,     // JSON's object { ... }
            ARRAY,      // JSON's array [ ... ]
            NIL         // JSON's null
        };
        
        // Forward declaration
        class Value;

        class String {
        public:
            String();
            String(const String &aOther);
            String(const std::string &aString);

            String& operator=(const String &aOther);
            
            const std::string& str() const;
        private:
            std::string iString;
        };
        
        class Association {
        public:
            Association();
            Association(const String &aName, const Value &aValue);
            Association(const std::string &aName, const Value &aValue);
            ~Association();

            const std::string& name() const;
            const Value& value() const;
        protected:
            struct AssociationPrivate *iPIMPL;
        };
        
        /** A JSON object, i.e., a container whose keys are strings, this
            is roughly equivalent to a Python dictionary, a PHP's associative
            array, a Perl or a C++ map (depending on the implementation). */
        class Object {
        public:
            /** Constructor. */
            Object();
            
            /** Copy constructor. 
                @param o object to copy from
            */
            Object(const Object& o);
            
            /** Move constructor. */
            Object(Object&& o);

            
            Object(std::initializer_list<Association> aArgs);
            
            
            /** Assignment operator. 
                @param o object to copy from
            */
            Object& operator=(const Object& o);
            
            /** Move operator. 
                @param o object to copy from
            */
            Object& operator=(Object&& o);
            
            /** Destructor. */
            ~Object();
            
            /** Subscript operator, access an element by key.
                @param key key of the object to access
            */
            Value& operator[] (const std::string& key);
            
            /** Subscript operator, access an element by key.
                @param key key of the object to access
            */
            const Value& operator[] (const std::string& key) const;
            
            /** Retrieves the starting iterator (const).
                @remark mainly for printing
            */
            std::map<std::string, Value>::const_iterator begin() const;
            
            /** Retrieves the ending iterator (const).
                @remark mainly for printing
            */
            std::map<std::string, Value>::const_iterator end() const;
            
            /** Retrieves the starting iterator */
            std::map<std::string, Value>::iterator begin();
            
            /** Retrieves the ending iterator */
            std::map<std::string, Value>::iterator end();
            
            /** Inserts a field in the object.
                @param v pair <key, value> to insert
                @return an iterator to the inserted object
            */
            std::pair<std::map<std::string, Value>::iterator, bool> insert(const std::pair<std::string, Value>& v);

            void insert(const String &aString, const Value &aValue);
            
            /** Size of the object. */
            size_t size() const;

            std::string toString() const;
        protected:
            
            /** Inner container. */
            std::map<std::string, Value> _object;
        };
        
        /** A JSON array, i.e., an indexed container of elements. It contains
            JSON values, that can have any of the types in ValueType. */
        class Array
        {
        public:
            
            /** Constructor. */
            Array();
            
            /** Destructor. */
            ~Array();
            
            /** Copy constructor. 
                @param o the object to copy from
            */
            Array(const Array& a);
            
            /** Assignment operator. 
                @param a array to copy from
            */
            Array& operator=(const Array& a);
            
            /** Copy constructor. 
                @param o the object to copy from
            */
            Array(Array&& a);

            Array(std::initializer_list<Value> aArgs);
            
            /** Assignment operator. 
                @param a array to copy from
            */
            Array& operator=(Array&& a);
            
            /** Subscript operator, access an element by index. 
                @param i index of the element to access
            */
            Value& operator[] (size_t i);
            
            /** Subscript operator, access an element by index. 
                @param i index of the element to access
            */
            const Value& operator[] (size_t i) const;
            
            /** Retrieves the starting iterator (const).
                @remark mainly for printing
            */
            std::vector<Value>::const_iterator begin() const;
            
            /** Retrieves the ending iterator (const).
                @remark mainly for printing
            */
            std::vector<Value>::const_iterator end() const;
            
            /** Retrieves the starting iterator. */
            std::vector<Value>::iterator begin();
            
            /** Retrieves the ending iterator */
            std::vector<Value>::iterator end();
            
            /** Inserts an element in the array.
                @param n (a pointer to) the value to add
            */
            void push_back(const Value& n);
            
            /** Size of the array. */
            size_t size() const;
            
        protected:
            
            /** Inner container. */
            std::vector<Value> _array;
            
        };
        
        /** A JSON value. Can have either type in ValueTypes. */
        class Value
        {
        public:
            /** Default constructor (type = NIL). */
            Value();
            
            /** Copy constructor. */
            Value(const Value& v);
            
            /** Constructor from int. */
            Value(const long long int i);
            
            /** Constructor from int. */
            Value(const long int i);
            
            /** Constructor from int. */
            Value(const int i);
            
            /** Constructor from float. */
            Value(const long double f);
            
            /** Constructor from float. */
            Value(const double f);
            
            /** Constructor from bool. */
            Value(const bool b);
            
            /** Constructor from pointer to char (C-string).  */
            Value(const char* s);
            
            /** Constructor from STD string  */
            Value(const std::string& s);
            
            /** Constructor from pointer to Object. */
            Value(const Object& o);
            
            /** Constructor from pointer to Array. */
            Value(const Array& a);
            
            /** Move constructor. */
            Value(Value&& v);
            
            /** Move constructor from STD string  */
            Value(std::string&& s);
            
            /** Move constructor from pointer to Object. */
            Value(Object&& o);
            
            /** Move constructor from pointer to Array. */
            Value(Array&& a);
            
            /** Type query. */
            ValueType type() const
            {
                return type_t;
            }
            
            /** Subscript operator, access an element by key.
                @param key key of the object to access
            */
            Value& operator[] (const std::string& key);
            
            /** Subscript operator, access an element by key.
                @param key key of the object to access
            */
            const Value& operator[] (const std::string& key) const;
            
            /** Subscript operator, access an element by index. 
                @param i index of the element to access
            */
            Value& operator[] (size_t i);
            
            /** Subscript operator, access an element by index. 
                @param i index of the element to access
            */
            const Value& operator[] (size_t i) const;
            
            /** Assignment operator. */
            Value& operator=(const Value& v);
            
            /** Move operator. */
            Value& operator=(Value&& v);
            
            /** Cast operator for float */
            explicit operator long double() const { return float_v; }
            
            /** Cast operator for int */
            explicit operator long long int() const { return int_v; }
            
            /** Cast operator for bool */
            explicit operator bool() const { return bool_v; }
            
            /** Cast operator for string */
            explicit operator std::string () const { return string_v; }
            
            /** Cast operator for Object */
            operator Object () const { return object_v; }
            
            /** Cast operator for Object */
            operator Array () const { return array_v; }
            
            /** Cast operator for float */
            long double as_float() const { return float_v; }
            
            /** Cast operator for int */
            long long int as_int() const { return int_v; }
            
            /** Cast operator for bool */
            bool as_bool() const { return bool_v; }
            
            /** Cast operator for string */
            std::string as_string() const { return string_v; }
            
            
        protected:
            
            long double         float_v;
            long long int       int_v;
            bool                bool_v;
            std::string         string_v;
            
            Object              object_v;
            Array               array_v;
            
            ValueType           type_t;
        };
        
        /** Indentation counter */
        static unsigned int ind;
        
        /** Print correct indentation before printing anything */
        static void indent(std::ostream& os = std::cout);
        
        JSON::Value parseFile(const char *aFilename);
        JSON::Value parseString(const std::string &aString);
        
    } /* namespace JSON */

} /* namespace QtC */

/** Output operators */
std::ostream& operator<<(std::ostream&, const QtC::JSON::Value&);
std::ostream& operator<<(std::ostream&, const QtC::JSON::Association&);
std::ostream& operator<<(std::ostream&, const QtC::JSON::Object&);
std::ostream& operator<<(std::ostream&, const QtC::JSON::Array&);

/** Construction operators */
QtC::JSON::Association operator|(const QtC::JSON::String &a, const QtC::JSON::Object &b);
QtC::JSON::Association operator|(const QtC::JSON::String &a, const QtC::JSON::Value &b);

#endif /* QTC_COMMON_JSON_H */
    
