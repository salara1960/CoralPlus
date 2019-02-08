#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef WIN32
    #include <windows.h>
#else
    #include <arpa/inet.h>
    #include <endian.h>
#endif

#define MAX_DIGIT_PORT 8
//#define SET_DEBUG

#define max_adr 600//(12x12)x16=2304

#endif // GLOBAL_H

