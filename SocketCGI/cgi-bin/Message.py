#!/usr/bin/env python3

import socket, struct
from threading import Thread
from time import sleep
from datetime import datetime
import sys

from MessageHeader import *
from MessageTypes import *
from MessageRecipients import *

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
