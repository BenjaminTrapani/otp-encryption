#include "Message.h"
using namespace OTPEncryptor;

Message::Message(char * pData, unsigned int len, FastByteArray * pKey) : FastByteArray(pData, len), _pKey(pKey)
{
    _isEncrypted = false;
    /*if (_pData[len-1] != '\0'){
        char * nullTermData = new char[len+1];
        memcpy(nullTermData, _pData, len);
        nullTermData[len] = '\0';
        delete [] _pData;
        _pData = nullTermData;
        ++_len;
    }*/
}

unsigned long Message::getHeadingLength(){
        return 65L+64L;
}

void Message::xorWithKey(){
    for(unsigned int i = 0; i < _len; i++)
        _pData[i]^= _pKey->charAt(i);
}
void Message::encrypt(){
    xorWithKey();
    _isEncrypted = true;
}
void Message::decrypt(){
    xorWithKey();
    _isEncrypted = false;
}

Message::~Message()
{
    delete _pKey;
}
