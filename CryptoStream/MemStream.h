#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
#define MAX_BUFF_SIZE 1024

class IMemStreamBuf : public std::streambuf {
public:
	IMemStreamBuf(const char * begin, int size);
	explicit IMemStreamBuf(const char * begin);//use strlen to get the end, but it's not safe enough

	IMemStreamBuf(const IMemStreamBuf &);

private:
	IMemStreamBuf &operator= (const IMemStreamBuf &) = delete;

	virtual int_type underflow();
	virtual int_type uflow();
	virtual int_type pbackfail(int_type ch);
	virtual std::streamsize showmanyc();

	const char * const begin_;
	const char * const end_;
	const char * curr_;
};

class MemStreamBuf : public std::streambuf {
public:
	MemStreamBuf(char * const hMem,int size);
	~MemStreamBuf();


private:
	MemStreamBuf(const MemStreamBuf &) = delete; //coping not allowed
	MemStreamBuf &operator= (const MemStreamBuf &) = delete; //assignment not allowed

	//c is the charactor when calling overflow
	virtual int overflow(int c);
	//flush the data to the file, no matter whether the buffer is full
	virtual int sync();

	//flush for input buffer empty
	virtual int_type underflow();
	virtual int_type uflow();
	virtual int_type pbackfail(int_type ch);
	virtual std::streamsize showmanyc();

	const char * const gbegin_;
	const char * const gend_;
	const char * gcurr_;

	char * const pMem_;
	char * const pMemEnd_;
	int memSize_;

	const size_t bufSize_;
	char* pbuf_;  //output buffer
	char* gbuf_;  //input buffer
};

class IMemStream : public std::iostream {
private:
	IMemStreamBuf streambuf_;
public:
	IMemStream(void* hMem, int size) :
		std::iostream(NULL),streambuf_((char *)hMem, size) {
		rdbuf(&streambuf_);//set here instead of the iostream list initialization,
		// because the iostream(base) will initialized before member variable
	}
	~IMemStream() {}
};

//this is not that useful, because it doesn't support the SEEK
//once its output buf overflow, clear the state flagbit before input
class MemStream : public std::iostream {
private:
	MemStreamBuf streambuf_;
public:
	MemStream(void* hMem, int size) :
			std::iostream(NULL),streambuf_((char *)hMem, size) {
		rdbuf(&streambuf_);
	}
	~MemStream() {}
};
