#include "pch.h"
#include "Message.h"

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

void Message::send(CSocket& s, int mTo, int mFrom, int mType, const string& data) {
	Message m(mTo, mFrom, mType, data);
	m.send(s);
}

// Функция для отправки ответов из хранилища
Message Message::sendArc(unsigned int mType, const string& data) {
	CSocket s;
	s.Create();

	if (!s.Connect(_T("127.0.0.1"), 12345))
	{
		DWORD dwError = GetLastError();
		throw runtime_error("Connection error");
	}

	Message m(MR_USER, MR_STORAGE, mType, data);
	m.send(s);

	return m;
}