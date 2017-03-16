/**
 * @Filename: htest.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/14/2017 08:07:23 AM
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if( strcmp(argv[1], "test") )
    {
        printf("Incorrect password\n");
    }
    else
    {
        printf("Correct password\n");
    }

    return 0;
}
