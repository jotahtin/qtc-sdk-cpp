/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/EDS/Collection.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage Collection
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_EDS_COLLECTION_H
#define QTC_EDS_COLLECTION_H

#include <string>
#include <functional>

#include <memory>
#include <istream>

#include <boost/system/error_code.hpp>

#include <QtC/Common/JSON.h>

namespace QtC {

    class EDS;
    
    typedef boost::system::error_code ErrorCode;

    class FileUploadStream {
    public:
        FileUploadStream();
        FileUploadStream(const FileUploadStream &aOther);
        FileUploadStream(std::shared_ptr<std::istream> aInputStream);
        ~FileUploadStream();

        std::shared_ptr<std::istream> stream();
        
        FileUploadStream& setFilename(const std::string &aFilename);
        const std::string& filename() const;

        FileUploadStream& setContentType(const std::string &aContentType);
        const std::string& contentType() const;
    private:
        struct FileUploadStreamPrivate *iPIMPL;
    };

    
    class Collection {
        friend class EDS;
    public:
        typedef std::function<void (const boost::system::error_code& aError, 
                                    JSON::Value aValue)> Callback;
        typedef std::function<void (const boost::system::error_code& aError, 
                                    JSON::Value aValue)> FileDownloadCallback;
    public:
        Collection();
        Collection(const Collection &aOther);
    protected:
        Collection(EDS &aEDS,const std::string &aCollectionName);
    public:
        ~Collection();
       
        Collection& operator=(const Collection &aOther);

        bool isValid() const;        
        const std::string& collectionName() const;
        
        /* Asynchronous API's */
        void find(const JSON::Object &aQuery, Callback aCallback);
        void findOne(const std::string &aObjectId, Callback aCallback);
        void insert(const JSON::Object &aValue, Callback aCallback);
        void update(const std::string &aObjectId, const JSON::Object &aValue, Callback aCallback);
        void remove(const std::string &aObjectId, Callback aCallback);

        void attachFile(const std::string &aObjectId, 
                        const std::string &aPropertyName,
                        FileUploadStream aFileReader,
                        Callback aCallback);
        void removeFile(const std::string &aObjectId, 
                        const std::string &aPropertyName,
                        Callback aCallback);
        void downloadFile(const std::string &aFileId, 
                          FileDownloadCallback aCallback,
                          const std::string &aVariant=std::string());
        
        void getFileInfo(const std::string &aFileId, 
                         Callback aCallback);
        void getFileDownloadUrl(const std::string &aFileId, 
                                Callback aCallback,
                                const std::string &aVariant=std::string());
        

#if 0

/**
 * Download file.
 * @param {string} fileId - A file id
 * @param {string} filePath - A path where file will be stored on local filesystem. You can also rename the file here.
 * @param {string} [variant] - A file variant name
 * @param {function} cb - A callback function to be called with query results
 */
Collection.prototype.downloadFile = function(fileId, filePath, variant, cb){
    if(!cb){
        if(!variant || (variant == "")){
            variant = noop;
        } else {
            cb = variant;
            variant = "";
        }
    }

    var self = this;

    // sanitize filePath
    filePath = path.normalize(filePath);

    this.getFileDownloadUrl(fileId, variant, function(e,res){
       if(!e){
           var originalFileName = path.basename(url.parse(res.expiringUrl).pathname);
           if ((filePath.charAt(filePath.length - 1) == "/") || (filePath.charAt(filePath.length - 1) == "\\")) {
               filePath = filePath + originalFileName;
           }

           var req = {
               address: res.expiringUrl,
               headers: self.db.getApiHeaders(),
               filePath: filePath
           };
           if(this.debug) console.log(req);

           edsFileDownloadRequest(req, cb);
       } else {
           cb(e, res);
       }
    });
}
#endif
        /*
          var edsFileDownloadRequest = function(options, callback){
          callback = callback || noop;
          
          request({
          method: "GET",
          url: options.address,
          headers: options.headers
          }, function(e, r, body) {
          if(!e && (r.statusCode == 200)){
          callback(null, body);
          } else {
          callback(r.statusCode);
          }
          }).pipe(fs.createWriteStream(options.filePath));
          };
          
          module.exports = {
          restRequest: restRequest,
          edsFileUploadRequest: edsFileUploadRequest,
          edsFileDownloadRequest: edsFileDownloadRequest
          }
        */
    private:
        struct CollectionPrivate *iPIMPL;
    };
    
} /* namespace QtC */

#endif /* QTC_EDS_COLLECTION_H */
