using Goldtree.Lib;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Http.Features;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Goldtree.Usb
{
    internal class UsbServer : IServer
    {
        private readonly IUsb usb;
        private readonly RequestBuilder requestBuilder;

        public IFeatureCollection Features => new FeatureCollection();

        public UsbServer()
        {
            usb = new Lib.UsbK.Usb();
            requestBuilder = new RequestBuilder(usb);
        }

        public async Task StartAsync<TContext>(IHttpApplication<TContext> application, CancellationToken cancellationToken)
        {
            await WaitForConnection(cancellationToken);

            while(!cancellationToken.IsCancellationRequested)
            {
                FeatureCollection features = new FeatureCollection();

                features.Set<IHttpRequestFeature>(requestBuilder.Build());
                features.Set<IHttpResponseFeature>(new ResponseFeature(usb));

                TContext context = application.CreateContext(features);
                await application.ProcessRequestAsync(context);
                application.DisposeContext(context, null);
            }
        }

        public Task StopAsync(CancellationToken cancellationToken) => Task.CompletedTask;
        public void Dispose() { }

        private Task WaitForConnection(CancellationToken cancellationToken = default)
        {
            return Task.Run(() =>
            {
                while (cancellationToken != default && !cancellationToken.IsCancellationRequested)
                {
                    try
                    {
                        usb.Connect(0x057E, 0x3000);
                        return;
                    }
                    catch { }
                }
            });
        }
    }
}
