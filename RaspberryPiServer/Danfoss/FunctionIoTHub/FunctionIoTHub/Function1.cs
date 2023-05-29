using System;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Azure.WebJobs;
using Microsoft.Extensions.Logging;

namespace FunctionIoTHub
{
    public class Function1
    {
        private readonly ILogger _logger;

        public Function1(ILoggerFactory loggerFactory)
        {
            _logger = loggerFactory.CreateLogger<Function1>();
        }

        [FunctionName("RawTelemetryFunction")]
        [StorageAccount("DeadLetterStorage")]
        public static async Task RunAsync(
    [EventHubTrigger("iothub-ehub-vikhub-23500461-0926dbeb43", Connection = "HostName=vikhub.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=qcp+VnvpqkwUKbfgShY+/TbzjcyC0DOyZBIoExLAOGk=\"", ConsumerGroup = "%EventHubConsumerGroup%")] EventData[] messages,
    [Queue("deadletterqueue")] IAsyncCollector<DeadLetterMessage> deadLetterMessages,
    ILogger logger)
        {
            foreach (var message in messages)
            {
                DeviceState deviceState = null;

                try
                {
                    deviceState = telemetryProcessor.Deserialize(message.Body.Array, logger);
                }
                catch (Exception ex)
                {
                    logger.LogError(ex, "Error deserializing message", message.SystemProperties.PartitionKey, message.SystemProperties.SequenceNumber);
                    await deadLetterMessages.AddAsync(new DeadLetterMessage { Issue = ex.Message, EventData = message });
                }

                try
                {
                    await stateChangeProcessor.UpdateState(deviceState, logger);
                }
                catch (Exception ex)
                {
                    logger.LogError(ex, "Error updating status document", deviceState);
                    await deadLetterMessages.AddAsync(new DeadLetterMessage { Issue = ex.Message, EventData = message, DeviceState = deviceState });
                }
            }
        }

    }
    public class DeadLetterMessage
    {
        public string Issue { get; set; }
        public EventData EventData { get; set; }
        public DeviceState DeviceState { get; set; }
    }
}
