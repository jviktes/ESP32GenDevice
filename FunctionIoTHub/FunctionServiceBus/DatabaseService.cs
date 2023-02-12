using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net;
using System.Threading.Tasks;
using Azure.Core;
using IoTLibrary;
using Microsoft.Azure.Cosmos;
using Microsoft.IdentityModel.Abstractions;
using Container = Microsoft.Azure.Cosmos.Container;

namespace FunctionServiceBus
{
    public interface IDatabaseService
    {
        Task InsertDeviceData(MessageIoT deviceData);
    }

    public class DatabaseService : IDatabaseService
    {
        Container _devicesDBContainer;
        CosmosClient client;

        public DatabaseService(string endpoint, string key, string databaseName, string containerName)
        {
            client = new CosmosClient(endpoint, key);
            Database _database = client.GetDatabase(databaseName);
            _devicesDBContainer = _database.GetContainer(containerName);
        }

        public async Task InsertDeviceData(MessageIoT deviceData)
        {
            try
            {
                var createdItem = await _devicesDBContainer.CreateItemAsync<MessageIoT>(item: deviceData, partitionKey: new PartitionKey(deviceData.DeviceId));
            }
            catch (Exception ex)
            {

                throw;
            }

        }
    }
}
