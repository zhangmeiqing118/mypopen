/**
 * @Filename: test1.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 11/23/2017 03:33:15 PM
 */
#include <stdio.h>
#include <string.h>

#define TRUE    1
#define FALSE   0

int ip_to_string(int ip_addr, char *ip_string)
{
    int i;
    unsigned char ip_buffer[4];
    unsigned char temp;
    int flag;

    for (i = 0; i < 4; i++)
    {
        ip_buffer[3-i] = (unsigned char) ((ip_addr >> 8*i) & 0xFF);
    }

    temp = ip_buffer[0];
    flag = FALSE;

    if (temp >= 100)
    {
        *ip_string = temp/100+'0';
        ip_string++;
        temp = temp%100;
        flag = TRUE;
    }
    if (flag  || temp >= 10)
    {
        *ip_string = temp/10+'0';
        ip_string++;
        temp = temp%10;
    }
    *ip_string = temp + '0';
    ip_string++;

    *ip_string = '.';
    ip_string++;

    temp = ip_buffer[1];
    flag = FALSE;
    if (temp >= 100)
    {
        *ip_string = temp/100+'0';
        ip_string++;
        temp = temp%100;
        flag = TRUE;
    }
    if (flag  || temp >= 10)
    {
        *ip_string = temp/10+'0';
        ip_string++;
        temp = temp%10;
    }
    *ip_string = temp+'0';
    ip_string++;

    *ip_string = '.';
    ip_string++;

    temp = ip_buffer[2];
    flag = FALSE;
    if (temp >= 100)
    {
        *ip_string = temp/100+'0';
        ip_string++;
        temp = temp%100;
        flag = TRUE;
    }
    if (flag  || temp >= 10)
    {
        *ip_string = temp/10+'0';
        ip_string++;
        temp = temp%10;
    }
    *ip_string = temp+'0';
    ip_string++;

    *ip_string = '.';
    ip_string++;

    temp = ip_buffer[3];
    flag = FALSE;
    if (temp >= 100)
    {
        *ip_string = temp/100+'0';
        ip_string++;
        temp = temp%100;
        flag = TRUE;
    }
    if (flag  || temp >= 10)
    {
        *ip_string = temp/10+'0';
        ip_string++;
        temp = temp%10;
    }
    *ip_string = temp+'0';
    ip_string++;

    *ip_string = '\0';

    return 0;
}

int floatstr2int(char *str, int mul, int *result)
{
    char *ptr;
    char data[32];
    int value;

    *result = 0;
    if ((mul != 10) && (mul != 100)) {
        return -1;
    }
    if (strlen(str) > 31) {
        printf("string length is too long\n");
        return -1;
    }

    value = 0;
    memset(data, 0x00, 32);
    strcpy(data, str);
    ptr = strchr(data, '.');
    if (NULL == ptr) {
        value = atoi(data);
        value *= mul;
    } else {
        ptr++;
        if ((ptr[0] == '\0') || !isdigit(ptr[0])) {
            printf("Invalid float string");
            return -1;
        }
        if (mul == 10) {
            ptr[1] = '\0';///舍去1位小数后的小数
        } else{
            if ((ptr[1] == '0') || !isdigit(ptr[1])) {
                ptr[1] = '0';///补0
            }
            ptr[2] = '\0';
        }
        value = atoi(data);
        if (value >= 0) {
            value = value * mul + atoi(ptr);
        } else {
            value = value *mul - atoi(ptr);
        }
    }
    *result = value;

    return 0;
}

int main(int argc, char *argv[])
{
#if 0
    char data[64];
    int ip_addr;

    ip_addr = atoi(argv[1]);
    memset(data, 0x00, 64);
    ip_to_string(ip_addr, data);
    printf("data:%s\n", data);
    memset(data, 0x00, 64);
    sprintf(data, "%d.%d.%d.%d\n", ((ip_addr >> 24) & 0xff),
            ((ip_addr >> 16) & 0xff), ((ip_addr >> 8) & 0xff),
            ((ip_addr >> 0) & 0xff));
    printf("data:%s\n", data);
#endif

    int result;

    result = 0;
    if (floatstr2int(argv[2], atoi(argv[1]), &result)) {
        return -1;
    }
    printf("result:%d\n", result);

    return 0;
}
