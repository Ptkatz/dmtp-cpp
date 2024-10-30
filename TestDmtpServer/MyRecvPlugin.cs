using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TouchSocket.Core;
using TouchSocket.Dmtp;

namespace TestDmtpServer
{
    internal class MyRecvPlugin : PluginBase, IDmtpReceivedPlugin
    {
        public async Task OnDmtpReceived(IDmtpActorObject client, DmtpMessageEventArgs e)
        {
            ConsoleLogger.Default.Info($"DmtpMessage Protocol Flag: {e.DmtpMessage.ProtocolFlags}");
            await e.InvokeNext();
        }
    }
}
