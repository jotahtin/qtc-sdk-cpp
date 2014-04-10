/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/HttpConnection.cpp
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage Collection
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <list>
#include <deque>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "QtC/Common/Base64.h"
#include "QtC/Common/HttpConnection.h"

using namespace std;

namespace QtC {
    
    /*
    ** HttpConnectionWorker
    */
    class HttpConnectionWorker {
    public:
        HttpConnectionWorker();
        ~HttpConnectionWorker();
        
        void start();
        void stop();

        void svc();
        
        inline boost::asio::io_service& service() { return iIOService; }
        
        static std::shared_ptr<HttpConnectionWorker> getSharedSingleton();
    private:
        bool iActive;
        std::thread *iThread;
        boost::asio::io_service iIOService;
    private:
        static std::mutex gSharedMutex;
        static std::weak_ptr<HttpConnectionWorker> gSharedSingleton;
    };
        
    HttpConnectionWorker::HttpConnectionWorker() 
        : iActive(false), iThread(nullptr)
    {
    }
    
    HttpConnectionWorker::~HttpConnectionWorker() {
        std::lock_guard<std::mutex> lock(gSharedMutex);
        stop();
    }
    
    void HttpConnectionWorker::start() {
        printf("START\n");
        iActive = true;
        iThread = new std::thread(std::bind(&HttpConnectionWorker::svc,this));
    }
    
    void HttpConnectionWorker::stop() {
        printf("STOP\n");
        if (iThread == nullptr)
            return;
        iActive = false;
        iIOService.stop();
        iThread->join();
        delete iThread;
        iThread = nullptr;
    }
    void HttpConnectionWorker::svc() {
        //cout << "SVC Entry" << endl;
        while(iActive) {
            iIOService.run();
            if (!iActive) 
                break;
            
            iIOService.reset();

            // TODO: improve...
            sleep(1);
        }
        //cout << "SVC Exit" << endl;
    }
    
    std::mutex HttpConnectionWorker::gSharedMutex;
    std::weak_ptr<HttpConnectionWorker> HttpConnectionWorker::gSharedSingleton;
    
    std::shared_ptr<HttpConnectionWorker> HttpConnectionWorker::getSharedSingleton() {
        std::shared_ptr<HttpConnectionWorker> worker;
        std::lock_guard<std::mutex> lock(gSharedMutex);
        
        if (!(worker = gSharedSingleton.lock())) {
            worker = std::make_shared<HttpConnectionWorker>();
            gSharedSingleton = worker;
            worker->start();
        }
        
        return worker;
    }
    
    /*
    ** HttpReply
    */  
    class HttpReplyPrivate : public HttpReply {
    public:
        typedef std::shared_ptr<HttpReplyPrivate> var;
    public:
        HttpReplyPrivate();
    public:
        virtual int status();
        virtual const Headers& headers();
        virtual const std::string& body() const;
    public:
        void setStatus(int aStatus);
        void addHeader(const std::string &aName,
                       const std::string &aValue);        
    public:
        /* Direct access to body buffer */
        std::string& directBody() { return iBody; }
    private:
        int iStatus;
        Headers iHeaders;
        std::string iBody;
    };

    HttpReplyPrivate::HttpReplyPrivate() 
        : iStatus(0) 
    {
    }

    int HttpReplyPrivate::status() {
        return iStatus;
    }
    const HttpReply::Headers& HttpReplyPrivate::headers() {
        return iHeaders;
    }
    const std::string& HttpReplyPrivate::body() const {
        return iBody;
    }
    
    void HttpReplyPrivate::setStatus(int aStatus) {
        iStatus = aStatus;
    }
    void HttpReplyPrivate::addHeader(const std::string &aName,
                                     const std::string &aValue)
    {
        iHeaders.push_back( Header(aName,aValue) );
    }
    
    HttpReply::HttpReply() {}

    /*
    ** HttpFormData
    */
    class HttpFormDataPrivate : public HttpFormData {
    public:
        typedef std::shared_ptr<HttpFormDataPrivate> var;
    public:
        class Value {
        public:
            typedef std::shared_ptr<Value> var;
        public:
            virtual void printOut(std::ostream &os,
                                  const std::string &aBoundary,
                                  const std::string &aName) const = 0;
        };

