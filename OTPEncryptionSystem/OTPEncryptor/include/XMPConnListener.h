#ifndef XMPCONNLISTENER_H
#define XMPCONNLISTENER_H

#include <connectionlistener.h>

class XMPConnListener: public ConnectionListener
{
    public:
        XMPConnListener();
        virtual ~XMPConnListener();

        //overides
        void onConnect();
        void onDisconnect(ConnectionError e);
        bool onTLSConnect(const CertInfo& info);

    protected:
    private:
};

#endif // XMPCONNLISTENER_H
