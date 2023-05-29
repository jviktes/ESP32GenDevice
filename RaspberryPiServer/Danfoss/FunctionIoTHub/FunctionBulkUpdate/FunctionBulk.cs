using System.Net;
using IoTServices;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Azure.Functions.Worker.Http;
using Microsoft.Extensions.Logging;

namespace FunctionBulkUpdate
{
    public class FunctionBulk
    {
        private readonly ILogger _logger;
        private readonly IDatabaseService _databaseService;
        public FunctionBulk(ILoggerFactory loggerFactory, IDatabaseService databaseService)
        {
            _logger = loggerFactory.CreateLogger<FunctionBulk>();
            _databaseService= databaseService;
    }

        [Function("FunctionBulk")]
        public async Task<HttpResponseData> Run([HttpTrigger(AuthorizationLevel.Function, "get", "post")] HttpRequestData req)
        {
            _logger.LogInformation("FunctionBulk HTTP trigger function processed a request.");

            await _databaseService.GetAndUpdateESPData();

            var response = req.CreateResponse(HttpStatusCode.OK);
            response.Headers.Add("Content-Type", "text/plain; charset=utf-8");

            response.WriteString("Welcome to Azure Functions!");

            return response;
        }
    }
}
