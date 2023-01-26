using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Net.Http.Headers;
using Azure;
using Azure.Storage.Blobs;
using Microsoft.Azure.Cosmos;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json.Linq;

namespace FunctionServiceBus
{
    public class Function1
    {
        private readonly ILogger _logger;

        public Function1(ILoggerFactory loggerFactory)
        {
            _logger = loggerFactory.CreateLogger<Function1>();
        }

        [Function("Function1")]
        public async Task Run(
        [ServiceBusTrigger("devices", Connection = "ServiceBusConnectionString")]
        string myQueueItem,
        Int32 deliveryCount,
        DateTime enqueuedTimeUtc,
        string messageId,
        ILogger log)
        {
            _logger.LogInformation($"C# ServiceBus queue trigger function processed message: {myQueueItem}");


            if (myQueueItem.Contains("TopeniObejvak"))
            {

                try
                {
                    using CosmosClient client = new CosmosClient("https://cosmodbvik.documents.azure.com:443/", "mKeThz76XaBf3nlggKpAGcYxD3ZStG3EeZujqGXrLCiWeK2tMJ59ZopcbJLhaTHLuTSQ57WL3HkaACDbJJsiuQ==");
                    Database database = await client.CreateDatabaseIfNotExistsAsync("devicesDB");

                    Microsoft.Azure.Cosmos.Container container = await database.CreateContainerIfNotExistsAsync(
                        id: "devicesDB",
                        partitionKeyPath: "/devicesData",
                        //partitionKeyPath: "/categoryId",
                        throughput: 400
                    );

                    //DeviceData newData = new(
                    //categoryId: "61dba35b-4f02-45c5-b648-c6badc0cbd79",
                    //categoryName: "temperature",
                    //nameDevice: "TopeniObejvak",
                    //temperature:250,
                    //eventTime: new DateTime()
                    //);
                    String categoryId = "61dba35b-4f02-45c5-b648-c6badc0cbd79";//"61dba35b-4f02-45c5-b648-c6badc0cbd79";
                    String categoryName = "temperature";

                    DeviceData _deviceData = ParseData(myQueueItem, categoryId, categoryName);
                    var createdItem = await container.CreateItemAsync<DeviceData>(
                        item: _deviceData,
                        partitionKey: new PartitionKey("61dba35b-4f02-45c5-b648-c6badc0cbd79")
                    );
                }
                catch (Exception ex)
                {
                    _logger.LogInformation($"exception: {ex.Message}");
                    //throw;
                }
            }


        }
        public static DeviceData ParseData(string rawData, string categoryId, string categoryName)
        {

            //String stringToRaplace = "C# ServiceBus queue trigger function processed message:";

            //JObject jsonComplete = JObject.Parse(rawData);
            //var rawMessage = jsonComplete["message"].ToString();
            //String eventString = jsonComplete["time"].ToString();
            //string messageString = rawMessage.Replace(stringToRaplace, "");
            JObject jsonMessage = JObject.Parse(rawData);

            String deviceId = jsonMessage["deviceId"].ToString();
            String temperature = jsonMessage["temperature"].ToString();

            int _temperature = 0;
            int.TryParse(temperature, out  _temperature);
            DateTime _eventTime = DateTime.Now;
            //DateTime.TryParse(eventString, out _eventTime);

            DeviceData deviceData = new DeviceData(){ id=Guid.NewGuid().ToString(), nameDevice = deviceId, devicesData = categoryId , categoryName= categoryName, temperature= _temperature, eventTime= _eventTime };
            return deviceData;
        }
    }



    public class DeviceData
    {
        public string id { get; set; }
        public string devicesData { get; set; }
        public string categoryName { get; set; }
        public string nameDevice { get; set; }
        public int temperature { get; set; }
        public DateTime eventTime { get; set; }
    }

}
