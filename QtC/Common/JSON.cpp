/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/JSON.cpp
*/

#include <sstream>
#include <stdexcept>

#include "QtC/Common/JSON.h"

using namespace std;

namespace QtC {
    
    namespace JSON {
        
        Value::Value() : type_t(NIL) { }
        
        Value::Value(const long long int i) : int_v(i), type_t(INT) { }
        
        Value::Value(const long int i) : int_v(static_cast<long long int>(i)), type_t(INT) { }
        
        Value::Value(const int i) : int_v(static_cast<int>(i)), type_t(INT) { }
        
        Value::Value(const long double f) : float_v(f), type_t(FLOAT) { }
        
        Value::Value(const double f) : float_v(static_cast<long double>(f)), type_t(FLOAT) { }
        
        Value::Value(const bool b) : bool_v(b), type_t(BOOL) { }
        
        Value::Value(const char* s) : string_v(s), type_t(STRING) { }
        
        Value::Value(const string& s) : string_v(s), type_t(STRING) { }
        
        Value::Value(const Object& o) : object_v(o), type_t(OBJECT) { }
        
        Value::Value(const Array& o) : array_v(o), type_t(ARRAY) { }
        
        Value::Value(string&& s) : string_v(move(s)), type_t(STRING) { }
        
        Value::Value(Object&& o) : object_v(move(o)), type_t(OBJECT) { }
        
        Value::Value(Array&& o) : array_v(move(o)), type_t(ARRAY) { }
        
        Value::Value(const Value& v)
        { 
            switch(v.type())
                {
                    /** Base types */
                case INT:
                    int_v = v.int_v;
                    type_t = INT;
                    break;
                    
                case FLOAT:
                    float_v = v.float_v;
                    type_t = FLOAT;
                    break;
                    
                case BOOL:
                    bool_v = v.bool_v;
                    type_t = BOOL;
                    break;
                    
                case NIL:
                    type_t = NIL;
                    break;
                    
                case STRING:
                    string_v = v.string_v;
                    type_t = STRING;
                    break;
                    
                    /** Compound types */
                case ARRAY:
                    array_v = v.array_v;
                    type_t = ARRAY;
                    break;
                    
                case OBJECT:
                    object_v = v.object_v;
                    type_t = OBJECT;
                    break;
                    
                }
        }
        
        Value::Value(Value&& v)
        { 
            switch(v.type())
                {
                    /** Base types */
                case INT:
                    int_v = move(v.int_v);
                    type_t = INT;
                    break;
                    
                case FLOAT:
                    float_v = move(v.float_v);
                    type_t = FLOAT;
                    break;
                    
                case BOOL:
                    bool_v = move(v.bool_v);
                    type_t = BOOL;
                    break;
                    
                case NIL:
                    type_t = NIL;
                    break;
                    
                case STRING:
                    string_v = move(v.string_v);
                    type_t = STRING;
                    break;
                    
                    /** Compound types */
                case ARRAY:
                    array_v = move(v.array_v);
                    type_t = ARRAY;
                    break;
                    
                case OBJECT:
                    object_v = move(v.object_v);
                    type_t = OBJECT;
                    break;
                    
                }
        }
        
        Value& Value::operator=(const Value& v)
        {
            switch(v.type())
                {
                    /** Base types */
                case INT:
                    int_v = v.int_v;
                    type_t = INT;
                    break;
                    
                case FLOAT:
                    float_v = v.float_v;
                    type_t = FLOAT;
                    break;
                    
                case BOOL:
                    bool_v = v.bool_v;
                    type_t = BOOL;
                    break;
                    
                case NIL:
                    type_t = NIL;
                    break;
                    
                case STRING:
                    string_v = v.string_v;
                    type_t = STRING;
                    break;
                    
                    /** Compound types */
                case ARRAY:
                    array_v = v.array_v;
                    type_t = ARRAY;
                    break;
                    
                case OBJECT:
                    object_v = v.object_v;
                    type_t = OBJECT;
                    break;
                    
                }
            
            return *this;
            
        }
        
