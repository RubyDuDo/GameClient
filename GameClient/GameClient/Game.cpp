//
//  Game.cpp
//  GameClient
//
//  Created by pinky on 2025-02-19.
//

#include "Game.hpp"
#include <iostream>
#include <thread>

using namespace std;

constexpr short SVR_PORT = 8081;
constexpr std::string SVR_IP = "127.0.0.1";


void CGame::dispatchMsg( const Msg& msg)
{
    lock_guard lk( m_mutex );
    m_msgs.push( msg );
}

int CGame::InitNetwork()
{
    m_sock = NetUtil::createTcpSocket();
    if( m_sock )
    {
        auto res = m_sock->Connect( SVR_IP, SVR_PORT);
        if( res == -1 )
        {
            cout<<"Log: connect Fail"<<endl;
            return -1;
        }
    }
    
//    NetUtil::setnoblock( true );
    return 0;
}

void CGame::run()
{
    cout<<"Game Started"<<endl;
    
    if( InitNetwork() == -1 )
    {
        return ;
    }
    
    auto sendThread = std::thread( &CGame::sendThread, this );
    sendThread.detach();
    
    auto receiveThread = std::thread( &CGame::receiveThread, this );
    receiveThread.detach();
    
    
    while( true )
    {
        string str;
        cout<<"Input your next behavior:"<<endl;
        cin>>str;
        
        if( str == "end" )
        {
            break;
        }
        dispatchMsg( Msg(str) );
        
        while( true )
        {
            std::this_thread::sleep_for( 10ms );
            lock_guard lk( m_rmutex);
            if( !m_rmsgs.empty())
            {
                Msg msg = m_rmsgs.front();
                cout<<"Receive :"<< msg.m_strAction<<endl;
                m_rmsgs.pop();
                break;
            }
        }
        
        
    }
    
    cout<<"Game ends!"<<endl;
}

#define receiveBuffSize 1024
void CGame::receiveThread()
{
    while( true )
    {
        char buff[receiveBuffSize] {};
        int res = m_sock->RecvData( buff, receiveBuffSize );
        if( res < 0 )
        {
            cout<<"receive error"<<endl;
            break;
        }
        else{
            std::string str( buff );
            Msg msg( str );
            lock_guard lk( m_rmutex);
            m_rmsgs.push( msg );
        }
    }
}

void CGame::sendThread()
{
    while( true )
    {
        std::this_thread::sleep_for( 10ms );
        lock_guard lk( m_mutex );
        if( !m_msgs.empty())
        {
            auto msg = m_msgs.front();
            m_msgs.pop();
            
            m_sock->SendData( (char*)msg.m_strAction.c_str(), msg.m_strAction.length() + 1 );
        }
    }
    
}
