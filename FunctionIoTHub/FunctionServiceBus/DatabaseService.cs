using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net;
using System.Threading.Tasks;
using Azure.Core;
using Microsoft.Azure.Cosmos;
using Microsoft.IdentityModel.Abstractions;
using Container = Microsoft.Azure.Cosmos.Container;

namespace FunctionServiceBus
{
    public interface IDatabaseService
    {
        //Task InsertUserInventoryAsync(UserInventory userInventory, UserInventoryScope scope);
        //Task InsertGuestInventoryAsync(GuestInventory userInventory, GuestInventoryScope scope);
        //Task<List<CrestCodeAggregation>> GetCrestCodeAggregationAsync();
        Task InsertDeviceData(DeviceData deviceData);
    }

    public class DatabaseService : IDatabaseService
    {
        Container _devicesDBContainer;
        CosmosClient client;

        static ItemRequestOptions _itemRequestOpts = new ItemRequestOptions
        {
            EnableContentResponseOnWrite = false,
        };
        static PatchItemRequestOptions _patchOpts = new PatchItemRequestOptions
        {
            EnableContentResponseOnWrite = false,
        };
        public DatabaseService(string endpoint, string key, string databaseName, string containerName)
        {
            client = new CosmosClient(endpoint, key);
            Database _database = client.GetDatabase(databaseName);
            _devicesDBContainer = _database.GetContainer(containerName);
        }

        public async Task InsertDeviceData(DeviceData deviceData)
        {
            
            var createdItem = await _devicesDBContainer.CreateItemAsync<DeviceData>(
            item: deviceData,
            partitionKey: new PartitionKey("devices")
        );
        }
    }
}
