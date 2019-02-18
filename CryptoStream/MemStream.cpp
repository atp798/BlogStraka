//
// Created by tianpeng.lan on 2019-02-03.
//
#include <assert.h>
#include "MemStream.h"

#define MIN( x, y ) ( ((x)<(y))?(x):(y) )

IMemStreamBuf::IMemStreamBuf(const char * begin, int size) :
	begin_(begin), end_(begin + size), curr_(begin) {
	assert(std::less_equal<const char *>()(begin_, end_));
}

IMemStreamBuf::IMemStreamBuf(const char * begin) : 
	begin_(begin), end_(begin + std::strlen(begin)), curr_(begin) {
	assert(std::less_equal<const char *>()(begin_, end_));
}

IMemStreamBuf::IMemStreamBuf(const IMemStreamBuf & stream): 
	begin_(stream.begin_), end_(stream.end_), curr_(stream.curr_) {

}

IMemStreamBuf::int_type IMemStreamBuf::underflow() {
	return traits_type::to_int_type(*curr_);
}

IMemStreamBuf::int_type IMemStreamBuf::uflow(){
	if (curr_ == end_)
		return traits_type::eof();
	return traits_type::to_int_type(*curr_++);
}

IMemStreamBuf::int_type IMemStreamBuf::pbackfail(int_type ch){
	if (curr_ == begin_ || (ch != traits_type::eof() && ch != curr_[-1]))
		return traits_type::eof();
	return traits_type::to_int_type(*--curr_);
}

std::streamsize IMemStreamBuf::showmanyc(){
	assert(std::less_equal<const char *>()(curr_, end_));
	return end_ - curr_;
}

//*****************************************************************************************************

MemStreamBuf::MemStreamBuf(char * const hMem, int size) :
	bufSize_(size),pMem_(hMem),memSize_(size),pMemEnd_(hMem+size),
	gbegin_(hMem),gend_(hMem + size){
	assert(hMem!=NULL);
	assert(size > 0);

	gcurr_ = gbegin_;
	setp(pMem_, pMemEnd_); 
}

MemStreamBuf::~MemStreamBuf() {
}

int MemStreamBuf::sync() {
	std::cout <<pptr()-pbase()<<" : "<< epptr() - pbase() << std::endl;
	return pptr()<epptr()?0:-1;
}

int MemStreamBuf::overflow(int c) {
	//return traits_type::eof();
	if (-1 == sync()) {
		return traits_type::eof();
	}
	else {
		// put c into buffer after successful sync
		if (!traits_type::eq_int_type(c, traits_type::eof())) {
			sputc(traits_type::to_char_type(c));
		}

		// return eq_int_type(c, eof()) ? eof():c;
		return traits_type::not_eof(c);
	}
}

MemStreamBuf::int_type MemStreamBuf::underflow() {
	return traits_type::to_int_type(*gcurr_);
}

MemStreamBuf::int_type MemStreamBuf::uflow() {
	if (gcurr_ >= gend_)
		return traits_type::eof();
	return traits_type::to_int_type(*gcurr_++);
}

MemStreamBuf::int_type MemStreamBuf::pbackfail(int_type ch) {
	if (gcurr_ == gbegin_ || (ch != traits_type::eof() && ch != gcurr_[-1]))
		return traits_type::eof();
	return traits_type::to_int_type(*--gcurr_);
}

std::streamsize MemStreamBuf::showmanyc() {
	assert(std::less_equal<const char *>()(gcurr_, gend_));
	return gend_ - gcurr_;
}