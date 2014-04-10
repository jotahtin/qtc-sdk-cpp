/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/URI.cpp
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    URI (Uniform resource identifier).
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#include <vector>
#include <sstream>

#include "QtC/Common/URI.h"

using namespace std;

namespace QtC {

    /*
    ** UserIUnfo : username [ ":" password]
    */
    struct URIUserInfoPrivate {
        std::string username;
        std::string password;
    };
    
    URI::UserInfo::UserInfo() 
        : iPIMPL(new URIUserInfoPrivate)
    {
    }
    
    URI::UserInfo::UserInfo(const UserInfo &aUserInfo) 
        : iPIMPL(new URIUserInfoPrivate)
    {
        iPIMPL->username = aUserInfo.iPIMPL->username;
        iPIMPL->password = aUserInfo.iPIMPL->password;
    }
    
    URI::UserInfo::UserInfo(const std::string &aUserInfoString) 
        : iPIMPL(new URIUserInfoPrivate)
    {
        size_t r;

        r=aUserInfoString.find_first_of(':');
        if (r != std::string::npos) {
            setUsername(aUserInfoString.substr(0,r));
            setPassword(aUserInfoString.substr(r+1));
        } else {
            setUsername(aUserInfoString);
        }
    }
    
    URI::UserInfo::~UserInfo() {
        delete iPIMPL;
    }
    
    URI::UserInfo& URI::UserInfo::operator=(const UserInfo &aUserInfo) {
        iPIMPL->username = aUserInfo.iPIMPL->username;
        iPIMPL->password = aUserInfo.iPIMPL->password;
        return *this;
    }
    
    bool URI::UserInfo::empty() const {
        return username().empty() && password().empty();
    }
    
    const std::string& URI::UserInfo::username() const {
        return iPIMPL->username;
    }
    
    void URI::UserInfo::setUsername(const std::string &aUsername) {
        iPIMPL->username = aUsername;
    }
            
    const std::string& URI::UserInfo::password() const {
        return iPIMPL->password;
    }
    void URI::UserInfo::setPassword(const std::string &aPassword) {
        iPIMPL->password = aPassword;
    }
    
    std::string URI::UserInfo::toString() const {
        std::ostringstream stream;
        
        stream << username();
        
        if (!password().empty()) {
            stream << ":" << password();
        }
        
        return stream.str();
    }


    /*
    ** Authority : [ userinfo "@"] host [ ":" port]
    */  
    struct URIAuthorityPrivate {
        URI::UserInfo userinfo;
        std::string   hostname;
        std::string   port;
        std::string   defaultPort;
    };
    URI::Authority::Authority() 
        : iPIMPL(new URIAuthorityPrivate)
    {
    }
    URI::Authority::Authority(const Authority &aAuthority) 
        : iPIMPL(new URIAuthorityPrivate)
    {
        iPIMPL->userinfo = aAuthority.iPIMPL->userinfo;
        iPIMPL->hostname = aAuthority.iPIMPL->hostname;
        iPIMPL->port = aAuthority.iPIMPL->port;
        iPIMPL->defaultPort = aAuthority.iPIMPL->defaultPort;
    }
    URI::Authority::Authority(const std::string &aAuthorityString) 
        : iPIMPL(new URIAuthorityPrivate)
    {
        size_t r,pos=0;
        
        r=aAuthorityString.find_first_of('@',pos);
        if (r != std::string::npos) {
            setUserInfo(aAuthorityString.substr(pos,r-pos));
            pos=r+1;
        }
        
        r=aAuthorityString.find_first_of(':',pos);
        if (r != std::string::npos) {
            setHostname(aAuthorityString.substr(pos,r-pos));
            setPort(aAuthorityString.substr(r+1));
        } else {
            setHostname(aAuthorityString.substr(pos));
        }
    }
    URI::Authority::~Authority() {
        delete iPIMPL;
    }
    
    URI::Authority& URI::Authority::operator=(const Authority &aAuthority) {
        iPIMPL->userinfo = aAuthority.iPIMPL->userinfo;
        iPIMPL->hostname = aAuthority.iPIMPL->hostname;
        iPIMPL->port = aAuthority.iPIMPL->port;
        iPIMPL->defaultPort = aAuthority.iPIMPL->defaultPort;
        return *this;
    }
    
    const URI::UserInfo& URI::Authority::userInfo() const {
        return iPIMPL->userinfo;
    }
    void URI::Authority::setUserInfo(const UserInfo &aUserInfo) {
        iPIMPL->userinfo = aUserInfo;
    }

    const std::string& URI::Authority::hostname() const {
        return iPIMPL->hostname;
    }
    void URI::Authority::setHostname(const std::string &aHostname) {
        iPIMPL->hostname = aHostname;
    }
    
