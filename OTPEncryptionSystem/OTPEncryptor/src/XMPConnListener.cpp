#include "XMPConnListener.h"

XMPConnListener::XMPConnListener()
{
    //ctor
}

XMPConnListener::~XMPConnListener()
{
    //dtor
}

void XMPConnListener::onConnect(){
}
void XMPConnListener::onDisconnect(ConnectionError e){
}

bool XMPConnListener::onTLSConnect(const CertInfo& info){

}
