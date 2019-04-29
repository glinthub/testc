/*
 * pdu.c
 *
 *  Created on: 2018��9��11��
 *      Author: a
 */
#include <stdint.h>
#include <stdlib.h>
#include "utils/log.h"

typedef struct _pdu {
    struct _pdu *nextFreePdu;
    struct _pdu *nextPdu;
    uint16_t length;
    uint8_t buf[0];
} Pdu_t;

typedef struct {
    uint8_t initialized;
    Pdu_t *pduPool;
    Pdu_t *pduFreeList;
    uint16_t pduSize;
    uint16_t poolSize;
} PduMgr_t;

PduMgr_t pduMan = {0};

int Pdu_PoolInit(uint16_t pduSize, uint16_t poolSize)
{
    int i = 0;

    if (pduMan.initialized)
    {
        printf("PDU pool already initiated!\n");
        return -1;
    }
    pduMan.pduPool = (Pdu_t *) malloc( (sizeof(Pdu_t)+pduSize) * poolSize);
    if (NULL == pduMan.pduPool)
    {
        printf("PDU pool initiate failed!\n");
        return -1;
    }
    for (i=0; i<poolSize-1; i++)
    {
        pduMan.pduPool[i].nextFreePdu = &pduMan.pduPool[i+1];
    }
    pduMan.pduPool[poolSize - 1].nextFreePdu = NULL;
    pduMan.pduFreeList = pduMan.pduPool;
    return 0;
}

Pdu_t *Pdu_Alloc()
{
    if (NULL == pduMan.pduFreeList)
    {
        printf("PDU pool exhausted!\n");
        return NULL;
    }
    Pdu_t *pdu = pduMan.pduFreeList;
    pduMan.pduFreeList = pdu->nextFreePdu;
    pdu->nextFreePdu = NULL;
    printf("PDU alloc ok!\n");
    return pdu;
}

void Pdu_Release(Pdu_t *pdu)
{
    if (NULL == pdu)
    {
        printf("Try to free NULL PDU!\n");
        return;
    }
    pdu->nextFreePdu = pduMan.pduFreeList;
    pdu->nextPdu = NULL;
    pduMan.pduFreeList = pdu;
    printf("PDU release ok!\n");
}

void Pdu_Release_List(Pdu_t *pduList)
{
    Pdu_t *pdu = pduList;
    Pdu_t *pduNext = NULL;
    if (NULL == pdu)
    {
        return;
    }
    else
    {
        while (NULL != pdu)
        {
            pduNext = pdu->nextPdu;
            Pdu_Release(pdu);
            pdu = pduNext;
        }
    }
}

Pdu_t * Pdu_Detach(Pdu_t **pduList)
{
    Pdu_t *pdu = *pduList;
    if (NULL == pdu)
    {
        printf("Try to detach empty PDU list!\n");
        return NULL;
    }
    *pduList = pdu->nextPdu;
    pdu->nextPdu = NULL;
    printf("PDU detach ok!\n");
    return pdu;
}

void Pdu_Append(Pdu_t **pduList, Pdu_t *tailPdu)
{
    Pdu_t *pdu = *pduList;
    if (NULL == pdu)
    {
        *pduList = tailPdu;
    }
    else
    {
        while (NULL != pdu->nextPdu)
        {
            pdu = pdu->nextPdu;
        }
        pdu->nextPdu = tailPdu;
    }
}

uint8_t *Pdu_Get_Buffer(Pdu_t *pdu)
{
    return pdu->buf;
}

uint16_t Pdu_Get_Length(Pdu_t *pdu)
{
    return pdu->length;
}

Pdu_t *Pdu_Get_Next(Pdu_t *pdu)
{
    return pdu->nextPdu;
}

void Pdu_Set_Next(Pdu_t *pdu, Pdu_t *pduNext)
{
    pdu->nextPdu = pduNext;
}

void Pdu_Set_Length(Pdu_t *pdu, uint16_t length)
{
    pdu->length = length;
}

