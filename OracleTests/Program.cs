using System;
using System.Data;
using System.Data.Common;
using Oracle.ManagedDataAccess.Client;

namespace OracleTests
{
    class Program
    {
        static void Main(string[] args)
        {
            var connectionString = new OracleConnectionStringBuilder()
            {
                DataSource = "SMADDEN-ORA12C",
                UserID = "DOCSUSER",
                Password = "isgreat"
            };

            using (var conn = new OracleConnection(connectionString.ConnectionString))
            {
                conn.Open();
                using (var command = conn.CreateCommand() as DbCommand)
                {
                    // Lets just see if we have a good db connection
                    command.CommandText = "SELECT SYSTEM_ID, LIBRARY_NAME FROM DOCSADM.REMOTE_LIBRARIES";
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            Console.WriteLine($"SYSTEM_ID={reader.GetInt32(0)} LIBRARY_NAME={reader.GetString(1)}");
                        }
                    }
                

                    // Now try to add something
                    command.CommandText = "INSERT INTO DOCSADM.KEY_PROVIDERS "
                        + " (ID, NAME, PROVIDER_TYPE, DATA, KEY_ORIG_PERIOD, KEY_USE_PERIOD, CREATED, NOT_BEFORE, ORIG_EXPIRES, USE_EXPIRES, MODIFIED) "
                        + " VALUES (:0, :1, :2, :3, :4, :5, :6, :7, :8, :9, :10)";
                    command.Parameters.Add(new OracleParameter(":0", "c752337b-8788-4763-8396-85be92ad454c") as DbParameter);
                    command.Parameters.Add(new OracleParameter(":1", "Test Provider") as DbParameter);      // ID
                    command.Parameters.Add(new OracleParameter(":2", "DPAPI") as DbParameter);              // NAME
                    command.Parameters.Add(new OracleParameter(":3", "Data goes here") as DbParameter);     // PROVIDER_TYPE
                    command.Parameters.Add(new OracleParameter(":4", OracleDbType.Decimal, 1, ParameterDirection.Input) as DbParameter);                    // KEY_ORIG_PERIOD
                    command.Parameters.Add(new OracleParameter(":5", OracleDbType.Decimal, 2, ParameterDirection.Input) as DbParameter);                    // KEY_USE_PERIOD
                    command.Parameters.Add(new OracleParameter(":6", DateTime.UtcNow) as DbParameter);           // CREATED
                    command.Parameters.Add(new OracleParameter(":7", OracleDbType.Date) as DbParameter);         // NOT_BEFORE
                    command.Parameters.Add(new OracleParameter(":8", OracleDbType.Date) as DbParameter);           // ORIG_EXPIRES
                    command.Parameters.Add(new OracleParameter(":9", OracleDbType.Date) as DbParameter);        // USE_EXPIRES
                    command.Parameters.Add(new OracleParameter(":10", DateTime.UtcNow) as DbParameter);     // MODIFIED
                    var rowsChanged = command.ExecuteNonQuery();
                }
            }
            Console.Write("Press any key to exit... ");
            Console.ReadKey();
        }
    }
}
