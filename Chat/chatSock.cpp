//chatSock.cpp

#include "chatSock.h"
#include <thread>
#include <fstream>
#include <chrono>

//struct Chat

const char* WHO = "Author: Kovalchuk Urii\nVariant: 4 (port 1029)\nProject: chat\n";

Chat::Chat()
	: name("")
	, InterlocutorName("")
	, connection(0)
	, serverStatus(0)
{}

void Chat::Connect() {
	char status;
	std::cout << "Enter your name: ";
	std::cin >> name;
	std::cout << "Do you want to connect to another user [C] or wait for him to connect to you [S]: ";
	std::cin >> status;
	switch (tolower(status))
	{
	case 'c':
		serverStatus = 0;
		break;
	case 's':
		serverStatus = 1;
		break;
	default :
		throw (Exeption)1;
		break;
	}
	if (serverStatus) {
		ListenSock ListenSocket;
		if (ListenSocket.Bind())	throw (Exeption)4;
		if (ListenSocket.Listen(1)) throw (Exeption)5;
		std::cout << "Wait for the interlocutor to connect...\n";
		Interlocutor.ReSock(ListenSocket.Accept());
		std::cout << "The interlocutor has connected\n";
	}
	else if (Interlocutor.Connect()) throw (Exeption)3;
	logOut((std::string)"Conect");// + InterlocutorName);
	if (Interlocutor.Send(name) < 0) throw (Exeption)6;
	logOut((std::string)"send| message (last peace): " + name);
	if (Interlocutor.Recv(InterlocutorName) < 0) throw (Exeption)7;
	logOut((std::string)"recv| message (last peace): " + InterlocutorName);
	std::cout << "Conect to " << InterlocutorName << '\n';
	std::ofstream out("log.txt", std::ios::app);	out.close();
	connection = 1;
	clearChat();
}

void Chat::Start()
{
	clearChat();
	std::thread RECV(&Chat::Recv, this);
	RECV.detach();
	std::string command = "";
	std::string message = "";
	int commandCode = 0;
	while (connection) {
		std::cin >> command;
		if (!connection) break;
		commandCode = commandProcessing(command);
		switch (commandCode)
		{
		case 1: 
			if (serverStatus) std::cout << WHO;
			else { 
				char buf[21] = "3";
				Interlocutor.Send(buf);
				logOut((std::string)"send| who: " + buf);
			}
			break;
		case 2: 
			std::getline(std::cin, message);
			Send(message, 0); 
			break;
		case 3: {
			char buf[21] = "0";
			Interlocutor.Send(buf);
			logOut((std::string)"send| exit: " + buf);
			connection = 0; }
			break;
		default:
			std::cout << "'" << command << "' is an unknown command\n";
			break;
		}
	}
}

void Chat::logOut(const std::string& event)
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	char buffer[26];
	ctime_s(buffer, sizeof(buffer), &time);
	buffer[24] = '\0';
	std::ofstream out("log.txt", std::ios::app);
	out << '[' << buffer << "]: " << event << '\n';
	out.close();
}

/*
msgType:
	0 - standart msg
	1 - Who information
*/

void Chat::Send(std::string msg, bool msgType)
{
	char head = msgType ? '4' : '1';
	std::string msgOrWho = (std::string)"send| " + (msgType ? "return who " : "message ");
	if(!msgType) print(msg, 1);
	while (msg.size() >= 20) {
		msg = head + msg;
		char buf[21];
		buf[msg.copy(buf, 20, 0)] = '\0';
		msg.erase(0, 20);
		if(Interlocutor.Send(buf) < 0) throw (Exeption)6;
		logOut(msgOrWho + "(piece): " + buf);
	}
	msg = (char)(head + 1) + msg;
	char buf[21];
	buf[msg.copy(buf, msg.size(), 0)] = '\0';
	if(Interlocutor.Send(buf) < 0) throw (Exeption)6;
	logOut(msgOrWho + "(last piece): " + buf);
}

void Chat::Recv()
{
	std::string msg = "";
	std::string who = "";
	char buf[21];
	while (connection) {
		if(Interlocutor.Recv(buf) < 0) throw (Exeption)7;
		switch (buf[0])
		{
		case '0':	//exit
			logOut((std::string)"recv| exit: " + buf);
			connection = 0;
			break;
		case '1':	//message (piece)
			logOut((std::string)"recv| message (piece): " + buf);
			msg.append(buf + 1);
			break;
		case '2':	//message (last piece)
			logOut((std::string)"recv| message (last piece): " + buf);
			msg.append(buf + 1); 
			print(msg, 0);
			msg = "";
			break;
		case '3':	//Who
			logOut((std::string)"recv| who: " + buf);
			Send(WHO, 1);
			break;
		case '4':	//return who (piece)
			logOut((std::string)"recv| return who (piece): " + buf);
			who.append(buf + 1);
			break;
		case '5':	//return who (last piece)
			logOut((std::string)"recv| return who (last piece): " + buf);
			who.append(buf + 1);
			std::cout << who;
			who = "";
			break;
		default: break;
		}
	}
}

inline int Chat::commandProcessing(std::string command)
{
	int commandCode = -1;
	for (int i = 0; i != command.size(); ++i)
		command[i] = tolower(command[i]);
	if (command == "who" ) commandCode = 1;
	if (command == "send") commandCode = 2;
	if (command == "exit") commandCode = 3;
	return commandCode;
}

inline std::string pathChat(const char* name) {
	return (std::string)name + "_chat.txt";
}

void Chat::clearChat()
{
	std::ofstream out(pathChat(name));
	out.close();
}

void Chat::print(const std::string& msg, bool who)
{
	std::ofstream out(pathChat(name), std::ios::app);
	out << (who ? name : InterlocutorName) << ": " << msg << '\n';
	out.close();
}
