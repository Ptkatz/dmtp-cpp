using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TouchSocket.Core;
using TouchSocket.Dmtp;

namespace TestDmtpServer
{
    public class MyHandleshakedPlugin : PluginBase, IDmtpHandshakedPlugin
    {
        public async Task OnDmtpHandshaked(IDmtpActorObject client, DmtpVerifyEventArgs e)
        {
            ConsoleLogger.Default.Info($"Client [{client.DmtpActor.Id}] handleshaked");
            await e.InvokeNext();
        }
    }
}
