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
            //CosmosClientOptions opts = new CosmosClientOptions
            //{
            //    Serializer = new CosmosSystemTextJsonSerializer(new System.Text.Json.JsonSerializerOptions
            //    {
            //        PropertyNamingPolicy = System.Text.Json.JsonNamingPolicy.CamelCase,
            //        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.Never
            //    }),
            //    MaxRetryAttemptsOnRateLimitedRequests = 200,
            //    MaxRetryWaitTimeOnRateLimitedRequests = TimeSpan.FromSeconds(1),
            //    AllowBulkExecution = true,
            //    ApplicationName = "DHL Inventory uploader",
            //    ConnectionMode = ConnectionMode.Direct,
            //};
            //var cosmosClient = new CosmosClient(endpoint, credential, opts);
            //var db = cosmosClient.GetDatabase(database);
            //_inventory = db.GetContainer(inventoryContainer);

            client = new CosmosClient(endpoint, key);
            Database _database = client.GetDatabase(databaseName);//await client.CreateDatabaseIfNotExistsAsync("devicesDB");
            _devicesDBContainer = _database.GetContainer(containerName);
            //Microsoft.Azure.Cosmos.Container container = await _database.CreateContainerIfNotExistsAsync(
            //    id: "devicesDB",
            //    partitionKeyPath: "/devicesData",
            //    //partitionKeyPath: "/categoryId",
            //    throughput: 400
            //);
        }

        public async Task InsertDeviceData(DeviceData deviceData)
        {
            
            var createdItem = await _devicesDBContainer.CreateItemAsync<DeviceData>(
            item: deviceData,
            partitionKey: new PartitionKey("61dba35b-4f02-45c5-b648-c6badc0cbd79")
        );
        }

        //public async Task InsertUserInventoryAsync(UserInventory userInventory, UserInventoryScope scope)
        //{
        //    //try update
        //    List<PatchOperation> operations = new List<PatchOperation>();

        //    switch (scope)
        //    {
        //        case UserInventoryScope.AzureAd:
        //            operations.Add(PatchOperation.Set("/aadUser", userInventory.aadUser));
        //            break;
        //        case UserInventoryScope.Teams:
        //            operations.Add(PatchOperation.Set("/teamsUser", userInventory.teamsUser));
        //            break;
        //        default:
        //            throw new ArgumentException(nameof(scope));
        //    }

        //    var patchResult = await _inventory.PatchItemStreamAsync(userInventory.Id, new PartitionKey(userInventory.Pk), operations, _patchOpts).ConfigureAwait(false);
        //    if (!patchResult.IsSuccessStatusCode)
        //    {
        //        if (patchResult.StatusCode == System.Net.HttpStatusCode.NotFound)
        //        {
        //            await _inventory.CreateItemAsync(userInventory, new PartitionKey(userInventory.Pk), _itemRequestOpts).ConfigureAwait(false);
        //        }
        //        else
        //            throw new CosmosException($"Error when patching item {userInventory.Id}: {patchResult.ErrorMessage}", patchResult.StatusCode, 0, null, 0);
        //    }
        //}

        //public async Task InsertGuestInventoryAsync(GuestInventory guestInventory, GuestInventoryScope scope)
        //{
        //    //try update
        //    List<PatchOperation> operations = new List<PatchOperation>();

        //    switch (scope)
        //    {
        //        case GuestInventoryScope.AzureAd:
        //            operations.Add(PatchOperation.Set("/aadUser", guestInventory.aadUser));
        //            break;
        //        default:
        //            throw new ArgumentException(nameof(scope));
        //    }
        //    var patchResult = await _inventory.PatchItemStreamAsync(guestInventory.Id, new PartitionKey(guestInventory.Pk), operations, _patchOpts).ConfigureAwait(false);
        //    if (!patchResult.IsSuccessStatusCode)
        //    {
        //        if (patchResult.StatusCode == System.Net.HttpStatusCode.NotFound)
        //        {
        //            await _inventory.CreateItemAsync(guestInventory, new PartitionKey(guestInventory.Pk), _itemRequestOpts).ConfigureAwait(false);
        //        }
        //        else
        //            throw new CosmosException($"Error when patching item {guestInventory.Id}: {patchResult.ErrorMessage}", patchResult.StatusCode, 0, null, 0);
        //    }
        //}

        
    }
}
