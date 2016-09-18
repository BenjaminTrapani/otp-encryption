#include "OTPRosterManager.h"
#include "stdio.h"
#include <iostream>
using namespace OTPEncryptor;
using namespace std;
OTPRosterManager::OTPRosterManager(ClientBase * pClient, const JID * pSubscribeID): RosterManager(pClient)
{
    subscribe(*pSubscribeID);
    registerRosterListener(this);
    synchronize();
    Roster * pCurRoster = roster();
    for(Roster::iterator iter = pCurRoster->begin(); iter != pCurRoster->end(); iter++){
        cout << iter->first << ": " << iter->second << endl;
    }
}

OTPRosterManager::~OTPRosterManager()
{
    //dtor
}

void OTPRosterManager::handlePresence(const Presence & presence){
    _pPresenceHandler->handlePresence(presence);
}

bool OTPRosterManager::handleSubscriptionRequest(const JID & jid, const std::string & msg){
    std::cout << "Authorized subscription request." << std::endl;
    return true;
}

void OTPRosterManager::handleSubscription (const Subscription &subscription){
    std::cout << "Handled subscription from: " << subscription.from().full() << std::endl;
}

void OTPRosterManager::handleItemAdded (const JID &jid){}
void OTPRosterManager::handleItemSubscribed (const JID &jid){}
void OTPRosterManager::handleItemRemoved (const JID &jid){}
void OTPRosterManager::handleItemUpdated (const JID &jid){}
void OTPRosterManager::handleItemUnsubscribed (const JID &jid){}
void OTPRosterManager::handleRoster (const Roster &roster){}
void OTPRosterManager::handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg){}
void OTPRosterManager::handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg){}
bool OTPRosterManager::handleUnsubscriptionRequest (const JID &jid, const std::string &msg){
    std::cout << "Authorized unsubscribe request." << std::endl;
    return true;
}
void OTPRosterManager::handleNonrosterPresence (const Presence &presence){}
void OTPRosterManager::handleRosterError (const IQ &iq){}
