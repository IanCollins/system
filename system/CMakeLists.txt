cmake_minimum_required(VERSION 3.22)
project(utils)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare")

add_library(utils STATIC Exception.cc
                         SocketAutoFd.cc
                         Agent.cc
                         AutoFd.cc
                         Time.cc
                         NetDb.cc
                         Stat.cc
                         Timestamp.cc
                         Log.h String.h Types.h Typelist.h Tee.h Streambuf.h AutoStreambuf.h Stat.h)

install(TARGETS utils DESTINATION lib)
install(FILES
            Exception.h
            SocketAutoFd.h
            Agent.h
            AutoFd.h
            Time.h
            NetDb.h
            Timestamp.h
            Log.h String.h Types.h Typelist.h Tee.h Streambuf.h AutoStreambuf.h Stat.h
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/build")

include(CPack)
