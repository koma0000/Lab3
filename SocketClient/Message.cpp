#include "pch.h"
#include "Message.h"

//string GetLastErrorString(DWORD ErrorID = 0)
//{
//	if (!ErrorID)
//		ErrorID = GetLastError();
//	if (!ErrorID)
//		return string();
//
//	LPSTR pBuff = nullptr;
//	size_t mSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pBuff, 0, NULL);
//	string s(pBuff, mSize);
//	LocalFree(pBuff);
//
//	return s;
//}
//
//int Message::clientID = 0;
//
//void Message::send(CSocket& s, int mTo, int mFrom, int mType, const string& data)
//{
//	Message m(mTo, mFrom, mType, data);
//	m.send(s);
//}
//
//Message Message::send(int mTo, int mType, const string& data)
//{
//	CSocket s;
//	s.Create();
//	if (!s.Connect("127.0.0.1", 12345))
//	{
//		throw runtime_error(GetLastErrorString());
//	}
//	Message m(mTo, clientID, mType, data);
//	m.send(s);
//	if (m.receive(s) == MT_INIT)
//	{
//		clientID = m.header.mTo;
//	}
//	return m;
//}

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
		s.Send(data.c_str(), (int)header.mSize+1);//+1 ?
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