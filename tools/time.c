/**
 * @Filename: time.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/26/2017 11:17:49 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    time_t timer;
    struct tm *t_tm;
    struct timeval now;

    timer = time(NULL);
    gettimeofday(&now, NULL);
    printf("time:%d, now:%d", timer, now.tv_sec);
    t_tm = gmtime(&timer);

    printf("adp_local_time_get curent time: %4d/%02d/%02d %02d:%02d:%02d\n\r", t_tm->tm_year+1900,t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
    t_tm = localtime(&timer);
    printf("adp_local_time_get curent time: %4d/%02d/%02d %02d:%02d:%02d\n\r", t_tm->tm_year+1900,t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);

    char buf[128];
    memset(buf, 0x00, 128);
    strftime(buf, 128, argv[1], t_tm);
    printf("buff:%s\n", buf);

    return 0;
}
