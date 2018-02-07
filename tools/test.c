/**
 * @Filename: test.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 11/07/2017 06:41:43 PM
 */
#include <stdio.h>

int main(int argc, char *argv[])
{
    int num = -477;

    if (num < 0) {
        num = num * (-1);
        printf("-%d.%02d", num/100, num%100);
    } else {
        printf("%d.%02d", num/100, num%100);
    }
#if 0
    //printf("  %-3s %-9s %-18s %-18s %-9s", "NO.", "Level", "SpeedA(r/min)", "SpeedB(r/min)","Mode");
    int i, recvlen, index;
    char ch;
    char recvdata[4096];

    for (i = 0; i < 4096; i++) {
        recvdata[i] = 'a' + i % 26;
    }

    recvlen = atoi(argv[1]);
    recvdata[recvlen] = '\0';
    printf("recvlen:%d\n", recvlen);
    if (recvlen) {
        index = 0;
        while (recvlen - index > 1024) {
            ch = recvdata[index + 1024];
            recvdata[index + 1024] = '\0';
            printf("%s", &(recvdata[index]));
            recvdata[index + 1024] = ch;
            index += 1024;
        }
        if (recvlen - index > 0) { 
            printf("%s", &(recvdata[index]));
        }
    }
#endif

    return 0;
}
