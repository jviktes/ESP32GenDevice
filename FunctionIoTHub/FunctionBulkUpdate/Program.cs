using IoTServices;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

var host = new HostBuilder()
    .ConfigureFunctionsWorkerDefaults()
            .ConfigureServices((ctx, services) =>
            {
                services.AddSingleton<IDatabaseService>((s) =>
                {
                    //var cred = new DefaultAzureCredential(false);
                    var endpoint = "https://iotdbvik.documents.azure.com:443/";//ctx.Configuration.GetValue<string>("DB-ENDPOINT");
                    var key = "35yQrPGdaMwwR1lDyEN8IneM494grqHgoEAHYTv9qkmEC0KVg921IAlMYHg7ii1t5bXybfzM2xwiACDbZS3Pcw==";
                    return new DatabaseService(endpoint, key, "DeviceData", "DeviceDataContainer");
                });

            })
    .Build();

host.Run();