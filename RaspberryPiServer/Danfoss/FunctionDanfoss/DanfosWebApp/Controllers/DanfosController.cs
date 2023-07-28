using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Net;
using System.Security.Principal;
using System.Text;
using System.Text.Json.Serialization;

using System.Collections.Generic;
using System.Text.Json;

namespace DanfosWebApp.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class DanfossController : ControllerBase
    {
   
        private readonly ILogger<DanfossController> _logger;
        private static string _clienSecret = "ODI1bkZmUWlUQ21ZU2g5cHJLQXU1YndmRnI1SnNvbGE6R0E0a1pFSU9XV2FVNndRUg==";
        
        public DanfossController(ILogger<DanfossController> logger)
        {
            _logger = logger;
        }

        [HttpGet]
        public async Task<DanfossData.Result> Get()
        {
            string responseString = await ProcessGetRequestToDanfassAPI();
            DanfossData.Root objectData = PrepeareTelemetryData(responseString);

            //
            //objectData.Result.Status

            Dictionary<string, string> datav2 = new Dictionary<string, string>();
            


            // Add key-value pairs to the list
            //datav2.Add(new KeyValuePair<string, string>("Key1", "Value1"));

            
            foreach (var item in objectData.Result.Status)
            {
                datav2.Add(item.Code, item.Value.ToString());
                //datav2.Add(new KeyValuePair<string, string>(item.Code, item.Value.ToString()));
            }
            
            string json = System.Text.Json.JsonSerializer.Serialize(datav2);
            objectData.Result.Datav2 = json;

            DanfossData.Result res = objectData.Result;
            res.Status = null;

            //var tt = res.Status.Where(x => x.Code == "lower_temp").FirstOrDefault();
            //var trt = tt.Value;
            //zde proberu, co budu chtit:


            return res;
        }

        [HttpPost]
        public async Task<String> Post([FromBody] DanfosInputModel model)
        {
            Console.WriteLine("POST request");
            int _teplota =50;

            try
            {
                //Console.WriteLine(Request.Form.ToString());
                //var tt = await Request.BodyReader.ReadAsync();
                //var tt =await Request.ReadFormAsync();
                var ttt = model.teplota;
                //var re = tt.ToString();
                //var encodedString = Encoding.UTF8.GetString(tt.Buffer., 0, tt.Buffer.Length);
                //var value = Request.Form["teplota"]; //httpContext.Request.Form["teplota"];
                int.TryParse(ttt, out _teplota);
            
            }
            catch (Exception ex)
            {

                Console.WriteLine(ex);
                throw;
            };
            

            Console.WriteLine("Teplota");
            Console.WriteLine(_teplota);
            string responseString = await ProcessPostRequestToDanfassAPI(_teplota);
            return responseString;
        }

        private static DanfossData.Root PrepeareTelemetryData(string responseString)
        {
            DanfossData.Root json  = JsonConvert.DeserializeObject<DanfossData.Root>(responseString); //JObject.Parse(responseString);
            return json;

            //var temm = json["result"]["status"];

            //JArray a = (JArray)temm;
            //string temmperatureHeater = "0";

            //foreach (var item in a)
            //{
            //    string tt = item["code"].ToString();
            //    if (tt == "temp_current")
            //    {
            //        temmperatureHeater = item["value"].ToString();
            //    }
            //}

            ////Fixing values: 
            //float innTemmperatureHeater = 0;
            //if (float.TryParse(temmperatureHeater, out innTemmperatureHeater))
            //{
            //    innTemmperatureHeater = innTemmperatureHeater / 10;
            //    temmperatureHeater = innTemmperatureHeater.ToString();
            //}

            //MessageIoT messageIoT = new MessageIoT();
            //messageIoT.id = Guid.NewGuid().ToString();
            //messageIoT.MessageDate = DateTime.Now;
            //messageIoT.DeviceId = deviceId;

            //List<IoTLibrary.Data> dataTelemetry = new List<IoTLibrary.Data>()
            //    {
            //        new IoTLibrary.Data() { Name = "temperature", Value = temmperatureHeater },
            //    };

            //messageIoT.MessageData = new MessageData() { IdMessage = messageIoT.id, DataSet = dataTelemetry };

            //messageIoT.CategoryName = deviceId;
            //messageIoT.DeviceData = new DeviceData() { nameDevice = "Danfoss v obyvacim pokoji", DeviceId = deviceId };

            //string messageString = JsonConvert.SerializeObject(messageIoT);
            //return messageString;

        }

        private static async Task<string> ProcessGetRequestToDanfassAPI()
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

        private static async Task<string> ProcessPostRequestToDanfassAPI(int teplota)
        {
            string responseFromServer = "";
            var request = (HttpWebRequest)WebRequest.Create("https://api.danfoss.com/ally/devices/bfe1d8f60334b694eelvds/commands");

            String accessToken = await GetAccessTokenDanfassAPI();
            Console.WriteLine(accessToken);
            request.Headers.Add("Authorization", $"Bearer {accessToken}");
            request.ContentType = "application/json";
            request.Method = "POST";


            //string postString = "{\r\n  \"commands\": [\r\n    {\r\n      \"code\": \"temp_set\",\r\n      \"value\": 80\r\n    }\r\n  ]\r\n}";

            DanfossCommand.Root danfossCommand = new DanfossCommand.Root();
            danfossCommand.commands = new List<DanfossCommand.Command>();
            danfossCommand.commands.Add(new DanfossCommand.Command() { code = "temp_set", value = teplota });
            JsonSerializerSettings jsonSettings = new JsonSerializerSettings
            {
                Formatting = Formatting.Indented
            };
            string postString = Newtonsoft.Json.JsonConvert.SerializeObject(danfossCommand, jsonSettings);

            Console.WriteLine("poststring");
            Console.WriteLine(postString);

            try
            {
                byte[] bArray = Encoding.UTF8.GetBytes(postString);
                
                request.ContentLength = bArray.Length;
                Stream webData = request.GetRequestStream();
                webData.Write(bArray, 0, bArray.Length);
                webData.Close();

                WebResponse webResponse = request.GetResponse();

                Console.WriteLine(((HttpWebResponse)webResponse).StatusCode);
                Console.WriteLine(((HttpWebResponse)webResponse).StatusDescription);

                webData = webResponse.GetResponseStream();
                StreamReader reader = new StreamReader(webData);
                responseFromServer = reader.ReadToEnd();
                Console.WriteLine(responseFromServer);
                reader.Close();
                webData.Close();
                webResponse.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                throw;
            }

            return responseFromServer;

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


        [HttpGet("TestData")]
        public IEnumerable<WeatherForecast> GetWeather()
        {
            return Enumerable.Range(1, 5).Select(index => new WeatherForecast
            {
                Date = DateOnly.FromDateTime(DateTime.Now.AddDays(index)),
                TemperatureC = Random.Shared.Next(-20, 55),
            })
            .ToArray();
        }

    }
}