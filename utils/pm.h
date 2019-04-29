/*
 * pm.h
 *
 *  Created on: Sep 27, 2018
 *      Author: zws
 */

#ifndef UTILS_PM_H
#define UTILS_PM_H

typedef struct _pm Pm_t;

Pm_t    *pm_alloc(uint32_t granularity, uint32_t factor);
void    pm_release(Pm_t * pm);
void    pm_clear(Pm_t * pm);
void    pm_update(Pm_t *pm, uint32_t pktSize);
void    pm_enable(Pm_t *pm);
void    pm_disable(Pm_t *pm);
void    pm_show(Pm_t *pm);

#endif //UTILS_PM_H
