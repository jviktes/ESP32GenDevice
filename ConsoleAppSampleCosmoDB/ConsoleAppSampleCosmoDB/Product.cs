using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleAppSampleCosmoDB
{
    public record Product(
    string id,
    string categoryId,
    string categoryName,
    string name,
    int quantity,
    bool sale
);
}
