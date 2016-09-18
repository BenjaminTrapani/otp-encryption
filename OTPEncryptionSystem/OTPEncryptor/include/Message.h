#ifndef MESSAGE_H
#define MESSAGE_H

#include "FastByteArray.h"
#include <sstream>

using namespace std;

namespace OTPEncryptor{

typedef enum PacketCode : char{
    ESTABLISH_CONNETION = 'a',
    MARK_ONLINE = 'b',
    MARK_OFFLINE = 'c',
    IP_ADDR = 'd',
    MESSAGE = 'e',
    PEER_OFFLINE = 'f'
} PacketCode;

typedef struct MessageHeading{
    //fields
    unsigned long _length;
    unsigned long _beginReadLocation;
    char _packetCode;
    //64 byte id of either the destination peer or
    //the local peer, depending on context
    char _uniqueId[64];

    //Expects packet code (1 char), length, begin read index and uniqueID
    //Longs are 32 characters padded with l if no number.
    MessageHeading(string data){
        _packetCode = data.at(0);

        unsigned int index = 1;
        while(data.at(index) == 'l')
            index++;

        _length = atol(data.substr(index, 33).c_str());
        index = 33;

        while(data.at(index)=='l')
            index++;

        _beginReadLocation = atol(data.substr(index, 65).c_str());

        for(unsigned int i = 65; i < 65 + 64; i++){
            _uniqueId[i-65] = data.at(i);
        }
    }

    MessageHeading(unsigned long length, unsigned long beginReadLoc, PacketCode packCode)
        :_length(length), _beginReadLocation(beginReadLoc), _packetCode(packCode){}

    MessageHeading(unsigned long length, unsigned long beginReadLoc, PacketCode packCode, const string & id)
        :_length(length), _beginReadLocation(beginReadLoc), _packetCode(packCode){
        for(unsigned int i = 0; i < 64; i++)
            _uniqueId[i] = id.at(i);
    }

    string toString()const{
        stringstream result;
        result << _packetCode;
        result << paddAndSerialize(_length);
        result << paddAndSerialize(_beginReadLocation);
        result << _uniqueId;
        return result.str();
    }

    string paddAndSerialize(unsigned long val)const{
        stringstream ss;
        ss << val;

        stringstream ssLengthl;
        int ssLength = ss.str().length();
        for(int i = 0; i < 32-ssLength; i++)
            ssLengthl << string("l");

        return ssLengthl.str() + ss.str();
    }

} MessageHeading;

class Message : public FastByteArray
{
    public:
        //owns pKey
        Message(char * pData, unsigned int len, FastByteArray * pKey);
        void encrypt();
        void decrypt();
        static unsigned long getHeadingLength();
        ~Message();
    protected:
    private:
        bool _isEncrypted;
        FastByteArray * _pKey;
        void xorWithKey();
};

} //namespace OTPEncryptor

#endif // MESSAGE_H
