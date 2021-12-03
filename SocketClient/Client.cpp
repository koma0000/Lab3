#include "pch.h"
#include "Client.h"

void Client::send(CSocket& s, int mTo, int mFrom, int mType, const string& data) {
	Message m(mTo, mFrom, mType, data);
	m.send(s);
}

void Client::connect(CSocket& s) {
    s.Create();
    s.Connect(_T("127.0.0.1"), 12345);
}

void Client::disconnect(CSocket& S) {
    S.Close();
}

void Client::start()
{
    AfxSocketInit();
    CSocket s;
    connect(s);
    send(s, 0, 0, MT_INIT);
    Message m;
    if (m.receive(s) == MT_CONFIRM) {
        id = m.getHeader().mTo;
        cout << "My id is " << id << endl;
        thread t(&GetData, this);
        t.detach();
    }
    else {
        cout << "Something went wrong" << endl;
        return;
    }
    disconnect(s);

    // Передача и прием сообщений
    while (true) {
        cout << "1. Send Message\n2. Get all messages\n3. Exit\n" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            int ClientID = MR_ALL;
            cout << "1. One client\n2. All Clients\n" << endl;
            int choice2;
            cin >> choice2;

            if (choice2 == 1) {
                cout << "\nEnter ID of the client: ";
                cin >> ClientID;
            }

            cout << "\nEnter your message: ";
            string str;
            cin.ignore();
            getline(cin, str);
            connect(s);
            Message::send(s, ClientID, id, MT_DATA, str);
            if (m.receive(s) == MT_CONFIRM) {
                cout << "\nOk\n" << endl;
            }
            else {
                cout << "\nSomething went wrong\n" << endl;
            }
            disconnect(s);

            break;
        }
        case 2: {
            CSocket s;
            s.Create();

            if (!s.Connect("127.0.0.1", 12345))
            {
                DWORD dwError = GetLastError();
                throw runtime_error("Connection error");
            }

            m.send(s, id, id, MT_GETMESSAGES);

            m.receive(s);

            if (m.getHeader().mType == MT_NODATA) {
                cout << "\nNo messages in the archive\n" << endl;
            }
            else
            {
                unsigned int messagesLen = stoi(m.getData());

                cout << "\n\nMessage history:\n" << endl;

                for (unsigned int i = 0; i < messagesLen; i++) {

                    m.receive(s);

                    cout << "===============================================" << endl;;
                    cout << "Message from client - " << m.getHeader().mFrom << " : " << m.getData() << endl;
                    cout << "===============================================" << endl;

                }
                cout << "\n" << endl;
            }

            disconnect(s);
            s.Close();

            break;
        }
        case 3: {
            connect(s);
            Message::send(s, MR_USER, id, MT_EXIT);
            if (m.receive(s) == MT_CONFIRM) {
                cout << "\nOk\n" << endl;
            }
            else {
                cout << "\nSomething went wrong\n" << endl;
            }
            disconnect(s);
            return;
        }
        default:
        {

            cout << "\nIt's not a correct number. Please, enter 1 or 2.\n" << endl;
            break;
        }
        }
    }
}

void GetData(Client* client) {
    while (true) {
        CSocket s;
        client->connect(s);
        Message m;
        client->send(s, MR_USER, client->id, MT_GETDATA);
        if (m.receive(s) == MT_DATA) {
            cout << "I've got message from client with ID = " << m.getHeader().mFrom << ": " << m.getData() << endl;
        }
        client->disconnect(s);
        Sleep(1000);
    }
}