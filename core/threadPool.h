#pragma once
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <atomic>
#include <pthread.h>
#include "const.h"

using namespace std;

#ifndef THREADPOOL_H
#define THREADPOOL_H

class Runnable
{
private:
    atomic<bool> done;

public:
    virtual void Run() = 0;
    void setDone(bool done);
    bool isDone();
};

enum ThreadPoolShutdown
{
    GRACEFUL_SHUTDOWN = 1, // 等待线程结束后关闭
    IMMEDIATE_SHUTDOWN = 2 // 立即关闭
};

class ThreadPool
{
private:
    pthread_mutex_t lock;
    pthread_cond_t notify;
    queue<Runnable *> threadQueue;
    pthread_t *threads;
    int nThread;
    int isShutdown;
    static void *threadPoolThread(void *threadPool);
    void Init(int number);

public:
    ThreadPool();
    ~ThreadPool();
    void AddTask(Runnable *object);
    void Destory(ThreadPoolShutdown flag = GRACEFUL_SHUTDOWN);
};

#endif