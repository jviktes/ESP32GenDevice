using IoTLibrary;
using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp1
{
    internal class IoTMessage2
    {
        private DeviceClient deviceClient;
        private string deviceId;
        public IoTMessage2()
        {
            string deviceKey = "yzam5Dj52O+8cAIHQlvnJ1GKAtgc/9CIXLv4VNtvtg8=";
            deviceId = "nodeJsTestPC";
            string iotHubHostName = "vikhub.azure-devices.net";
            var deviceAuthentication = new DeviceAuthenticationWithRegistrySymmetricKey(deviceId, deviceKey);
            deviceClient = DeviceClient.Create(iotHubHostName, deviceAuthentication, TransportType.Mqtt);
            StartLoops();
        }
        public async void StartLoops()
        {
            Task<bool> t1 = ReceivingLoop();
            Task<bool> t2 = SendingDataLoop();
            await Task.WhenAll(t1, t2);

        }

        public async Task<bool> ReceivingLoop()
        {
            while (true)
            {
                Message receivedMessage = await deviceClient.ReceiveAsync();
                if (receivedMessage == null) continue;

                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Received message: {0}",
                Encoding.ASCII.GetString(receivedMessage.GetBytes()));
                Console.ResetColor();

                await deviceClient.CompleteAsync(receivedMessage);
            }
        }

        public async Task<bool> SendingDataLoop()
        {
            Random random = new Random();
            int messageId = 0;
            while (true)
            {

                MessageIoT messageIoT = new MessageIoT();
                messageIoT.id = Guid.NewGuid().ToString();
                messageIoT.MessageDate = DateTime.Now;
                messageIoT.DeviceId = deviceId;

                List<Data> dataTelemetry = new List<Data>()
                {
                    new Data() { Name = "huminidy", Value = "99" },
                    new Data() { Name = "light", Value = "1025" },
                    new Data() { Name = "pressure", Value = "87" },
                    new Data() { Name = "temperature", Value = "25.58" },
                    new Data() { Name = "isValid", Value = "OK" },
                };

                messageIoT.MessageData = new MessageData() { IdMessage = messageIoT.id, DataSet= dataTelemetry };

                messageIoT.CategoryName = deviceId;

                List<Data> dataDevice = new List<Data>()
                {
                    new Data() { Name = "voltage", Value = "99" },
                    new Data() { Name = "batery", Value = "1025" },
                    new Data() { Name = "version", Value = "87.4578" },

                };

                messageIoT.DeviceData = new DeviceData() { DataSet= dataDevice, nameDevice = "node js testovaci cidlo", DeviceId= deviceId };

                string messageString = JsonConvert.SerializeObject(messageIoT);
                Message message = new Message(Encoding.ASCII.GetBytes(messageString));

                await deviceClient.SendEventAsync(message);
                Console.WriteLine("{0} > Sending message: {1}", DateTime.Now, messageString);

                await Task.Delay(1000);
            }
        }



    }
}