    const std::string& URI::Authority::port(bool aIfEmptyReturnDefault) const {
        if (aIfEmptyReturnDefault && iPIMPL->port.empty()) {
            return iPIMPL->defaultPort;
        }
        return iPIMPL->port;
    }
    void URI::Authority::setPort(const std::string &aPort) {
        iPIMPL->port = aPort;
    }
    void URI::Authority::setDefaultPort(const std::string &aPort) {
        iPIMPL->defaultPort = aPort;
    }
    
    std::string URI::Authority::toString() const {
        std::ostringstream stream;
     
        if (!userInfo().empty()) {
            stream << userInfo() << "@";
        }

        stream << hostname();
        
        if (!port(false).empty()) {
            stream << ":" << port(false);
        }
        
        return stream.str();
    }

    
    /*
    ** Path Element : a/b/c
    */  
    struct URIPathPrivate {
        std::vector<std::string> elements;
    };
    URI::Path::Path() 
        : iPIMPL(new URIPathPrivate)
    {
    }
    URI::Path::Path(const Path &aOther)
        : iPIMPL(new URIPathPrivate)
    {
        iPIMPL->elements = aOther.iPIMPL->elements;
    }
    URI::Path::Path(const std::string &aPath)
        : iPIMPL(new URIPathPrivate)
    {
        size_t r,pos=0;
        std::string s;

        if (aPath.empty())
            return;

        while(true) {
            r=aPath.find_first_of('/',pos);
            if (r == std::string::npos) {
                addElement(aPath.substr(pos));
                break;
            }
            addElement(aPath.substr(pos,r-pos));
            pos=r+1;
        }
    }
    URI::Path::~Path() {
        delete iPIMPL;
    }
    
    URI::Path& URI::Path::operator=(const URI::Path &aOther) {
        iPIMPL->elements = aOther.iPIMPL->elements;
        return *this;
    }
    
    void URI::Path::addElement(const std::string &aElement) {
        iPIMPL->elements.push_back(aElement);
    }
    
    size_t URI::Path::depth() const {
        return iPIMPL->elements.size();
    }
    bool URI::Path::empty() const {
        return depth()==0;
    }
    const std::string& URI::Path::operator[](int aIndex) const {
        return iPIMPL->elements[aIndex];
    }
    
    std::string URI::Path::toString() const {
        bool first=true;
        std::ostringstream stream;
        std::vector<std::string>::const_iterator element;
        
        for(element=iPIMPL->elements.begin();element!=iPIMPL->elements.end();++element) {
            if (first) first=false; else stream << "/";
            stream << *element;
        }
        
        return stream.str();
    }

    /*
    ** Query Element : a=1&b=2&c=3
    */  
    struct URIQueryPrivate {
        std::vector< std::pair<std::string,std::string> > associations;
    };
    URI::Query::Query() 
        : iPIMPL(new URIQueryPrivate)
    {
    }
    URI::Query::Query(const Query &aOther)
        : iPIMPL(new URIQueryPrivate)
    {
        iPIMPL->associations = aOther.iPIMPL->associations;
    }
    URI::Query::Query(const std::string &aQuery)
        : iPIMPL(new URIQueryPrivate)
    {
        size_t r,pos=0;
        std::string s;
        
        if (aQuery.empty())
            return;
        
        while(true) {
            r=aQuery.find_first_of('&',pos);
            if (r == std::string::npos) {
                addAssociation(aQuery.substr(pos));
                break;
            }
            addAssociation(aQuery.substr(pos,r-pos));
            pos=r+1;
        }
    }
    URI::Query::~Query() {
        delete iPIMPL;
    }
    
    URI::Query& URI::Query::operator=(const URI::Query &aOther) {
        iPIMPL->associations = aOther.iPIMPL->associations;
        return *this;
    }

    void URI::Query::addAssociation(const std::string &aNameValue) {
        size_t r;
        r=aNameValue.find_first_of('=');
        if (r!=std::string::npos) {
            addAssociation(decode(aNameValue.substr(0,r)),
                           decode(aNameValue.substr(r+1)));
        }
    }
    void URI::Query::addAssociation(const std::string &aName,const std::string &aValue) {
        iPIMPL->associations.push_back(std::pair<std::string,std::string>(aName,aValue));
    }
    
    size_t URI::Query::size() const {
        return iPIMPL->associations.size();
    }
    bool URI::Query::empty() const {
        return size()==0;
    }

    const std::string& URI::Query::name(int aIndex) const {
        return iPIMPL->associations[aIndex].first;
    }
    const std::string& URI::Query::value(int aIndex) const {
        return iPIMPL->associations[aIndex].second;
    }
    
