#include <unistd.h> /* nice */
#include <utility> /* stdd::pair */
#include <memory> /* shared pointer */

#include "thread_pool.hpp"

namespace ilrd
{

/*********************************THREAD UNIT************************************************/

ThreadPool::ThreadUnit::ThreadUnit(ThreadPool *m_thread_pool) : m_thread_pool(m_thread_pool), 
        m_thread_state(READY), m_thread(std::thread(&ThreadPool::ThreadUnit::ThreadRun, this))
{
    //empty
}

ThreadPool::ThreadUnit::~ThreadUnit()
{
    m_thread.join();
}

void ThreadPool::ThreadUnit::ThreadRun() noexcept
{
    std::mutex mx;
    std::unique_lock<std::mutex> ul(mx);
    
    while(m_thread_pool->m_state != DONE && m_thread_state == READY) //m_m_thread_state
    {
        m_thread_pool->cv.wait(ul);
        while(m_thread_pool->m_state == RUN)
        {
            std::pair<std::shared_ptr<Task>, PRIORITY> out_task;
            m_thread_pool->m_task_queue.Pop(out_task);
            (*out_task.first)();
        }
    }
}
/********************************************************************************************/

/*********************************THREAD POOL************************************************/

ThreadPool::ThreadPool(size_t n_threads, size_t niceness) : m_threads(n_threads), m_state(PAUSE)
{
    errno = 0;
    
    if(-1 == (nice(niceness)) && errno != 0)
    {
        throw std::system_error(errno, std::generic_category());
    }
    
    for (size_t i = 0; i < n_threads; ++i)
    {
        std::shared_ptr<ThreadUnit> new_tu(new ThreadUnit(this));
        m_thread_list.push_back(new_tu);
    }
}

ThreadPool::~ThreadPool() noexcept
{
    m_state = DONE;
    cv.notify_all();

    AddTasks();

    while(!m_thread_list.empty())
    {
        m_thread_list.pop_front();
    }
}

void ThreadPool::Add(std::shared_ptr<Task> function, PRIORITY pr)
{
    m_task_queue.Push(std::pair<std::shared_ptr<Task>, PRIORITY>(function, pr));
}

void ThreadPool::Run() noexcept
{
    m_state = RUN;
    cv.notify_all();
}

void ThreadPool::Pause() noexcept
{
    m_state = PAUSE;

    AddTasks();
}

void ThreadPool::Stop() noexcept
{
    m_state = STOP;

    Flush();
}

void ThreadPool::SetNumberOfThreads(size_t n_threads)
{
    if(m_threads < n_threads)
    {
        for (size_t i = 0; i < (n_threads - m_threads); ++i)
        {
            std::shared_ptr<ThreadUnit> new_tu(new ThreadUnit(this));
            m_thread_list.push_back(new_tu);
        }
    }
    else
    {
        Pause();
        for(size_t i = 0; i < (m_threads - n_threads); ++i)
        {
            ThreadUnit *tu = m_thread_list.back().get();
            tu->SetState(EXIT);
            cv.notify_all();
            m_thread_list.pop_back();
        }
        Run();
    }
}
/********************************************************************************************/


/***********************************AUX FUNCS************************************************/

void ThreadPool::Flush() noexcept
{
    AddTasks();

    while (!m_task_queue.IsEmpty())
    {
        std::pair<std::shared_ptr<Task>, PRIORITY> out_task;
        m_task_queue.Pop(out_task);
    }
}

void ThreadPool::AddTasks()
{
    size_t num_threads = m_thread_list.size();
    
    for(size_t i = 0; i < num_threads; ++i)
    {
        Add(std::shared_ptr<Task>(new Default_Task([]() {})), SYS_RESERVED);
    }
}

/********************************************************************************************/


/*****************************************TASK***********************************************/

Default_Task::Default_Task(std::function<void()> func)
    : m_func(func)
{
    // empty
}

Default_Task::~Default_Task()
{
}

/********************************************************************************************/

}