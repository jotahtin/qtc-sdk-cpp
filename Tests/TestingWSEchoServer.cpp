
#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// using namespace std;


typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;


// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
  std::cout << "on_message called with hdl: " << hdl.lock().get()
	    << " and message: " << msg->get_payload()
	    << std::endl;

  try {
    s->send(hdl, msg->get_payload(), msg->get_opcode());
  } catch (const websocketpp::lib::error_code& e) {
    std::cout << "Echo failed because: " << e
	      << "(" << e.message() << ")" << std::endl;
  }
}

int main() {
  
  std::cout << "WebSocket, echo server." << std::endl;

  server echo_server;
  
  try {
    // Set logging settings
    echo_server.set_access_channels(websocketpp::log::alevel::all);
    echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
    
    // Initialize ASIO
    echo_server.init_asio();
    echo_server.set_reuse_addr(true);
    
    // Register our message handler
    echo_server.set_message_handler(std::bind(&on_message,&echo_server,std::placeholders::_1,std::placeholders::_2));
    
    // Listen on port 9002
    echo_server.listen(9002);
    
    // Start the server accept loop
    echo_server.start_accept();

    // Start the ASIO io_service run loop
    echo_server.run();
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
  } catch (websocketpp::lib::error_code e) {
    std::cout << e.message() << std::endl;
  } catch (...) {
    std::cout << "other exception" << std::endl;
  }
  
  

  return 0;
}
