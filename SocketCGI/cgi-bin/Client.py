#!/usr/bin/env python3

import socket, struct
from threading import Thread
from time import sleep
from datetime import datetime
import sys

from Message import *


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

class Client():

    def __init__(self, form):
        self.form = form
        self.myID = 0

    def start(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            connect(s)
            send(s, 0, 0, MessageTypes.MT_INIT)
            m=Message()
            if (m.ReceiveData(s)==MessageTypes.MT_CONFIRM):
                self.myID=m.header.mTo  
                disconnect(s)

    def post(self, ClientID, str):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            connect(s)
            send(s, ClientID, self.myID, MessageTypes.MT_DATA, str)
            m=Message()
            if m.ReceiveData(s)==MessageTypes.MT_CONFIRM:
                disconnect(s)
                return True
            else: 
                disconnect(s)
                sleep(1000)
                return False
            

    def getAll(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            connect(s)
            send(s, self.myID, self.myID, MessageTypes.MT_GETMESSAGES);
            m=Message()
            if m.ReceiveData(s)==MessageTypes.MT_NODATA:
                disconnect(s)
                return None
            else:
                messagesLen = int(m.data.decode('utf-8'))

                result = """<br><br>Message history: """

                for number in range(messagesLen):
                    m.ReceiveData(s)
                    result = "".join([result, """<br>Message from client with ID = """, str(m.header.mFrom), """ : """, m.data.decode('utf-8')])
             
                disconnect(s)
        return result