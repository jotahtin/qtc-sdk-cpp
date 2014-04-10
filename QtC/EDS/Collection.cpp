/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/EDS/Collection.cpp
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage Collection
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#include "QtC/Common/URI.h"

#include "QtC/EDS/Collection.h"
#include "QtC/EDS/EDS.h"
#include "QtC/EDS/EDSPrivate.h"

namespace QtC {

    /*
    ** FileUploadStream
    */
    struct FileUploadStreamPrivate {
        std::shared_ptr<std::istream> inputStream;
        std::string filename;
        std::string contentType;
    };
    FileUploadStream::FileUploadStream() 
        : iPIMPL(new FileUploadStreamPrivate)
    {
    }
    FileUploadStream::FileUploadStream(const FileUploadStream &aOther) 
        : iPIMPL(new FileUploadStreamPrivate)
    {
        iPIMPL->inputStream = aOther.iPIMPL->inputStream;
        iPIMPL->filename = aOther.iPIMPL->filename;
        iPIMPL->contentType = aOther.iPIMPL->contentType;
    }
    FileUploadStream::FileUploadStream(std::shared_ptr<std::istream> aInputStream)
        : iPIMPL(new FileUploadStreamPrivate)
    {
        iPIMPL->inputStream = aInputStream;
    }
    FileUploadStream::~FileUploadStream() {
        delete iPIMPL;
    }
    std::shared_ptr<std::istream> FileUploadStream::stream() {
        return iPIMPL->inputStream;
    }

    FileUploadStream& FileUploadStream::setFilename(const std::string &aFilename) {
        iPIMPL->filename = aFilename;
        return *this;
    }
    const std::string& FileUploadStream::filename() const {
        return iPIMPL->filename;
    }

    FileUploadStream& FileUploadStream::setContentType(const std::string &aContentType) {
        iPIMPL->contentType = aContentType;
        return *this;
    }
    const std::string& FileUploadStream::contentType() const {
        return iPIMPL->contentType;
    }

    /*
    ** Collection
    */
    struct CollectionPrivate {
        CollectionPrivate()
            : eds(nullptr),
              objectsPath("v1/objects"),
              filesPath("v1/files")
        {}

        
        HttpRequest::var prepareRequest(HttpRequest::var request);
        void restRequest(HttpRequest::var aRequest, Collection::Callback aCallback);

        struct EDSPrivate *eds;
        std::string collectionName;

        URI::Path objectsPath;
        URI::Path filesPath;
    };

    HttpRequest::var CollectionPrivate::prepareRequest(HttpRequest::var request) {
        if (eds) {
            const URL &url = eds->connectionPool->url();
            const URL::Authority &authority = url.authority();
            
            request->addHeader("Host",               authority.hostname());
            request->addHeader("Accept-Encoding",    "*"                 );
            request->addHeader("User-Agent",         "qtc-sdk-cpp/1.0"   );
            request->addHeader("Enginio-Backend-Id", eds->backendId      );
        }
        return request;
    }
    
    void CollectionPrivate::restRequest(HttpRequest::var aRequest, Collection::Callback aCallback) {
        HttpConnectionPool::var pool;
        HttpConnection::var connection;

        if (eds == nullptr) {
            // TODO Improve error code
            aCallback(boost::system::error_code(),JSON::Value());
            return;
        }

        pool = eds->connectionPool;
        if (pool) {
            connection = pool->getConnection();
        }

        if (!connection) {
            // TODO Improve error code
            aCallback(boost::system::error_code(),JSON::Value());
            return;
        }
        
        connection->query(aRequest, [pool,connection,aCallback](const boost::system::error_code& aError,
                                                                HttpReply::var aReply)
                          {
                              if (aError) {
                                  if (aCallback) {
                                      aCallback(aError,JSON::Value());
                                  }
                              } else {
                                  if (aCallback) {
                                      aCallback(aError,JSON::parseString(aReply->body()));
                                  }
                                  pool->releaseConnection(connection);
                              }
                          });
    }
    
    Collection::Collection() 
        : iPIMPL(new CollectionPrivate)
    {
    }
    
    Collection::Collection(const Collection &aOther) 
        : iPIMPL(new CollectionPrivate)
    {
        iPIMPL->eds=aOther.iPIMPL->eds;
    }
    
    Collection::Collection(EDS &aEDS,const std::string &aCollectionName) 
        : iPIMPL(new CollectionPrivate)
    {
        iPIMPL->eds = aEDS.pimpl();
        iPIMPL->collectionName = aCollectionName;
    }

    Collection::~Collection() {
        delete iPIMPL;
    }
    
    Collection& Collection::operator=(const Collection &aOther) {
        iPIMPL->eds=aOther.iPIMPL->eds;
        return *this;
    }

    bool Collection::isValid() const {
        return iPIMPL->eds != nullptr;
    }

    const std::string& Collection::collectionName() const {
        return iPIMPL->collectionName;
    }
                
