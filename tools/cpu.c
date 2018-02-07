/**
 * @Filename: cpu.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/28/2017 09:21:01 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROCSTATFILE "/proc/stat"
#define PROC_LINE_LENGTH    512


typedef struct CPUData_ {
    unsigned long long int totalTime;
    unsigned long long int userTime;
    unsigned long long int systemTime;
    unsigned long long int systemAllTime;
    unsigned long long int idleAllTime;
    unsigned long long int idleTime;
    unsigned long long int niceTime;
    unsigned long long int ioWaitTime;
    unsigned long long int irqTime;
    unsigned long long int softIrqTime;
    unsigned long long int stealTime;
    unsigned long long int guestTime;

    unsigned long long int totalPeriod;
    unsigned long long int userPeriod;
    unsigned long long int systemPeriod;
    unsigned long long int systemAllPeriod;
    unsigned long long int idleAllPeriod;
    unsigned long long int idlePeriod;
    unsigned long long int nicePeriod;
    unsigned long long int ioWaitPeriod;
    unsigned long long int irqPeriod;
    unsigned long long int softIrqPeriod;
    unsigned long long int stealPeriod;
    unsigned long long int guestPeriod;
} CPUData_t;

#define WRAP_SUBTRACT(a,b) (a > b) ? a - b : 0
#define CLAMP(x,low,high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))

typedef enum {
    CPU_METER_NICE = 0,
    CPU_METER_NORMAL = 1,
    CPU_METER_KERNEL = 2,
    CPU_METER_IRQ = 3,
    CPU_METER_SOFTIRQ = 4,
    CPU_METER_STEAL = 5,
    CPU_METER_GUEST = 6,
    CPU_METER_IOWAIT = 7,
    CPU_METER_ITEMCOUNT = 8, // number of entries in this enum
} CPUMeterValues;


double Platform_setCPUValues(CPUData_t *cpuData, int cpu)
{
    double total;
    double percent;
    double v[9];

    memset(v, 0x00, sizeof(v));
    total = (double) (cpuData->totalPeriod == 0 ? 1 : cpuData->totalPeriod);
    v[CPU_METER_NICE] = cpuData->nicePeriod / total * 100.0;
    v[CPU_METER_NORMAL] = cpuData->userPeriod / total * 100.0;
    if (this->pl->settings->detailedCPUTime) {
        v[CPU_METER_KERNEL]  = cpuData->systemPeriod / total * 100.0;
        v[CPU_METER_IRQ]     = cpuData->irqPeriod / total * 100.0;
        v[CPU_METER_SOFTIRQ] = cpuData->softIrqPeriod / total * 100.0;
        v[CPU_METER_STEAL]   = cpuData->stealPeriod / total * 100.0;
        v[CPU_METER_GUEST]   = cpuData->guestPeriod / total * 100.0;
        v[CPU_METER_IOWAIT]  = cpuData->ioWaitPeriod / total * 100.0;
        Meter_setItems(this, 8); 
        if (this->pl->settings->accountGuestInCPUMeter) {
            percent = v[0]+v[1]+v[2]+v[3]+v[4]+v[5]+v[6]
        } else {
            percent = v[0]+v[1]+v[2]+v[3]+v[4];
        }   
    } else {
        v[2] = cpuData->systemAllPeriod / total * 100.0;
        v[3] = (cpuData->stealPeriod + cpuData->guestPeriod) / total * 100.0;
        Meter_setItems(this, 4); 
        percent = v[0]+v[1]+v[2]+v[3];
    }   
    percent = CLAMP(percent, 0.0, 100.0);
    if (isnan(percent)) {
        percent = 0.0;
    }

    return percent;
}

int scanCPUTime(CPUData_t *cpuData, int ncpu)
{
    int i;
    int cpuid;
    FILE* file;
    char *recode;
    char buffer[PROC_LINE_LENGTH + 1];
    unsigned long long int usertime, nicetime, systemtime, idletime;
    unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
    unsigned long long int totaltime, idlealltime, systemalltime, virtalltime;

    file = fopen(PROCSTATFILE, "r");
    if (file == NULL) {
        printf("Cannot open " PROCSTATFILE);
    }

    recode = fgets(buffer, PROC_LINE_LENGTH, file);
    if (recode) {
        printf("Invalid proc stat file\n");
        fclose(file);
        return -1;
    }

    sscanf(buffer, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu", &usertime, &nicetime, &systemtime, 
            &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
    usertime = usertime - guest;
    nicetime = nicetime - guestnice;
    // Fields existing on kernels >= 2.6
    // (and RHEL's patched kernel 2.4...)
    idlealltime = idletime + ioWait;
    systemalltime = systemtime + irq + softIrq;
    virtalltime = guest + guestnice;
    totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

    cpuData[0].userPeriod = WRAP_SUBTRACT(usertime, cpuData[0].userTime);
    cpuData[0].nicePeriod = WRAP_SUBTRACT(nicetime, cpuData[0].niceTime);
    cpuData[0].systemPeriod = WRAP_SUBTRACT(systemtime, cpuData[0].systemTime);
    cpuData[0].systemAllPeriod = WRAP_SUBTRACT(systemalltime, cpuData[0].systemAllTime);
    cpuData[0].idleAllPeriod = WRAP_SUBTRACT(idlealltime, cpuData[0].idleAllTime);
    cpuData[0].idlePeriod = WRAP_SUBTRACT(idletime, cpuData[0].idleTime);
    cpuData[0].ioWaitPeriod = WRAP_SUBTRACT(ioWait, cpuData[0].ioWaitTime);
    cpuData[0].irqPeriod = WRAP_SUBTRACT(irq, cpuData[0].irqTime);
    cpuData[0].softIrqPeriod = WRAP_SUBTRACT(softIrq, cpuData[0].softIrqTime);
    cpuData[0].stealPeriod = WRAP_SUBTRACT(steal, cpuData[0].stealTime);
    cpuData[0].guestPeriod = WRAP_SUBTRACT(virtalltime, cpuData[0].guestTime);
    cpuData[0].totalPeriod = WRAP_SUBTRACT(totaltime, cpuData[0].totalTime);

    cpuData[0].userTime = usertime;
    cpuData[0].niceTime = nicetime;
    cpuData[0].systemTime = systemtime;
    cpuData[0].systemAllTime = systemalltime;
    cpuData[0].idleAllTime = idlealltime;
    cpuData[0].idleTime = idletime;
    cpuData[0].ioWaitTime = ioWait;
    cpuData[0].irqTime = irq;
    cpuData[0].softIrqTime = softIrq;
    cpuData[0].stealTime = steal;
    cpuData[0].guestTime = virtalltime;
    cpuData[0].totalTime = totaltime;

    for (i = 0; i < ncpu; i++) {
        memset(buffer, 0x00, PROC_LINE_LENGTH + 1);
        ioWait = irq = softIrq = steal = guest = guestnice = 0;

        recode = fgets(buffer, PROC_LINE_LENGTH, file);
        if (!recode) {
            break;
        }
        sscanf(buffer, "cpu%4d %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu", &cpuid, &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        // Guest time is already accounted in usertime
        usertime = usertime - guest;
        nicetime = nicetime - guestnice;
        // Fields existing on kernels >= 2.6
        // (and RHEL's patched kernel 2.4...)
        idlealltime = idletime + ioWait;
        systemalltime = systemtime + irq + softIrq;
        virtalltime = guest + guestnice;
        totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

        // Since we do a subtraction (usertime - guest) and cputime64_to_clock_t()
        // used in /proc/stat rounds down numbers, it can lead to a case where the
        // integer overflow.
        cpuData[i + 1].userPeriod = WRAP_SUBTRACT(usertime, cpuData[i + 1].userTime);
        cpuData[i + 1].nicePeriod = WRAP_SUBTRACT(nicetime, cpuData[i + 1].niceTime);
        cpuData[i + 1].systemPeriod = WRAP_SUBTRACT(systemtime, cpuData[i + 1].systemTime);
        cpuData[i + 1].systemAllPeriod = WRAP_SUBTRACT(systemalltime, cpuData[i + 1].systemAllTime);
        cpuData[i + 1].idleAllPeriod = WRAP_SUBTRACT(idlealltime, cpuData[i + 1].idleAllTime);
        cpuData[i + 1].idlePeriod = WRAP_SUBTRACT(idletime, cpuData[i + 1].idleTime);
        cpuData[i + 1].ioWaitPeriod = WRAP_SUBTRACT(ioWait, cpuData[i + 1].ioWaitTime);
        cpuData[i + 1].irqPeriod = WRAP_SUBTRACT(irq, cpuData[i + 1].irqTime);
        cpuData[i + 1].softIrqPeriod = WRAP_SUBTRACT(softIrq, cpuData[i + 1].softIrqTime);
        cpuData[i + 1].stealPeriod = WRAP_SUBTRACT(steal, cpuData[i + 1].stealTime);
        cpuData[i + 1].guestPeriod = WRAP_SUBTRACT(virtalltime, cpuData[i + 1].guestTime);
        cpuData[i + 1].totalPeriod = WRAP_SUBTRACT(totaltime, cpuData[i + 1].totalTime);

        cpuData[i + 1].userTime = usertime;
        cpuData[i + 1].niceTime = nicetime;
        cpuData[i + 1].systemTime = systemtime;
        cpuData[i + 1].systemAllTime = systemalltime;
        cpuData[i + 1].idleAllTime = idlealltime;
        cpuData[i + 1].idleTime = idletime;
        cpuData[i + 1].ioWaitTime = ioWait;
        cpuData[i + 1].irqTime = irq;
        cpuData[i + 1].softIrqTime = softIrq;
        cpuData[i + 1].stealTime = steal;
        cpuData[i + 1].guestTime = virtalltime;
        cpuData[i + 1].totalTime = totaltime;
    }
    fclose(file);

    return 0;
}

int main(int argc ,char *argv[])
{
    CPUData_t cpu[3];
    
    scanCPUTime(cpu, 3);
    printf("total:%lf",cpu[0].totalPeriod);

    return 0;
}
