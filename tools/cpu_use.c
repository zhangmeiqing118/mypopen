/**
 * @Filename: cpu_use.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/15/2017 02:36:13 PM
 */
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  

#define __DEBUG__ 1  
#define CK_TIME 1  

int main(int argc ,char *argv[])  
{  
    FILE *fp;  
    char buf[128];  
    char cpu[5];  
    long int user,nice,sys,idle,iowait,irq,softirq;  

    long int all1,all2,idle1,idle2;  
    long int cpu_use0, cpu_use1, all;


    while(1)  
    {  
        fp = fopen("/proc/stat","r");  
        if(fp == NULL)  
        {  
            perror("fopen:");  
            exit (0);  
        }  


        fgets(buf,sizeof(buf),fp);  
#if __DEBUG__  
        printf("buf=%s",buf);  
#endif  
        sscanf(buf,"%s%d%d%d%d%d%d%d",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);  
        /* 
#if __DEBUG__ 
printf("%s,%d,%d,%d,%d,%d,%d,%d\n",cpu,user,nice,sys,idle,iowait,irq,softirq); 
#endif 
*/  
        all1 = user+nice+sys+idle+iowait+irq+softirq;  
        idle1 = idle;  
        rewind(fp);  
        /*第二次取数据*/  
        sleep(CK_TIME);  
        memset(buf,0,sizeof(buf));  
        cpu[0] = '\0';  
        user=nice=sys=idle=iowait=irq=softirq=0;  
        fgets(buf,sizeof(buf),fp);  
#if __DEBUG__  
        printf("buf=%s",buf);  
#endif  
        sscanf(buf,"%s%d%d%d%d%d%d%d",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);  
        /* 
#if __DEBUG__ 
printf("%s,%d,%d,%d,%d,%d,%d,%d\n",cpu,user,nice,sys,idle,iowait,irq,softirq); 
#endif 
*/  
        all2 = user+nice+sys+idle+iowait+irq+softirq;  
        idle2 = idle;  

        idle = (all2-all1-(idle2-idle1));
        all = (all2-all1);
        cpu_use0 = (idle * 100) / all;
        cpu_use1 = ((idle * 100) % all) * 100 / all;


        printf("all=%d\n",all);  
        printf("ilde=%d\n",idle);  
        printf("cpu use = %2d.%02d\% \n", cpu_use0, cpu_use1);  
        printf("=======================\n");  

        fclose(fp);  
    }  
    return 1;  
}  
