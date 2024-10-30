using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TouchSocket.Sockets;
using TouchSocket.Dmtp;
using TouchSocket.Core;

namespace TestDmtpServer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var service = new TcpDmtpService();
            var config = new TouchSocketConfig()//配置
                   .SetListenIPHosts(7789)
                   .ConfigureContainer(a =>
                   {
                       a.AddConsoleLogger();
                   })
                   .ConfigurePlugins(a => 
                   {
                       a.Add<MyConnectedPlugin>();
                       a.Add<MyHandleshakingPlugin>();
                       a.Add<MyHandleshakedPlugin>();
                       a.Add<MyRecvPlugin>();
                   })
                   .SetDmtpOption(new DmtpOption()
                   {
                       VerifyToken = "Dmtp"//设定连接口令，作用类似账号密码
                   });

            service.SetupAsync(config).Wait();

            service.StartAsync().Wait();

            service.Logger.Info($"{service.GetType().Name}已启动");
            while (true)
            {
                Console.ReadLine();
            }
        }
    }
}
