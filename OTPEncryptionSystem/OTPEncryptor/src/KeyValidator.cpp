#include "KeyValidator.h"
using namespace OTPEncryptor;

KeyValidator::KeyValidator(vector<ValidationPoint> * pVec): _pValPoints(pVec) {}

bool KeyValidator::validateKey(const KeyManager * pKeyManager)const{

    for (vector<ValidationPoint>::size_type i = 0; i < _pValPoints->size(); i++){
        ValidationPoint * curValPoint = &_pValPoints->at(i);
        FastByteArray * pArray = pKeyManager->getKey(curValPoint->_indexToCheck, 1);

        if (pArray == nullptr || pArray->charAt(0) != curValPoint->_expectedVal)
            return false;
    }

    return true;
}

KeyValidator::~KeyValidator()
{
    delete _pValPoints;
}
