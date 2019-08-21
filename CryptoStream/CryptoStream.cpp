//
// Created by straka on 2019-02-03.
//
#include <assert.h>
#include "CryptoStream.h"

#define MIN( x, y ) ( ((x)<(y))?(x):(y) )

CryptoStreamBuf::CryptoStreamBuf(std::string strPath,std::string strKey) :
        bufSize_(MIN(256,MAX_BUFF_SIZE)), path_(strPath) {
    assert(strKey.length()==32);

    path_ = strPath;
    fin_ = fopen(strPath.c_str(),"r+");
    assert(fin_);
    fout_ = fopen(strPath.c_str(),"a+");

    key_ = strKey;
    uint8_t const*  iv = (uint8_t const*)"0123456789abcdef";
    AesOfbInitialiseWithKey( &aesInCtx_, (uint8_t const*)key_.c_str(), AES_KEY_SIZE_256, iv );
    AesOfbInitialiseWithKey( &aesOutCtx_, (uint8_t const*)key_.c_str(), AES_KEY_SIZE_256, iv );

    fseek(fout_, 0, SEEK_END);
    long deLeft = ftell(fout_);

    uint8_t deBuff[AES_BLOCK_SIZE];
    while(deLeft>0){
        unsigned int deSize = MIN(deLeft,AES_BLOCK_SIZE);
        //this only calc the crypted key flow
        AesOfbOutput(&aesOutCtx_,deBuff,deSize);
        deLeft -= deSize;
    }

    pbuf_ = new char[bufSize_];
    gbuf_ = new char[bufSize_];

    setp(pbuf_, pbuf_ + bufSize_); // set the pointers for output buf
    setg(gbuf_, gbuf_,gbuf_);
}

CryptoStreamBuf::~CryptoStreamBuf(){
    if(pbuf_){
        delete[] pbuf_;
    }
    if(gbuf_){
        delete[] gbuf_;
    }
    if(fin_){
       fclose(fin_);
    }
    if(fout_){
        fclose(fout_);
    }
}

int CryptoStreamBuf::sync() {
    int encrypted = 0;
    int toEncrypt = pptr() - pbase();  // data that can be flushed
    char buf[MAX_BUFF_SIZE];
    AesOfbXor(&aesOutCtx_,pbase(),buf,toEncrypt);

    int ret = fwrite(buf,1,toEncrypt,fout_);
    if (ret == toEncrypt){
        fflush(fout_);
    }
    else {
        return -1;
    }
    setp(pbase(), pbase() + bufSize_);  // reset the buffer
    pbump(0);  // reset pptr to buffer head
    return 0;
}

int CryptoStreamBuf::overflow(int c) {
    if (-1 == sync()) {
        return traits_type::eof();
    }
    else {
        // put c into buffer after successful sync
        if (!traits_type::eq_int_type(c, traits_type::eof())) {
            sputc(traits_type::to_char_type(c));
        }
        return traits_type::not_eof(c);
    }
}

int CryptoStreamBuf::underflow() {
    char buf[MAX_BUFF_SIZE];
    int ret = fread(buf,1,bufSize_, fin_);
    if (ret > 0) {
        char bufDecrypt[MAX_BUFF_SIZE];
        AesOfbXor(&aesInCtx_,buf,bufDecrypt,ret);
        memcpy(eback(),bufDecrypt,ret);
        setg(eback(), eback(), eback() + ret);
        return traits_type::to_int_type(*gptr());
    } else {
        return traits_type::eof();
    }
}
