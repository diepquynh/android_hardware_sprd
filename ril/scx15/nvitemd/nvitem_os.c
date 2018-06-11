
#include "nvitem_os.h"

#if 1
typedef struct
{
	pthread_mutex_t	mutex;
	pthread_cond_t	notify;
}THREAD_COMMUNICAT;
static THREAD_COMMUNICAT communicate;

void initEvent(void)
{
	pthread_mutex_init(&communicate.mutex, 0);
	pthread_cond_init(&communicate.notify, 0);
}

void waiteEvent(void)
{
	pthread_cond_wait(&communicate.notify, 0);
}

void giveEvent(void)
{
	pthread_cond_signal(&communicate.notify);
}

void getMutex(void)
{
	pthread_mutex_lock(&communicate.mutex);
}

void putMutex(void)
{
	pthread_mutex_unlock(&communicate.mutex);
}

#else

void initEvent(void){}

void waiteEvent(void){}

void giveEvent(void){}

void getMutex(void){}

void putMutex(void){}


#endif
