// See https://aka.ms/new-console-template for more information

using IoTLibrary;
using Newtonsoft.Json;
using System.Text.Json;
using System.Text.Json.Serialization;

String inputMessage = "{\"id\":\"836a602f-fb73-45a5-9b18-e5d9ad8ca0a8\",\"MessageData\":{\"IdMessage\":\"836a602f-fb73-45a5-9b18-e5d9ad8ca0a8\",\"DataSet\":[{\"Name\":\"temperature\",\"Value\":\"240\"}]},\"DeviceId\":\"TopeniObejvak\",\"DeviceData\":{\"nameDevice\":\"Danfoss v obyvacim pokoji\",\"DeviceId\":\"TopeniObejvak\",\"DataSet\":null},\"CategoryName\":\"TopeniObejvak\",\"MessageDate\":\"2023-02-26T18:34:01.3171195+00:00\"}";

MessageIoT? messageIoT;

try
{

    //Unexpected character encountered while parsing value
    var options = new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Include };
    messageIoT = JsonConvert.DeserializeObject<MessageIoT>(inputMessage, options);

}
catch(Exception ex)
{

}

try
{
    MessageIoT? weatherForecast = System.Text.Json.JsonSerializer.Deserialize<MessageIoT>(inputMessage);
}
catch (Exception ex)
{

    
}


Console.WriteLine("Hello, World!");

