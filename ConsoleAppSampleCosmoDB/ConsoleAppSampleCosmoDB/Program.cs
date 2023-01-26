// See https://aka.ms/new-console-template for more information
using ConsoleAppSampleCosmoDB;
using Microsoft.Azure.Cosmos;

try
{
    Console.WriteLine("Hello, World!");
    // New instance of CosmosClient class
    //string connectionString = "AccountEndpoint=https://cosmodbvik.documents.azure.com:443/;AccountKey=mKeThz76XaBf3nlggKpAGcYxD3ZStG3EeZujqGXrLCiWeK2tMJ59ZopcbJLhaTHLuTSQ57WL3HkaACDbJJsiuQ==;";
    
    using CosmosClient client = new CosmosClient("https://cosmodbvik.documents.azure.com:443/", "mKeThz76XaBf3nlggKpAGcYxD3ZStG3EeZujqGXrLCiWeK2tMJ59ZopcbJLhaTHLuTSQ57WL3HkaACDbJJsiuQ==");
    Database database = await client.CreateDatabaseIfNotExistsAsync("devicesDB");
    //database.DefineContainer("products", "/categoryId");
    // Container reference with creation if it does not alredy exist
    Container container = await database.CreateContainerIfNotExistsAsync(
        id: "products",
        partitionKeyPath: "/categoryId",
        throughput: 400
    );



    int i = 1;
    while (i<50)
    {
        //Create new object and upsert(create or replace) to container
        Random random = new Random();
        int rInt = random.Next(0, 100);

        Product newItem = new(
            id: i.ToString(),
            categoryId: "61dba35b-4f02-45c5-b648-c6badc0cbd79",
            categoryName: "temperature",
            name: "obyvak",
            quantity: rInt,
            sale: false
        );
        Product createdItem = await container.CreateItemAsync<Product>(
            item: newItem,
            partitionKey: new PartitionKey("61dba35b-4f02-45c5-b648-c6badc0cbd79")
        );
        i++;
    }

    //Product createdItem = await container.CreateItemAsync<Product>(
    //    item: newItem,
    //    partitionKey: new PartitionKey("61dba35b-4f02-45c5-b648-c6badc0cbd79")
    //);

    //Console.WriteLine($"Created item:\t{createdItem.id}\t[{createdItem.categoryName}]");
    //Console.WriteLine($"New container:\t{container.Id}");

    // Create query using a SQL string and parameters
    var query = new QueryDefinition(
        query: "SELECT * FROM products p WHERE p.categoryId = @categoryId"
    )
        .WithParameter("@categoryId", "61dba35b-4f02-45c5-b648-c6badc0cbd79");

    using FeedIterator<Product> feed = container.GetItemQueryIterator<Product>(
        queryDefinition: query
    );

    while (feed.HasMoreResults)
    {
        FeedResponse<Product> response = await feed.ReadNextAsync();
        foreach (Product item in response)
        {
            Console.WriteLine($"Found item:\t{item.name}");
        }
    }

}
catch (Exception ex)
{

    throw;
}
