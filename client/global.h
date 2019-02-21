#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef _WIN32
    #include <windows.h>
#else
    #include <arpa/inet.h>
    #include <endian.h>
#endif

#define MAX_DIGIT_PORT 8

#define max_adr 600//(12x12)x10=1440

#endif // GLOBAL_H

