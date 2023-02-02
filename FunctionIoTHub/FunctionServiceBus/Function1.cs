using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Linq;
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
        private readonly IDatabaseService _databaseService;

        public Function1(ILoggerFactory loggerFactory, IDatabaseService databaseService)
        {
            _logger = loggerFactory.CreateLogger<Function1>();
            _databaseService = databaseService;
          
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
            _logger.LogInformation($"C# 2.0 ServiceBus queue trigger function processed message: {myQueueItem}");


            if (myQueueItem.Contains("messageId") && myQueueItem.Contains("Temperature") && myQueueItem.Contains("Humidity"))
            {

                try
                {

                    JObject jsonMessage = JObject.Parse(myQueueItem);

                    String deviceId = "esp32Temperature";
                    String temperature = jsonMessage["Temperature"].ToString();
                    String humidity = jsonMessage["Humidity"].ToString();
                    String categoryId = "devices";//"61dba35b-4f02-45c5-b648-c6badc0cbd79";
                    String categoryName = "temperature";

                    double _temperature = 0;
                    double.TryParse(temperature, out _temperature);

                    double _humidity = 0;
                    double.TryParse(humidity, out _humidity);

                    DateTime _eventTime = DateTime.Now;
                    //DateTime.TryParse(eventString, out _eventTime);

                    DeviceData deviceData = new DeviceData() { id = Guid.NewGuid().ToString(), nameDevice = deviceId, devicesData = categoryId, categoryName = categoryName, temperature = _temperature,huminidy=_humidity, eventTime = _eventTime };
                    await _databaseService.InsertDeviceData(deviceData);
                }
                catch (Exception ex)
                {
                    _logger.LogInformation($"exception: {ex.Message}");
                    //throw;
                }

            }

         

            if (myQueueItem.Contains("TopeniObejvak"))
            {

                try
                {

                    String categoryId = "devices";//"61dba35b-4f02-45c5-b648-c6badc0cbd79";
                    String categoryName = "temperature";
                    DeviceData _deviceData = ParseData(myQueueItem, categoryId, categoryName);
                    await _databaseService.InsertDeviceData(_deviceData);
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
        public double temperature { get; set; }
        public double huminidy { get; set; }
        public DateTime eventTime { get; set; }
    }

}
