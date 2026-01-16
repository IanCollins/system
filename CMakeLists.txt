cmake_minimum_required(VERSION 3.22)
project(system)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare")

include_directories(include)

add_library(masuma-system STATIC Exception.cc
                         SocketAutoFd.cc
                         Agent.cc
                         AutoFd.cc
                         Time.cc
                         NetDb.cc
                         Stat.cc
                         Timestamp.cc)

install(TARGETS masuma-system DESTINATION lib)
install(FILES
        include/Exception.h
        include/SocketAutoFd.h
        include/Agent.h
        include/AutoFd.h
        include/Time.h
        include/NetDb.h
        include/Timestamp.h
        include/Log.h
        include/String.h
        include/Types.h
        include/Typelist.h
        include/Tee.h
        include/Streambuf.h
        include/AutoStreambuf.h
        include/Stat.h
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/build")

include(CPack)
