using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using TouchSocket.Core;
using TouchSocket.Dmtp;
using TouchSocket.Sockets;

namespace TestDmtpClient
{
    internal class Program
    {
        public static void TestDmtpClient()
        {
            var client = new TcpDmtpClient();
            client.Setup(new TouchSocketConfig()
                .SetRemoteIPHost("127.0.0.1:7789")
                .SetDmtpOption(new DmtpOption()
                {
                    VerifyToken = "Dmtp",
                    Id = "test00"
                }));
            client.Connect();
        }

        public static void TestMySelfDmtpClient()
        {
            IPAddress ipAddress = IPAddress.Parse("127.0.0.1");
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, 7789);
            Socket sender = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            sender.Connect(remoteEP);
            Console.WriteLine("Connected to {0}", sender.RemoteEndPoint.ToString());
            var verifyToken = "Dmtp";
            var id = "test01";
            var metadata = new Metadata();
            var waitVerify = new WaitVerify()
            {
                Token = verifyToken,
                Id = id,
                Metadata = metadata
            };
            var bytes = SerializeConvert.JsonSerializeToBytes(waitVerify);
            ushort protocol = 1;
            var transferBytes = new ArraySegment<byte>[] 
            {
                new ArraySegment<byte>(DmtpMessage.Head),
                new ArraySegment<byte>(TouchSocketBitConverter.BigEndian.GetBytes(protocol)),
                new ArraySegment<byte>(TouchSocketBitConverter.BigEndian.GetBytes(bytes.Length)),
                new ArraySegment<byte>(bytes, 0, bytes.Length) 
            };
            int bytesSent = sender.Send(transferBytes);
            Console.WriteLine("Sent {0} bytes to server.", bytesSent);
            // 接收服务器的响应
            byte[] resbytes = new byte[1024];
            int bytesRec = sender.Receive(resbytes);
            Console.WriteLine("Received response: {0}", Encoding.UTF8.GetString(resbytes, 0, bytesRec));




        }

        static void Main(string[] margs)
        {
            TestMySelfDmtpClient();
            while (true) 
            {
                Console.ReadLine();
            }
        }
    }
}
