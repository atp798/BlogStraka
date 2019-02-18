//
// Created by tianpeng.lan on 2019-02-03.
//

#ifndef WINCRYPT_CRYPTOSTREAM_H
#define WINCRYPT_CRYPTOSTREAM_H
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "WjCryptLib_AesOfb.h"

#define MAX_BUFF_SIZE 1024

class CryptoStreamBuf : public std::streambuf {
public:
    CryptoStreamBuf(std::string strPath,std::string strKey);
    virtual ~CryptoStreamBuf();

    //c is the charactor when calling overflow
    int overflow(int c);
    //flush the data to the file, no matter whether the buffer is full
    int sync();

    //flush for input buffer empty
    int underflow();
private:
    CryptoStreamBuf(const CryptoStreamBuf &) = delete; //coping not allowed
    CryptoStreamBuf &operator= (const CryptoStreamBuf &) = delete; //assignment not allowed

    std::string path_;
    FILE *fin_,*fout_;

    const size_t bufSize_;
    char* pbuf_;  //output buffer
    char* gbuf_;  //input buffer

    std::string key_;
    AesOfbContext aesInCtx_, aesOutCtx_;
};

class CryptoStream : public std::iostream {
private:
    CryptoStreamBuf streambuf_;
public:
    CryptoStream(std::string strPath,std::string strKey):
            std::iostream(NULL),streambuf_(strPath, strKey) {
        rdbuf(&streambuf_);
    }
    virtual ~CryptoStream() {}
};

#endif //WINCRYPT_CRYPTOSTREAM_H
