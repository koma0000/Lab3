#include "pch.h"
#include "Server.h"

Server::Server()
{
	InitializeCriticalSection(&criticalSection);
}

Server::~Server() {
	DeleteCriticalSection(&criticalSection);
}

void Server::start()
{
	AfxSocketInit();

	CSocket Server;
	Server.Create(12345);

	thread tt(&TimeOut, this);
	tt.detach();

	while (true)
	{
		Server.Listen();
		CSocket s;
		Server.Accept(s);
		thread t(&ProcessClient, this, s.Detach());
		t.detach();
	}
}

int Server::give_id() {
	time_t rawtime = time(NULL);
	struct tm timeinfo;
	char buffer[1024];
	localtime_s(&timeinfo, &rawtime);
	asctime_s(buffer, &timeinfo);

	int id = timeinfo.tm_hour * pow(10, 4) +
		timeinfo.tm_min * pow(10, 2) +
		timeinfo.tm_sec;

	return id;
}

static void TimeOut(Server* ser) {
	while (true)
	{
		for (auto i = ser->clientSessions.begin(); i != ser->clientSessions.end();) {
			if (ser->clientSessions.find(i->first) != ser->clientSessions.end()) {
				if (double(clock() - i->second->time) > 1000000) {
					ser->myMutex.lock();
					cout << "Client " << i->first << " has been disconnected" << endl;
					ser->myMutex.unlock();
					i = ser->clientSessions.erase(i);
				}
				else
					i++;
			}
		}
		Sleep(1000);
	}
}

void ProcessClient(Server* ser, SOCKET hSock)
{
	CSocket s;
	s.Attach(hSock);

	Message m;
	int code = m.receive(s);
	switch (code)
	{
		case MT_INIT:
		{
			int ID = ser->give_id();
			ser->clientSessions[ID] = make_shared<Session>(ID, clock());
			ser->myMutex.lock();
			cout << "Client with ID = " << ID << " has been connected\n";
			ser->myMutex.unlock();
			Message::send(s, ID, MR_USER, MT_CONFIRM);

			// Отправляем копию в хранилище
			Message::SendToStorage(MR_STORAGE, ID, MT_INIT);

			break;
		}
		case MT_EXIT:
		{
			ser->clientSessions.erase(m.getHeader().mFrom);
			ser->myMutex.lock();
			cout << "Client with ID = " << m.getHeader().mFrom << " has been disconnected\n";
			ser->myMutex.unlock();
			Message::send(s, m.getHeader().mFrom, MR_USER, MT_CONFIRM);
			// Отправляем копию в хранилище
			Message::SendToStorage(MR_STORAGE, m.getHeader().mFrom, MT_EXIT);
			break;
		}
		case MT_GETDATA:
		{
			auto iSession = ser->clientSessions.find(m.getHeader().mFrom);
			if (iSession != ser->clientSessions.end())
			{
				iSession->second->send(s);
				iSession->second->time = clock();
			}
			break;
		}
		case MT_GETMESSAGES:
		{
			ser->myMutex.lock();
			cout << "Client with ID = " << m.getHeader().mFrom << " wants to get all his messages." << endl;
			ser->myMutex.unlock();

			CSocket sock;
			sock.Create();

			if (!sock.Connect("127.0.0.1", 54321))
			{
				DWORD dwError = GetLastError();
				throw runtime_error("Connection error to storage");
			}

			m.send(sock);

			m.receive(sock);

			if (m.getHeader().mType == MT_NODATA) {

				Message::send(s, m.getHeader().mFrom, m.getHeader().mTo, MT_NODATA);
			}
			else
			{
				unsigned int messagesLen = stoi(m.getData());

				Message::send(s, m.getHeader().mTo, m.getHeader().mFrom, MT_DATA, to_string(messagesLen));

				for (unsigned int i = 0; i < messagesLen; i++) {
					m.receive(sock);

					Message::send(s, m.getHeader().mTo, m.getHeader().mFrom, MT_DATA, m.getData());

					//Sleep(100);
				}
			}

			sock.Close();
			break;
		}
		case MT_DATA:
		{
			auto iSessionFrom = ser->clientSessions.find(m.getHeader().mFrom);
			if (iSessionFrom != ser->clientSessions.end())
			{
				auto iSessionTo = ser->clientSessions.find(m.getHeader().mTo);
				if (iSessionTo != ser->clientSessions.end())
				{
					// Отправляем копию в хранилище адресату
					Message::SendToStorage(m.getHeader().mTo, m.getHeader().mFrom, MT_DATA, m.getData());
					shared_ptr<Session> ss = iSessionTo->second;
					ss->add(m);
					
					ser->myMutex.lock();
					cout << "Client with ID = " << m.getHeader().mFrom << " has sent message to client with ID = " << m.getHeader().mTo << endl;
					ser->myMutex.unlock();
				}
				else if (m.getHeader().mTo == MR_ALL)
				{
					// Отправляем копию в хранилище адресату
					Message::SendToStorage(m.getHeader().mTo, m.getHeader().mFrom, MT_DATA, m.getData());
					for (auto& [id, session] : ser->clientSessions)
					{
						if (id != m.getHeader().mFrom)
							session->add(m);
					}

					ser->myMutex.lock();
					cout << "Client with ID = " << m.getHeader().mFrom << " has sent message to all clients" << endl;
					ser->myMutex.unlock();
				}

				Message::send(s, m.getHeader().mFrom, MR_USER, MT_CONFIRM);
				ser->clientSessions[m.getHeader().mFrom]->time = clock();
			}
			break;
		}
		case MT_CONFIRM:
		{
			break;
		}
		default:
		{
			cout << "Something went wrong...";
			break;
		}
	}
}