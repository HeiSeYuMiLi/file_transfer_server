cmake_minimum_required(VERSION 3.10)

project(fileserver)

set(SOURCES
	src/RequestHandler.cpp
	src/Server.cpp
	src/Session.cpp
        src/Logger.cpp
	src/main.cpp
)

find_package(Boost COMPONENTS thread REQUIRED)

add_executable(fileserver ${SOURCES})

target_include_directories(fileserver
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(fileserver PRIVATE Boost::thread)

