#include "pch.h"
#include "Session.h"

Session::Session() {
	InitializeCriticalSection(&criricalSection);
}

Session::Session(int ID, clock_t t)
	:id(ID), time(t) {
	InitializeCriticalSection(&criricalSection);
}

Session::~Session() {
	DeleteCriticalSection(&criricalSection);
}

void Session::add(Message& m)
{
	EnterCriticalSection(&criricalSection);
	messages.push_back(m);
	LeaveCriticalSection(&criricalSection);
}

void Session::send(CSocket& s)
{
	EnterCriticalSection(&criricalSection);

	Message m;
	if (messages.empty())
	{
		Message::send(s, id, MR_USER, MT_NODATA);
	}
	else
	{
		Message::send(s, id, MR_STORAGE, MT_DATA, to_string((int)messages.size()));

		for (Message m : messages)
		{
			Sleep(100);
			Message::send(s, m.getHeader().mTo, m.getHeader().mFrom, MT_DATA, m.getData());
		}
	}

	LeaveCriticalSection(&criricalSection);
}

void Session::setTime(clock_t t)
{
	time = t;
}