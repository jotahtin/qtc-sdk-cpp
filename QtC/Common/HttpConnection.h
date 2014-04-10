/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/HttpConnection.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage Collection
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_COMMON_CONNECTION_H
#define QTC_COMMON_CONNECTION_H

#include <iostream>
#include <memory>
#include <functional>
#include <list>

#include <boost/system/error_code.hpp>

#include <QtC/Common/URI.h>
#include <QtC/Common/JSON.h>

namespace QtC {

    class HttpReply {
    public:
        typedef std::shared_ptr<HttpReply> var;
        typedef std::pair<std::string, std::string> Header;
        typedef std::list< Header > Headers;
    protected:
        HttpReply();
    public:
        virtual int status() = 0;
        virtual const Headers& headers() = 0;
        virtual const std::string& body() const = 0;        
    };

    class HttpFormData {
    public:
        typedef std::shared_ptr<HttpFormData> var;
    protected:
        HttpFormData();
    public:
        virtual std::string contentType() const = 0;

        virtual void append(const std::string &aName,
                            const std::string &aValue) = 0;
        virtual void append(const std::string &aName,
                            std::shared_ptr<std::istream> aInputStream,
                            const std::string &aFilename = std::string(),
                            const std::string &aContentType = std::string()) = 0;
    public:
        /* Global getter */
        static HttpFormData::var get();
    };
    
    class HttpRequest {
    public:
        typedef std::shared_ptr<HttpRequest> var;
        typedef std::function< void(const boost::system::error_code &aError,
                                    HttpReply::var aReply) > Callback;
        enum Method {
            MethodGet,
            MethodPost,
            MethodPut,
            MethodDelete
        };
    protected:
        HttpRequest();
    public:
        
        virtual void addHeader(const std::string &aName,
                               const std::string &aValue) = 0;
        virtual void removeHeader(const std::string &aName) = 0;

        virtual void setContentType(const std::string &aContentType) = 0;
        virtual void setContentLength(size_t aContentLength) = 0;
        
        virtual void setBody(const std::string &aBody) = 0;
        virtual void setBody(const JSON::Object &aValue) = 0;
        virtual void setBody(HttpFormData::var aFormData) = 0;
        
        virtual std::string toString() const = 0;
    public:
        /* Global getter */
        static HttpRequest::var get(Method aMethod,
                                    const URI::FullPath &aRequestPath);
        
        /* get Get/Post/Delete Request */
        static HttpRequest::var getGet(const URI::FullPath &aRequestPath);
        static HttpRequest::var getPost(const URI::FullPath &aRequestPath);
        static HttpRequest::var getPut(const URI::FullPath &aRequestPath);
        static HttpRequest::var getDelete(const URI::FullPath &aRequestPath);
    };
    
    class HttpConnection {
    public:
        typedef std::shared_ptr<HttpConnection> var;
    protected:
        HttpConnection();
    public:
        virtual bool isConnected() const = 0;

        virtual void query(HttpRequest::var aRequest,
                           HttpRequest::Callback aCallback) = 0;
    public:
        static HttpConnection::var get(const URL &aURL);
    };
    
    class HttpConnectionPool : public std::enable_shared_from_this<HttpConnectionPool> {
    public:
        typedef std::shared_ptr<HttpConnectionPool> var;
    protected:
        /* 
        ** HttpConnection pool will be shared between different collections, etc.
        ** Enforcing allocation as shared heap object.
        ** Use global getter to get new instance.
        */
        HttpConnectionPool();
    public:        
        /* URL */
        virtual const URL& url() const = 0;

        /*
        ** Get available connection from pool
        */
        virtual HttpConnection::var getConnection() = 0;
        virtual void releaseConnection(HttpConnection::var aConnection) = 0;
    public:
        static HttpConnectionPool::var get(const URL &aURL);
    };
    
} /* namespace QtC */

#endif /* QTC_COMMON_CONNECTION_H */
