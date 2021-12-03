using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace SocketClientSharp
{
    class Program
    {
        public static void Main(string[] args)
        {
            Client client = new Client();
            client.Start();
        }
    }
}
