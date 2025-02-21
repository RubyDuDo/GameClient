//
//  Buffer.cpp
//  GameClient
//
//  Created by pinky on 2025-02-19.
//

#include "Buffer.hpp"
#include <cstring>
#include <algorithm>

Buffer::Buffer( int size ): m_maxSize( size)
{
    m_pBuff = new char[m_maxSize];
    m_head = 0;
    m_tail = 0;
}

int Buffer::getSize()const
{
    return m_tail - m_head;
}

int Buffer::getAvailableSize() const
{
    return m_maxSize - getSize();
}

void Buffer::moveToStart()
{
    memmove( m_pBuff, m_pBuff + m_head, getSize() );

    m_tail = m_tail - m_head;
    m_head = 0;
}
bool Buffer::addData( char* pData, int len  )
{
    if( len > getAvailableSize() )
    {
        return false;
    }
    
    if( m_tail + len > m_maxSize )
    {
        moveToStart();
    }
    memcpy( m_pBuff + m_tail, pData, len );
    m_tail += len;
    
    return true;
}

int Buffer::getData( char* pBuff, int maxLen)
{
    int size = getSize();

    int len = std::min( maxLen, size );
    memcpy( pBuff, m_pBuff + m_head, len);
    return len ;
    
}

void Buffer::consumeData( int len )
{
    m_head += len;
}
