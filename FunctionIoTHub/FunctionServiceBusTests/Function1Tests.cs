using Microsoft.VisualStudio.TestTools.UnitTesting;
using FunctionServiceBus;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FunctionServiceBus.Tests
{
    [TestClass()]
    public class Function1Tests
    {
        public String categoryId = "devices";
        public String categoryName = "temperature";

        [TestMethod()]
        public void ParseDataTest()
        {
            String test1 = "{\"deviceId\":\"TopeniObejvak\",\"temperature\":\"244\"}";

            DeviceData deviceData = Function1.ParseData(test1, categoryId, categoryName);


            Assert.Fail();
        }
    }
}