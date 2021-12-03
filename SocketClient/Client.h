#pragma once
#include "Message.h"

class Client
{
public:
	int id = -1;

	void send(CSocket&, int, int, int mType = MT_DATA, const string& data = "");
	void connect(CSocket&);
	void disconnect(CSocket&);
	void start();
};

static void GetData(Client*);