
#include <iostream>
#include <sstream>

#include <QtC/QtC.h>

#include <QtC/Common/HttpConnection.h>
#include <QtC/Common/JSON.h>

using namespace std;
using namespace QtC;

void test_1() {
  URL url("http://localhost:3000/upload_receiver");
  
  HttpConnectionPool::var pool = HttpConnectionPool::get(url);
  HttpConnection::var conn = pool->getConnection();
  
  HttpFormData::var form = HttpFormData::get();
  form->append("field1", "value1");
  form->append("field2", "value2");
  
  form->append("attachment",
	       std::make_shared<std::istringstream>("Mydata"),
	       "myfile.txt",
	       "image/png");
 
  HttpRequest::var request = HttpRequest::getPost(url);

  request->addHeader("Host",               url.authority().toString());
  request->addHeader("Connection",         "keep-alive"              );
  request->addHeader("User-Agent",         "qtc-sdk-cpp/1.0"         );
  request->addHeader("Accept-Encoding",    "gzip,deflate"            );
  
  request->setBody(form);
  
  conn->query(request, [](const boost::system::error_code& aError,
			  HttpReply::var aReply)
	      {
		if (aError) {
		  std::cout << "Query failed: " << aError.message() << "\n";
		} else {
		  cout << "Got reply: " << aReply->status() << endl;

		  const HttpReply::Headers& hdrs=aReply->headers();
		  HttpReply::Headers::const_iterator i;
		  for(i=hdrs.begin();i!=hdrs.end();++i) {
		    cout << " : " << (*i).first << " = " << (*i).second << endl;

		    // Location = http://127.0.0.1:3000/upload/index
		  }

		  cout << "---" << endl;
		  cout << aReply->body() << endl;

		}
	      });

  cout << "Waiting..." << endl;
  sleep(5);
}

void test_2() {
 
  URL url("http://asterix/SourceCodes/ATLAS/atlas3.10.1.tar.bz2");
  //URL url("http://asterix/index.html");
  
  cout << url << endl;

  HttpConnection::var connection;
  connection = HttpConnection::get(url);

  HttpRequest::var request = HttpRequest::getGet(url);

  request->addHeader("Host",               url.authority().toString());
  request->addHeader("Connection",         "keep-alive"              );
  request->addHeader("User-Agent",         "qtc-sdk-cpp/1.0"         );
  //request->addHeader("Accept-Encoding",    "gzip,deflate"            );

  connection->query(request,
		    [](const boost::system::error_code& aError,
		       HttpReply::var aReply)
		    {
		      if (aError) {
			std::cout << "Query failed: " << aError.message() << "\n";
		      } else {
			cout << "Got reply: " << aReply->status() << endl;
			
			const HttpReply::Headers& hdrs=aReply->headers();
			HttpReply::Headers::const_iterator i;
			for(i=hdrs.begin();i!=hdrs.end();++i) {
			  cout << " : " << (*i).first << " = " << (*i).second << endl;
			  
			  // Location = http://127.0.0.1:3000/upload/index
			}
			
			cout << "---" << endl;
			cout << aReply->body().size() << endl;			
			cout << "---" << endl;
			//cout << aReply->body() << endl;			
		      }
		      
		    });   
  


  cout << "Waiting..." << endl;
  sleep(5);
}
	      


int main() {
  cout << "Testing.." << endl;


  //test_1();
  test_2();
  
  
  return 0;
}
