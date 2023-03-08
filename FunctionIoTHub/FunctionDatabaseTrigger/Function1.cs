using System;
using System.Collections.Generic;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;

namespace FunctionDatabaseTrigger
{
    public class Function1
    {
        private readonly ILogger _logger;

        public Function1(ILoggerFactory loggerFactory)
        {
            _logger = loggerFactory.CreateLogger<Function1>();
        }
        //LeaseCollectionName = "leases")] IReadOnlyList<MyDocument> input
        [Function("Function1")]
        public void Run([CosmosDBTrigger(
            databaseName: "DeviceData",
            collectionName: "DeviceDataContainer",
            ConnectionStringSetting = "AzureWebJobsStorage",
           LeaseCollectionName = "leases")] IReadOnlyList<MyDocument> input)
        {
            _logger.LogInformation("Documents modified Function1");

            if (input != null && input.Count > 0)
            {
                _logger.LogInformation("Documents modified: " + input.Count);
                _logger.LogInformation("First document Id: " + input[0].Id);
            }

            //tady projit obsah zpravy--> pro deviceId najit vsechny action_DB
            //ActionDB - co d?lat se zprávou:
            //rozparsovat obsah: pokud nap?. SendEmail --> zavolaní, TriggerEvent, poslat Message

        }
    }

    public class MyDocument
    {
        public string Id { get; set; }

        public string Text { get; set; }

        public int Number { get; set; }

        public bool Boolean { get; set; }
    }
}
