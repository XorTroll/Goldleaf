using Goldtree.Usb;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;

namespace Goldtree
{
    internal class Startup
    {
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<NSPRepository>();

            services.AddMvcCore(options =>
            {
                options.OutputFormatters.Insert(0, new UsbOutputFormatter());
            }).SetCompatibilityVersion(CompatibilityVersion.Latest);
        }
        public void Configure(IApplicationBuilder app)
        {
            app.UseMvc();
        }
    }
}