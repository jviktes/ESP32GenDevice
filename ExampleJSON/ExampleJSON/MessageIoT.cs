namespace IoTLibrary
{

        /// <summary>
        /// format zpravy
        /// </summary>
        public class MessageIoT
        {
            public string id { get; set; }
            public MessageData MessageData { get; set; }
            public string DeviceId { get; set; }
            public DeviceData DeviceData { get; set; }

            public string CategoryName { get; set; }

            public DateTime MessageDate { get; set; }
        }

        public class DeviceData
        {
            public string nameDevice { get; set; }
            public string DeviceId { get; set; }
            public List<Data> DataSet { get; set; }
        }

        public class MessageData
        {
            public string IdMessage { get; set; }
            public List<Data> DataSet { get; set; }
            
        }

        public class Data
        {
            public string Name { get; set; }
            public string Value { get; set; }
        }

}