using Azure.Identity;
using FunctionServiceBus;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using System.Net;

internal class Program
{
    private static void Main(string[] args)
    {
        var host = new HostBuilder()
        .ConfigureFunctionsWorkerDefaults()
        .ConfigureServices((ctx, services) =>
        {
            services.AddSingleton<IDatabaseService>((s) =>
            {
                //var cred = new DefaultAzureCredential(false);
                var endpoint = "https://cosmodbvik.documents.azure.com:443/";//ctx.Configuration.GetValue<string>("DB-ENDPOINT");
                var key = "mKeThz76XaBf3nlggKpAGcYxD3ZStG3EeZujqGXrLCiWeK2tMJ59ZopcbJLhaTHLuTSQ57WL3HkaACDbJJsiuQ==";
                return new DatabaseService(endpoint, key, "devicesDB", "devicesDB");
            });

        })
        .Build();
            host.Run();
        }
}