    std::string URI::Query::toString() const {
        bool first=true;
        std::ostringstream stream;
        std::vector< std::pair<std::string,std::string> >::const_iterator i;
        
        for(i=iPIMPL->associations.begin();i!=iPIMPL->associations.end();++i) {
            if (first) first=false; else stream << "&";
            stream << encode((*i).first) << "=" << encode((*i).second);
        }
        
        return stream.str();
    }

    /*
    ** FullPath
    */
    struct URIFullPathPrivate {
        URI::Path path;
        URI::Query query;
        std::string fragment;
    };

    URI::FullPath::FullPath() 
        : iPIMPL(new URIFullPathPrivate)
    {
    }
    URI::FullPath::FullPath(const FullPath &aOther) 
        : iPIMPL(new URIFullPathPrivate)
    {
        iPIMPL->path = aOther.iPIMPL->path;
        iPIMPL->query = aOther.iPIMPL->query;
        iPIMPL->fragment = aOther.iPIMPL->fragment;
    }
    URI::FullPath::~FullPath() {
        delete iPIMPL;
    }

    URI::FullPath& URI::FullPath::operator=(const FullPath &aOther) {
        iPIMPL->path = aOther.iPIMPL->path;
        iPIMPL->query = aOther.iPIMPL->query;
        iPIMPL->fragment = aOther.iPIMPL->fragment;

        return *this;
    }

    const URI::Path& URI::FullPath::path() const {
        return iPIMPL->path;
    }    
    URI::Path& URI::FullPath::path() {
        return iPIMPL->path;
    }    
    const URI::Query& URI::FullPath::query() const {
        return iPIMPL->query;
    }
    URI::Query& URI::FullPath::query() {
        return iPIMPL->query;
    }
    const std::string& URI::FullPath::fragment() const {
        return iPIMPL->fragment;
    }
    std::string URI::FullPath::toString() const {
        std::ostringstream stream;
        
        if (!path().empty()) {
            stream << "/" << path();
        }
        
        if (!query().empty()) {
            stream << "?" << query();
        }
        
        return stream.str();
    }
    URIFullPathPrivate *URI::FullPath::pimpl() {
        return iPIMPL;
    }
    
    /*
    ** URI
    */
    struct URIPrivate {
        std::string scheme;
        URI::Authority authority;
        URI::FullPath fullpath;
    };
    URI::URI() 
        : iPIMPL(new URIPrivate)
    {
    }
    URI::URI(const URI &aURI)
        : iPIMPL(new URIPrivate)
    {
        iPIMPL->scheme = aURI.iPIMPL->scheme;
        iPIMPL->authority = aURI.iPIMPL->authority;
        iPIMPL->fullpath = aURI.iPIMPL->fullpath;
    }
    URI::URI(const std::string &aURI) 
        : iPIMPL(new URIPrivate)
    {
        size_t r,pos=0;
        
        r=aURI.find_first_of("://",pos);
        if (r != std::string::npos) {
            setScheme(aURI.substr(pos,r-pos));
            pos = r+3;
        }
        
        r=aURI.find_first_of('/',pos);
        if (r != std::string::npos) {
            setAuthority(aURI.substr(pos,r-pos));
            pos = r+1;
        } else {
            setAuthority(aURI.substr(pos));
            return;
        }

        r=aURI.find_first_of('?',pos);
        if (r != std::string::npos) {
            setPath(aURI.substr(pos,r-pos));
            pos = r+1;
            r=aURI.find_first_of('#',pos);
            if (r != std::string::npos) {
                setQuery(aURI.substr(pos,r-pos));
                setFragment(aURI.substr(r+1));
            } else {
                setQuery(aURI.substr(pos));
            }
        } else {
            r=aURI.find_first_of('#',pos);
            if (r != std::string::npos) {
                setPath(aURI.substr(pos,r-pos));
                setFragment(aURI.substr(r+1));
            } else {
                setPath(aURI.substr(pos));
            }
        }
    }

    URI& URI::operator=(const URI &aURI) {
        iPIMPL->scheme = aURI.iPIMPL->scheme;
        iPIMPL->authority = aURI.iPIMPL->authority;
        iPIMPL->fullpath = aURI.iPIMPL->fullpath;

        return *this;
    }

    URI::~URI() {
        delete iPIMPL;
    }

    const std::string& URI::scheme() const {
        return iPIMPL->scheme;
    }
    void URI::setScheme(const std::string &aScheme) {
        iPIMPL->scheme = aScheme;
    }

    void URI::setAuthority(const Authority &aAuthority) {
        iPIMPL->authority = aAuthority;
    }
    const URI::Authority& URI::authority() const {
        return iPIMPL->authority;
    }
    URI::Authority& URI::authority() {
        return iPIMPL->authority;
    }
    
