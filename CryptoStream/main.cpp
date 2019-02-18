#include <iostream>
#include "CryptoStream.h"
#include "MemStream.h"

void testIMemStream(){
    int size = 10;
    char * mem = new char[size];
    printf("imemstream:");
    memcpy(mem, "0123456789abcdef", 10);
    IMemStream stream(mem, size);
    char c;
    while(stream>>c){
        if(stream.eof()){
           break;
        }
        printf("%c-",c);
    }
    printf("\n");
    delete[] mem;
}


void testMemStream(){
    int size = 10;
    char * mem = new char[size];
    printf("memstream normal case:");
    memcpy(mem, "0123456789abcdef", size);
    MemStream stream(mem, size);
    stream<<"qwertyuiop"<<std::flush;
    char c;
    while(stream>>c){
        if(stream.eof()){
            break;
        }
        printf("%c-",c);
    }
    printf("\n");

    printf("memstream out of buf:");
    memcpy(mem, "0123456789abcdef", size);
    MemStream stream2(mem, size);
    stream2<<"asdfghjkl;'"<<std::flush;

    //clear the failbit state, set by the overflow eof of output streambuf, if not unset this, the input will fail and cause serious problem
    stream2.clear();//setstate is unuseful
    while(stream2>>c){
        if(stream2.eof()){
            break;
        }
        printf("%c-",c);
    }
    printf("\n");
    delete[] mem;
}

void testCryptoStream() {
    printf("cryptostream:");
    CryptoStream stream("cry.db","0123456789abcdef0123456789abcdef");
    //stream << "0123456789abcdefghijklmnopqrstuvwxyz" << std::flush;
    stream<<"11"<<std::flush;
    int i = 0;
    char c;
    while(stream>>c){
        if(stream.eof()){
            break;
        }
        i++;
        if(i>10)break;
        printf("%c-",c);
    }
    stream << "kk" << std::endl;
    i=0;
    while(stream>>c){
        if(stream.eof()){
            break;
        }
        i++;
        if(i>10)break;
        printf("%c-",c);
    }
    stream << "77" << std::flush;
    while(stream>>c){
        if(stream.eof()){
            break;
        }
        //break;
        printf("%c-",c);
    }
    printf("\n");
    stream<<"aa";
}

int main() {
    testCryptoStream();
    testIMemStream();
    testMemStream();
    return 0;
}