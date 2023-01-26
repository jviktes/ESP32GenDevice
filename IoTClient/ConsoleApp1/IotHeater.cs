using Microsoft.Azure.Amqp;
using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp1
{
    internal class IotHeater
    {
        private DeviceClient deviceClient;
        private string deviceId;
        public IotHeater()
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

                

                var request = (HttpWebRequest)WebRequest.Create("https://api.danfoss.com/ally/devices/bfe1d8f60334b694eelvds");

                //var postData = "thing1=" + Uri.EscapeDataString("hello");
                //postData += "&thing2=" + Uri.EscapeDataString("world");
                //var data = Encoding.ASCII.GetBytes(postData);

                request.Method = "GET";
                request.ContentType = "application/x-www-form-urlencoded";
                //request.ContentLength = data.Length;

                //using (var stream = request.GetRequestStream())
                //{
                //    stream.Write(data, 0, data.Length);
                //}

                request.Headers.Add("Authorization", "Bearer MoK6N6V5E43hJaGbp4WxzkEaY3sa");

                var response = (HttpWebResponse)request.GetResponse();

                var responseString = new StreamReader(response.GetResponseStream()).ReadToEnd();
                string messageStringRawData = responseString;

                JObject json = JObject.Parse(messageStringRawData);

                var temm = json["result"]["status"];

                JArray a = (JArray)temm;
                string temmp = "0";
                foreach (var item in a)
                {
                   string tt =  item["code"].ToString();
                   if (tt== "temp_current")
                    {
                        temmp = item["value"].ToString();
                    }
                }

                var telemetryDataPoint = new
                {
                    messageId = messageId++,
                    deviceId = deviceId,
                    temperature = temmp,
                };
                string messageString = JsonConvert.SerializeObject(telemetryDataPoint);

                //IList<Person> person = a.ToObject<IList<Person>>();

                Message message = new Message(Encoding.ASCII.GetBytes(messageString));

                //message.Properties.Add("temperatureAlert", (currentTemperature > 30) ? "true" : "false");

                await deviceClient.SendEventAsync(message);
                Console.WriteLine("{0} > Sending message: {1}", DateTime.Now, messageString);

               await Task.Delay(30000);
            }
        }

    }
}
