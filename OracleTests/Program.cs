using System;
using System.Data;
using System.Data.Common;
using System.Data.SqlClient;
using Oracle.ManagedDataAccess.Client;

namespace OracleTests
{
    class Program
    {
        public static DbParameter CreateOraDbParameter(string name, object value)
        {
            if (value is DateTime || value is DateTime?)
            {
                return new OracleParameter(name, value);
            }

            if (value is Int32 || value is UInt32 || value is Int64 || value is UInt64)
            {
                return new OracleParameter(name, OracleDbType.Decimal, value, ParameterDirection.Input);

            }
            return new OracleParameter(name, value) as DbParameter;
        }

        public static DbParameter CreateSqlDbParameter(string name, object value)
        {
            return new SqlParameter(name, value) as DbParameter;
        }


        static void TestMsSql()
        {
            var connectionString = (new SqlConnectionStringBuilder()
            {
                DataSource = "SMADDEN-SP4",
                InitialCatalog = "DM165SP",
                UserID = "DOCSUSER",
                Password = "Madden99a"
            }).ToString();

            using (var conn = new SqlConnection(connectionString))
            {
                conn.Open();
                using (var command = conn.CreateCommand() as DbCommand)
                {
                    Console.WriteLine("BEFORE:");
                    command.CommandText = "SELECT ID, CREATED, ORIG_EXPIRES, USE_EXPIRES FROM DOCSADM.KEY_PROVIDERS";
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            Console.WriteLine($"ID={reader.GetString(0)} CREATED={(reader.IsDBNull(1) ? "null" : reader.GetDateTime(1).ToString())}  ORIG_EXPIRES={(reader.IsDBNull(2) ? "null" : reader.GetDateTime(2).ToString())}  USE_EXPIRES={(reader.IsDBNull(3) ? "null" : reader.GetDateTime(3).ToString())}   ");
                        }
                    }

                    string id = Guid.NewGuid().ToString();

                    // Now try to add something
                    command.CommandText = "INSERT INTO DOCSADM.KEY_PROVIDERS "
                        + " (ID, NAME, PROVIDER_TYPE, DATA, KEY_ORIG_PERIOD, KEY_USE_PERIOD, CREATED, NOT_BEFORE, ORIG_EXPIRES, USE_EXPIRES, MODIFIED) "
                        + " VALUES (@0, @1, @2, @3, @4, @5, @6, @7, @8, @9, @10)";
                    command.Parameters.Add(CreateSqlDbParameter("@0", id) as DbParameter);
                    command.Parameters.Add(CreateSqlDbParameter("@1", "Test Provider"));      // ID
                    command.Parameters.Add(CreateSqlDbParameter("@2", "DPAPI"));              // NAME
                    command.Parameters.Add(CreateSqlDbParameter("@3", "Data goes here"));     // PROVIDER_TYPE
                    command.Parameters.Add(CreateSqlDbParameter("@4", 1));                    // KEY_ORIG_PERIOD
                    command.Parameters.Add(CreateSqlDbParameter("@5", 2));                    // KEY_USE_PERIOD
                    command.Parameters.Add(CreateSqlDbParameter("@6", DateTime.UtcNow));           // CREATED
                    command.Parameters.Add(CreateSqlDbParameter("@7", DBNull.Value));         // NOT_BEFORE NULL
                    command.Parameters.Add(CreateSqlDbParameter("@8", DBNull.Value));           // ORIG_EXPIRES NULL
                    command.Parameters.Add(CreateSqlDbParameter("@9", DBNull.Value));        // USE_EXPIRES NULL
                    command.Parameters.Add(CreateSqlDbParameter("@10", DateTime.UtcNow));     // MODIFIED
                    var rowsChanged = command.ExecuteNonQuery();

                    Console.WriteLine("AFTER:");
                    command.CommandText = "SELECT ID, CREATED, ORIG_EXPIRES, USE_EXPIRES FROM DOCSADM.KEY_PROVIDERS";
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            Console.WriteLine($"ID={reader.GetString(0)} CREATED={(reader.IsDBNull(1) ? "null" : reader.GetDateTime(1).ToString())}  ORIG_EXPIRES={(reader.IsDBNull(2) ? "null" : reader.GetDateTime(2).ToString())}  USE_EXPIRES={(reader.IsDBNull(3) ? "null" : reader.GetDateTime(3).ToString())}   ");
                        }
                    }
                }
            }
        }



        static void TestOracle()
        {
            var oracleConnectionString = (new OracleConnectionStringBuilder()
            {
                DataSource = "SMADDEN-ORA12C",
                UserID = "DOCSUSER",
                Password = "isgreat"
            }).ToString();

            using (var conn = new OracleConnection(oracleConnectionString))
            {
                conn.Open();
                using (var command = conn.CreateCommand() as DbCommand)
                {
                    Console.WriteLine("BEFORE:");
                    command.CommandText = "SELECT ID, CREATED, ORIG_EXPIRES, USE_EXPIRES FROM DOCSADM.KEY_PROVIDERS";
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            Console.WriteLine($"ID={reader.GetString(0)} CREATED={(reader.IsDBNull(1) ? "null" : reader.GetDateTime(1).ToString())}  ORIG_EXPIRES={(reader.IsDBNull(2) ? "null" : reader.GetDateTime(2).ToString())}  USE_EXPIRES={(reader.IsDBNull(3) ? "null" : reader.GetDateTime(3).ToString())}   ");
                        }
                    }

                    string id = Guid.NewGuid().ToString();

                    // Now try to add something
                    command.CommandText = "INSERT INTO DOCSADM.KEY_PROVIDERS "
                        + " (ID, NAME, PROVIDER_TYPE, DATA, KEY_ORIG_PERIOD, KEY_USE_PERIOD, CREATED, NOT_BEFORE, ORIG_EXPIRES, USE_EXPIRES, MODIFIED) "
                        + " VALUES (:0, :1, :2, :3, :4, :5, :6, :7, :8, :9, :10)";
                    command.Parameters.Add(new OracleParameter(":0", id) as DbParameter);
                    command.Parameters.Add(new OracleParameter(":1", "Test Provider") as DbParameter);      // ID
                    command.Parameters.Add(new OracleParameter(":2", "DPAPI") as DbParameter);              // NAME
                    command.Parameters.Add(new OracleParameter(":3", "Data goes here") as DbParameter);     // PROVIDER_TYPE
                    command.Parameters.Add(new OracleParameter(":4", OracleDbType.Decimal, 1, ParameterDirection.Input) as DbParameter);                    // KEY_ORIG_PERIOD
                    command.Parameters.Add(new OracleParameter(":5", OracleDbType.Decimal, 2, ParameterDirection.Input) as DbParameter);                    // KEY_USE_PERIOD
                    command.Parameters.Add(new OracleParameter(":6", DateTime.UtcNow) as DbParameter);           // CREATED
                    command.Parameters.Add(new OracleParameter(":7", OracleDbType.Date) as DbParameter);         // NOT_BEFORE NULL
                    //command.Parameters.Add(CreateOraDbParameter(":8", DBNull.Value));           // ORIG_EXPIRES NULL
                    command.Parameters.Add(new OracleParameter(":8", DBNull.Value));           // ORIG_EXPIRES NULL
                    command.Parameters.Add(new OracleParameter(":9", OracleDbType.Date) as DbParameter);        // USE_EXPIRES NULL
                    command.Parameters.Add(new OracleParameter(":10", DateTime.UtcNow) as DbParameter);     // MODIFIED
                    var rowsChanged = command.ExecuteNonQuery();

                    Console.WriteLine("AFTER:");
                    command.CommandText = "SELECT ID, CREATED, ORIG_EXPIRES, USE_EXPIRES FROM DOCSADM.KEY_PROVIDERS";
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            Console.WriteLine($"ID={reader.GetString(0)} CREATED={(reader.IsDBNull(1) ? "null" : reader.GetDateTime(1).ToString())}  ORIG_EXPIRES={(reader.IsDBNull(2) ? "null" : reader.GetDateTime(2).ToString())}  USE_EXPIRES={(reader.IsDBNull(3) ? "null" : reader.GetDateTime(3).ToString())}   ");
                        }
                    }
                }
            }
        }


        static void Main(string[] args)
        {
            TestOracle();
            TestMsSql();
            Console.Write("Press any key to exit... ");
            Console.ReadKey();
        }
    }
}
