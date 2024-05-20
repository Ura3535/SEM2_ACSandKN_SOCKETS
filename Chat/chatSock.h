//chatSock.h

#pragma once
#include "sock.h"

class Chat {
public:
	Chat();
	void Connect();
	void Start();
private:
	void logOut(const std::string&);
	void Send(std::string, bool);
	void Recv();
	inline int commandProcessing(std::string);
	inline void clearChat();
	inline void print(const std::string&, bool);

	char name[20];
	char InterlocutorName[20];
	bool connection;
	bool serverStatus;
	ConnectSock Interlocutor;
};
