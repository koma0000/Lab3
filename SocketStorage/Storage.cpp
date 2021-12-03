#include "pch.h"
#include "Storage.h"

void Storage::start()
{
    AfxSocketInit();

    CSocket Server;
    Server.Create(54321);

    while (true)
    {
        if (!Server.Listen())
            break;
        CSocket s;
        Server.Accept(s);
        thread t(&ProcessClient, this, s.Detach());
        t.detach();
    }
}

void ProcessClient(Storage* sto, SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);

    Message m;
    int typeCode = m.receive(s);
    cout << m.getHeader().mFrom << ": " << typesNames[typeCode] << endl;

    switch (typeCode)
    {
        case MT_INIT:
        {
            auto pSession = make_shared<Session>(m.getHeader().mFrom, clock());
            sto->sessions[pSession->id] = pSession;
            Message::sendArc(MT_CONFIRM);
            break;
        }
        case MT_EXIT:
        {
            sto->sessions.erase(m.getHeader().mTo);
            Message::sendArc(MT_CONFIRM);
            return;
        }
        case MT_GETMESSAGES:
        {
            if (sto->sessions.find(m.getHeader().mTo) != sto->sessions.end())
            {
                sto->sessions[m.getHeader().mTo]->send(s);
            }
            Message::sendArc(MT_CONFIRM);
            break;
        }
        case MT_DATA:
        {
            if (sto->sessions.find(m.getHeader().mFrom) != sto->sessions.end())
            {
                if (sto->sessions.find(m.getHeader().mTo) != sto->sessions.end())
                {
                    sto->sessions[m.getHeader().mTo]->add(m);
                }
                else if (m.getHeader().mTo == MR_ALL)
                {
                    for (auto i = sto->sessions.begin(); i != sto->sessions.end(); ++i)
                    {
                        if (i->first != m.getHeader().mFrom)
                        {
                            i->second->add(m);
                        }
                    }
                }
            }
            Message::sendArc(MT_CONFIRM);
            break;
        }
        default:
        {
            break;
        }
    }
}