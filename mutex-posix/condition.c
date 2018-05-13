#include "condition.h"

//初使化条件变量，可想而知是对互斥锁和条件变量进行初始化
int condition_init(condition_t *cond)
{
    int status;
    if ((status = pthread_mutex_init(&cond->pmutex, NULL)))
        return status;

    if ((status = pthread_cond_init(&cond->pcond, NULL)))
        return status;

    return 0;
}

//对互斥锁进行锁定
int condition_lock(condition_t *cond)
{
    return pthread_mutex_lock(&cond->pmutex);
}

//对互斥锁进行解锁
int condition_unlock(condition_t *cond)
{
    return pthread_mutex_unlock(&cond->pmutex);
}

//在条件变量上等待条件
int condition_wait(condition_t *cond)
{
    return pthread_cond_wait(&cond->pcond, &cond->pmutex);
}

//具有超时功能的等待功能
int condition_timedwait(condition_t *cond, const struct timespec *abstime)
{
    return pthread_cond_timedwait(&cond->pcond, &cond->pmutex, abstime);
}

//向等待线程发起一个通知
int condition_signal(condition_t *cond)
{
    return pthread_cond_signal(&cond->pcond);
}

//向等待线程发起广播
int condition_broadcast(condition_t* cond)
{
    return pthread_cond_broadcast(&cond->pcond);
}

//销毁条件变量
int condition_destroy(condition_t* cond)
{
    int status;
    if ((status = pthread_mutex_destroy(&cond->pmutex)))
        return status;

    if ((status = pthread_cond_destroy(&cond->pcond)))
        return status;

    return 0;
}