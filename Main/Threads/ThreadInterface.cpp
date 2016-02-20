#include "ThreadInterface.hpp"
#include <pthread.h>

ThreadInterface::ThreadInterface() : m_tid(0), m_running(0), m_detached(0) {}

ThreadInterface::~ThreadInterface()
{
    if (m_running == 1 && m_detached == 0) {
        pthread_detach(m_tid);
    }
    if (m_running == 1) {
        pthread_cancel(m_tid);
    }
}

static void* runThread(void* arg);

int ThreadInterface::start()
{
    int result = pthread_create(&m_tid, NULL, runThread, this);
    if (result == 0) {
        m_running = 1;
    }
    return result;
}

static void* runThread(void* arg)
{
    return ((ThreadInterface*)arg)->run();
}

int ThreadInterface::join()
{
    int result = -1;
    if (m_running == 1) {
        result = pthread_join(m_tid, NULL);
        if (result == 0) {
            m_detached = 1;
        }
    }
    return result;
}

int ThreadInterface::detach()
{
    int result = -1;
    if (m_running == 1 && m_detached == 0) {
        result = pthread_detach(m_tid);
        if (result == 0) {
            m_detached = 1;
        }
    }
    return result;
}

pthread_t ThreadInterface::self() {
    return m_tid;
}