        typedef std::pair<std::string, Value::var> Field;
        typedef std::vector<Field> Fields;

        class ValueString : public Value {
        public:
            ValueString(const std::string &aString)
                : iString(aString)
            {}
        public:
            virtual void printOut(std::ostream &os,
                                  const std::string &aBoundary,
                                  const std::string &aName) const 
            {
                os << "--" << aBoundary << "\r\n"
                   << "Content-Disposition: form-data; name=\"" << aName << "\"\r\n\r\n"
                   << iString << "\r\n";
            }
        public:
            static ValueString::var get(const std::string &aString) {
                return std::make_shared<ValueString>(aString);
            }
        private:
            std::string iString;
        };

        class ValueStream : public Value {
        public:
            ValueStream(std::shared_ptr<std::istream> &aInputStream,
                        const std::string &aFilename,
                        const std::string &aContentType)
                : iInputStream(aInputStream),
                  iFilename(aFilename),
                  iContentType(aContentType)
            {}
        public:
            virtual void printOut(std::ostream &os,
                                  const std::string &aBoundary,
                                  const std::string &aName) const 
            {
                os << "--" << aBoundary << "\r\n"
                   << "Content-Disposition: form-data; name=\"" << aName << "\"";
                
                if (!iFilename.empty()) {
                    os << "; filename=\"" << iFilename << "\"";
                }
                os << "\r\n";

                if (!iContentType.empty()) {
                    os<< "Content-Type: " << iContentType << "\r\n";
                }

                os << "Content-Transfer-Encoding: " << "binary" << "\r\n"; // Optional ?

                os << "\r\n";
                
                if (iInputStream) {
                    char buffer[8192];
                    while(*iInputStream) {
                        iInputStream->read(buffer,sizeof(buffer));
                        os.write(buffer,iInputStream->gcount());
                    }
                }
            }
        public:
            static ValueStream::var get(std::shared_ptr<std::istream> &aInputStream,
                                        const std::string &aFilename,
                                        const std::string &aContentType) 
            {
                return std::make_shared<ValueStream>(aInputStream,
                                                     aFilename,
                                                     aContentType);
            }
        private:
            std::shared_ptr<std::istream> iInputStream;
            std::string iFilename;
            std::string iContentType;
        };
    public:
        HttpFormDataPrivate();
    public:
        virtual std::string contentType() const;
        virtual void append(const std::string &aName,
                            const std::string &aValue);
        virtual void append(const std::string &aName,
                            std::shared_ptr<std::istream> aInputStream,
                            const std::string &aFilename = std::string(),
                            const std::string &aContentType = std::string());
    public:
        void printOut(std::ostream &os) const;
    private:
        std::string iBoundary;
        Fields iFields;
    };
    
    HttpFormDataPrivate::HttpFormDataPrivate() {
        /* Initialize form boundary */
        int n,salt[8];
        iBoundary = "---------------";
        srand(time(0));
        for(n=0;n<sizeof(salt)/sizeof(int);++n) {
            salt[n] = rand();
        }
        iBoundary += Base64::encode(std::string((const char *)salt,sizeof(salt)));
        if (iBoundary.length()>70) {
            iBoundary.resize(70); // boundary length <= 70 (RFC 2046, section 5.1.1).
        }
    }

    std::string HttpFormDataPrivate::contentType() const {
        return std::string("multipart/form-data; boundary=") + iBoundary;
    }
    
    void HttpFormDataPrivate::append(const std::string &aName,
                                     const std::string &aValue)
    {
        iFields.push_back(Field(aName,ValueString::get(aValue)));
    }
    
    void HttpFormDataPrivate::append(const std::string &aName,
                                     std::shared_ptr<std::istream> aInputStream,
                                     const std::string &aFilename,
                                     const std::string &aContentType)
    {
        iFields.push_back(Field(aName,ValueStream::get(aInputStream,
                                                       aFilename,
                                                       aContentType)));
    }

    void HttpFormDataPrivate::printOut(std::ostream &os) const {
        Fields::const_iterator i;
        
        for(i=iFields.begin();i!=iFields.end();++i) {
            (*i).second->printOut(os,iBoundary,(*i).first);
        }
        
        os << "--" << iBoundary << "--\r\n";
    }
    
    HttpFormData::HttpFormData() {}
    HttpFormData::var HttpFormData::get() {
        return std::make_shared<HttpFormDataPrivate>();
    }
    
