/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/Any.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Any utility (for rad, should be removed .. portability).
** Author(s):  Public Domain.
*/

#ifndef QTC_COMMON_ANY_H
#define QTC_COMMON_ANY_H

#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>

namespace QtC {
  
    // using StorageType = typename decay<typename remove_reference<T>::type>::type;
    template<class T>
    using StorageType = typename decay<T>::type; 
    
    struct Any {
        bool is_null() const { return !ptr; }
        bool not_null() const { return ptr; }
        
        template<typename U> Any(U&& value)
            : ptr(new Derived<StorageType<U>>(forward<U>(value)))
        {}
        
        template<class U> bool is() const {
            typedef StorageType<U> T;
            auto derived = dynamic_cast<Derived<T>*> (ptr);
            return derived;
        }
        
        template<class U>
        StorageType<U>& as() {
            typedef StorageType<U> T;
            
            auto derived = dynamic_cast<Derived<T>*> (ptr);
            
            if (!derived)
                throw bad_cast();
            
            return derived->value;
        }
        
        template<class U>
        operator U() {
            return as<StorageType<U>>();
        }
        
        Any() : ptr(nullptr) {}
        Any(Any& that) : ptr(that.clone()) {}
        Any(Any&& that) : ptr(that.ptr) { that.ptr = nullptr; }
        Any(const Any& that) : ptr(that.clone()) { }
        Any(const Any&& that) : ptr(that.clone()) { }
        Any& operator=(const Any& a) {
            if (ptr == a.ptr)
                return *this;
            
            auto old_ptr = ptr;
            
            ptr = a.clone();
            
            if (old_ptr)
                delete old_ptr;
            
            return *this;
        }
        Any& operator=(Any&& a) {
            if (ptr == a.ptr)
                return *this;
            
            swap(ptr, a.ptr);
            
            return *this;
        }
        
        ~Any() {
            if (ptr)
                delete ptr;
        }
    private:
        struct Base {
            virtual ~Base() {}
            virtual Base* clone() const = 0;
        };
        
        template<typename T>
        struct Derived : Base {
            template<typename U> Derived(U&& value) : value(forward<U>(value)) { }
            
            T value;
            
            Base* clone() const { return new Derived<T>(value); }
        };
        
        Base* clone() const {
            if (ptr)
                return ptr->clone();
            else
                return nullptr;
        }
        
        Base* ptr;
    };
    
} /* namespace QtC */

#endif /* QTC_COMMON_ANY_H */
