#ifndef KEYVALIDATOR_H
#define KEYVALIDATOR_H
#include <vector>
#include "KeyManager.h"

namespace OTPEncryptor{
    using namespace std;

    typedef struct ValidationPoint{
        unsigned long _indexToCheck;
        char _expectedVal;

        ValidationPoint(unsigned long index, char expected):
            _indexToCheck(index), _expectedVal(expected) {}

        bool operator ==(const ValidationPoint & other)const{
            return _indexToCheck == other._indexToCheck && _expectedVal == other._expectedVal;
        }

    } ValidationPoint;

    class KeyValidator
    {
        public:
            //owns input vector
            KeyValidator(vector<ValidationPoint> * pVec);

            bool validateKey(const KeyManager * pKeyManager)const;

            virtual ~KeyValidator();
        protected:
        private:
            vector<ValidationPoint> * _pValPoints;
    };
};
#endif // KEYVALIDATOR_H