    /*
    ** HttpRequest
    */
    class HttpRequestPrivate : public HttpRequest {
    public:
        typedef std::pair<std::string, std::string> Header;
    public:
        HttpRequestPrivate(Method aMethod,
                           const URI::FullPath &aRequestPath);
        
    public:
        virtual void addHeader(const std::string &aName,
                               const std::string &aValue);
        virtual void removeHeader(const std::string &aName);

        virtual void setContentType(const std::string &aContentType);
        virtual void setContentLength(size_t aContentLength);

        virtual void setBody(const std::string &aBody);
        virtual void setBody(const JSON::Object &aValue);
        virtual void setBody(HttpFormData::var aFormData);

        virtual std::string toString() const;
    private:
        Method iMethod;
        URI::FullPath iRequestPath;
        std::list< Header > iHeaders;
        
        std::string iBody;
        HttpFormDataPrivate::var iFormData;
    };
    
    HttpRequestPrivate::HttpRequestPrivate(Method aMethod,
                                           const URI::FullPath &aRequestPath)
        : iMethod(aMethod), iRequestPath(aRequestPath)
    {
    }
    
    void HttpRequestPrivate::addHeader(const std::string &aName,
                                       const std::string &aValue)
    {
        removeHeader(aName);
        iHeaders.push_back( Header(aName,aValue) );
    }
    
    void HttpRequestPrivate::removeHeader(const std::string &aName) {
        std::list< Header >::iterator header;
        for(header=iHeaders.begin();header!=iHeaders.end();) {
            if ((*header).first==aName) {
                header=iHeaders.erase(header);
            } else {
                ++header;
            }
        }
    }

    void HttpRequestPrivate::setContentType(const std::string &aContentType) {
        const std::string name="Content-Type";
        removeHeader(name);
        addHeader(name,aContentType);
    }

    void HttpRequestPrivate::setContentLength(size_t aContentLength) {
        const std::string name="Content-Length";
        removeHeader(name);
        char buf[16]; sprintf(buf,"%u",(unsigned int)aContentLength);
        addHeader(name,buf);
    }

    void HttpRequestPrivate::setBody(const std::string &aBody) {
        iBody = aBody;
        
        // Todo support : chunked_transfer_encoding 

        std::cout << iBody << "\n";
        //exit(0);
        

        setContentLength(iBody.empty()?0:iBody.size());
    }
    void HttpRequestPrivate::setBody(const JSON::Object &aValue) {
        setBody(aValue.toString());
    }
    void HttpRequestPrivate::setBody(HttpFormData::var aFormData) {
        iFormData = std::dynamic_pointer_cast<HttpFormDataPrivate>(aFormData);
        if (!iFormData) {
            /* ERROR */
            return;
        }

        setContentType(iFormData->contentType());

        // Todo support : chunked_transfer_encoding 

        // Immediate body construction
        std::ostringstream bodyStream;
        iFormData->printOut(bodyStream);
           
        setBody(bodyStream.str());
    }

    std::string HttpRequestPrivate::toString() const {
        std::stringstream request;
        
        // Header
        switch(iMethod) {
        case MethodGet:    request << "GET ";    break;
        case MethodPost:   request << "POST ";   break;
        case MethodPut:    request << "PUT ";   break;
        case MethodDelete: request << "DELETE "; break;
        }
        request << iRequestPath << " HTTP/1.1\r\n";
        
        std::list< Header >::const_iterator header;
        for(header=iHeaders.begin();header!=iHeaders.end();++header) {
            request << (*header).first << ": " << (*header).second << "\r\n";
        }
        
        request << "\r\n";

        if (!iBody.empty()) {
            request << iBody;
        }

        return request.str();
    }
    
    HttpRequest::HttpRequest() {}
    HttpRequest::var HttpRequest::get(Method aMethod,
                                      const URI::FullPath &aRequestPath) 
    {
        return std::make_shared<HttpRequestPrivate>(aMethod,aRequestPath);
    }
    HttpRequest::var HttpRequest::getGet(const URI::FullPath &aRequestPath) {
        return get(MethodGet,aRequestPath);
    }
    HttpRequest::var HttpRequest::getPost(const URI::FullPath &aRequestPath) {
        return get(MethodPost,aRequestPath);
    }
    HttpRequest::var HttpRequest::getPut(const URI::FullPath &aRequestPath) {
        return get(MethodPut,aRequestPath);
    }
    HttpRequest::var HttpRequest::getDelete(const URI::FullPath &aRequestPath) {
        return get(MethodDelete,aRequestPath);
    }

