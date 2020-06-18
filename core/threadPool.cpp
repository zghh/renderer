#include "threadPool.h"

void Runnable::setDone(bool done)
{
    this->done = done;
}

bool Runnable::isDone()
{
    bool result = this->done;
    return result;
}

ThreadPool::ThreadPool()
{
    this->isShutdown = 0;
    this->nThread = 0;
    this->Init(NUMBER_OF_THREAD);
}

ThreadPool::~ThreadPool()
{
    pthread_mutex_lock(&lock);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&notify);
    delete[] threads;
}

void ThreadPool::AddTask(Runnable *object)
{
    if (this->nThread == 0 || object == NULL)
        return;
    if (pthread_mutex_lock(&lock) != 0)
        return;
    if (this->isShutdown)
        return;

    this->threadQueue.push(object);

    if (pthread_cond_signal(&notify) != 0)
        return;
    pthread_mutex_unlock(&lock);
}

void ThreadPool::Destory(ThreadPoolShutdown flag)
{
    do
    {
        if (pthread_mutex_lock(&lock) != 0)
            return;

        this->isShutdown = flag;

        if ((pthread_cond_broadcast(&notify) != 0) || (pthread_mutex_unlock(&lock) != 0))
            break;

        for (int i = 0; i < this->nThread; i++)
            if (pthread_join(this->threads[i], NULL) != 0)
                break;
        return;
    } while (0);
}

void *ThreadPool::threadPoolThread(void *threadPool)
{
    ThreadPool *pool = (ThreadPool *)threadPool;
    while (1)
    {
        pthread_mutex_lock(&(pool->lock));
        while ((pool->threadQueue.empty()) && (!pool->isShutdown))
        {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if ((pool->isShutdown == IMMEDIATE_SHUTDOWN) ||
            ((pool->isShutdown == GRACEFUL_SHUTDOWN) && (pool->threadQueue.empty())))
        {
            break;
        }

        Runnable *runnable;
        if (!pool->threadQueue.empty())
        {
            runnable = pool->threadQueue.front();
            pool->threadQueue.pop();
        }

        pthread_mutex_unlock(&(pool->lock));

        runnable->Run();
    }

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return (NULL);
}

void ThreadPool::Init(int number)
{
    do
    {
        if (pthread_mutex_init(&lock, NULL))
            break;
        if (pthread_cond_init(&notify, NULL))
            break;

        this->threads = new pthread_t[number];

        for (int i = 0; i < number; i++)
        {
            if (pthread_create(threads + i, NULL, threadPoolThread, (void *)this) != 0)
            {
                this->Destory();
                break;
            }
            this->nThread++;
        }
        return;

    } while (0);
    this->nThread = 0;
}