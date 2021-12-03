#pragma once

enum MessageTypes
{
	MT_INIT,
	MT_EXIT,
	MT_GETDATA,
	MT_DATA,
	MT_NODATA,
	MT_CONFIRM,
	MT_GETMESSAGES
};

enum MessageRecipients
{
	MR_STORAGE = -2,
	MR_ALL = -1,
	MR_USER = 0
};

struct MessageHeader
{
	int mTo;
	int mFrom;
	int mType;
	int mSize;
};

class Message
{
private:
	MessageHeader header = { 0 };
	string data = "";

	static int id;
public:
	Message();
	Message(int, int, int mType = MT_DATA, const string& data = "");
	MessageHeader getHeader();
	string getData();
	void send(CSocket&); // Отправка сообщения
	int receive(CSocket&); // Получение сообщения
	static Message send(unsigned int to, unsigned int type = MT_DATA, const string& data = "");
	static void send(CSocket&, int, int, int mType = MT_DATA, const string& data = "");
	static Message SendToStorage(unsigned int to, unsigned int from, unsigned int type, const string& data = "");
};


