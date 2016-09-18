#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Message.h>
#include <KeyManager.h>
#include <chrono>
#include <XMPClient.h>

using namespace std;
using namespace OTPEncryptor;

XMPClient * pClient;

void clientDriverFuncn(){
    bool shouldQuit = false;
    cout << "Type HELP for a list of commands." << endl;
    while(!shouldQuit){
        string input;
        cin >> input;
        if (input == "SEND"){
            input.clear();
            string toRead;
            std::cin.ignore();
            cout << "Input text:" << endl;
            std::getline( std::cin, toRead);
            pClient->sendMessage(toRead);
        }else if (input == "HELP"){
            cout << "Available commands:" << endl;
            cout << "HELP: display list of commands and functionality." << endl;
            cout << "SEND: initiate send operation to client" << endl;
            cout << "ONLINE: prints 'y' if peer is online and 'n' otherwise." << endl;
            cout << "QUIT: disconnects the current session and exits the application." << endl;
            cout << "DATA_LEFT: displays the amount of write data remaining in bytes." << endl;
        }else if (input == "ONLINE"){
            cout << "Checking if peer is online:" << endl;
            if (pClient->isPeerOnline())
                cout << "y" << endl;
            else
                cout << "n" << endl;
        }else if (input == "DATA_LEFT"){
            cout << "Data remaining = " << pClient->getDataLeft() << endl;
        }else if (input == "QUIT"){
            cout << "Quiting, please wait." << endl;
            pClient->disconnect();
            break;
        }else {
            cout << "Unrecognized command " << input << endl;
        }
    }
}

int main(){
    cout << "Client directory:";
    string clientDir;
    cin >> clientDir;
    std::cin.ignore();

    #ifndef RELEASE
    if (clientDir == "DEFAULT1"){
        cout << "Default path used." << endl;
        clientDir.assign("/home/benjamintrapani/Dropbox/Work/Programming/Linux/OTPEncryptionSystem/OTPEncryptorDemo/Peer1Dir");
    }else if (clientDir == "DEFAULT2"){
        cout << "Default path used." << endl;
        clientDir.assign("/home/benjamintrapani/Dropbox/Work/Programming/Linux/OTPEncryptionSystem/OTPEncryptorDemo/Peer2Dir");
    }
    #endif

    std::vector<ValidationPoint> * pRVals = new std::vector<ValidationPoint>();
    pRVals->reserve(4);
    pRVals->push_back(ValidationPoint(0, '2'));
    pRVals->push_back(ValidationPoint(1, 'i'));

    std::vector<ValidationPoint> * pWVals = new std::vector<ValidationPoint>();

    pClient = new XMPClient(clientDir, pRVals, pWVals);

    boost::thread* thr = new boost::thread(clientDriverFuncn);
    pClient->connect();

    thr->join();
    delete thr;
    delete pClient;
    return 0;
}

