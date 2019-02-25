#ifdef WINDOWS_KEY_WORD
    #define SET_WIN32
#else
    #undef SET_WIN32
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

#ifdef SET_WIN32
    #include <windows.h>
#else
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <sys/un.h>
    #include <sys/resource.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <ctype.h>
    #include <sys/msg.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <termios.h>

    #define SPEED B9600
#endif

//#define get_timer_sec(tm) (time(NULL) + tm)
//#define check_delay_sec(tm) (time(NULL) >= tm ? 1 : 0)

//-----------------------------------------------------------------------

int src = -1;
uint8_t buf[256] = {0};
char ts[128] = {0};

//---------------------------------------------------------------------
char *ThisTime(char *t)
{
struct timeval tvl;

    gettimeofday(&tvl, NULL);
    time_t it_ct = tvl.tv_sec;//QDateTime::currentDateTime().toTime_t();
    struct tm *ctimka = localtime(&it_ct);
    sprintf(t, "%02d.%02d %02d:%02d:%02d.%03d | ",
                ctimka->tm_mday,
                ctimka->tm_mon+1,
                ctimka->tm_hour,
                ctimka->tm_min,
                ctimka->tm_sec,
                (int)(tvl.tv_usec/1000));
    return t;
}
//-----------------------------------------------------------------------
int readserdmp()
{
int ret = 0;
uint8_t eop = 0xee;//238;
uint8_t byte;
int len = 0, lp = 1, i, ind = 0;

    memset(buf, 0, sizeof(buf));

    if (src == -1) src = open("demo.dmp", O_RDONLY);
    if (src == -1) return ret;

    while (lp) {
        len = read(src, &byte, 1);
        if (!len) {
            close(src);
            src = open("dmp.dmp", O_RDONLY);
            if (src == -1) return -1;
        }
        buf[ind] = byte; ind++;
        for (i = 0; i < ind; i++) {
            if ((buf[i] == eop) || (ind > 22)) {
                lp = 0;
                break;
            }
        }
    }

    return  ind;
}
//-----------------------------------------------------------------------
int main (int argc, char *argv[])
{
char chap[512];
int i = 0;
uint32_t total_pack = 0;
char dev_name[256] = {0};
char par[256] = {0};
uint32_t wt, ispd = 0;
int rlen = 0;
#ifdef SET_WIN32
    uint32_t maxp = 2500;//2.5 sec
    const char *def_par = "serial.exe COM3:9600";
    DCB dcb = {0};
    HANDLE fd;
    VOID *vuk = (void *)buf;
    DWORD txlen, rxlen = 0;
    char spds[32] = {0};
#else
    int res = 0;
    uint32_t spd;
    uint32_t maxp = 2000000;//2 sec
    const char *def_par = "./serial /dev/ttyUSB1:9600";
    spd = SPEED;
    int rxlen = 0;
    int fd;
    struct timeval mytv;
    fd_set Fds;
    struct termios oldtio, newtio;
#endif


    if (argc < 2) {
        printf ("%s ERROR: you must enter 1 param. For example: %s\n\n", ThisTime(ts), def_par);
        return 1;
    }

    strcpy(par, argv[1]);
    char *uk = strchr(par, ':');
    if (uk) {
        ispd = atoi(uk + 1);
        *uk = '\0';
#ifndef SET_WIN32
	switch (ispd) {
	    case 9600 : spd = B9600; break;
	    case 19200 : spd = B19200; break;
	    case 38400 : spd = B38400; break;
	    case 57600 : spd = B57600; break;
	    case 115200 : spd = B115200; break;
	}
#endif
    }
    strcpy(dev_name, par);

    printf("%s Start serial emulator (Coral FlexiCom Traffic serial port) for device %s\n", ThisTime(ts), dev_name);

#ifdef SET_WIN32
    fd = CreateFile(dev_name,
    	    GENERIC_WRITE,
	        0,
	        0,
	        OPEN_EXISTING,
	        0,
	        0);
    if (fd == INVALID_HANDLE_VALUE) {
        printf("%s Can't open %s file\n", ThisTime(ts), dev_name);
        return 1;
    }
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    sprintf(spds, "%d,n,8,1", ispd);
    if (!BuildCommDCB(spds, &dcb)) {
        printf ("%s Can\'t build DCB - %s.\n", ThisTime(ts), strerror(errno));
        CloseHandle(fd);
        return 1;
    }
    if (!SetCommState(fd, &dcb)) {
        printf("%s Can\'t set comm state - %s.\n", ThisTime(ts), strerror(errno));
        CloseHandle(fd);
        return 1;
    }
    if (!SetupComm(fd, 2, 2)) {
        printf("%s Can\'t setupcomm() - %s.\n", ThisTime(ts), strerror(errno));
        CloseHandle(fd);
        return 1;
    }
#else
    fd = open(dev_name, O_WRONLY , 0664);
    if (fd < 0) {
        printf("%s Can't open %s file (%d)\n", ThisTime(ts), dev_name, fd);
        return 1;
    }
    tcgetattr(fd, &oldtio);
    bzero(&newtio, sizeof(newtio));
    memcpy(&newtio, &oldtio, sizeof(oldtio));
    newtio.c_cflag = spd | CS8 | CLOCAL;// | CREAD;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
#endif

    while (1) {

        rlen = readserdmp();

        if (rlen > 0) {
            total_pack++;
            sprintf(chap,"%s Read new pack #%u (%d bytes)", ThisTime(ts), total_pack, rlen);
            printf("%s\n", chap);
            rxlen = rlen;
#ifdef SET_WIN32
            if (!WriteFile(fd, vuk, rxlen, &txlen, NULL)) {
                sprintf(chap,"%s Error sending to device %lu bytes : %s", ThisTime(ts), txlen, strerror(errno));
            } else {
                sprintf(chap,"%s Write to device (%lu): ", ThisTime(ts), txlen);
            	for (i = 0; i < rxlen; i++) sprintf(chap+strlen(chap),"%02X", buf[i]);
            }
            printf("%s\n", chap);
#else
            FD_ZERO(&Fds); FD_SET(fd, &Fds);
            mytv.tv_sec = 0; mytv.tv_usec = 1000;
            if (select(fd + 1, NULL, &Fds, NULL, &mytv) > 0) {
                if (FD_ISSET(fd, &Fds)) {// event from my device
                    res = write(fd, buf, rxlen);
                    if (res == rxlen) {
                        sprintf(chap,"%s Write to device (%d): ", ThisTime(ts), res);
                        for (i = 0; i < rxlen; i++) sprintf(chap+strlen(chap),"%02X", buf[i]);
                    } else {
                        sprintf(chap,"%s Error sending to device %d bytes : %s", ThisTime(ts), res, strerror(errno));
                    }
                    printf("%s\n", chap);
                }
            }
#endif
            wt = rand() % maxp + 1;
#ifdef SET_WIN32
            wt += 500; if (wt > maxp) wt = maxp;
            Sleep(wt);
#else
            usleep(wt);
#endif
        }

    }//while

    printf("%s Send to device %d packets.\n", ThisTime(ts), total_pack);
#ifdef SET_WIN32
    CloseHandle(fd);
#else
    tcsetattr(fd, TCSANOW, &oldtio);
    if (fd > 0) close(fd);
#endif
    if (src > 0) close(src);

    return 0;
}
