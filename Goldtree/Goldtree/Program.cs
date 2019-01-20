using Goldtree.Usb;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;

namespace Goldtree
{
    public class Program
    {
        public static void Main(string[] args)
        {
            new WebHostBuilder()
                .UseConfiguration(new ConfigurationBuilder()
                    .AddCommandLine(args)
                    .Build())
                .UseServer(new UsbServer())
                .UseStartup<Startup>()
                .Build()
                .Run();
        }
    }
}
