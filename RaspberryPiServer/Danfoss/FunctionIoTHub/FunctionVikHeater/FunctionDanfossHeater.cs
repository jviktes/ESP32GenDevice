using System;
using Microsoft.Azure.Amqp.Framing;
using Microsoft.Azure.Devices.Client;
using System.Net;
using System.Text;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using System.Web;
using Microsoft.Azure.Functions.Worker.Http;

namespace FunctionDanfossHeater
{
    public class FunctionDanfossHeater
    {
        private readonly ILogger _logger;
        private static string _clienSecret = "ODI1bkZmUWlUQ21ZU2g5cHJLQXU1YndmRnI1SnNvbGE6R0E0a1pFSU9XV2FVNndRUg==";
        private static string deviceKey = "nvpJZhrBNna42U7OxNHxn5WjvMS/YNlseqbKjYwogKU=";
        private static string deviceId = "TopeniObejvak";
        private static string iotHubHostName = "vikhub.azure-devices.net";

        public FunctionDanfossHeater(ILoggerFactory loggerFactory)
        {
            _logger = loggerFactory.CreateLogger<FunctionDanfossHeater>();
        }

        [Function("GetDanfossData")]
        public async Task<HttpResponseData> Run([HttpTrigger(AuthorizationLevel.Function, "get", "post")] HttpRequestData req)
        {
            _logger.LogInformation($"HeaterFunctionV2 trigger executed at: {DateTime.Now}");

            //var deviceAuthentication = new DeviceAuthenticationWithRegistrySymmetricKey(deviceId, deviceKey);
            //DeviceClient deviceClient = DeviceClient.Create(iotHubHostName, deviceAuthentication, TransportType.Mqtt);

            //string responseString = await ProcessRequestToDanfassAPI();
            //_logger.LogInformation($"HeaterFunctionV2 responseString: {responseString}");


            //string messageString = PrepeareTelemetryData(deviceId, responseString);
            //_logger.LogInformation($"HeaterFunctionV2 messageString: {messageString}");

            //Message message = new Message(Encoding.ASCII.GetBytes(messageString));
            //await deviceClient.SendEventAsync(message);

            _logger.LogInformation("FunctionBulk HTTP trigger function processed a request.");

            var response = req.CreateResponse(HttpStatusCode.OK);
            response.Headers.Add("Content-Type", "text/plain; charset=utf-8");

            response.WriteString("Welcome to Azure Functions!");

            return response;
        }

        //private static string PrepeareTelemetryData(string deviceId, string responseString)
        //{
        //    JObject json = JObject.Parse(responseString);
        //    var temm = json["result"]["status"];

        //    JArray a = (JArray)temm;
        //    string temmperatureHeater = "0";

        //    foreach (var item in a)
        //    {
        //        string tt = item["code"].ToString();
        //        if (tt == "temp_current")
        //        {
        //            temmperatureHeater = item["value"].ToString();
        //        }
        //    }

        //    //Fixing values: 
        //    float innTemmperatureHeater = 0;
        //    if (float.TryParse(temmperatureHeater, out innTemmperatureHeater))
        //    {
        //        innTemmperatureHeater = innTemmperatureHeater / 10;
        //        temmperatureHeater = innTemmperatureHeater.ToString();
        //    }

        //    MessageIoT messageIoT = new MessageIoT();
        //    messageIoT.id = Guid.NewGuid().ToString();
        //    messageIoT.MessageDate = DateTime.Now;
        //    messageIoT.DeviceId = deviceId;

        //    List<IoTLibrary.Data> dataTelemetry = new List<IoTLibrary.Data>()
        //        {
        //            new IoTLibrary.Data() { Name = "temperature", Value = temmperatureHeater },
        //        };

        //    messageIoT.MessageData = new MessageData() { IdMessage = messageIoT.id, DataSet = dataTelemetry };

        //    messageIoT.CategoryName = deviceId;
        //    messageIoT.DeviceData = new DeviceData() { nameDevice = "Danfoss v obyvacim pokoji", DeviceId = deviceId };

        //    string messageString = JsonConvert.SerializeObject(messageIoT);
        //    return messageString;

        //}

        private static async Task<string> ProcessRequestToDanfassAPI()
        {
            var request = (HttpWebRequest)WebRequest.Create("https://api.danfoss.com/ally/devices/bfe1d8f60334b694eelvds");

            request.Method = "GET";
            request.ContentType = "application/x-www-form-urlencoded";
            String accessToken = await GetAccessTokenDanfassAPI();

            request.Headers.Add("Authorization", $"Bearer {accessToken}");

            var response = (HttpWebResponse)request.GetResponse();

            var responseString = new StreamReader(response.GetResponseStream()).ReadToEnd();
            return responseString;
        }

        private static async Task<string> GetAccessTokenDanfassAPI()
        {
            try
            {
                var request = (HttpWebRequest)WebRequest.Create("https://api.danfoss.com/oauth2/token");
                request.Method = "POST";
                request.ContentType = "application/x-www-form-urlencoded";
                request.Headers.Add("Authorization", $"Basic {_clienSecret}");
                string postString = String.Format("grant_type=client_credentials");

                byte[] bytes = Encoding.UTF8.GetBytes(postString);
                using (Stream requestStream = await request.GetRequestStreamAsync())
                {
                    requestStream.Write(bytes, 0, bytes.Length);
                }


                HttpWebResponse httpResponse = (HttpWebResponse)(await request.GetResponseAsync());
                string responseString;
                using (Stream responseStream = httpResponse.GetResponseStream())
                {
                    responseString = new StreamReader(responseStream).ReadToEnd();
                }
                JObject json = JObject.Parse(responseString);

                var _accessToken = json["access_token"];

                return _accessToken.ToString();
            }
            catch (Exception ex)
            {

                throw;
            }
        }

    }

}
