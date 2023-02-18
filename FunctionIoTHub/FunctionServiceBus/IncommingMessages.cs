using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Linq;
using System.Net.Http.Headers;
//using System.Text.Json;
using Azure;
using Azure.Storage.Blobs;
using IoTLibrary;
using Microsoft.Azure.Cosmos;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using IoTServices;

namespace FunctionServiceBus
{
    public class IncommingMessages
    {
        private readonly ILogger _logger;
        private readonly IDatabaseService _databaseService;

        public IncommingMessages(ILoggerFactory loggerFactory, IDatabaseService databaseService)
        {
            _logger = loggerFactory.CreateLogger<IncommingMessages>();
            _databaseService = databaseService;
          
        }

        [Function("IncommingMessages")]
        public async Task Run(
        [ServiceBusTrigger("devices", Connection = "ServiceBusConnectionString")]
        string myQueueItem,
        Int32 deliveryCount,
        DateTime enqueuedTimeUtc,
        string messageId,
        ILogger log)
        {
            _logger.LogInformation($"IncommingMessages v3.0 ServiceBus queue trigger function processed message: {myQueueItem}");
            //var tt = JsonSerializer.Deserialize<object>(myQueueItem);
            MessageIoT? messageIoT;
            
            try
            {
                //messageIoT = JsonSerializer.Deserialize<MessageIoT>(myQueueItem);

                //var sz = JsonConvert.SerializeObject(sampleGroupInstance);

                messageIoT=JsonConvert.DeserializeObject<MessageIoT>(myQueueItem);

                //musi se podarit na parsovat:
                if ((messageIoT.DeviceId!=null) || (messageIoT.id!=null)) {
                    await _databaseService.InsertDeviceData(messageIoT);
                }
                else
                {
                    return;
                }
            }
            catch (Exception ex)
            {
                return;
                //throw;
            }

            //if (myQueueItem.Contains("messageId") && myQueueItem.Contains("Temperature") && myQueueItem.Contains("Humidity"))
            //{

            //    try
            //    {

            //        JObject jsonMessage = JObject.Parse(myQueueItem);

            //        //String deviceId = "esp32Temperature";
            //        String deviceId = jsonMessage["deviceId"].ToString();

            //        String temperature = jsonMessage["Temperature"].ToString();
            //        String humidity = jsonMessage["Humidity"].ToString();
            //        String light = jsonMessage["Light"].ToString();
            //        String temperatureBMP = jsonMessage["TemperatureBMP"].ToString();
            //        String pressure = jsonMessage["Pressure"].ToString();

            //        String categoryId = "devices";//"61dba35b-4f02-45c5-b648-c6badc0cbd79";
            //        String categoryName = "temperature";

            //        double _temperature = 0;
            //        double.TryParse(temperature, out _temperature);

            //        double _humidity = 0;
            //        double.TryParse(humidity, out _humidity);

            //        double _light = 0;
            //        double.TryParse(light, out _light);

            //        double _temperatureBMP = 0;
            //        double.TryParse(temperatureBMP, out _temperatureBMP);

            //        double _pressure = 0;
            //        double.TryParse(pressure, out _pressure);

            //        DateTime _eventTime = DateTime.Now;
            //        //DateTime.TryParse(eventString, out _eventTime);

            //        //database: devicesData

            //        MessageIoT deviceData = new MessageIoT() { IdMessage = Guid.NewGuid().ToString(),
            //            nameDevice = deviceId, devicesData = categoryId, CategoryName = categoryName, 
            //            temperature = _temperature,
            //            huminidy=_humidity,
            //            light = _light,
            //            temperatureBMP = _temperatureBMP,
            //            pressure = _pressure,
            //            MessageDate = _eventTime };
            //        await _databaseService.InsertDeviceData(deviceData);
            //    }
            //    catch (Exception ex)
            //    {
            //        _logger.LogInformation($"exception: {ex.Message}");
            //        //throw;
            //    }

            //}

         

            //if (myQueueItem.Contains("TopeniObejvak"))
            //{

            //    try
            //    {

            //        String categoryId = "devices";//"61dba35b-4f02-45c5-b648-c6badc0cbd79";
            //        String categoryName = "temperature";
            //        MessageIoT _deviceData = ParseData(myQueueItem, categoryId, categoryName);
            //        await _databaseService.InsertDeviceData(_deviceData);
            //    }
            //    catch (Exception ex)
            //    {
            //        _logger.LogInformation($"exception: {ex.Message}");
            //        //throw;
            //    }
            //}


        }
        //public static MessageIoT ParseData(string rawData, string categoryId, string categoryName)
        //{

        //    //String stringToRaplace = "C# ServiceBus queue trigger function processed message:";

        //    //JObject jsonComplete = JObject.Parse(rawData);
        //    //var rawMessage = jsonComplete["message"].ToString();
        //    //String eventString = jsonComplete["time"].ToString();
        //    //string messageString = rawMessage.Replace(stringToRaplace, "");
        //    JObject jsonMessage = JObject.Parse(rawData);

        //    String deviceId = jsonMessage["deviceId"].ToString();
        //    String temperature = jsonMessage["temperature"].ToString();

        //    int _temperature = 0;
        //    int.TryParse(temperature, out  _temperature);
        //    DateTime _eventTime = DateTime.Now;
        //    //DateTime.TryParse(eventString, out _eventTime);

        //    MessageIoT deviceData = new MessageIoT(){ IdMessage=Guid.NewGuid().ToString(), nameDevice = deviceId, 
        //        devicesData = categoryId , CategoryName= categoryName, 
        //        temperature= _temperature,
        //        MessageDate= _eventTime };
        //    return deviceData;
        //}
    }



}
