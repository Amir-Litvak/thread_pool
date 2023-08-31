#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__
#include <functional>  /* function */
#include <thread> /* thread */
#include <list> /* list */
#include <queue> /* pq */
#include <memory> /* shared pointer */

#include "waitable_queue.hpp"
#include "pq_wrapper.hpp"

namespace thread_pool 
{
    enum PRIORITY
    {
        SYS_RESERVED,
        HIGH,
        MEDIUM,
        LOW
    };

    enum STATUS
    {
        STOP = 0,
        PAUSE, 
        RUN,
        DONE
    };

    enum THREAD_STATE
    {
        READY = 0,
        EXIT
    };

    class Task;

    class CompareFunctor;

    class ThreadPool
    {
        private:
        class ThreadUnit; //forward declaration

        public:
            explicit ThreadPool(size_t n_threads = std::thread::hardware_concurrency(), size_t niceness = 0);
            ~ThreadPool() noexcept;
            ThreadPool(const ThreadPool&) = delete;
            ThreadPool &operator=(const ThreadPool&) = delete;
            void Add(std::shared_ptr<Task> function, PRIORITY pr);
            void Run() noexcept;
            void Pause() noexcept; //stops pulling tasks from queue
            void Stop() noexcept;  //Pause + flush for task queue
            void SetNumberOfThreads(size_t n_threads);
        
        private:
            void Flush() noexcept;
            void AddTasks();
            Waitable_Queue<std::pair<std::shared_ptr<Task>, PRIORITY>, 
                        PQWrapper<std::pair<std::shared_ptr<Task>, PRIORITY>, 
                        std::vector<std::pair<std::shared_ptr<Task>, PRIORITY>>, 
                                                    CompareFunctor>> m_task_queue;
            std::list<std::shared_ptr<ThreadUnit>> m_thread_list;
            size_t m_threads;
            enum STATUS m_state;
            std::condition_variable cv;
            
            //nested RAII thread class
            class ThreadUnit
            {
            public:
                explicit ThreadUnit(ThreadPool *m_thread_pool);
                ~ThreadUnit();
                ThreadUnit(const ThreadUnit&) = delete;
                ThreadUnit operator=(const ThreadUnit&) = delete;
                THREAD_STATE GetState() const { return m_thread_state; }
                void SetState(THREAD_STATE ts) { m_thread_state = ts; }

            private:
                std::function<void()> m_task;
                ThreadPool *m_thread_pool;
                THREAD_STATE m_thread_state;
                std::thread m_thread;
                void ThreadRun() noexcept;
                
            };
    };

    class CompareFunctor
    {
    public:
        bool operator()(const std::pair<std::shared_ptr<Task>, PRIORITY> &lhs, 
                        const std::pair<std::shared_ptr<Task>, PRIORITY> &rhs)
        {
            return (lhs.second > rhs.second);
        }
    };

    class Task
    {
    public:
        void virtual operator()() = 0;
        virtual ~Task() {}

    private:
    };

    class Default_Task : public Task
    {
    public:
        Default_Task(std::function<void()> func);
        void virtual operator()()
        {
            m_func();
        }
        virtual ~Default_Task();

    private:
        std::function<void()> m_func;
    };

} // namespace thread_pool


#endif //__THREAD_POOL_HPP__