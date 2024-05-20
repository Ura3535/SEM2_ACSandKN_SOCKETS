//sock.h

#pragma once
#include <string>
#include <WinSock2.h>
#include <iostream>

struct Exeption {
	Exeption(int);
	friend std::ostream& operator << (std::ostream&, const Exeption&);
	std::string ErrorMsg;
};

struct Sock {
public:
	Sock();
	explicit Sock(SOCKET);
	Sock(const Sock&) = default;
	~Sock();
protected:
	SOCKET sock;
};

struct ListenSock : public Sock {
	ListenSock() = default;
	explicit ListenSock(SOCKET);
	bool Bind();
	bool Listen(int i);
	SOCKET Accept();
};

struct ConnectSock : public Sock {
	ConnectSock() = default;
	explicit ConnectSock(SOCKET);
	bool Connect();
	bool ReSock(SOCKET);
	int Send(const char*);
	int Recv(char*);
};
