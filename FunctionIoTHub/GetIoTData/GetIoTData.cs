using System.Collections.Generic;
using System.Net;
using IoTLibrary;
using IoTServices;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Azure.Functions.Worker.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace GetIoTData
{
    public class GetIoTData
    {
        private readonly ILogger _logger;
        private readonly IDatabaseService _databaseService;
        public GetIoTData(ILoggerFactory loggerFactory, IDatabaseService databaseService)
        {
            _logger = loggerFactory.CreateLogger<GetIoTData>();
            _databaseService = databaseService;
        }

        [Function("GetIoTData")]
        public async Task<HttpResponseData> Run([HttpTrigger(AuthorizationLevel.Function, "get", "post")] HttpRequestData req)
        {
            _logger.LogInformation("GetIoTData HTTP trigger function processed a request.");

            var response = req.CreateResponse(HttpStatusCode.OK);
            response.Headers.Add("Content-Type", "application/json; charset=utf-8");
            response.Headers.Add("Access-Control-Allow-Origin", "*");
            var query = System.Web.HttpUtility.ParseQueryString(req.Url.Query);
            _logger.LogInformation(query.ToString());
            var data = await _databaseService.GetData();

            String _data = JsonConvert.SerializeObject(data);
            //response.WriteString("Welcome to Azure Functions!");
            response.WriteString(_data);
            return response;
        }
    }
}
