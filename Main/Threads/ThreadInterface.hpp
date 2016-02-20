#ifndef THREADINTERFACE_HPP
#define THREADINTERFACE_HPP

#include <pthread.h>

class ThreadInterface
{
  public:
    ThreadInterface();
    virtual ~ThreadInterface();

    int start();
    int join();
    int detach();
    pthread_t self();

    virtual void* run() = 0;

  private:
    pthread_t  m_tid;
    int        m_running;
    int        m_detached;
};

#endif
