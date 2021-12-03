using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace SocketClientSharp
{
    class Client
    {
        private static int myID = 0;

        static void send(Socket s, int To, int From, MessageTypes Type = MessageTypes.MT_DATA, string Data = "")
        {
            Message m = new Message(To, From, Type, Data);
            m.Send(s);
        }

        static void connect(Socket s, IPEndPoint endPoint)
        {
            s.Connect(endPoint);
        }

        static void disconnect(Socket s)
        {
            s.Shutdown(SocketShutdown.Both);
            s.Close();
        }

        public static void GetData()
        {
            while (true)
            {
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Message m = new Message();

                connect(s, endPoint);
                send(s, 0, myID, MessageTypes.MT_GETDATA);
                if (m.Receive(s) == MessageTypes.MT_DATA)
                {
                    Console.WriteLine("Message from client " + m.getHeader().mFrom + ": " + m.Data);
                }
                disconnect(s);
                Thread.Sleep(1000);
            }
        }

        public void Start()
        {
            Message m = new Message();
            IPEndPoint server = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            connect(s, server);
            if (!s.Connected)
                return;

            send(s, (int)MessageRecipients.MR_USER, 0, MessageTypes.MT_INIT);
            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
            {
                myID = m.getHeader().mTo;

                Console.WriteLine("My id is " + myID);

                Thread t = new Thread(GetData);
                t.Start();
            }
            else
            {
                Console.WriteLine("Something went wrong");
            }
            disconnect(s);

            while (true)
            {
                server = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Console.WriteLine("1. Send Message\n2. Get all messages\n2. Exit\n");
                int choice = Convert.ToInt32(Console.ReadLine());
                switch (choice)
                {
                    case 1:
                    {
                        int ClientID = (int)MessageRecipients.MR_ALL;
                        Console.WriteLine("1. One client\n2. All Clients\n");
                        int choice2 = Convert.ToInt32(Console.ReadLine());
                        if (choice2 == 1)
                        {
                            Console.Write("\nEnter ID of the client: ");
                            ClientID = Convert.ToInt32(Console.ReadLine());
                        }

                        Console.Write("\nEnter your message: ");
                        string str = Console.ReadLine().ToString();
                        connect(s, server);
                        send(s, ClientID, myID, MessageTypes.MT_DATA, str);
                        if (m.Receive(s) == MessageTypes.MT_CONFIRM)
                        {
                            Console.WriteLine("\nOk\n");
                        }
                        else
                        {
                            Console.WriteLine("\nSomething went wrong\n");
                        }
                        disconnect(s);
                        break;

                    }
                    case 2:
                    {
                        IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                        connect(s, endPoint);

                        send(s, myID, myID, MessageTypes.MT_GETMESSAGES);

                        if (m.Receive(s) == MessageTypes.MT_NODATA)
                            Console.WriteLine("\nNo messages in the archive\n");
                        else
                        {
                            int messagesLen = Int32.Parse(m.Data);

                            Console.WriteLine("\n\nMessage history:\n");

                            for (int i = 0; i < messagesLen; i++)
                            {
                                m.Receive(s);

                                Console.WriteLine("===============================================");
                                Console.WriteLine("Message from client " + m.getHeader().mFrom + " : " + m.Data);
                                Console.WriteLine("===============================================");

                            }

                            Console.WriteLine("\n");
                        }

                        disconnect(s);
                        break;
                    }
                    case 3:
                    {
                        connect(s, server);
                        send(s, (int)MessageRecipients.MR_USER, myID, MessageTypes.MT_EXIT);
                        if (m.Receive(s) == MessageTypes.MT_CONFIRM)
                        {
                            Console.WriteLine("\nOk\n");
                        }
                        else
                        {
                            Console.WriteLine("\nSomething went wrong\n");
                        }

                        disconnect(s);
                        Environment.Exit(0);
                        break;
                    }
                    default:
                    {
                        Console.WriteLine("\nIt's not a correct number. Please, enter 1 or 2.\n");
                        break;
                    }
                }
            }
        }
    }
}
