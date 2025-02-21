//
//  Buffer.hpp
//  GameClient
//
//  Created by pinky on 2025-02-19.
//

#ifndef Buffer_hpp
#define Buffer_hpp

#include <stdio.h>

class Buffer
{
public:
    Buffer( int size = 2048);
    
    bool addData( char* pData, int len  );
    int getData( char* pBuff, int maxLen);
    void consumeData( int len );
    
    int getSize()const;
    int getAvailableSize() const;
private:
    void moveToStart();
private:
    int m_maxSize;
    char* m_pBuff;
    
    int m_head;
    int m_tail;
    
};

#endif /* Buffer_hpp */

