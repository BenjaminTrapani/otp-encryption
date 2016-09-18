#include "XMPClient.h"
#include "boost/format.hpp"
#include "disco.h"
#include "presence.h"
#include <string.h>
#include "xmlrpc-c/base64.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace OTPEncryptor;

XMPClient::XMPClient(const string & clientDir, vector<ValidationPoint> * pRValPoints,
                                               vector<ValidationPoint> * pWValPoints) : _isPeerOnline(false), _pThr(nullptr),_shouldTerminateHeartbeat(false), _pSession(nullptr)
{
    _pWriteKeyManager = new KeyManager(clientDir + "/writekey.txt", true);
    _pReadKeyManager = new KeyManager(clientDir + "/readkey.txt");

    if(pWValPoints && pRValPoints){
        KeyValidator * pWKeyValidator = new KeyValidator(pWValPoints);
        KeyValidator * pRKeyValidator = new KeyValidator(pRValPoints);

        if (! pWKeyValidator->validateKey(_pWriteKeyManager)){
            cout << "Invalid write key. Exiting" << endl;
            delete _pWriteKeyManager;
            delete _pReadKeyManager;
            delete pWKeyValidator;
            delete pRKeyValidator;

            std::exit(-1);
        }
        delete pWKeyValidator;

        if (! pRKeyValidator->validateKey(_pReadKeyManager)){
            cout << "Invalid read key. Exiting" << endl;
            delete _pWriteKeyManager;
            delete _pReadKeyManager;
            delete pRKeyValidator;

            std::exit(-1);
        }
        delete pRKeyValidator;
    }

    _pIDPair = new ClientIDPair(clientDir);

    ifstream inputFile(clientDir + "/miscdata.txt");
    char password[64];
    inputFile.read(password, 64);
    string stringPass(password);
    stringPass.resize(64);

    cout << "Password = " << stringPass << endl;
    cout << "local id = " << _pIDPair->_localID << endl;

    JID curJID(_pIDPair->_localID + "@otpencryptedxmpserver");
    _pPeerID = new JID(_pIDPair->_peerID + "@otpencryptedxmpserver");
    _pClient = new Client(curJID, stringPass, 5222);
    _pClient->setServer("104.236.219.59");
    _pClient->disco()->setVersion( "OTPEncryptedChat", GLOOX_VERSION, "Linux" );
    _pClient->disco()->addFeature( XMLNS_CHAT_STATES );
    _pClient->registerConnectionListener(this);
    _pClient->registerMessageSessionHandler(this, 0);
    //call super constructor
    _pRosterManager = new OTPRosterManager(_pClient,_pPeerID);
    _pRosterManager->setPresenceHandler(this);

    //_pClient->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);

}

void XMPClient::heartbeat(){
    while(!_shouldTerminateHeartbeat){
        if(_pSession)
            _pSession->send("o");
        boost::this_thread::sleep(boost::posix_time::milliseconds(5000)); //heartbeat once per second.
    }
}

void XMPClient::connect(){
    _pClient->connect(true);
}

void XMPClient::disconnect(){
    if(_pSession)
        _pSession->send("f");
    else
        return;

    _shouldTerminateHeartbeat = true;
    _pThr->join();
    delete _pThr;
    _pThr = nullptr;

    _pClient->disposeMessageSession(_pSession);
    _pSession = nullptr;
    _pClient->disconnect();
}

bool XMPClient::isPeerOnline(){
    return _isPeerOnline;
}

ifstream::pos_type XMPClient::getDataLeft()const{
    return _pWriteKeyManager->getBytesLeft();
}

void XMPClient::onConnect()
{
    cout << "Connected to server." << endl;
    createSession();

    if (!_pThr)
        _pThr = new boost::thread(boost::bind(&XMPClient::heartbeat, this));
}

void XMPClient::onDisconnect(ConnectionError e)
{
    cout << "Disconnected from server." << endl;
    if( e == ConnAuthenticationFailed )
        cout << "auth failed. reason:" << _pClient->authError() << endl;
    else if (e == ConnStreamError)
        cout << "Stream error. Reason: " << _pClient->streamError() << endl;
    else if (e == ConnParseError)
        cout << "Parse error occured." << endl;
    else if (e == ConnUserDisconnected || e == ConnNotConnected)
        cout << "Disconnected cleanly.";
}

bool XMPClient::onTLSConnect(const CertInfo& info)
{
      time_t from( info.date_from );
      time_t to( info.date_to );

      cout << (boost::format( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n"
              "from: %s\nto: %s\n") %
              info.status% info.issuer.c_str()% info.server.c_str() %
              info.protocol.c_str()% info.mac.c_str()% info.cipher.c_str()%
              info.compression.c_str()% ctime( &from )% ctime( &to ));

      return true;
}

void XMPClient::handleMessageSession(MessageSession * pSession){
    cout << "Recieved new session." << endl;
    if (_pSession != nullptr){
        _pClient->disposeMessageSession(_pSession);
        _pSession = nullptr;
    }
    _pSession = pSession;
    _pSession->registerMessageHandler(this);
    _pMessageFilter = new MessageEventFilter(_pSession);
    _pMessageFilter->registerMessageEventHandler(this);
    _pChatFilter = new ChatStateFilter(_pSession);
    _pChatFilter->registerChatStateHandler(this);
}

