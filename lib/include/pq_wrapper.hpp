#include <queue>

namespace ilrd
{
template<class T,
class Container = std::vector<T>,
class Compare = std::less<typename Container::value_type>>
class PQWrapper
{
    public:
    void push(const T &data);
    void pop();
    const T &front() const;

    private:
    std::priority_queue<T, Container, Compare>m_pq;
};

template <class T, class Container, class Compare>
void PQWrapper<T, Container, Compare>::push(const T &data)
{
    m_pq.push(data);
}

template <class T, class Container, class Compare>
const T &PQWrapper<T, Container, Compare>::front() const
{
    return m_pq.top();
}

template <class T, class Container, class Compare>
void PQWrapper<T, Container, Compare>::pop()
{
    m_pq.pop();
}

}