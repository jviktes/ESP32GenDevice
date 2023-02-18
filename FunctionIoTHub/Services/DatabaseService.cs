using IoTLibrary;
using Microsoft.Azure.Cosmos;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Text.Json;
using Container = Microsoft.Azure.Cosmos.Container;

namespace IoTServices
{
    public interface IDatabaseService
    {
        Task InsertDeviceData(MessageIoT deviceData);
        Task<List<object>> GetData();

    }

    public class DatabaseService : IDatabaseService
    {
        Container _devicesDBContainer;
        CosmosClient client;
        //Container _inventory;

        public DatabaseService(string endpoint, string key, string databaseName, string containerName)
        {
            client = new CosmosClient(endpoint, key);
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

                String queryString = @"SELECT c.id,c.DeviceId,c.MessageData,c.MessageDate FROM c WHERE c.DeviceId= 'TopeniObejvak'";

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
                catch (Exception)
                {

                    //throw;
                }
                return rslt;

            
        }

    }
}