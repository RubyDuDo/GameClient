//
//  TcpSocket.cpp
//  GameServer
//
//  Created by pinky on 2025-02-19.
//

#include "TcpSocket.hpp"

#include <unistd.h>     // close()
#include <sys/socket.h> // socket(), bind(), listen(), accept()
#include <arpa/inet.h> // sockaddr_in, inet_addr()

#include <iostream>
#include <string>


std::shared_ptr<TcpSocket> NetUtil::createTcpSocket()
{
    int sock = socket( AF_INET, SOCK_STREAM, 0);
    if( sock < 0 )
    {
        perror("socket() error");
        return nullptr;
    }
    
    return std::make_shared<TcpSocket>(sock);
}


TcpSocket::TcpSocket( int sock ): m_sock( sock )
{
    
}

TcpSocket::~TcpSocket()
{
    close( m_sock );
}

int TcpSocket::Bind( short port )
{
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons( port );
    
    int ret = bind( m_sock, (struct sockaddr*)&addr, sizeof(addr) );
    if( ret < 0 )
    {
        perror("Bind Error");
        return ret;
    }
    return 0;
}

int TcpSocket::Listen( int backlog )
{
    int ret = listen( m_sock, backlog );
    if( ret < 0 )
    {
        perror("Listen Error!");
        return ret;
    }
    return 0;
}

std::shared_ptr<TcpSocket> TcpSocket::Accept()
{
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int sock = accept( m_sock, (struct sockaddr*) &client_addr,  &client_len);
    
    if( sock > 0 )
    {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
        
        std::cout<<"Connect from:"<<ip_str<<std::endl;
        return std::make_shared<TcpSocket>(sock);
    }
    else{
        return nullptr;
    }
}

int TcpSocket::Connect( const std::string& ip, short port )
{
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    inet_pton( AF_INET, ip.c_str(), &addr.sin_addr );
    addr.sin_port = htons( port );
    int ret = connect( m_sock, (struct sockaddr*)&addr, sizeof(addr) );
    if( ret < 0 )
    {
        perror( "Connect Error");
        return ret;
    }
    return 0;
}

int TcpSocket::SendData( const char* buff, int len )
{
    int ret = send( m_sock, buff, len, 0);
    if( ret == -1 )
    {
        perror( "SendData Error");
    }
    
    return ret;
}

int TcpSocket::RecvData( char* buff, int maxLen )
{
    int ret = recv( m_sock, buff, maxLen, 0);
    if( ret == -1 )
    {
        perror("RecvData Error");
    }
    return ret;
}
