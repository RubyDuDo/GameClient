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

bool Buffer::addData( char* pData, int len  )
{
    if( m_tail + len <= m_maxSize )
    {
        memcpy( m_pBuff + m_tail, pData, len );
    }
    else{
        memcpy( m_pBuff, m_pBuff + m_head, m_tail - m_head );
        m_tail = m_tail - m_head;
        m_head = 0;
        
        if( m_tail + len <= m_maxSize )
        {
            memcpy( m_pBuff + m_tail, pData, len );
            m_tail += len;
        }
        else{
            return false;
        }
    }
    return true;
}

int Buffer::getData( char* pBuff, int maxLen)
{
    int availableLen = m_tail - m_head;

    int len = std::min( maxLen, availableLen );
    memcpy( pBuff, m_pBuff + m_head, len);
    return len ;
    
}

void Buffer::consumeData( int len )
{
    m_head += len;
}
