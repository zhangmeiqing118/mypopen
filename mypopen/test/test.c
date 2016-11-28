/**
 * @Filename: test.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 11/09/2016 08:01:38 AM
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char data[128];

    setbuf(stdout, NULL);
    printf("This is test program\r\n");
    ///fflush(stdout);
    while (1) {
        memset(data, 0x00, 128);
        scanf("%s", data);
        printf("input:%s\r\n", data);
        if (0 == strncmp(data, "ps", 2)) {
            printf("exec command ps\r\n");
        } else if (0 == strncmp(data, "help", 2)) {
            printf("exec command help\r\n");
        } else {
            printf("exec invalid command\r\n");
        }
        fflush(stdout);
        //sleep(1);
    }

    return 0;
}
