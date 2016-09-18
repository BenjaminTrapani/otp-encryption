#include "KeyManager.h"
using namespace OTPEncryptor;

KeyManager::KeyManager(const string & relativeKeyFilePath, bool savesLocation)
:_relativeKeyFilePath(relativeKeyFilePath), _savesLocation(savesLocation) {
    if(_savesLocation){
        ifstream inputFile(_relativeKeyFilePath + ".loc");
        char * coreData = new char[32];
        inputFile.read(coreData, 32);
        _readIndex = atol(coreData);
        inputFile.close();
    }else
        _readIndex = 0;

    ifstream keyFile(relativeKeyFilePath.c_str(), ios::binary | ios::ate);
    _fileSize = keyFile.tellg();
    cout << "Key file size = " << _fileSize << endl;
}

FastByteArray * KeyManager::getAndDestroyKeyForLength(unsigned long len){
    if ((ifstream::pos_type)(_readIndex + len) > _fileSize)
        return nullptr;

    ifstream inputFile(_relativeKeyFilePath.c_str());

    char * coreData = new char[len];
    inputFile.seekg(_readIndex, inputFile.beg);
    inputFile.read(coreData, len);
    _readIndex += len;

    inputFile.close();

    FastByteArray * result = new FastByteArray(coreData, len);
    return result;
}

FastByteArray * KeyManager::getKey(unsigned long origin, unsigned long offset)const{
    if ((ifstream::pos_type)(origin + offset) > _fileSize)
        return nullptr;

    ifstream inputFile(_relativeKeyFilePath.c_str());

    char * coreData = new char[offset];
    inputFile.seekg(origin, inputFile.beg);
    inputFile.read(coreData, offset);

    inputFile.close();

    FastByteArray * result = new FastByteArray(coreData, offset);
    return result;
}

ifstream::pos_type KeyManager::getBytesLeft()const{
    return _fileSize-(ifstream::pos_type)_readIndex;
}

ifstream::pos_type KeyManager::getTotalBytes()const{
    return _fileSize;
}

KeyManager::~KeyManager()
{
    if(_savesLocation){
        ofstream outputLoc(_relativeKeyFilePath + ".loc");
        outputLoc << _readIndex;
        outputLoc.close();
    }
}
