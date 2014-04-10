
#include <iostream>
#include <sstream>

#include <QtC/QtC.h>

#include <QtC/Common/URI.h>

using namespace std;
using namespace QtC;

void test_find() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");
  
  todo.find({},
	    // { JSON::String("name") | "Doe" },
	    
	    [] (const boost::system::error_code& aError,JSON::Value aValue) {
	      if (aError) {
		cerr << "Error: " << aError.message() << endl;
	      } else {
		cout << "Got Result" << endl;
		cout << aValue << endl;
		cout << "TITLE=" << aValue["results"][0]["title"] << endl;
	      }
	    });
  
  cout << "Waiting..." << endl;
  sleep(5);
}



void test_insert() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  /*
  {
    { foo, plah }
    { foo, { foo, plah } }
  }

  todo.insert('{ "name": "John"  }');
  */

  /*
  QtC::JSON::String("X")
  QtC::Eds::Ref() => QtC::JsonValue
  QtC::Eds::Time() => QtC::JsonValue
  QtC::Eds::GeoLoc() => QtC::JsonValue
  */

  /*
  todo.insert( { JSON::String("name")    | "John",
	         JSON::String("age")     | 31,
	         JSON::String("likes")   | JSON::Array({"pizza","coke"}),
	         JSON::String("address") | JSON::Object({ { "city",     "Springfield"}, // JSON::String("city") | "Springfield",
		                                          { "country",  "USA"}})
	},
    [] (const boost::system::error_code& aError,JSON::Value aValue) {
      if (aError) {
	cerr << "Error: " << aError.message() << endl;
      } else {
	cout << "Got Result" << endl;
	cout << aValue << endl;
      }
    });
  */

  todo.insert( { JSON::String("name")    | "Doe",
	         JSON::String("age")     | 32,
	         JSON::String("likes")   | JSON::Array({"chili"}),
	         JSON::String("address") | JSON::Object({ { "city",     "Springfield"}, // JSON::String("city") | "Springfield",
		                                          { "country",  "USA"}})
	},
    [] (const boost::system::error_code& aError,JSON::Value aValue) {
      if (aError) {
	cerr << "Error: " << aError.message() << endl;
      } else {
	cout << "Got Result" << endl;
	cout << aValue << endl;
      }
    });
  
  cout << "Waiting..." << endl;
  sleep(5);

}

void test_findOne() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  todo.findOne("532c49d000deff09a4001201",
	       [] (const boost::system::error_code& aError,JSON::Value aValue) {
		 if (aError) {
		   cerr << "Error: " << aError.message() << endl;
		 } else {
		   cout << "Got Result" << endl;
		   cout << aValue << endl;
		 }
	       });
  
  cout << "Waiting..." << endl;
  sleep(5);
}

void test_update() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  todo.update("532c49d000deff09a4001201",
	      //{ JSON::String("age")     | 42 }
	      { JSON::String("age")     | JSON::Value() }
	      ,
	       [] (const boost::system::error_code& aError,JSON::Value aValue) {
		 if (aError) {
		   cerr << "Error: " << aError.message() << endl;
		 } else {
		   cout << "Got Result" << endl;
		   cout << aValue << endl;
		 }
	       });
  
  cout << "Waiting..." << endl;
  sleep(5);
}

void test_remove() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");
  
  todo.remove("532c465e8bf75b6984001712",
	      [] (const boost::system::error_code& aError,JSON::Value aValue) {
		if (aError) {
		  cerr << "Error: " << aError.message() << endl;
		} else {
		  cout << "Got Result" << endl;
		  cout << aValue << endl;
		}
	      });
  
  cout << "Waiting..." << endl;
  sleep(5);
}

void test_file_upload() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  todo.attachFile("532c49d000deff09a4001201", "myfile"
		  ,
		  FileUploadStream(std::make_shared<std::istringstream>("Hello World"))
		  .setFilename("MyFileName.txt").setContentType("image/png")
		  ,
		  [] (const boost::system::error_code& aError,JSON::Value aValue) {
		    if (aError) {
		      cerr << "Error: " << aError.message() << endl;
		    } else {
		      cout << "Got Result" << endl;
		      cout << aValue << endl;
		    }
		  });
  cout << "Waiting..." << endl;
  sleep(5);
}

void test_file_remove() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  todo.removeFile("532c49d000deff09a4001201", "myfile"
		  ,
		  [] (const boost::system::error_code& aError,JSON::Value aValue) {
		    if (aError) {
		      cerr << "Error: " << aError.message() << endl;
		    } else {
		      cout << "Got Result" << endl;
		      cout << aValue << endl;
		    }
		  });
  cout << "Waiting..." << endl;
  sleep(5);
}

void test_file_download() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  todo.downloadFile("533d36de00deff16e00009ab",
		    [] (const boost::system::error_code& aError,JSON::Value aValue) {
		      if (aError) {
			cerr << "Error: " << aError.message() << endl;
		      } else {
			cout << "Got Result" << endl;
			cout << aValue << endl;
		      }
		    });
  
  cout << "Waiting..." << endl;
  sleep(5);
}

void test_file_info() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  

  Collection todo = eds.collection("Todo");
  todo.getFileInfo("533d36de00deff16e00009ab",
		   [] (const boost::system::error_code& aError,JSON::Value aValue) {
		     if (aError) {
		       cerr << "Error: " << aError.message() << endl;
		     } else {
		       cout << "Got Result" << endl;
		       cout << aValue << endl;
		     }
		   });
  cout << "Waiting..." << endl;

  sleep(5);
}

void test_file_url() {
  EDS eds("https://staging-api.engin.io", "5322d33100deff23640050fa");  
  Collection todo = eds.collection("Todo");

  todo.getFileDownloadUrl("533d36de00deff16e00009ab",
			  [] (const boost::system::error_code& aError,JSON::Value aValue) {
			    if (aError) {
			      cerr << "Error: " << aError.message() << endl;
			    } else {
			      cout << "Got Result" << endl;
			      cout << aValue << endl;
			    }
			  });
  cout << "Waiting..." << endl;
  sleep(5);
}

int main() {
  cout << "Testing.." << endl;
  
  
  //test_find();
  //test_insert();
  //test_findOne();
  //test_update();
  //test_remove();
  
  //test_file_upload();
  //test_file_remove();
  //test_file_download();
  test_file_info();
  //test_file_url();


  return 0;
}
