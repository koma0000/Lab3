#include "pch.h"
#include "Message.h"

Message::Message()
{
}

Message::Message(int mTo, int mFrom, int mType, const string& data)
{
	this->data = data;
	header = { mTo, mFrom, mType, (int)data.length() };
}

MessageHeader Message::getHeader()
{
	return header;
}

string Message::getData()
{
	return data;
}

void Message::send(CSocket& s)
{
	s.Send(&header, sizeof(MessageHeader));
	if (header.mSize)
	{
		s.Send(data.c_str(), (int)header.mSize);//+1 ?
	}
}

int Message::receive(CSocket& s) {
	if (!s.Receive(&header, sizeof(MessageHeader)))
	{
		return MT_NODATA;
	}
	if (header.mSize)
	{
		char* v = new char[header.mSize + 1];

		s.Receive(v, header.mSize + 1);
		v[header.mSize] = '\0';
		data = v;

		delete[] v;
	}
	return header.mType;
}

// Функции для работы с хранилищем
Message Message::send(unsigned int to, unsigned int type, const string& data)
{
	CSocket s;
	s.Create();

	if (!s.Connect("127.0.0.1", 12345))
	{
		DWORD dwError = GetLastError();
		throw runtime_error("Connection error");
	}

	Message m(to, id, type, data);
	m.send(s);
	m.receive(s);
	if (m.getHeader().mType == MT_INIT)
	{
		id = m.getHeader().mTo;
	}
	return m;
}

int Message::id = 0;

void Message::send(CSocket& s, int mTo, int mFrom, int mType, const string& data) {
	Message m(mTo, mFrom, mType, data);
	m.send(s);
}

// Функция для отправки служебной информации в хранилище
Message Message::SendToStorage(unsigned int to, unsigned int from, unsigned int type, const string& data)
{
	CSocket s;
	s.Create();
	if (!s.Connect("127.0.0.1", 54321))
	{
		DWORD dwError = GetLastError();
		throw runtime_error("Connection error");
	}
	Message m(to, from, type, data);
	m.send(s);
	m.receive(s);

	s.Close();
	return m;
}