using System;
using System.Text;
using System.Net.Sockets;

namespace SocketClientSharp
{
    enum MessageRecipients
    {
        MR_STORAGE = -2,
        MR_ALL = -1,
        MR_USER = 0
    };

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

    struct MessageHeader
    {
        public int mTo;
        public int mFrom;
        public int mType;
        public int mSize;
    };

    class Message
    {
        public string Data { get; set; } = "";

        public MessageHeader header = new MessageHeader { mTo = 0, mFrom = 0, mType = 0, mSize = 0 };

        public Message() {}

        public Message(int to, int from, MessageTypes type = MessageTypes.MT_DATA, string data = "")
        {
            header = new MessageHeader { mTo = to, mFrom = from, mType = (int)type, mSize = data.Length };
            Data = data;
        }

        public MessageHeader getHeader()
        {
            return header;
        }

        public void Send(Socket s)
        {
            s.Send(BitConverter.GetBytes(header.mTo), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(header.mFrom), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes((int)header.mType), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(header.mSize), sizeof(int), SocketFlags.None);

            if (header.mSize != 0)
            {
                s.Send(Encoding.UTF8.GetBytes(Data), header.mSize, SocketFlags.None);
            }

        }

        public MessageTypes Receive(Socket s)
        {
            byte[] buffer = new byte[4];
            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.mTo = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.mFrom = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.mType = (int)(MessageTypes)BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            header.mSize = BitConverter.ToInt32(buffer, 0);

            if (header.mSize != 0)
            {
                buffer = new byte[header.mSize];
                s.Receive(buffer, header.mSize, SocketFlags.None);
                Data = Encoding.UTF8.GetString(buffer, 0, header.mSize);
            }

            return (MessageTypes)(int)header.mType;
        }
    }
}
