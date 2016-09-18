#include "FastByteArray.h"
using namespace OTPEncryptor;

FastByteArray::FastByteArray(char * pData, unsigned int len)
    : _pData(pData), _len(len), _ownsData(true)
{

}

FastByteArray::FastByteArray(const std::string & string){
    const char * pData = string.c_str();
    _pData = new char [string.length()];
    for(unsigned int i = 0; i < string.length(); i++){
        _pData[i] = pData[i];
    }
    _len = string.length();
}

FastByteArray::~FastByteArray()
{
    if (_ownsData)
        delete [] _pData;

    _len = 0;
}
