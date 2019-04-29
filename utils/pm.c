/*
 * pm.c
 *
 *  Created on: Sep 27, 2018
 *      Author: zws
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct {
    uint32_t    totalPackets;
    uint32_t    totalBytes;
    uint32_t    velocity;
    uint32_t    velocityMax;
    struct timeval    firstUpdate;
    struct timeval    lastUpdate;
    uint32_t    granularity;    //range: 0.01-1 second, default: 1 second
    uint32_t    factor;         //range: 1-100, default: 50
    uint8_t     enabled;
} Pm_t;

void pm_clear(Pm_t * pm)
{
    memset(pm, 0, sizeof(Pm_t));
}

Pm_t * pm_alloc(uint32_t granularity, uint32_t factor)
{
    Pm_t *pm = (Pm_t *)malloc(sizeof(Pm_t));
    pm_clear(pm);
    pm->factor = factor;
    pm->granularity = granularity;
    return pm;
}

void pm_release(Pm_t * pm)
{
    free (pm);
}

Pm_t *pm_pool_init(uint32_t count)
{
    return (Pm_t *)malloc(sizeof(Pm_t)*count);
}


uint32_t time_delta(struct timeval *tv1, struct timeval* tv2)
{
    uint32_t delta = (tv2->tv_sec - tv1->tv_sec) * 1000000;
    delta = delta + tv2->tv_usec - tv1->tv_usec;
    return delta;
}

void pm_update(Pm_t *pm, uint32_t pktSize)
{

    if (!pm->enabled)
        return;

    struct timeval now;
    gettimeofday(&now, 0);
    if (pm->firstUpdate.tv_sec == 0)
    {
        pm->firstUpdate = now;
    }
    else
    {
        uint32_t delta = time_delta(&pm->lastUpdate, &now);
        if (delta < pm->granularity)
        {
            uint32_t v = (float)pktSize / ((float)delta/1000000);
            printf("#### pm_update b %u, d %u, v %u\n", pktSize, delta, v);
            if (pm->velocity == 0)
                pm->velocity = v;
            else
                pm->velocity = (pm->velocity * (100 - pm->factor) + v * pm->factor) / 100;
            if (pm->velocity > pm->velocityMax)
                pm->velocityMax = pm->velocity;
        }
    }
    pm->totalPackets++;
    pm->totalBytes += pktSize;
    pm->lastUpdate = now;
}

void pm_enable(Pm_t *pm)
{
    pm->enabled = 1;
}

void pm_disable(Pm_t *pm)
{
    pm->enabled = 0;
}

void pm_show(Pm_t *pm)
{
    uint32_t delta = time_delta(&pm->firstUpdate, &pm->lastUpdate);
    uint32_t avg_v = (float)pm->totalBytes / ((float)delta/1000000);
    printf("total packets:      %u\n", pm->totalPackets);
    printf("total bytes:        %u\n", pm->totalBytes);
    printf("firstUpdate:        %u.%u\n", (uint32_t)pm->firstUpdate.tv_sec, (uint32_t)pm->firstUpdate.tv_usec);
    printf("lastUpdate:         %u.%u\n", (uint32_t)pm->lastUpdate.tv_sec, (uint32_t)pm->lastUpdate.tv_usec);
    printf("duration:           %u us\n", delta);
    printf("peak velocity:      %u\n", pm->velocityMax);
    printf("average velocity:   %u\n", avg_v    );
}

#define PM_TEST_NUM 20
void pm_test()
{
    struct timeval now;
    gettimeofday(&now, 0);
    srandom(now.tv_usec);

    Pm_t *pm = pm_alloc(500000, 60);
    pm_enable(pm);
    int data[PM_TEST_NUM];
    int interval[PM_TEST_NUM];
    int i = 0;
    int totalBytes = 0;
    int duration = 0;
    for (i=0; i<PM_TEST_NUM; i++)
    {
        data[i] = random() % 2000;
        totalBytes += data[i];
    }
    for (i=0; i<PM_TEST_NUM; i++)
    {
        interval[i] = random() % 100000;
        if (i>0)
            duration += interval[i];
    }
    for (i=0; i<PM_TEST_NUM; i++)
    {
        usleep(interval[i]);
        pm_update(pm, data[i]);
    }
    printf("actual total bytes: %d, duration %d, average velocity: %f\n",
            totalBytes, duration, (float)totalBytes / ((float)duration/1000000));
    pm_show(pm);
    pm_release(pm);
}

