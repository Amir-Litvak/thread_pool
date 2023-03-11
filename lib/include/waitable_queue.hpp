#ifndef __WAITABLE_QUEUE_HPP__
#define __WAITABLE_QUEUE_HPP__

#include <queue>              // queue
#include <mutex>              //mutex
#include <atomic>             //atomic<std::uint64_t>
#include <condition_variable> //condition_variable
#include <chrono>             //chrono::seconds /milli

namespace ilrd
{
    // T -copyable and assignable
    // Q - must support T &front() (and optionally const T &front()const), void
    //     pop(), void push(const T&)
    template <class T, class Q = std::queue<T>>
    class Waitable_Queue
    {
    public:
        explicit Waitable_Queue();
        Waitable_Queue(const Waitable_Queue &other) = delete;
        void operator=(const Waitable_Queue &other) = delete;
        bool IsEmpty() const noexcept;
        void Push(const T &data);
        void Pop(T &outparam);
        bool Pop(T &outparam, const std::chrono::seconds &timeout);
        bool Pop(T &outparam, const std::chrono::milliseconds &timeout);

    private:
        std::atomic<std::uint64_t> m_size;
        Q m_queue;
        std::mutex m_lock;
        std::condition_variable m_cond;
    };

    template <class T, class Q>
    Waitable_Queue<T, Q>::Waitable_Queue() : m_size(0)
    {
        //empty
    }

    template <class T, class Q>
    bool Waitable_Queue<T, Q>::IsEmpty() const noexcept
    {
        return (0 == m_size);
    }

    template <class T, class Q>
    void Waitable_Queue<T, Q>::Push(const T &data)
    {
        std::unique_lock<std::mutex> u_lock(m_lock);

        m_queue.push(data);
        ++m_size;
        m_cond.notify_one();
    }

    template <class T, class Q>
    void Waitable_Queue<T, Q>::Pop(T &outparam)
    {
        std::unique_lock<std::mutex> u_lock(m_lock);

        while (0 == m_size)
        {
            m_cond.wait(u_lock);
        }

        outparam = m_queue.front();
        m_queue.pop();
        --m_size;
    }

    template <class T, class Q>
    bool Waitable_Queue<T, Q>::Pop(T &outparam, const std::chrono::seconds &timeout)
    {
        return Pop(outparam, std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
    }

    template <class T, class Q>
    bool Waitable_Queue<T, Q>::Pop(T &outparam, const std::chrono::milliseconds &timeout)
    {
        using namespace std::chrono;
        
        std::unique_lock<std::mutex> u_lock(m_lock);
        
        while (0 == m_size)
        {
            if (std::cv_status::timeout == m_cond.wait_until(u_lock, system_clock::now() + timeout))
            {
                return false;
            }
        }

        outparam = m_queue.front();
        m_queue.pop();
        --m_size;

        return true;
    }

    
}

#endif //__WAITABLE_QUEUE_HPP__
