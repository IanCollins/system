cmake_minimum_required(VERSION 3.22)
project(system)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare")

include_directories(include/system)

add_library(masuma-system STATIC Exception.cc
                         SocketAutoFd.cc
                         Agent.cc
                         AutoFd.cc
                         Time.cc
                         NetDb.cc
                         Stat.cc
                         Timestamp.cc)

install(TARGETS masuma-system DESTINATION lib)
install(DIRECTORY
        include/system
        DESTINATION "${CMAKE_INSTALL_PREFIX}/masuma")

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/build")

include(CPack)
