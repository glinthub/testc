/*
 * pdu.h
 *
 *  Created on: 2018��9��12��
 *      Author: a
 */

#ifndef UTILS_PDU_H
#define UTILS_PDU_H

//#include "autosar_ext.h"

typedef struct _pdu Pdu_t;

XL4_BOOL    Pdu_PoolInit();
Pdu_t*      Pdu_Alloc();
void        Pdu_Release(Pdu_t *pdu);
void        Pdu_Release_List(Pdu_t *pduList);
void        Pdu_Detach(Pdu_t **pduList);
void        Pdu_Append(Pdu_t **pduList, Pdu_t *tailPdu);
XL4_UCHAR8* Pdu_Get_Buffer(Pdu_t *pdu);
XL4_UINT32  Pdu_Get_Length(Pdu_t *pdu);
Pdu_t*      Pdu_Get_Next(Pdu_t *pdu);
void        Pdu_Set_Next(Pdu_t *pdu, Pdu_t *pduNext);
LogicalAddress Pdu_GetTargetAddressByPduId(PduIdType pduId);
void        Pdu_Set_Length(Pdu_t *pdu, XL4_UINT32 length);

#endif /* UTILS_PDU_H */
