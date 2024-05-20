//sock.cpp

#include "sock.h"
#include <Ws2tcpip.h>
#include <fstream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define DEFAULT_PROTOCOL IPPROTO_TCP

inline PCWSTR getIP(bool status) {
	std::string MyIP;
	std::string InterlocutorIP;
	std::ifstream in("IP.txt");
	in >> MyIP >> InterlocutorIP;
	in.close();
	std::string IP = (status ? MyIP : InterlocutorIP);
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, IP.c_str(), -1, NULL, 0);
	wchar_t* buffer = new wchar_t[bufferSize];
	MultiByteToWideChar(CP_UTF8, 0, IP.c_str(), -1, buffer, bufferSize);
	return buffer;
}

inline sockaddr_in getService(bool status) {
	sockaddr_in tmp;
	tmp.sin_family = PF_INET;
	InetPton(AF_INET, getIP(status), &tmp.sin_addr.s_addr);
	tmp.sin_port = htons(1029);
	return tmp;
}

const sockaddr_in ClientService = getService(0);
const sockaddr_in ServerService = getService(1);

//struct Sock

Sock::Sock()
	: sock(socket(PF_INET, SOCK_STREAM, DEFAULT_PROTOCOL))
{
	if (sock == INVALID_SOCKET) throw (Exeption)2;
}

Sock::Sock(SOCKET other)
	: sock(other)
{
	if (sock == INVALID_SOCKET) throw (Exeption)2;
}

Sock::~Sock()
{
	closesocket(sock);
}


//struct ListenSock

ListenSock::ListenSock(SOCKET other)
	: Sock(other)
{}

bool ListenSock::Bind() {
	return (bind(sock, (SOCKADDR*)&ServerService, sizeof(ServerService)) != 0);
}

bool ListenSock::Listen(int i) {
	int res = listen(sock, i);
	return res != 0;
}

SOCKET ListenSock::Accept() {
	return accept(sock, NULL, NULL);
}

//struct ConnectSock

ConnectSock::ConnectSock(SOCKET other)
	: Sock(other)
{}

bool ConnectSock::Connect() {
	return (connect(sock, (SOCKADDR*)&ClientService, sizeof(ClientService)) != 0);
}

bool ConnectSock::ReSock(SOCKET other) {
	//if (closesocket(sock) != SOCKET_ERROR) return 1;
	sock = other;
	return 0;
}

int ConnectSock::Send(const char* buf)
{
	return send(sock, buf, 21, 0);
}

int ConnectSock::Recv(char* buf)
{
	return recv(sock, buf, 21, 0);
}

Exeption::Exeption(int ERR_CODE)
{
	switch (ERR_CODE)
	{
	case 1: ErrorMsg = "Wrong input\n"; break;
	case 2: ErrorMsg = "Create socket error\n"; break;
	case 3: ErrorMsg = "Connect socket error\n"; break;
	case 4: ErrorMsg = "Bind error\n"; break;
	case 5: ErrorMsg = "Listen error\n"; break;
	case 6: ErrorMsg = "Send error\n"; break;
	case 7: ErrorMsg = "Recv error\n"; break;
	case 8: ErrorMsg = "Unknown command\n"; break;
	default:ErrorMsg = "Unknown error\n"; break;
	}
}

std::ostream& operator<<(std::ostream& os, const Exeption& obj)
{
	os << obj.ErrorMsg;
	return os;
}
