#pragma once
#include "Message.h"
#include "Session.h"

static map<int, string> typesNames = {
    { MT_INIT, "INIT"},
    { MT_EXIT, "EXIT"},
    { MT_GETDATA, "GETDATA"},
    { MT_DATA, "DATA"},
    { MT_NODATA, "NODATA"},
    { MT_CONFIRM, "CONFIRM"},
    { MT_GETMESSAGES, "GETMESSAGES"}
};

class Storage
{
public:
    map<int, shared_ptr<Session>> sessions;

    void start();
};

static void ProcessClient(Storage* sto, SOCKET hSock);
