using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TouchSocket.Core;
using TouchSocket.Dmtp;

namespace TestDmtpServer
{
    internal class MyHandleshakingPlugin : PluginBase, IDmtpHandshakingPlugin
    {
        public async Task OnDmtpHandshaking(IDmtpActorObject client, DmtpVerifyEventArgs e)
        {
            ConsoleLogger.Default.Info($"Client [{client.DmtpActor.Id}] handleshaking");
            await e.InvokeNext();
        }
    }
}
