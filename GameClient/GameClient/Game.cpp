//
//  Game.cpp
//  GameClient
//
//  Created by pinky on 2025-02-19.
//

#include "Game.hpp"
#include <iostream>
#include <thread>
#include "Buffer.hpp"

using namespace std;

constexpr short SVR_PORT = 8081;
constexpr std::string SVR_IP = "127.0.0.1";

MyGame::MsgHead* ProtobufHelp::CreatePacketHead( MsgType type )
{
    MsgHead* pHead = new MsgHead();
    pHead->set_type( type );
    
    return pHead;
    
    
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
    
    string strName, strPass;
    cout<<"Input your name and password:"<<endl;
    cin>>strName>>strPass;
    requestLogin( strName, strPass );
    
    while( true )
    {
        string str;
        cout<<"Input your next behavior:"<<endl;
        cin>>str;
        
        if( str == "end" )
        {
            requestLogout();
        }
        else if( str == "" )
        {
            continue;
        }
        else
        {
            requestAction( str );
        }
        
        //
        auto pMsg = m_rmsgs.wait_and_pop();
        if( pMsg )
        {
            dealRecvMsg( *pMsg );
        }
    }
    
    cout<<"Game ends!"<<endl;
}

#define receiveBuffSize 1024


void CGame::receiveThread()
{
    Buffer recvBuff;
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
            recvBuff.addData( buff, res );
            //TCP拆包
            if( recvBuff.getSize() > sizeof( short ) )
            {
//                cout<<"getSize:"<<recvBuff.getSize()<<endl;
                short len = 0;
                recvBuff.getData( (char*)&len, sizeof(short));
                len = ntohs( len );
                if( recvBuff.getSize() >= len + sizeof(short) )
                {
                    char msgbuff[receiveBuffSize]{};
                    recvBuff.consumeData( sizeof( short ) );
                    recvBuff.getData( msgbuff,  receiveBuffSize );
                    recvBuff.consumeData( len );
                    
                    std::string msg( msgbuff, len  );
                    
                    
                    onReceiveMsg( msg );
                }
            }
            

        }
    }
}

void CGame::sendThread()
{
    while( true )
    {
        auto pMsg = m_msgs.wait_and_pop();
        
        std::string strData = pMsg->SerializeAsString();
        
        short len = strData.length();
        cout<<"SendLen:"<<len<<endl;
        len = htons( len );
//        cout<<"send Msg:"<< msg.m_strAction<<std::endl;
        m_sock->SendData((char*)&len, sizeof(len));
        m_sock->SendData( (char*)strData.c_str(), strData.length() );
    }
}

void CGame::addTcpQueue( const Msg& msg )
{
    m_msgs.push( msg );
}

void CGame::requestLogin( const std::string& strName, const std::string strPass)
{
    RequestLogin req;
    req.set_strname( strName );
    req.set_strpass(  strPass );
    
    Msg outMsg;
    outMsg.set_allocated_head( ProtobufHelp::CreatePacketHead( MsgType_Login ) );
    outMsg.mutable_payload()->PackFrom( req );
    
    addTcpQueue( outMsg );
}

void CGame::requestAction( const std::string strAction )
{
    RequestAct req;
    req.set_action( strAction );

    cout<<"requestAct:"<<req.DebugString()<<endl;
    Msg outMsg;
    outMsg.set_allocated_head( ProtobufHelp::CreatePacketHead( MsgType_Act ) );
    outMsg.mutable_payload()->PackFrom( req );
    
    addTcpQueue( outMsg );
    
}
void CGame::requestLogout( )
{
    RequestLogout req;
    req.set_roleid( m_roleId );
    
    Msg outMsg;
    outMsg.set_allocated_head( ProtobufHelp::CreatePacketHead( MsgType_Logout ) );
    outMsg.mutable_payload()->PackFrom( req );
    
    addTcpQueue( outMsg );
    
}

void CGame::onReceiveMsg( const std::string& msg )
{
    Msg recvMsg;
    if( !recvMsg.ParseFromArray( msg.c_str(),  msg.length() ) )
    {
        cout<<"onReceiveMsg Error, protobuf parse head fail"<<endl;
        return;
    }
    
    m_rmsgs.push( recvMsg );
}

void CGame::dealRecvMsg( const Msg& msg )
{
    //dispatch the msg according the type
    cout<<"recvMsg:" << msg.head().type()<<endl;
    switch( msg.head().type() )
    {
    case MsgType_Login:
            onLogin( msg );
            break;
    case MsgType_Act:
            onAction( msg);
            break;
    case MsgType_Logout:
            onLogout( msg);
            break;
    default:
            break;
    }
    
}
void CGame::onLogin( const Msg& msg)
{
    ResponseLogin login;
    if( !msg.payload().UnpackTo( &login ))
    {
        cout<<"Login msg parse fail"<<endl;
        return;
    }
    
    cout<<"Role ID:"<< login.roleid()<<"_ level: "<<login.rolelevel()<<endl;
}
void CGame::onAction( const Msg& msg )
{
    ResponseAct act;
    if( !msg.payload().UnpackTo( &act ))
    {
        cout<<"act msg parse fail"<<endl;
        return;
    }
    
    cout<<"Response Action:"<< act.action()<<endl;
    
}
void CGame::onLogout( const Msg& msg )
{
    ResponseLogout logout;
    if( !msg.payload().UnpackTo( &logout ))
    {
        cout<<"Login msg parse fail"<<endl;
        return;
    }
    
    cout<<"Role ID:"<< logout.roleid()<<endl;
    
}
