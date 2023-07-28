using Newtonsoft.Json;

namespace DanfosWebApp
{
    public class DanfossData
    {

        

        // Root myDeserializedClass = JsonConvert.DeserializeObject<Root>(myJsonResponse);
        public class Result
        {
            public String Datav2 { get; set; }

            //public Dictionary<string, string> Datav2 { get; set; }

            [JsonProperty("active_time")]
            public int ActiveTime { get; set; }

            [JsonProperty("create_time")]
            public int CreateTime { get; set; }

            [JsonProperty("id")]
            public string Id { get; set; }

            [JsonProperty("name")]
            public string Name { get; set; }

            [JsonProperty("online")]
            public bool Online { get; set; }

            [JsonProperty("status")]
            public List<Status> Status { get; set; }

            [JsonProperty("sub")]
            public bool Sub { get; set; }

            [JsonProperty("time_zone")]
            public string TimeZone { get; set; }

            [JsonProperty("update_time")]
            public int UpdateTime { get; set; }

            [JsonProperty("device_type")]
            public string DeviceType { get; set; }
        }

        public class Root
        {
            [JsonProperty("result")]
            public Result Result { get; set; }

            [JsonProperty("t")]
            public long T { get; set; }

            
        }

        public class Status
        {
            [JsonProperty("code")]
            public string Code { get; set; }

            [JsonProperty("value")]
            public object Value { get; set; }
        }


    }


    // Root myDeserializedClass = JsonConvert.DeserializeObject<Root>(myJsonResponse);
    public class DanfossCommand
    {

        public class Command
        {
            public string code { get; set; }
            public int value { get; set; }
        }

        public class Root
        {
            public List<Command> commands { get; set; }
        }
    }

    public class DanfosInputModel
    {
        [JsonProperty("teplota")]
        public string teplota { get; set; }

    }
}
