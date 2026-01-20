cmake_minimum_required(VERSION 3.22)
project(system)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare")

include_directories(include/system)

add_library(System STATIC
         Exception.cc
         SocketAutoFd.cc
         Agent.cc
         AutoFd.cc
         Time.cc
         NetDb.cc
         Stat.cc
         Time.cc
         Timestamp.cc)
add_library(masuma::System ALIAS System)

install(TARGETS System DESTINATION lib)
install(DIRECTORY
        include/system
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/masuma")

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/build")

include(CPack)