        Value& Value::operator=(Value&& v)
        {
            switch(v.type())
                {
                    /** Base types */
                case INT:
                    int_v = move(v.int_v);
                    type_t = INT;
                    break;
                    
                case FLOAT:
                    float_v = move(v.float_v);
                    type_t = FLOAT;
                    break;
                    
                case BOOL:
                    bool_v = move(v.bool_v);
                    type_t = BOOL;
                    break;
                    
                case NIL:
                    type_t = NIL;
                    break;
                    
                case STRING:
                    string_v = move(v.string_v);
                    type_t = STRING;
                    break;
                    
                    /** Compound types */
                case ARRAY:
                    array_v = move(v.array_v);
                    type_t = ARRAY;
                    break;
                    
                case OBJECT:
                    object_v = move(v.object_v);
                    type_t = OBJECT;
                    break;
                    
                }
            
            return *this;
            
        }
        
        Value& Value::operator[] (const string& key)
        {
            if (type() != OBJECT)
                throw std::logic_error("Value not an object");
            return object_v[key];
        }
        
        const Value& Value::operator[] (const string& key) const
        {
            if (type() != OBJECT)
                throw std::logic_error("Value not an object");
            return object_v[key];
        }
        
        Value& Value::operator[] (size_t i)
        {
            if (type() != ARRAY)
                throw std::logic_error("Value not an array");
            return array_v[i];
        }
        
        const Value& Value::operator[] (size_t i) const
        {
            if (type() != ARRAY)
                throw std::logic_error("Value not an array");
            return array_v[i];
        }
        

        String::String() {
        }
        String::String(const String &aOther)
            : iString(aOther.iString)
        {}
        String::String(const std::string &aString) 
            : iString(aString)
        {
        }
        
        String& String::operator=(const String &aOther) {
            iString = aOther.iString;
            return *this;
        }

        const std::string& String::str() const {
            return iString;
        }
        
        struct AssociationPrivate {
            AssociationPrivate() {}
            AssociationPrivate(const String &aName, const Value &aValue)
                : iName(aName.str()), iValue(aValue)
            {}
            AssociationPrivate(const std::string &aName, const Value &aValue)
                : iName(aName), iValue(aValue)
            {}
            std::string iName;
            Value iValue;
        };
        Association::Association() 
            : iPIMPL(new AssociationPrivate)
        {
            
        }
        Association::Association(const String &aName, const Value &aValue) 
            : iPIMPL(new AssociationPrivate(aName,aValue))
        {
        }
        Association::Association(const std::string &aName, const Value &aValue)
            : iPIMPL(new AssociationPrivate(aName,aValue))
        {}
        Association::~Association() {
            delete iPIMPL;
        }
        const std::string& Association::name() const {
            return iPIMPL->iName;
        }
        const Value& Association::value() const {
            return iPIMPL->iValue;
        }
        

        Object::Object() { }
        
        Object::~Object() { }
        
        Object::Object(const Object& o) : _object(o._object) { }
        
        Object::Object(Object&& o) : _object(move(o._object)) { }
        
        Object::Object(std::initializer_list<Association> aArgs) {
            std::initializer_list<Association>::iterator i;
            for(i=aArgs.begin();i!=aArgs.end();++i) {
                insert(std::pair<string,Value> ( (*i).name(), (*i).value() ));
            }
        }
            
        Object& Object::operator=(const Object& o)
        {
            _object = o._object;
            return *this;
        }
        
        Object& Object::operator=(Object&& o)
        {
            _object = move(o._object);
            return *this;
        }
        
        Value& Object::operator[] (const string& key)
        {
            return _object[key];
        }
        
        const Value& Object::operator[] (const string& key) const
        {
            return _object.at(key);
        }
        
        pair<map<string, Value>::iterator, bool> Object::insert(const pair<string, Value>& v)
        {
            return _object.insert(v);
        }

        void Object::insert(const String &aString, const Value &aValue) {
            insert(pair<string, Value>(aString.str(),aValue));
        }
        
        map<string, Value>::const_iterator Object::begin() const
        {
            return _object.begin();
        }
        
