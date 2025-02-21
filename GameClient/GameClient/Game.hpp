//
//  Game.hpp
//  GameClient
//
//  Created by pinky on 2025-02-19.
//

#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include <mutex>
#include <queue>
#include <string>
#include "TcpSocket.hpp"

class Msg
{
public:
    Msg( const std::string& str ):m_strAction( str ){};
    std::string m_strAction;
    
};

class CGame
{
public:
    CGame() = default;
    
    void run();
    
    void dispatchMsg(const Msg& msg);
    
    void receiveThread();
    void sendThread();
    
    int InitNetwork();
    
private:
    std::mutex m_mutex;
    std::queue<Msg> m_msgs;
    
    std::mutex m_rmutex;
    std::queue<Msg> m_rmsgs;
    
    std::shared_ptr<TcpSocket> m_sock;
    
};

#endif /* Game_hpp */