    /*
    ** HttpConnectionTask
    */
    class HttpConnectionTask {
    public:
        typedef std::shared_ptr<HttpConnectionTask> var;
    public:
        HttpConnectionTask(HttpRequest::var aRequest,
                           HttpRequest::Callback aCallback);
        
        HttpRequest::var request() { return iRequest; }
        HttpRequest::Callback callback() { return iCallback; }
        HttpReplyPrivate::var reply() { return iReply; }

        void received(const char *aData, size_t aLength);

        void finalizeTask();

        inline bool taskCompleted() const { return iTaskCompleted; }
    private:
        HttpRequest::var iRequest;
        HttpRequest::Callback iCallback;
        HttpReplyPrivate::var iReply;
        bool iStatusCompleted;
        bool iHeaderCompleted;
        bool iTaskCompleted;
        size_t iContentLength;
    };
    
    HttpConnectionTask::HttpConnectionTask(HttpRequest::var aRequest,
                                           HttpRequest::Callback aCallback)
        : iRequest(aRequest), iCallback(aCallback),
          iReply(std::make_shared<HttpReplyPrivate>()),
          iStatusCompleted(false),
          iHeaderCompleted(false),
          iTaskCompleted(false),
          iContentLength(0)
    {}
    
    void HttpConnectionTask::received(const char *aData, size_t aLength) {
        size_t r;
        
        std::string &body = iReply->directBody();
        body.append(aData,aLength);
        
        while(!iHeaderCompleted) {
            r=body.find("\r\n");
            if (r==std::string::npos) 
                break;
            
            std::string row = body.substr(0,r);
            body.erase(0,r+2);
            
            if (row.empty()) {
                iHeaderCompleted = true;
            } else if (iStatusCompleted) {
                r=row.find(": ");
                if (r!=std::string::npos) {
                    std::string name=row.substr(0,r);
                    std::string value=row.substr(r+2);
                    
                    iReply->addHeader(name,value);

                    if (name=="Content-Length") {
                        iContentLength=atoi(value.c_str());
                    }
                }
            } else {
                r=row.find(" ");
                if (r!=std::string::npos) {
                    iReply->setStatus(atoi(row.substr(r+1).c_str()));
                }
                iStatusCompleted = true;
            }
        }
        
        if (iContentLength != 0 && body.length() == iContentLength) {
            finalizeTask();
        }
    }
    
    void HttpConnectionTask::finalizeTask() {
        if (iTaskCompleted)
            return;
        iTaskCompleted = true;
        iCallback(boost::system::error_code(), iReply);
    }

    /*
    ** HttpConnectionPrivateBase
    */
    class HttpConnectionPrivateBase : public HttpConnection {
    public:
        HttpConnectionPrivateBase(const URL &aURL);
    public:
        virtual bool isConnected() const;
    protected:
        URL iURL;
        std::shared_ptr<HttpConnectionWorker> iWorker;
        boost::asio::io_service &iIOService;
        
        std::mutex iMutex;

        std::deque< HttpConnectionTask::var > iTasks;
        HttpConnectionTask::var iActiveTask;

        bool iIsConnected;
        char iReadBuffer[32768];
    };
    HttpConnectionPrivateBase::HttpConnectionPrivateBase(const URL &aURL)
        : iURL(aURL),
          iWorker(HttpConnectionWorker::getSharedSingleton()),
          iIOService(iWorker->service()),
          iIsConnected(false)
    {
    }
    
    bool HttpConnectionPrivateBase::isConnected() const {
        return iIsConnected;
    }

    /*
    ** HttpConnectionPrivate
    */
    class HttpConnectionPrivate : public HttpConnectionPrivateBase {
    public:
        HttpConnectionPrivate(const URL &aURL);
        ~HttpConnectionPrivate();
    public:
        virtual void query(HttpRequest::var aRequest,
                           HttpRequest::Callback aCallback);
    public:
        void process_next_task_L();
        void connect_L();
        void disconnect_L();
        void send_request_L();
        