    void Collection::find(const JSON::Object &aQuery,
                          //const std::string &aQuery,
                          // options,
                          Callback aCallback)
    {
        if (!isValid()) {
            return;
        }
        
        HttpConnectionPool::var pool;
        HttpConnection::var connection;

        pool = iPIMPL->eds->connectionPool;
        connection = pool->getConnection();
        
        URI uri;
        uri.path() << iPIMPL->objectsPath << iPIMPL->collectionName;
        if (aQuery.size()>0) {
            uri.query().addAssociation("q",aQuery.toString());
        }
        
        HttpRequest::var request;
        request=iPIMPL->prepareRequest(HttpRequest::getGet(uri));
        
        std::cout << "URI = " << uri.fullpath() << "\n";

        /*
        // create query string
        var qsObj = {};
        qsObj.q = JSON.stringify(query);
        if(options.limit) qsObj.limit = options.limit;
        if(options.offset) qsObj.offset = options.offset;
        if(options.sort) qsObj.sort = JSON.stringify(options.sort);
        if(options.count) qsObj.count = 1;
        if(options.include) qsObj.include = JSON.stringify(options.include);
        */

        connection->query(request, [pool,connection,aCallback](const boost::system::error_code& aError,
                                                               HttpReply::var aReply)
                          {
                              if (aError) {
                                  aCallback(aError,JSON::Value());
                              } else {
                                  //std::cout << "Got reply" << std::endl;
                                  //std::cout << aReply->body() << std::endl;
                                  aCallback(aError,JSON::parseString(aReply->body()));
                              }
                              
                              pool->releaseConnection(connection);
                          });
        
    }

    void Collection::findOne(const std::string &aObjectId, Callback aCallback) {
        URI uri;
        uri.path() << iPIMPL->objectsPath << iPIMPL->collectionName << aObjectId;
        
        iPIMPL->restRequest(iPIMPL->prepareRequest(HttpRequest::getGet(uri)), 
                            aCallback);
    }
    
    void Collection::insert(const JSON::Object &aValue, Callback aCallback) {
        URI uri;
        uri.path() << iPIMPL->objectsPath << iPIMPL->collectionName;

        HttpRequest::var request;
        request=iPIMPL->prepareRequest(HttpRequest::getPost(uri));
        request->setBody(aValue);
        
        iPIMPL->restRequest(request, aCallback);
    }

    void Collection::update(const std::string &aObjectId, const JSON::Object &aValue, Callback aCallback) {
        URI uri;
        uri.path() << iPIMPL->objectsPath << iPIMPL->collectionName << aObjectId;
        
        HttpRequest::var request;
        request=iPIMPL->prepareRequest(HttpRequest::getPut(uri));        
        request->setBody(aValue.toString());
        
        iPIMPL->restRequest(request, aCallback);
    }

    void Collection::remove(const std::string &aObjectId,
                            Callback aCallback)
    {
        URI uri;
        uri.path() << iPIMPL->objectsPath << iPIMPL->collectionName << aObjectId;
        
        HttpRequest::var request;
        request=iPIMPL->prepareRequest(HttpRequest::getDelete(uri));
        
        iPIMPL->restRequest(request, aCallback);
    }

    void Collection::attachFile(const std::string &aObjectId, 
                                const std::string &aPropertyName,
                                FileUploadStream aFileReader,
                                Callback aCallback)
    {
        URI uri;
        uri.path() << iPIMPL->filesPath;
        
        HttpRequest::var request;
        request=iPIMPL->prepareRequest(HttpRequest::getPost(uri));

        HttpFormData::var form = HttpFormData::get();
        JSON::Object objData({
                { JSON::String("targetFileProperty"), 
                        JSON::Object({JSON::String("id") | aObjectId,
                                    JSON::String("objectType") | std::string("objects.")
                                                                 +collectionName(),
                                    JSON::String("propertyName") | aPropertyName
                                    })
                        }
            });
        form->append("object", objData.toString());
        form->append("file", 
                     aFileReader.stream(),
                     aFileReader.filename(),
                     aFileReader.contentType());
        
        request->setBody(form);
        
        iPIMPL->restRequest(request, aCallback);
    }

    void Collection::removeFile(const std::string &aObjectId, 
                                const std::string &aPropertyName,
                                Callback aCallback)
    {
        update(aObjectId, 
               JSON::Object({ { JSON::String(aPropertyName), JSON::Value() } }),
               aCallback);
    }
    
    void Collection::downloadFile(const std::string &aFileId, 
                                  FileDownloadCallback aCallback,
                                  const std::string &aVariant)
    {
        getFileDownloadUrl(aFileId,
                           [] (const boost::system::error_code& aError,JSON::Value aValue) {
                               using namespace std;
                               if (aError) {
                                   cerr << "Error: " << aError.message() << endl;
                                   //aCallback(aError,JSON::Value());
                                   return;
                               }
                               
                               cout << "Got Result" << endl;
                               cout << aValue["expiringUrl"] << endl;
                           }, aVariant);
    }

    void Collection::getFileInfo(const std::string &aFileId, 
                                 Callback aCallback)
    {
        URI uri;
        uri.path() << iPIMPL->filesPath << aFileId;
        
        iPIMPL->restRequest(iPIMPL->prepareRequest(HttpRequest::getGet(uri)), aCallback);
    }
    
    void Collection::getFileDownloadUrl(const std::string &aFileId, 
                                        Callback aCallback,
                                        const std::string &aVariant)
    {
        URI uri;
        uri.path() << iPIMPL->filesPath << aFileId << "download_url";
        if (!aVariant.empty()) {
            uri.query().addAssociation("variant",aVariant);
        }
        
        iPIMPL->restRequest(iPIMPL->prepareRequest(HttpRequest::getGet(uri)), aCallback);
    }
    
} /* namespace QtC */