void XMPClient::createSession(){
    if (_pSession == nullptr){
        _pSession = new MessageSession(_pClient, *_pPeerID);
        _pSession->registerMessageHandler(this);
        _pMessageFilter = new MessageEventFilter(_pSession);
        _pMessageFilter->registerMessageEventHandler(this);
        _pChatFilter = new ChatStateFilter(_pSession);
        _pChatFilter->registerChatStateHandler(this);
    }
}

void XMPClient::sendMessage(const string & message){
    if (!_pSession){
        cout << "Must connect to the server before sending message." << endl;
        return;
    }
    if (message.length() == 0)
        return;

    char * dataCopy = new char [message.length()];
    for (unsigned int i = 0; i < message.length(); i++){
        dataCopy[i] = message.at(i);
    }

    FastByteArray * pKey = _pWriteKeyManager->getAndDestroyKeyForLength(message.length());
    if (!pKey){
        cout << "Out of data." << endl;
        return;
    }

    OTPEncryptor::Message * pToEncrypt = new Message(dataCopy, message.length(),pKey);
    pToEncrypt->encrypt();

    std::vector<unsigned char> const vecData(&dataCopy[0], &dataCopy[message.length()]);

    stringstream messageBuilder;
    messageBuilder << _pWriteKeyManager->getReadLocation()-message.length() << ";";
    string finalString(xmlrpc_c::base64FromBytes(vecData));
    messageBuilder << finalString;
    //how to decode the xml into a character array.
   // vector<unsigned char> const param1Value(xmlFriendly.vectorUcharValue());
    cout << "Will send: " << messageBuilder.str() << endl;
    //createSession();
    //gloox::Message pMessage(gloox::Message::Chat, *_pPeerID, message,EmptyString,EmptyString,"en"); //chat type message = 1
    _pSession->send(messageBuilder.str());
    delete pToEncrypt;
}

void XMPClient::markRead(){
    _pMessageFilter->raiseMessageEvent(MessageEventDisplayed);
}

void XMPClient::setIsComposing(bool isComposing){
    if (isComposing){
        _pMessageFilter->raiseMessageEvent(MessageEventComposing);
        _pChatFilter->setChatState(ChatStateComposing);
    }else {
        _pChatFilter->setChatState(ChatStatePaused);
    }
}

void XMPClient::handleMessage( const gloox::Message& msg, MessageSession * pSession){
    if (msg.from() == _pClient->jid() || msg.body().length() == 0)
        return;

    const char * charArray = msg.body().c_str();

    if(charArray[0] == 'o'){
        _isPeerOnline = true;
        return;
    }else if (charArray[0] == 'f'){
        _isPeerOnline = false;
        return;
    }

    cout << (boost::format("type: %d, subject: %s, message: %s, thread id: %s\n") % msg.subtype()%
              msg.subject().c_str()% msg.body().c_str()% msg.thread().c_str()) << endl;


    stringstream posInfo;
    unsigned int iterIndex = 0;
    while(charArray[iterIndex]!= ';'){
        if(iterIndex>msg.body().length())
            return;

        posInfo << charArray[iterIndex];
        iterIndex ++;
    }
    unsigned long readLoc = atol(posInfo.str().c_str());
    string messageContent = msg.body().substr(iterIndex+1,msg.body().length());
    vector<unsigned char> regularBase(xmlrpc_c::bytesFromBase64(messageContent));

    FastByteArray * pDecodeKey = _pReadKeyManager->getKey(readLoc, regularBase.size());
    OTPEncryptor::Message * pMessage = new Message(reinterpret_cast<char*>(regularBase.data()),regularBase.size(),pDecodeKey);
    pMessage->encrypt();

    string decryptedMessage(pMessage->getData());
    decryptedMessage.resize(regularBase.size());
    cout << "Decoded message: " << decryptedMessage << endl;
}

void XMPClient::handleLog( LogLevel level, LogArea area, const std::string& message ){
    cout << (boost::format("log: level: %d, area: %d, %s\n") % level % area % message.c_str())
     << endl;
}

void XMPClient::handleChatState(const JID& from, ChatStateType state){
    cout << (boost::format("received state: %d from: %s\n")% state % from.full().c_str()) << endl;
}

void XMPClient::handleMessageEvent(const JID& from, MessageEventType event){
    cout << (boost::format("received event: %d from: %s\n")% event % from.full().c_str()) << endl;
}

void XMPClient::handlePresence(const Presence & presence){
    cout << "Presence update: " << presence.presence() <<endl;
    cout << "Presence update from " << presence.from().full() << endl;
}

XMPClient::~XMPClient()
{
    if (_pThr){
        _pThr->join();
        delete _pThr;
    }
    _pClient->disposeMessageSession(_pSession);
    delete _pRosterManager;
    delete _pIDPair;
    delete _pPeerID;
    delete _pClient;
    delete _pReadKeyManager;
    delete _pWriteKeyManager;
}
