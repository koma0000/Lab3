import socket, struct
from enum import IntEnum
from threading import Thread
from time import sleep
from datetime import datetime
import sys

class MessageRecipients(IntEnum):
    MR_STORAGE = -2,
    MR_ALL = -1,
    MR_USER = 0

class MessageTypes(IntEnum):
    MT_INIT = 0,
    MT_EXIT = 1,
    MT_GETDATA = 2,
    MT_DATA = 3,
    MT_NODATA = 4,
    MT_CONFIRM = 5,
    MT_GETMESSAGES = 6

class MessageHeader():
    def __init__(self, mTo=0, mFrom=0, mType=0, mSize=0):
        self.mTo=mTo
        self.mFrom=mFrom
        self.mType=mType
        self.mSize=mSize
    def HeaderInit(self, header):
        self.mTo=header[0]
        self.mFrom=header[1]
        self.mType=header[2]
        self.mSize=header[3]

class Message():
    def __init__(self, To=0, From=0, Type=MessageTypes.MT_DATA, Data=''):
        self.header=MessageHeader()
        self.header.mTo=To;
        self.header.mFrom=From;
        self.header.mType=Type;
        self.header.mSize=int(len(Data))
        self.data=Data
    def SendData(self, s):
        s.send(struct.pack('i', self.header.mTo))
        s.send(struct.pack('i', self.header.mFrom))
        s.send(struct.pack('i', self.header.mType))
        s.send(struct.pack('i', self.header.mSize))
        if self.header.mSize>0:
            s.send(struct.pack(f'{self.header.mSize}s', self.data.encode('utf-8')))
    def ReceiveData(self, s):
        self.header=MessageHeader()
        try: 
            self.header.HeaderInit(struct.unpack('iiii', s.recv(16)))
            if self.header.mSize>0:
                self.data=struct.unpack(f'{self.header.mSize}s', s.recv(self.header.mSize))[0]
        except:
            self.header.mSize = 0
            self.header.mType = MessageTypes.MT_NODATA

        return self.header.mType

def send(Socket, To, From, Type=MessageTypes.MT_DATA, Data=''):
    m=Message(To, From, Type, Data)
    m.SendData(Socket)

def receive(Socket):
    m=Message()
    res=m.ReceiveData(Socket)
    return res

def connect(Socket):
    Socket.connect(('127.0.0.1', 12345))

def disconnect(Socket):
    Socket.close()

def GetData(ID):
    while True:
        clientSock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connect(clientSock)
        send(clientSock, 0, ID, MessageTypes.MT_GETDATA)
        msg=Message()
        if (msg.ReceiveData(clientSock)==MessageTypes.MT_DATA):
            print('\nIve got message from client with ID = ', msg.header.mFrom, ': ', msg.data.decode('utf-8'), '\n')
            sleep(1)
        else:
            sleep(1)
        disconnect(clientSock)

myID=0

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    connect(s)
    send(s, 0, 0, MessageTypes.MT_INIT)
    m=Message()
    if (m.ReceiveData(s)==MessageTypes.MT_CONFIRM):
        myID=m.header.mTo
        
        print('My id is '+str(myID)+'\n')
        t=Thread(target=GetData, args=(myID,), daemon=True)
        t.start()      
        disconnect(s)
    else:
        print('Something went wrong')
        sleep(1000)
        sys.exit()
while True:
    # domain - AF_INET для сетевого протокола IPv4
    # type - SOCK_STREAM (надёжная потокоориентированная служба (сервис) или потоковый сокет)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        choice=int(input('1. Send Message\n2. Get all messages\n2. Exit\n'))
        if choice==1:
            ClientID=MessageRecipients.MR_ALL
            choice2=int(input('1. One client\n2. All Clients\n'))
            if choice2==1:
                ClientID=int(input('\nEnter ID of the client: '))
            str=input('\nEnter your message: ')
            connect(s)
            send(s, ClientID, myID, MessageTypes.MT_DATA, str)
            if m.ReceiveData(s)==MessageTypes.MT_CONFIRM:
                print('\nOk\n')
            else: 
                print('\nSomething went wrong\n')
                sleep(1000)
            disconnect(s)
        elif choice==2:
           connect(s)

           send(s, myID, myID, MessageTypes.MT_GETMESSAGES);

           if m.ReceiveData(s)==MessageTypes.MT_NODATA:
               print("\nNo messages in the archive\n")
           else:
               messagesLen = int(m.data.decode('utf-8'))

               print("\n\nMessage history:\n")

               for number in range(messagesLen):
                   m.ReceiveData(s)
                   print("===============================================")
                   print("Message from client - ", m.header.mFrom, " : ", m.data.decode('ascii'))
                   print("===============================================")
                       
           disconnect(s)
        elif choice==3:
           connect(s)
           send(s, 0, myID, MessageTypes.MT_EXIT)
           if m.ReceiveData(s)==MessageTypes.MT_CONFIRM:
               print('\nOk\n')
               sleep(1)
               break
           else: print('\nSomething went wrong\n')
           disconnect(s)
        else:
            print("It's not a correct number. Please, enter 1 or 2.")
