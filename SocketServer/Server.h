#pragma once
#include "Session.h"


class Server
{
public:
	map<int, shared_ptr<Session>> clientSessions;
	mutex myMutex;

	CRITICAL_SECTION criticalSection;

	Server();
	~Server();
	void start();
	int give_id();
};

static void TimeOut(Server* ser);
static void ProcessClient(Server* ser, SOCKET hSock);