#include <bits/stdc++.h>
#include <arpa/inet.h>
using namespace std;

int main(int argc, char const *argv[])
{
    unsigned int x = 0x12345678;
    unsigned char* p = (unsigned char*)&x;
    cout << hex << (unsigned int)p[0] << " " 
         << hex << (unsigned int)p[1] << " "
         << hex << (unsigned int)p[2] << " "
         << hex << (unsigned int)p[3] << endl;   //x86平台是小端字节序

    unsigned int y = htonl(x);
    p = (unsigned char*) &y;
    // int y = 16;
    // cout <<hex << (y)  << endl;
    printf("%x %0x %0x %0x\n", p[0], p[1], p[2], p[3]);

    return 0;
}