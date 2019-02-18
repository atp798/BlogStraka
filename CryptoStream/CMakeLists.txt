cmake_minimum_required(VERSION 3.12)
project(cryptostream)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(main
        CryptoStream.cpp
        CryptoStream.h
        MemStream.cpp
        MemStream.h
        main.cpp
        WjCryptLib_Aes.cpp
        WjCryptLib_Aes.h
        WjCryptLib_AesOfb.cpp
        WjCryptLib_AesOfb.h
        )
