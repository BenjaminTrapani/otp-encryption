#ifndef XMPCLIENT_H
#define XMPCLIENT_H

#include "messagesessionhandler.h"
#include "messageeventhandler.h"
#include "messageeventfilter.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "message.h"
#include "client.h"

#include "chatstatehandler.h"
#include "chatstatefilter.h"
#include "connectionlistener.h"
#include "loghandler.h"
#include "rostermanager.h"
#include "gloox.h"
#include "boost/thread.hpp"
#include "KeyManager.h"
#include "OTPRosterManager.h"
//otp encryptor message header, not gloox
#include "Message.h"
#include "KeyValidator.h"

using namespace std;
using namespace gloox;

namespace OTPEncryptor{

typedef struct ClientIDPair{
    string _localID;
    string _peerID;

    //Construct from client dir
    ClientIDPair(const string & rootDir){
        _localID = read64File(rootDir + "/uid.txt");
        _peerID = read64File(rootDir + "/peerid.txt");
    };

    private:
        string read64File(const string & file)const{
            ifstream inputFile(file);
            if(!inputFile){
                cout << "Error opening file with path " << file << endl;
                return string("");
            }
            char contents[64];
            inputFile.read(contents, 64);
            string contentString(contents);
            contentString.resize(64);
            return contentString;
        }
}ClientIDPair;

class XMPClient: public MessageSessionHandler, ConnectionListener, LogHandler,
                    MessageEventHandler, MessageHandler, ChatStateHandler, PresenceHandler
{
    public:
        XMPClient(const string & clientDir, vector<ValidationPoint> * pRValPoints,
                                            vector<ValidationPoint> * pWValPoints);
        virtual ~XMPClient();

        //marks all messages up until the time of call read.
        void connect();
        void disconnect();
        bool isPeerOnline();
        ifstream::pos_type getDataLeft()const;
        void markRead();
        void setIsComposing(bool isComposing);
        void sendMessage(const string & message);
        //gloox overides
        virtual void onConnect();
        virtual void onDisconnect(ConnectionError e);
        virtual bool onTLSConnect(const CertInfo& info);

        virtual void handleMessageSession(MessageSession * pSession);
        virtual void handleLog(LogLevel level, LogArea area, const std::string& message);
        virtual void handleChatState(const JID& from, ChatStateType state);
        virtual void handleMessageEvent(const JID& from, MessageEventType event);
        virtual void handleMessage( const gloox::Message& msg, MessageSession * pSession);
        virtual void handlePresence(const Presence& presence);

    protected:
    private:
        KeyManager * _pWriteKeyManager;
        KeyManager * _pReadKeyManager;
        ClientIDPair * _pIDPair;
        OTPRosterManager * _pRosterManager;
        bool _isPeerOnline;
        boost::thread* _pThr;
        bool _shouldTerminateHeartbeat;
        //gloox fields
        Client * _pClient;
        JID * _pPeerID;
        MessageSession * _pSession;
        MessageEventFilter * _pMessageFilter;
        ChatStateFilter * _pChatFilter;

        void createSession();
        void heartbeat();
};


} //namespace OTPEncryptor

#endif // XMPCLIENT_H
