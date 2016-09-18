#ifndef OTPROSTERMANAGER_H
#define OTPROSTERMANAGER_H

#include "gloox.h"
#include "rostermanager.h"
#include "rosterlistener.h"
#include "client.h"
#include "presencehandler.h"
#include "presence.h"

using namespace gloox;
namespace OTPEncryptor{
class OTPRosterManager: public RosterManager, RosterListener
{
    public:
        OTPRosterManager(ClientBase * pClient, const JID * pSubscribeID);
        ~OTPRosterManager();

        inline void setPresenceHandler(PresenceHandler * pHandler) { _pPresenceHandler = pHandler;}
        virtual void handlePresence(const Presence & presence);
        virtual void handleSubscription (const Subscription &subscription);
        virtual bool handleSubscriptionRequest(const JID & jid, const std::string & msg);
        virtual void handleItemAdded (const JID &jid);
        virtual void handleItemSubscribed (const JID &jid);
        virtual void handleItemRemoved (const JID &jid);
        virtual void handleItemUpdated (const JID &jid);
        virtual void handleItemUnsubscribed (const JID &jid);
        virtual void handleRoster (const Roster &roster);
        virtual void handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg);
        virtual void handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg);
        virtual bool handleUnsubscriptionRequest (const JID &jid, const std::string &msg);
        virtual void handleNonrosterPresence (const Presence &presence);
        virtual void handleRosterError (const IQ &iq);

    protected:
    private:
        PresenceHandler * _pPresenceHandler;
};
}
#endif // OTPROSTERMANAGER_H
