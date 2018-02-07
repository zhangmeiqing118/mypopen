/**
 * @Filename: test.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 11/16/2016 11:05:49 AM
 */
#include <stdio.h>

int action_port_str_transfer(char *src, char dst[128])
{
    int i, len, flag;
    int start, end;
    char *ptr;

    len = 0;
    flag = 0;
    memset(dst, 0x00, 128);
    ptr = src;
    while (ptr && *ptr) {
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }
        start = atoi(ptr);
        while (ptr && isdigit(*ptr)) {
            ptr++;
        }
        if ((NULL == ptr) || (ptr && ((*ptr == ',') || isspace(*ptr) || (*ptr == '\0')))) {
            if (start == 0) {
                return -1;
            }
            if ((start - 1) < 10) {
                if (0 == flag) {
                    flag = 1;
                    len += sprintf(dst + len, "xe%1d", start - 1);
                } else {
                    len += sprintf(dst + len, ",xe%1d", start - 1);
                }
            } else if ((start - 1) < 22) {///port num xe0-xe22
                if (0 == flag) {
                    flag = 1;
                    len += sprintf(dst + len, "xe%2d", start - 1);
                } else {
                    len += sprintf(dst + len, ",xe%2d", start - 1);
                }
            }
            if ((NULL == ptr) || (*ptr == '\0')) {
                break;
            }
            ptr++;

            continue;
        }
        if (ptr && (*ptr == '-')) {
            ptr++;
            end = atoi(ptr);
            while (ptr && isdigit(*ptr)) {
                ptr++;
            }
            for (i = start; i <= end; i++) {
                if (i == 0) {
                    return -1;
                }
                if (i - 1< 10) {
                    if (0 == flag) {
                        flag = 1;
                        len += sprintf(dst + len, "xe%1d", i - 1);
                    } else {
                        len += sprintf(dst + len, ",xe%1d", i - 1);
                    }
                } else if ((i - 1) < 22) {///port num:x0-xe22
                    if (0 == flag) {
                        flag = 1;
                        len += sprintf(dst + len, "xe%2d", i - 1);
                    } else {
                        len += sprintf(dst + len, ",xe%2d", i - 1);
                    }
                }
            }
            if (ptr && (*ptr == ',')) {
                ptr++;
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char src[128];
    char dst[128];

    memset(src, 0x00, 128);
    strcpy(src, argv[1]);
    action_port_str_transfer(argv[1], dst);
    printf("%s\n", dst);

    return ;
}
