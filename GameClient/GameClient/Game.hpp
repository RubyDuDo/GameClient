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

#include "./proto/msg.pb.h"
#include "MsgQueue.hpp"

using namespace MyGame;

class ProtobufHelp
{
public:
    static MsgHead* CreatePacketHead( MsgType type );
    static MsgRspHead* CreateRspHead( MsgType type, MsgErrCode res );
};

class CGame
{
public:
    CGame() = default;
    
    void run();
    
    void receiveThread();
    void sendThread();
    
    int InitNetwork();
private:
    void onReceiveMsg( const string& msg );
    
private:
    //all the request
    void requestLogin( const std::string& strName, const std::string strPass);
    void requestAction( const std::string strAction );
    void requestLogout( );
    
    void addTcpQueue(  const Msg& msg );
    
private:
    void dealRecvMsg( const MsgRsp& msg );
    // all the response deal
    void onLogin( const MsgRsp& msg);
    void onAction( const MsgRsp& msg );
    void onLogout( const MsgRsp& msg );
    
private:
    MsgQueue<Msg> m_msgs;
    
    MsgQueue<MsgRsp> m_rmsgs;

    
    
    std::shared_ptr<TcpSocket> m_sock;
    
    int m_roleId = 0;
    
};

#endif /* Game_hpp */