        void active_task_failed(const boost::system::error_code& error);
    public:
        void handle_connect(const boost::system::error_code& error);
        void handle_write(const boost::system::error_code& error,
                          size_t bytes_transferred);
        void handle_read(const boost::system::error_code& error,
                         size_t bytes_transferred);
    private:
        boost::asio::ip::tcp::socket iSocket;
    };
    
    HttpConnectionPrivate::HttpConnectionPrivate(const URL &aURL)
        : HttpConnectionPrivateBase(aURL),
          iSocket(iIOService)
    {
    }
    
    HttpConnectionPrivate::~HttpConnectionPrivate() {
        std::lock_guard<std::mutex> lock(iMutex);
        disconnect_L();
    }
        
    void HttpConnectionPrivate::query(HttpRequest::var aRequest,
                                      HttpRequest::Callback aCallback)
    {
        std::lock_guard<std::mutex> lock(iMutex);
        iTasks.push_back( std::make_shared<HttpConnectionTask>(aRequest,aCallback) );
        
        if (iActiveTask) {
            return;
        }
        
        process_next_task_L();
    }

    void HttpConnectionPrivate::process_next_task_L() {
        if (iTasks.empty()) {
            iActiveTask = nullptr;
            return;
        } else {
            iActiveTask = iTasks.front();
            iTasks.pop_front();
        }
        
        if (isConnected()) {
            send_request_L();
        } else {
            connect_L();
        }
    }
    
    void HttpConnectionPrivate::connect_L() {
        const URL::Authority &authority = iURL.authority();

        // Resolve
        boost::asio::ip::tcp::resolver resolver(iIOService);
        boost::asio::ip::tcp::resolver::query query(authority.hostname(),
                                                    authority.port());
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        // Connect
        boost::asio::async_connect(iSocket, endpoint_iterator,
                                   boost::bind(&HttpConnectionPrivate::handle_connect, 
                                               this,
                                               boost::asio::placeholders::error));
    }
    
    void HttpConnectionPrivate::disconnect_L() {
        if (iSocket.is_open()) {
            iSocket.close();
        }
    }
    
    void HttpConnectionPrivate::send_request_L() {
        if (!iActiveTask) {
            /* No active task */
            return;
        }
        
        boost::asio::async_write(iSocket, 
                                 boost::asio::buffer(iActiveTask->request()->toString()),
                                 boost::bind(&HttpConnectionPrivate::handle_write, this, 
                                             boost::asio::placeholders::error, 
                                             boost::asio::placeholders::bytes_transferred));
    }
    
    void HttpConnectionPrivate::active_task_failed(const boost::system::error_code& error) {
        std::lock_guard<std::mutex> lock(iMutex);

        if (iActiveTask) {
            iActiveTask->callback()(error,nullptr);
        } else {
            /*
            std::cerr << "Task failure (without active task): " 
                      << error.message() << "\n";
            */
        }
        
        process_next_task_L();
    }
    
    void HttpConnectionPrivate::handle_connect(const boost::system::error_code& error) {
        if (!error) {
            iIsConnected = true;
            
            /* Start async reading chain. */
            iSocket.async_read_some(boost::asio::buffer(iReadBuffer, sizeof(iReadBuffer)),
                                    boost::bind(&HttpConnectionPrivate::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));

            send_request_L();
        } else {
            active_task_failed(error);
        }
    }

    void HttpConnectionPrivate::handle_write(const boost::system::error_code& error,
                                             size_t bytes_transferred)
    {
        if (!error) {
            // TODO...
            // cout << "Write completed..." << bytes_transferred << endl;	
            /*
            iSocket.async_read_some(boost::asio::buffer(iReadBuffer, sizeof(iReadBuffer)),
                                    boost::bind(&HttpConnectionPrivate::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
            */
        } else {
            active_task_failed(error);
        }
    }
  
