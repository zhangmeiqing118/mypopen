/**
 * @Filename: threads.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/28/2017 12:41:52 PM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define THREAD_NUM  10
#define SLEEP_TIME  100

void *thread_test(void *args)
{
    int times;
    ///pthread_t *tid;
     pid_t tid;

    times = 0;
    ///tid = (pthread_t *)args;
    //tid = getpid();
    tid = pthread_self();
    while (1) {
        times++;
        ///printf("thread %lu run :%d\n", *tid, times);
        //printf("thread %lu run :%d\n", tid, times);
        usleep(SLEEP_TIME);
    }
}

int main(int argc, char *argv[])
{
    int i;
    pid_t pid;
    pthread_t tid[THREAD_NUM];

    memset(tid, 0x00, sizeof(tid));
    for (i = 0; i < THREAD_NUM; i++) {
        pthread_create(&(tid[i]), NULL, thread_test, &(tid[i]));
    }

    pid = getpid();
    while (1) {
        thread_test(&pid);
    }

    return 0;
}
