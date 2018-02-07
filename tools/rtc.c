/**
 * @Filename: rtc.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 07/10/2017 12:36:25 PM
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>

#define RTC_SET_TIME   _IOW('p', 0x0a, struct linux_rtc_time) /*  Set RTC time    */
#define RTC_RD_TIME    _IOR('p', 0x09, struct linux_rtc_time) /*  Read RTC time   */

struct linux_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

int main(int argc, char *argv[])
{
    int rtc;
    struct tm r_tm;
    struct tm *w_tm;
    time_t t_val;
    char buf[128];

    rtc = open("/dev/rtc", O_RDONLY);
    if (rtc < 0) {
        printf("open /dev/rtc failed\n");
        return -1;
    }

    memset(&r_tm, 0x00, sizeof(struct tm));
    if (ioctl(rtc, RTC_RD_TIME, &r_tm) < 0) {
        printf("set rtc time failed\n");
        close(rtc);
    }
    printf("read rtc time ok\n");
    memset(buf, 0x00, 128);
    strftime(buf, 128, "%D %T", &r_tm);
    printf("firt rtc time:%s\n", buf);
    close(rtc);

    rtc = open("/dev/rtc", O_WRONLY);
    if (rtc < 0) {
        printf("open /dev/rtc failed\n");
        return -1;
    }

    t_val = time(NULL);
    w_tm = localtime(&t_val);
    if (ioctl(rtc, RTC_SET_TIME, w_tm) < 0) {
        printf("set rtc time failed\n");
        close(rtc);
    }
    close(rtc);
    
    printf("set rtc time ok\n");

    return 0;
}
