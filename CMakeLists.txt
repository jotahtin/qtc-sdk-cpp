cmake_minimum_required(VERSION 2.8)

project(qtc)
set(CMAKE_CXX_FLAGS "-std=c++11 -s -O4 -Os -Wl,--gc-sections -ffunction-sections -fdata-sections -D_WEBSOCKETPP_CPP11_STL_ -D_WEBSOCKETPP_CPP11_THREAD_ -D_WEBSOCKETPP_CPP11_FUNCTIONAL_ -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB")
# set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -D_WEBSOCKETPP_CPP11_STL_ -D_WEBSOCKETPP_CPP11_THREAD_ -D_WEBSOCKETPP_CPP11_FUNCTIONAL_")
# set(CMAKE_CXX_FLAGS "-std=c++11 -O3")
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")

include_directories("${qtc_SOURCE_DIR}")
include_directories("${qtc_SOURCE_DIR}/3rdParty")

find_package(BISON REQUIRED)
set(BISON_OUTPUT ${qtc_SOURCE_DIR}/QtC/Common/JSONGrammar.cpp)
set(BISON_DEFINES ${qtc_SOURCE_DIR}/QtC/Common/JSONGrammar.h)
include_directories(${qtc_SOURCE_DIR})
if(BISON_FOUND)
	add_custom_command(
		OUTPUT ${BISON_OUTPUT}
		COMMAND ${BISON_EXECUTABLE} 
			--defines=${BISON_DEFINES}
			--output=${BISON_OUTPUT}
			${qtc_SOURCE_DIR}/QtC/Common/JSONGrammar.y
		COMMENT "Generating JSONGrammar.h, JSONGrammar.cpp (parser)"
	)
endif(BISON_FOUND)

find_package(FLEX REQUIRED)
set(FLEX_OUTPUT ${qtc_SOURCE_DIR}/QtC/Common/JSONLexer.cpp)
if(FLEX_FOUND)
	add_custom_command(
		OUTPUT ${FLEX_OUTPUT}
		COMMAND ${FLEX_EXECUTABLE}
			--outfile=${FLEX_OUTPUT}
			${qtc_SOURCE_DIR}/QtC/Common/JSONLexer.l
		COMMENT "Generating JSONLexer.cpp (lexer)"
	)
endif(FLEX_FOUND)

find_package (Threads)

find_package( Boost COMPONENTS system REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

find_package(OpenSSL)

add_library(qtc
      QtC/Common/URI.cpp
      QtC/Common/HttpConnection.cpp
      QtC/Common/Base64.cpp
      QtC/Common/JSON.cpp
      QtC/Common/JSONGrammar.cpp
      QtC/Common/JSONLexer.cpp
      QtC/EDS/EDS.cpp
      QtC/EDS/Collection.cpp
      )

# Development Tests
add_executable(TestingHttpRequest Tests/TestingHttpRequest.cpp)
target_link_libraries(TestingHttpRequest qtc ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ) 

add_executable(TestingEDS Tests/TestingEDS.cpp)
target_link_libraries(TestingEDS qtc ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ) 

add_executable(TestingMWS Tests/TestingMWS.cpp)
target_link_libraries(TestingMWS qtc ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ) 

add_executable(TestingWSEchoServer Tests/TestingWSEchoServer.cpp)
target_link_libraries(TestingWSEchoServer qtc ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ) 

add_executable(TestingWSBroadcastServer Tests/TestingWSBroadcastServer.cpp)
target_link_libraries(TestingWSBroadcastServer qtc ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ) 

add_executable(TestingWSBroadcastClient Tests/TestingWSBroadcastClient.cpp)
target_link_libraries(TestingWSBroadcastClient qtc ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ) 

# strip -s -R .comment -R .gnu.version --strip-unneeded TestingEDS

