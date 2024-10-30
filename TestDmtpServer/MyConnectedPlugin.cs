using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TouchSocket.Core;
using TouchSocket.Sockets;

namespace TestDmtpServer
{
    public class MyConnectedPlugin : PluginBase, ITcpConnectedPlugin
    {
        public Task OnTcpConnected(ITcpClientBase client, ConnectedEventArgs e)
        {
            ConsoleLogger.Default.Info($"Client Connected from {client.IP}");
            return e.InvokeNext();
        }
    }
}
