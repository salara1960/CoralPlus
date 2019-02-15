#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <math.h>


//-----------------------------------------------------------------------
#define SPEED B9600

//#define get_timer_sec(tm) (time(NULL) + tm)
//#define check_delay_sec(tm) (time(NULL) >= tm ? 1 : 0)

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
int len = 0, ind = 0, lp = 1, i;

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
int fd, i = 0, res = 0;
struct timeval mytv;
fd_set Fds;
struct termios oldtio, newtio;
uint32_t total_pack = 0;
int rxlen = 0;
char dev_name[256] = {0};
char par[256] = {0};
uint32_t maxp = 1500000;
uint32_t spd = SPEED, ispd = 0;

    if (argc < 2) {
        printf ("%s ERROR: you must enter 1 param. For example: ./serial /dev/ttyUSB1:9600\n\n", ThisTime(ts));
        return 1;
    }

    strcpy(par, argv[1]);
    char *uk = strchr(par, ':');
    if (uk) {
        ispd = atoi(uk + 1);
        *uk = '\0';
	switch (ispd) {
	    case 9600 : spd = B9600; break;
	    case 19200 : spd = B19200; break;
	    case 38400 : spd = B38400; break;
	    case 57600 : spd = B57600; break;
	    case 115200 : spd = B115200; break;
	}
    }
    strcpy(dev_name, par);

    printf("%s Start serial emulator (Coral FlexiCom Traffic serial port) for device %s\n", ThisTime(ts), dev_name);

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

    while (1) {

        rxlen = readserdmp();

        if (rxlen > 0) {
	    total_pack++;
	    sprintf(chap,"%s Read new pack #%u (%d bytes)", ThisTime(ts), total_pack, rxlen);
	    printf("%s\n", chap);
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
	    
	    usleep(rand() % maxp + 1);
	    //sleep(8);
        }

    }//while

    printf("%s Send to device %d packets.\n", ThisTime(ts), total_pack);

    tcsetattr(fd, TCSANOW, &oldtio);
    if (fd > 0) close(fd);
    if (src > 0) close(src);

    return 0;
}
