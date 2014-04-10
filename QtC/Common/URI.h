/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/URI.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    URI (Uniform resource identifier).
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_COMMON_URI_H
#define QTC_COMMON_URI_H

#include <stddef.h>

#include <string>
#include <iostream>

namespace QtC {

    class URI {
    public:
        class UserInfo {
        public:
            UserInfo();
            UserInfo(const UserInfo &aUserInfo);
            UserInfo(const std::string &aUserInfoString);
            ~UserInfo();

            UserInfo& operator=(const UserInfo &aUserInfo);

            bool empty() const;

            const std::string& username() const;
            void setUsername(const std::string &aUsername);
            
            const std::string& password() const;
            void setPassword(const std::string &aPassword);

            std::string toString() const;
        private:
            struct URIUserInfoPrivate *iPIMPL;
        };
        
        class Authority {
        public:
            Authority();
            Authority(const Authority &aAuthority);
            Authority(const std::string &aAuthorityString);
            ~Authority();
            
            Authority& operator=(const Authority &aAuthority);

            const UserInfo& userInfo() const;
            void setUserInfo(const UserInfo &aUserInfo); 

            const std::string& hostname() const;
            void setHostname(const std::string &aHostname);

            const std::string& port(bool aIfEmptyReturnDefault=true) const;
            void setPort(const std::string &aPort);
            void setDefaultPort(const std::string &aPort);
            
            std::string toString() const;
        private:
            struct URIAuthorityPrivate *iPIMPL;
        };
        class Path {
        public:
            Path();
            Path(const Path &aOther);
            Path(const std::string &aPath);
            ~Path();
            
            Path& operator=(const Path &aOther);

            void addElement(const std::string &aElement);

            size_t depth() const;
            bool empty() const;
            const std::string& operator[](int aIndex) const;

            std::string toString() const;
        private:
            struct URIPathPrivate *iPIMPL;
        };

        class Query {
        public:
            Query();
            Query(const Query &aOther);
            Query(const std::string &aQuery);
            ~Query();
            
            Query& operator=(const Query &aOther);
                        
            void addAssociation(const std::string &aNameValue);
            void addAssociation(const std::string &aName,const std::string &aValue);
            
            size_t size() const;
            bool empty() const;
            
            const std::string& name(int aIndex) const;
            const std::string& value(int aIndex) const;

            std::string toString() const;
        private:
            struct URIQueryPrivate *iPIMPL;
        };

        /* full path = path ["?" query] ["#" fragment] part from url */
        class FullPath {
            friend class URI;
        public:
            FullPath();
            FullPath(const FullPath &aOther);
            ~FullPath();

            FullPath& operator=(const FullPath &aOther);

            const Path& path() const;
            Path& path();

            const Query& query() const;
            Query& query();
            
            const std::string& fragment() const;

            std::string toString() const;
        protected:
            struct URIFullPathPrivate *pimpl();
        private:
            struct URIFullPathPrivate *iPIMPL;
        };

    public:
        URI();
        URI(const URI &aURI);
        URI(const std::string &aURI);
        ~URI();

        URI& operator=(const URI &aURI);
        
        const std::string& scheme() const;
        void setScheme(const std::string &aScheme);
        
        void setAuthority(const Authority &aAuthority);
        const Authority& authority() const;
        Authority& authority();
        
        void setPath(const Path &aPath);
        const Path& path() const;
        Path& path();
        
        void setQuery(const Query &aQuery);
        const Query& query() const;
        Query& query();
        
        void setFragment(const std::string &aFragment);
        const std::string& fragment() const;

        const FullPath& fullpath() const;

        std::string toString() const;

        // Impicit casts
        inline operator const Authority() const { return authority(); }
        inline operator const Path() const { return path(); }
        inline operator const Query() const { return query(); }
        inline operator const FullPath() const { return fullpath(); }
    public:
        static std::string encode(const std::string &aURLstring);
        static std::string decode(const std::string &aURLstring);
    private:
        struct URIPrivate *iPIMPL;
    };

    class URL : public URI {
    public:
        URL();
        URL(const URL &aURL);
        URL(const char *aURLString);
        URL(const std::string &aURLString);

        URL& operator=(const URL &aURL);
    private:
        void initializeDefaults();
    };
    
} /* namespace QtC */

std::ostream& operator<<(std::ostream &os, const QtC::URI::UserInfo &aUserInfo);
std::ostream& operator<<(std::ostream &os, const QtC::URI::Authority &aAuthority);
std::ostream& operator<<(std::ostream &os, const QtC::URI::Path &aPath);
std::ostream& operator<<(std::ostream &os, const QtC::URI::Query &aQuery);
std::ostream& operator<<(std::ostream &os, const QtC::URI::FullPath &aFullPath);
std::ostream& operator<<(std::ostream &os, const QtC::URI &aURI);

QtC::URI::Path& operator<<(QtC::URI::Path &aPath, const std::string &aElement);
QtC::URI::Path& operator<<(QtC::URI::Path &aPath, const QtC::URI::Path &aOther);

#endif /* QTC_COMMON_URI_H */
