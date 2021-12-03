from enum import IntEnum

class MessageTypes(IntEnum):
    MT_INIT = 0,
    MT_EXIT = 1,
    MT_GETDATA = 2,
    MT_DATA = 3,
    MT_NODATA = 4,
    MT_CONFIRM = 5,
    MT_GETMESSAGES = 6
