//
//  TcpSocket.hpp
//  GameServer
//
//  Created by pinky on 2025-02-19.
//

#ifndef TcpSocket_hpp
#define TcpSocket_hpp

#include <stdio.h>
#include <memory>

constexpr int BACKLOG_D = 500;

class TcpSocket
{
private:
    int m_sock;
public:
    TcpSocket( int sock );
    ~TcpSocket();
    
    int Bind( short port );
    int Listen( int backlog = BACKLOG_D );
    std::shared_ptr<TcpSocket> Accept();
    
    int Connect( const std::string& ip, short port );
    
    int SendData( const char* buff, int len );
    
    int RecvData( char* buff, int maxLen );
    
};

class NetUtil
{
public:
    static std::shared_ptr<TcpSocket> createTcpSocket();
    
};

#endif /* TcpSocket_hpp */
