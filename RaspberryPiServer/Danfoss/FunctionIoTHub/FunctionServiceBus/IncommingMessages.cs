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
using System.Text.Json.Serialization;
using System.Text.Json;
using static System.Net.Mime.MediaTypeNames;
using System.Security.Cryptography;

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
            
            MessageIoT? messageIoT;
            
            try
            {

                //Unexpected character encountered while parsing value
                var options = new JsonSerializerSettings { NullValueHandling= NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Include};
                messageIoT =JsonConvert.DeserializeObject<MessageIoT>(myQueueItem, options);

                //musi se podarit na parsovat:
                if ((messageIoT.DeviceId!=null) || (messageIoT.id!=null))
                {
                    //fixing:
                    if (messageIoT.MessageDate == DateTime.MinValue)
                    {
                        messageIoT.MessageDate = DateTime.UtcNow;
                    }

                    ValidationPartionKey(messageIoT);
                   
                    //


                    await _databaseService.InsertDeviceData(messageIoT);
                }
                else
                {
                    return;
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "IncommingMessages error");
                return;
                //throw;
            }

        }



        private static void ValidationPartionKey(MessageIoT? messageIoT)
        {
            //must not be used in Id
            char[] invalidIdChars = new char[] { ',', '/', '#', '?' };
            //limits of CosmosDB
            if (string.IsNullOrWhiteSpace(messageIoT.DeviceId))
                throw new ArgumentException($"{nameof(messageIoT.DeviceId)} parameter cannot be whitespace");
            if (messageIoT.DeviceId.IndexOfAny(invalidIdChars) > -1)
                throw new ArgumentException($"{nameof(messageIoT.DeviceId)} parameter contains invalid character");
            if (messageIoT.DeviceId.EndsWith(" "))
                throw new ArgumentException($"{nameof(messageIoT.DeviceId)} parameter must not end with space");
            if (messageIoT.DeviceId.Length > 255)
                throw new ArgumentException($"{nameof(messageIoT.DeviceId)} parameter is too long");
        }
    }
}
