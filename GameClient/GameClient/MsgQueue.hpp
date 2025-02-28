//
//  MsgQueue.hpp
//  GameClient
//
//  Created by pinky on 2025-02-27.
//

#ifndef MsgQueue_hpp
#define MsgQueue_hpp

#include <stdio.h>
#include <queue>
#include <condition_variable>
#include <memory>
using namespace std;

template <typename T>
class MsgQueue
{
public:
    void push( const T& value );
    
    shared_ptr<T> try_pop();
    shared_ptr<T> wait_and_pop();
    
    bool empty();
    int size();
    
private:
    std::queue<T> m_queue;
    
    mutex m_mut;
    condition_variable m_cond;
    
    
    
    
    
};

template <typename T>
void MsgQueue<T>::push( const T& value )
{
    {
        lock_guard lk(m_mut);
        m_queue.push( value );
    }

    m_cond.notify_one();
}

template <typename T>
shared_ptr<T> MsgQueue<T>::try_pop()
{
    lock_guard lk( m_mut );
    if( !m_queue.empty())
    {
        auto data = m_queue.front();
        m_queue.pop();
        return make_shared<T>( data );
    }
    else{
        return nullptr;
    }
    
}

template <typename T>
shared_ptr<T> MsgQueue<T>::wait_and_pop()
{
    unique_lock lk(m_mut);
    m_cond.wait( lk, [this]{ return !m_queue.empty();});
    auto res( make_shared<T>( m_queue.front() ));
    m_queue.pop();
    return res;
}

template <typename T>
bool MsgQueue<T>::empty()
{
    lock_guard lk(m_mut);
    bool isEmpty = m_queue.empty();
    
    return isEmpty;
}

template <typename T>
int MsgQueue<T>::size()
{
    lock_guard lk(m_mut);
    int size = m_queue.size();
    
    return size;
    
}


#endif /* MsgQueue_hpp */
