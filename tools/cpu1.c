/**
 * @Filename: cpu1.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/30/2017 11:30:38 AM
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define __DEBUG__   0
#define MAX_THREADS     128


typedef struct thread_cpurate {
    int tid;
    int idle;
    int rate;
} thread_cpurate_t;

int get_cpu_num(void)
{
    FILE *fp;
    char buf[512];
    int num;

    fp = fopen("/proc/stat","r");
    if(fp == NULL) {  
        printf("fopen /proc/stat error\n");
        return -1; 
    }
    memset(buf, 0x00, 512);
    fgets(buf, sizeof(buf), fp);//total

    num = 0;
    memset(buf, 0x00, 512);
    while (fgets(buf, sizeof(buf), fp)) {
        if (0 == strncmp(buf, "cpu", 3)) {
            num++;
        }
        memset(buf, 0x00, 512);
    }
    
    fclose(fp);
    return num;
}

int get_cpu_rate_total(void)
{
    FILE *fp;
    char buf[512];
    char cpu[5];
    int total;
    int user,nice,sys,idle,iowait,irq,softirq;

    fp = fopen("/proc/stat","r");
    if(fp == NULL) {  
        printf("fopen /proc/stat error\n");
        return -1; 
    }
    memset(buf, 0x00, 512);
    fgets(buf, sizeof(buf), fp);
    fclose(fp);
#if __DEBUG__
    printf("1, /proc/stat, buf=%s\n",buf);
#endif
    sscanf(buf,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);
    total = user+nice+sys+idle+iowait+irq+softirq;

    return total;
}

int get_thread_rate_idle(int pid ,int tid)
{
    FILE *fp;
    char buf[512], *ptr;
    char bufname[50];
    char sdata2[20], sdata3[5];
    int data1,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15;


    snprintf(bufname, 50, "/proc/%d/task/%d/stat", pid, tid);
#if __DEBUG__
    printf("thread bufname=%s\n",bufname);
#endif
    fp = fopen(bufname,"r");
    if(fp == NULL) {  
        return 0; 
    }  

    memset(buf, 0x00, 512);
    fgets(buf, sizeof(buf), fp);
    fclose(fp);
#if __DEBUG__
    printf("1, %s, buf=%s\n", bufname, buf);
#endif
    sscanf(buf,"%ld%s%s%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld",
            &data1,sdata2,sdata3,&data4,&data5,
            &data6,&data7,&data8,&data9,&data10,
            &data11,&data12,&data13,&data14,&data15);

    return (data14 + data15);
}

int main(int argc, char *argv[])
{
    int i;
    int pid;
    long int total1, total2, idle;
    char path[128], dir_name[128];
    DIR *dir;
    struct dirent *ptr;
    int thread_num, cpunum;
    thread_cpurate_t cpurate[MAX_THREADS];

    if (argc < 2) {
        printf("Usage:%s [proc id]\n", argv[0]);
        return 0;
    }

    pid = atoi(argv[1]);
    memset(path, 0x00, 128);
    snprintf(path, 128, "/proc/%d/task/", pid);
    
    dir = opendir(path);
    if (NULL == dir) {
        printf("open dir %s error\n", path);
        return 0;
    }
    thread_num = 0;
    while (NULL != (ptr=readdir(dir))) {
        if (strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0) {
            continue;
        } else if (ptr->d_type == 4) {///dir
            memset(dir_name, 0x00, sizeof(dir_name));
            strcpy(dir_name, path);
            strcat(dir_name, "/");
            strcat(dir_name, ptr->d_name);
            cpurate[thread_num].tid = atoi(ptr->d_name);
            thread_num++;
        } else if (ptr->d_type == 8) {///file
            printf("d_name:%s/%s\n", path, ptr->d_name);
        } else if (ptr->d_type == 10) {///link file
            printf("d_name:%s/%s\n", path, ptr->d_name);
        }

    }
    closedir(dir);

    cpunum = get_cpu_num();
    total1 = get_cpu_rate_total();
    for (i = 0; i < thread_num; i++) {
        idle = get_thread_rate_idle(pid, cpurate[i].tid);
        cpurate[i].idle = idle;
    }
    
    sleep(1);
    total2 = get_cpu_rate_total();
    for (i = 0; i < thread_num; i++) {
        idle = get_thread_rate_idle(pid, cpurate[i].tid);
        if (total2 - total1) {
            cpurate[i].rate = ((idle - cpurate[i].idle) * cpunum * 10000) / (total2 - total1);
        } else {
            cpurate[i].rate = 0;
        }
    }
    printf("  Id   Pid   tid   rate\n");
    for (i = 0; i < thread_num; i++) {
        printf("%-4d %-6d   %-6d   %d.%02d%%\n", i, pid, cpurate[i].tid, cpurate[i].rate/100, cpurate[i].rate%100);
    }

    return 0;
}
