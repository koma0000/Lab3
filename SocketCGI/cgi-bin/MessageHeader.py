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
