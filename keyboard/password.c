/**
 * @Filename: password.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 11/10/2016 03:31:29 PM
 */
#include <stdio.h>
#include <stdlib.h>  
#include <termios.h>  

#define PASSWORD_LEN 8  

int main()  
{  
    struct termios initialrsettings, newrsettings;  
    char password[PASSWORD_LEN + 1];  

    tcgetattr(fileno(stdin), &initialrsettings);  
    newrsettings = initialrsettings;  
    newrsettings.c_lflag &= ~ECHO;  
    printf("Enter password: ");  
    if(tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0) {  
        fprintf(stderr,"Could not set attributes\n");  
    }  
    else {  
        fgets(password, PASSWORD_LEN, stdin);  
        tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);  
        fprintf(stdout, "\nYou entered %s\n", password);  
    }  
    exit(0);  
}