    void HttpConnectionPrivate::handle_read(const boost::system::error_code& error,
                                            size_t bytes_transferred)
    {
        if (!error) {
            std::lock_guard<std::mutex> lock(iMutex);
            
            if (iActiveTask) {
                iActiveTask->received(iReadBuffer, bytes_transferred);
                
                if (iActiveTask->taskCompleted()) {
                    process_next_task_L();                    
                }
            }
            
            if (isConnected()) {
                iSocket.async_read_some(boost::asio::buffer(iReadBuffer, sizeof(iReadBuffer)),
                                        boost::bind(&HttpConnectionPrivate::handle_read, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
            }
        } else {
            active_task_failed(error);
        }
    }

    /*
    ** HttpsConnectionPrivate
    */
    class HttpsConnectionPrivate : public HttpConnectionPrivateBase {
    public:
        HttpsConnectionPrivate(const URL &aURL);
        ~HttpsConnectionPrivate();
    public:
        virtual void query(HttpRequest::var aRequest,
                           HttpRequest::Callback aCallback);
    public:
        void process_next_task_L();
        void connect_L();
        void disconnect_L();
        void send_request_L();
        
        void active_task_failed(const boost::system::error_code& error);
    public:
        void handle_connect(const boost::system::error_code& error);
        void handle_handshake(const boost::system::error_code& error);
        void handle_write(const boost::system::error_code& error,
                          size_t bytes_transferred);
        void handle_read(const boost::system::error_code& error,
                         size_t bytes_transferred);
        
        bool verify_certificate(bool preverified,
                                boost::asio::ssl::verify_context& ctx);
    private:
        boost::asio::ssl::context iSslContext;
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> iSocket;
    };
    HttpsConnectionPrivate::HttpsConnectionPrivate(const URL &aURL)
        : HttpConnectionPrivateBase(aURL),
          iSslContext(boost::asio::ssl::context::sslv23),
          iSocket(iIOService,iSslContext)
    {
        // TODO: improve.
        iSslContext.load_verify_file("/etc/ssl/certs/ca-certificates.crt");
    }
    
    HttpsConnectionPrivate::~HttpsConnectionPrivate() {
        std::lock_guard<std::mutex> lock(iMutex);
        disconnect_L();
    }

    void HttpsConnectionPrivate::query(HttpRequest::var aRequest,
                                      HttpRequest::Callback aCallback)
    {
        std::lock_guard<std::mutex> lock(iMutex);
        iTasks.push_back( std::make_shared<HttpConnectionTask>(aRequest,aCallback) );
        
        if (iActiveTask) {
            return;
        }
        
        process_next_task_L();
    }

    void HttpsConnectionPrivate::process_next_task_L() {
        if (iTasks.empty()) {
            iActiveTask = nullptr;
            return;
        } else {
            iActiveTask = iTasks.front();
            iTasks.pop_front();
        }
        
        if (isConnected()) {
            send_request_L();
        } else {
            connect_L();
        }
    }
    
    void HttpsConnectionPrivate::connect_L() {
        const URL::Authority &authority = iURL.authority();

        // Resolve
        boost::asio::ip::tcp::resolver resolver(iIOService);
        boost::asio::ip::tcp::resolver::query query(authority.hostname(),
                                                    authority.port());
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        // Setup socket
        iSocket.set_verify_mode(boost::asio::ssl::verify_peer);
        iSocket.set_verify_callback(boost::bind(&HttpsConnectionPrivate::verify_certificate,
                                                this, _1, _2));
        
        // Connect
        boost::asio::async_connect(iSocket.lowest_layer(), endpoint_iterator,
                                   boost::bind(&HttpsConnectionPrivate::handle_connect, 
                                               this,
                                               boost::asio::placeholders::error));
    }
    
    void HttpsConnectionPrivate::disconnect_L() {
        if (iSocket.lowest_layer().is_open()) {
            iSocket.lowest_layer().close();
        }
    }
    
    void HttpsConnectionPrivate::send_request_L() {
        if (!iActiveTask) {
            /* No active task */
            return;
        }
        
        boost::asio::async_write(iSocket, 
                                 boost::asio::buffer(iActiveTask->request()->toString()),
                                 boost::bind(&HttpsConnectionPrivate::handle_write, this, 
                                             boost::asio::placeholders::error, 
                                             boost::asio::placeholders::bytes_transferred));
    }
    
    void HttpsConnectionPrivate::active_task_failed(const boost::system::error_code& error) {
        std::lock_guard<std::mutex> lock(iMutex);

        if (iActiveTask) {
            iActiveTask->callback()(error,nullptr);
        } else {
            /*
            std::cerr << "Task failure (without active task): " 
                      << error.message() << "\n";
            */
        }
        
        process_next_task_L();
    }
    
    void HttpsConnectionPrivate::handle_connect(const boost::system::error_code& error) {
        if (!error) {
            iIsConnected = true;
            
            iSocket.async_handshake(boost::asio::ssl::stream_base::client,
                                    boost::bind(&HttpsConnectionPrivate::handle_handshake, this,
                                                boost::asio::placeholders::error));
        } else {
            active_task_failed(error);
        }
    }

    void HttpsConnectionPrivate::handle_handshake(const boost::system::error_code& error) {
        if (!error) {
            std::lock_guard<std::mutex> lock(iMutex);

            /* Start async reading chain. */
            iSocket.async_read_some(boost::asio::buffer(iReadBuffer, sizeof(iReadBuffer)),
                                    boost::bind(&HttpsConnectionPrivate::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));

            send_request_L();
        } else {
            active_task_failed(error);
        }
    }

    void HttpsConnectionPrivate::handle_write(const boost::system::error_code& error,
                                             size_t bytes_transferred)
    {
        if (!error) {
            // TODO...
            // cout << "Write completed..." << bytes_transferred << endl;	
            /*
            iSocket.async_read_some(boost::asio::buffer(iReadBuffer, sizeof(iReadBuffer)),
                                    boost::bind(&HttpsConnectionPrivate::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
            */
        } else {
            active_task_failed(error);
        }
    }
  
    void HttpsConnectionPrivate::handle_read(const boost::system::error_code& error,
                                            size_t bytes_transferred)
    {
        if (!error) {
            std::lock_guard<std::mutex> lock(iMutex);
            
            if (iActiveTask) {
                iActiveTask->received(iReadBuffer, bytes_transferred);
                
                if (iActiveTask->taskCompleted()) {
                    process_next_task_L();                    
                }
            }
            
            if (isConnected()) {
                iSocket.async_read_some(boost::asio::buffer(iReadBuffer, sizeof(iReadBuffer)),
                                        boost::bind(&HttpsConnectionPrivate::handle_read, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
            }
        } else {
            active_task_failed(error);
        }
    }
    
    bool HttpsConnectionPrivate::verify_certificate(bool preverified,
                                               boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.
        
        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        
        //TODO
        //std::cout << "Verifying " << subject_name << "\n";
        
        return preverified;
    }
    
    HttpConnection::HttpConnection() {}
    HttpConnection::var HttpConnection::get(const URL &aURL) {
        if (aURL.scheme() == "https") {
            return std::make_shared<HttpsConnectionPrivate>(aURL);
        } else {
            return std::make_shared<HttpConnectionPrivate>(aURL);
        }
    }
    
    /*
    ** HttpConnectionPool
    */
    class HttpConnectionPoolPrivate : public HttpConnectionPool {
    public:
        HttpConnectionPoolPrivate(const URL &aURL);
        
        virtual const URL& url() const;

        virtual std::shared_ptr<HttpConnection> getConnection();
        virtual void releaseConnection(HttpConnection::var aConnection);
    private:
        URL iURL;
        std::shared_ptr<HttpConnectionWorker> iWorker;
    };
    
    HttpConnectionPoolPrivate::HttpConnectionPoolPrivate(const URL &aURL) 
        : iURL(aURL)
    {
        iWorker   = HttpConnectionWorker::getSharedSingleton();
    }

    const URL& HttpConnectionPoolPrivate::url() const {
        return iURL;
    }
    
    std::shared_ptr<HttpConnection> HttpConnectionPoolPrivate::getConnection() {
        return HttpConnection::get(iURL);

        /*          

        if (aURL.scheme() == "https") {
            return std::make_shared<HttpsConnectionPrivate>(shared_from_this(),
                                                            iWorker->service());
        } else {
            return std::make_shared<HttpConnectionPrivate>(shared_from_this(),
                                                           iWorker->service());
        }
        */
    }
    void HttpConnectionPoolPrivate::releaseConnection(HttpConnection::var aConnection) {
        // TODO...
    }
    
    /* Global getter */
    HttpConnectionPool::HttpConnectionPool() {}
    HttpConnectionPool::var HttpConnectionPool::get(const URL &aURL) {
        return std::make_shared<HttpConnectionPoolPrivate>(aURL);
    }
    
} /* namespace QtC */