        map<string, Value>::const_iterator Object::end() const
        {
            return _object.end();
        }
        
        map<string, Value>::iterator Object::begin()
        {
            return _object.begin();
        }
        
        map<string, Value>::iterator Object::end()
        {
            return _object.end();
        }
        
        size_t Object::size() const
        {
            return _object.size();
        }

        std::string Object::toString() const {
            std::ostringstream stream;
            stream << *this;
            return stream.str();
        }
        
        Array::Array() { }
        
        Array::~Array() { }
        
        Array::Array(const Array& a) : _array(a._array) { }
        
        Array::Array(Array&& a) : _array(move(a._array)) { }
        
        Array::Array(std::initializer_list<Value> aArgs) {
            std::initializer_list<Value>::iterator i;
            for(i=aArgs.begin();i!=aArgs.end();++i) {
                push_back(*i);
            }
        }

        Array& Array::operator=(const Array& a)
        {
            _array = a._array;
            return *this;
        }
        
        Array& Array::operator=(Array&& a)
        {
            _array = move(a._array);
            return *this;
        }
        
        
        Value& Array::operator[] (size_t i)
        {
            return _array.at(i);
        }
        
        const Value& Array::operator[] (size_t i) const
        {
            return _array.at(i);
        }
        
        vector<Value>::const_iterator Array::begin() const
        {
            return _array.begin();
        }
        
        vector<Value>::const_iterator Array::end() const
        {
            return _array.end();
        }
        
        vector<Value>::iterator Array::begin()
        {
            return _array.begin();
        }
        
        vector<Value>::iterator Array::end()
        {
            return _array.end();
        }
        
        size_t Array::size() const
        {
            return _array.size();
        }
        
        void Array::push_back(const Value& v)
        {
            _array.push_back(v);
        }
        
        void indent(ostream& os) {
            for (unsigned int i  = 0; i < ind; i++)
                os << "\t";
        }
                
    } /* namespace JSON */

} /* namespace QtC */


ostream& operator<<(ostream& os, const QtC::JSON::Value& v) {
    using namespace QtC::JSON;
    
    switch(v.type()) {
        /** Base types */
    case INT:
        os << (long long int)v;
        break;
        
    case FLOAT:
        os << (long double)v;
        break;
        
    case BOOL:
        os << ((bool)v ? "true" : "false");
        break;
        
    case NIL:
        os << "null";
        break;
        
    case STRING:
        os << '"' << (string)v << '"';                
        break;
        
        /** Compound types */
    case ARRAY:
        os << (Array)v;                
        break;
        
    case OBJECT:
        os << (Object)v;                
        break;
        
    }
    return os;
}

ostream& operator<<(ostream& os, const QtC::JSON::Association &a) {
    os << '"' << a.name() << '"' << ": " << a.value();
    return os;
}

ostream& operator<<(ostream& os, const QtC::JSON::Object& o) {
    os << "{" << endl;
    QtC::JSON::ind++;
    for (auto e = o.begin(); e != o.end();)
        {
            QtC::JSON::indent(os);
            os << '"' << e->first << '"' << ": " << e->second;
            if (++e != o.end())
                os << ",";
            os << endl;
        }    
    QtC::JSON::ind--;
    QtC::JSON::indent(os);
    os << "}";
    
    return os;
}

ostream& operator<<(ostream& os, const QtC::JSON::Array& a) {
    os << "[" << endl;
    QtC::JSON::ind++;
    for (auto e = a.begin(); e != a.end();)
        {
            QtC::JSON::indent(os);
            os << (*e);
            if (++e != a.end())
                os << ",";
            os << endl;
        }    
    QtC::JSON::ind--;
    QtC::JSON::indent(os);
    os << "]";
    
    return os;
}

QtC::JSON::Association operator|(const QtC::JSON::String &a, const QtC::JSON::Object &b) {
  return QtC::JSON::Association(a,b);
}

QtC::JSON::Association operator|(const QtC::JSON::String &a, const QtC::JSON::Value &b) {
  return QtC::JSON::Association(a,b);
}