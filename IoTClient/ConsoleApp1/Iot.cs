using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp1
{
    internal class IotDevice
    {
        private DeviceClient deviceClient;
        private string deviceId;
        public IotDevice()
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

                double currentTemperature = 20 + random.NextDouble() * 15;
                double currentHumidity = 60 + random.NextDouble() * 20;

                var telemetryDataPoint = new
                {
                    messageId = messageId++,
                    deviceId = deviceId,
                    temperature = currentTemperature,
                    humidity = currentHumidity
                };
                string messageString = JsonConvert.SerializeObject(telemetryDataPoint);
                Message message = new Message(Encoding.ASCII.GetBytes(messageString));
                message.Properties.Add("temperatureAlert", (currentTemperature > 30) ? "true" : "false");

                await deviceClient.SendEventAsync(message);
                Console.WriteLine("{0} > Sending message: {1}", DateTime.Now, messageString);

                await Task.Delay(1000);
            }
        }

    }
}
