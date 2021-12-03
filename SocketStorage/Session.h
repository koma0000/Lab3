#pragma once
#include "Message.h"

class Session
{
private:
	vector<Message> messages;
	CRITICAL_SECTION criricalSection;
public:
	int id;
	clock_t  time;

	Session();
	Session(int, clock_t);
	~Session();
	void add(Message&);
	void send(CSocket&);
	void setTime(clock_t t);
};