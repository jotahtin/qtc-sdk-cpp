

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>


typedef websocketpp::client<websocketpp::config::asio_client> client;

int main() {

  std::string uri = "ws://localhost:9002";

  try {
    
    client endpoint;

    endpoint.set_access_channels(websocketpp::log::alevel::all);
    endpoint.set_error_channels(websocketpp::log::elevel::all);
    
    // Initialize ASIO
    endpoint.init_asio();
    
    // Register our handlers
    /*
    m_endpoint.set_socket_init_handler(bind(&type::on_socket_init,this,::_1));
    m_endpoint.set_tls_init_handler(bind(&type::on_tls_init,this,::_1));
    m_endpoint.set_message_handler(bind(&type::on_message,this,::_1,::_2));
    m_endpoint.set_open_handler(bind(&type::on_open,this,::_1));
    m_endpoint.set_close_handler(bind(&type::on_close,this,::_1));
    */
    
    
    websocketpp::lib::error_code ec;
    client::connection_ptr con = endpoint.get_connection(uri, ec);

    endpoint.connect(con);
    
    

    std::thread th([&] () {
	printf("Worker\n");
	endpoint.run();
	printf("Worker-Done\n");
      });

    while(1) {
      std::string row;
      std::getline (std::cin,row);
      
      endpoint.send(con, row, websocketpp::frame::opcode::text);
    }
    
    
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
  } catch (websocketpp::lib::error_code e) {
    std::cout << e.message() << std::endl;
  } catch (...) {
    std::cout << "other exception" << std::endl;
  }


  return 0;
}
