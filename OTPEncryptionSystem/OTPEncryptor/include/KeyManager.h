#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "stdio.h"
#include <iostream>
#include <fstream>

#include "FastByteArray.h"

using namespace std;
namespace OTPEncryptor{

class KeyManager
{
    public:
        KeyManager(const string & relativeKeyFilePath, bool savesLocation=false);
        FastByteArray * getAndDestroyKeyForLength(unsigned long len);
        FastByteArray * getKey(unsigned long origin, unsigned long offset)const;
        inline unsigned long getReadLocation()const {return _readIndex;};
        ifstream::pos_type getBytesLeft()const;
        ifstream::pos_type getTotalBytes()const;
        ~KeyManager();
    protected:
    private:
        const string _relativeKeyFilePath;
        unsigned long _readIndex;
        ifstream::pos_type _fileSize;
        bool _savesLocation;
};

} // namespace OTPEncryptor
#endif // KEYMANAGER_H
