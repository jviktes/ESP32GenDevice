using Microsoft.Extensions.Hosting;

internal class Program
{
    private static void Main(string[] args)
    {
        var host = new HostBuilder()
    .ConfigureFunctionsWorkerDefaults()
    //.ConfigureLogging(logging =>
    //{
    //    logging.ClearProviders();
    //    logging.AddConsole();
    //}
    //)
    .Build();

        host.Run();
    }
}