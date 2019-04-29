#ifndef LOCK_H
#define LOCK_H

#if 1
#define HERE1 printf(" at %s:%d\n", __FUNCTION__, __LINE__)
#define RESULT printf("result %d\n", ret)
#else
#endif

#if 1
#define LOCKER sem_t
#define LOCK_INIT(l)           sem_init(l, 0, 0)
#define LOCK_LOCK(l)           sem_wait(l)
#define LOCK_TIMEDLOCK(l, t)   sem_timedwait(l, t)
#define LOCK_UNLOCK(l)         sem_post(l)
#define LOCK_DESTROY(l)        sem_destroy(l)
#else
#define LOCKER pthread_mutex_t
#define LOCK_INIT(l)           pthread_mutex_init(l, NULL)
#define LOCK_LOCK(l)           pthread_mutex_lock(l)
#define LOCK_TIMEDLOCK(l, t)   pthread_mutex_timedlock(l, t)
#define LOCK_UNLOCK(l)         pthread_mutex_unlock(l)
#define LOCK_DESTROY(l)        pthread_mutex_destroy(l)
#endif

#endif  //LOCK_H
