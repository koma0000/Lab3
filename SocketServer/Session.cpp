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
	messages.push(m);
	LeaveCriticalSection(&criricalSection);
}

void Session::send(CSocket& s)
{
	EnterCriticalSection(&criricalSection);

	if (messages.empty())
	{
		Message::send(s, id, MR_USER, MT_NODATA);
	}
	else
	{
		messages.front().send(s);
		messages.pop();
	}

	LeaveCriticalSection(&criricalSection);
}