    void URI::setPath(const Path &aPath) {
        iPIMPL->fullpath.pimpl()->path = aPath;
    }
    const URI::Path& URI::path() const {
        return iPIMPL->fullpath.pimpl()->path;
    }
    URI::Path& URI::path() {
        return iPIMPL->fullpath.pimpl()->path;
    }

    void URI::setQuery(const Query &aQuery) {
        iPIMPL->fullpath.pimpl()->query = aQuery;
    }
    const URI::Query& URI::query() const {
        return iPIMPL->fullpath.pimpl()->query;
    }
    URI::Query& URI::query() {
        return iPIMPL->fullpath.pimpl()->query;
    }

    void URI::setFragment(const std::string &aFragment) {
        iPIMPL->fullpath.pimpl()->fragment = aFragment;
    }
    
    const std::string& URI::fragment() const {
        return iPIMPL->fullpath.pimpl()->fragment;
    }

    const URI::FullPath& URI::fullpath() const {
        return iPIMPL->fullpath;
    }

    std::string URI::toString() const {
        std::ostringstream stream;
        
        if (!scheme().empty()) {
            stream << scheme() << "://";
        }
        
        stream << authority();

        if (!path().empty()) {
            stream << "/" << path();
        }

        if (!query().empty()) {
            stream << "?" << query();
        }
        
        if (!fragment().empty()) {
            stream << "#" << fragment();
        }

        return stream.str();
    }


    std::string URI::encode(const std::string &aURLstring) {
        char buf[4];
        std::string result;
        std::string::const_iterator i;
        
        result.reserve(aURLstring.length());
        
        for(i=aURLstring.begin();i!=aURLstring.end();++i) {
            switch(*i) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case 'a': case 'b': case 'c': case 'd': case 'e':
            case 'f': case 'g': case 'h': case 'i': case 'j':
            case 'k': case 'l': case 'm': case 'n': case 'o':
            case 'p': case 'q': case 'r': case 's': case 't':
            case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E':
            case 'F': case 'G': case 'H': case 'I': case 'J':
            case 'K': case 'L': case 'M': case 'N': case 'O':
            case 'P': case 'Q': case 'R': case 'S': case 'T':
            case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            case '-': case '.': case '_': case '~':
                result += *i;
                break;
            default:
                snprintf(buf,4,"%%%02X",((unsigned int)(*i))&0xff);
                result.append(buf,3);
                break;
            }
        }
        return result;
    }
    std::string URI::decode(const std::string &aURLstring) {
        std::string result;
        
        std::string::const_iterator i;
        
        for(i=aURLstring.begin();i!=aURLstring.end();++i) {
            if (*i == '%' && isxdigit(*(i+1)) && isxdigit(*(i+2))) {
                char *ptr,hexstr[3] = { *(i+1), *(i+2), 0 };
                result += (char)(strtoul(hexstr,&ptr,16)&0xff);
                i+=2;
            } else {
                result += *i;
            }
        }
        
        return result;
    }

    /*
    ** URL
    */
    URL::URL() 
    {
    }

    URL::URL(const URL &aURL)
        : URI(aURL)
    {}

    URL::URL(const char *aURLString) 
        : URI(aURLString)
    {
        initializeDefaults();
    }
    
    URL::URL(const std::string &aURLString) 
        : URI(aURLString)
    {
        initializeDefaults();
    }

    URL& URL::operator=(const URL &aURL) {
        URI::operator=(aURL);
        return *this;
    }
    
    void URL::initializeDefaults() {
        if (authority().port().empty()) {
            authority().setDefaultPort(scheme());
        }
        
    }
    
} /* namespace QtC */

std::ostream& operator<<(std::ostream &os, const QtC::URI::UserInfo &aUserInfo) {
    return os << aUserInfo.toString();
}
std::ostream& operator<<(std::ostream &os, const QtC::URI::Authority &aAuthority) {
    return os << aAuthority.toString();
}
std::ostream& operator<<(std::ostream &os, const QtC::URI::Path &aPath) {
    return os << aPath.toString();
}
std::ostream& operator<<(std::ostream &os, const QtC::URI::Query &aQuery) {
    return os << aQuery.toString();
}
std::ostream& operator<<(std::ostream &os, const QtC::URI::FullPath &aFullPath) {
    return os << aFullPath.toString();
}
std::ostream& operator<<(std::ostream &os, const QtC::URI &aURI) {
    return os << aURI.toString();
}

QtC::URI::Path& operator<<(QtC::URI::Path &aPath, const std::string &aElement) {
    aPath.addElement(aElement);
    return aPath;
}
QtC::URI::Path& operator<<(QtC::URI::Path &aPath, const QtC::URI::Path &aOther) {
    for(size_t n=0;n<aOther.depth();++n) {
        aPath.addElement(aOther[n]);
    }
    return aPath;
}
