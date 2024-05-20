//main.cpp

#define WIN32_LEAN_AND_MEAN

#include <fstream>
//#include <iostream>
#include "chatSock.h"

int main()
{
    try {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return 1;
        std::ofstream log("log.txt");
        Chat newChat;
        newChat.Connect();
        newChat.Start();
    }
    catch (Exeption SockError) {
        std::cerr << SockError;
    }
    catch (...) {
        std::cerr << "Unknown error\n";
    }
    WSACleanup();
}
