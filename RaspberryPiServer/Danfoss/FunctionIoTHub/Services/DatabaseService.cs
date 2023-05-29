using IoTLibrary;
using Microsoft.Azure.Cosmos;
using System.Collections.Concurrent;
//using System.ComponentModel;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Threading;

namespace IoTServices
{
    public interface IDatabaseService
    {
        Task InsertDeviceData(MessageIoT deviceData);
        Task<List<object>> GetData();
        Task GetAndUpdateESPData();
    }

    public class DatabaseService : IDatabaseService
    {
        Container _devicesDBContainer;
        CosmosClient client;
        //Container _inventory;

        public DatabaseService(string endpoint, string key, string databaseName, string containerName)
        {
            client = new CosmosClient(endpoint, key, new CosmosClientOptions() { AllowBulkExecution = true });
            
            Database _database = client.GetDatabase(databaseName);
            _devicesDBContainer = _database.GetContainer(containerName);
            //_inventory = _database.GetContainer("");
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

        public async Task<List<object>> GetData()
        {

                var rslt = new List<object>();

            String queryString = @"SELECT TOP 1000 c.id,c.DeviceId,c.MessageData,c.MessageDate FROM c WHERE c.DeviceId IN( 'TopeniObejvak', 'esp32Temperature') AND (CONTAINS(c.MessageDate,'2023')) ORDER BY c._ts DESC";

                var query = new QueryDefinition(queryString);
                //    .WithParameter("@batchSize", this._config.BatchSizeNotUpdatedDetailedTeamsData)
                //    .WithParameter("@numberOfDaysToSetOutdatedRecords", this._config.NumberOfDaysToSetOutdatedRecords);

                using var iterator = _devicesDBContainer.GetItemQueryIterator<dynamic>(query);

                try
                {

                    while (iterator.HasMoreResults)
                    {
                        var rsp = await iterator.ReadNextAsync();
                        rslt.Add(rsp);
                    }
                }
                catch (Exception ex)
                {

                    //throw;
                }
                return rslt;

            
        }

        public async Task GetAndUpdateESPData()
        {

            var _itemsToUpdate = new List<MessageIoT>();

            String queryString = @"SELECT TOP 1000 * FROM c WHERE c.DeviceId IN('esp32Temperature') AND (CONTAINS(c.MessageDate,'2023')) ORDER BY c._ts DESC";

            var query = new QueryDefinition(queryString);

            //using var iterator = _devicesDBContainer.GetItemQueryIterator<dynamic>(query);

            // Query multiple items from container
            using FeedIterator<MessageIoT> iterator = _devicesDBContainer.GetItemQueryIterator<MessageIoT>(
                queryText: queryString
            );

            try
            {

                while (iterator.HasMoreResults)
                {

                    FeedResponse<MessageIoT> response = await iterator.ReadNextAsync();
                    foreach (MessageIoT item in response)
                    {

                        _itemsToUpdate.Add(item);
                    }
                }
            }
            catch (Exception ex)
            {

                //throw;
            }


            foreach (var item in _itemsToUpdate)
            {
                if (item.DeviceData.DataSet.Any(x=>x.Name=="voltage"))
                {
                    var itemToUpdateVoltage = item.DeviceData.DataSet.Where(x => x.Name == "voltage").FirstOrDefault();
                    itemToUpdateVoltage.Value = "2000";
                }
            }

            //Update dat:
            List<Task> tasks = new List<Task>();

            try
            {
                foreach (var item in _itemsToUpdate)
                {
                    tasks.Add(_devicesDBContainer.UpsertItemAsync(item));
                    //var res = await _devicesDBContainer.UpsertItemAsync(item);
                }
                await Task.Run(() => Task.WaitAll(tasks.ToArray()), CancellationToken.None);
            }
            catch (Exception ex)
            {

               
            }
        }

    }
}