#ifndef FASTBYTEARRAY_H
#define FASTBYTEARRAY_H

#include "stdlib.h"
#include <string>

namespace OTPEncryptor{

class FastByteArray
{
    friend class Message;
    public:
        //owns pData. Fast way of managing character arrays.
        FastByteArray(char * pData, unsigned int len);
        FastByteArray(const std::string & string);

        inline char & charAt(unsigned int i){return _pData[i];}
        inline char * getData() const {return _pData;}
        inline unsigned int getLength()const {return _len;}
        inline void setOwnsData(const bool owns) {_ownsData = owns;}
        ~FastByteArray();
    protected:
    private:
        char * _pData;
        unsigned int _len;
        bool _ownsData;
};

}

#endif // FASTBYTEARRAY_H
