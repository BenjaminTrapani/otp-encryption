#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <random>

using namespace std;

int main()
{
    cout << "Welcome to the byte key generator. How long should the generated key be (in megabytes)?" << endl;
    int sizeInMegs = 0;
    cin >> sizeInMegs;
    int sizeInBytes = sizeInMegs * 1000000;

    cout << "Output file (absolute path and extension should be txt):" << endl;
    string filePath;
    cin >> filePath;

    random_device rdevice;
    ofstream output(filePath.c_str());
    for (int i = 0; i < sizeInBytes; i++)
        output << (char)(rdevice() % 256);

    output.close();

    return 0;
}
