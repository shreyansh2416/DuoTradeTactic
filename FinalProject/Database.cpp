//
//  Database.cpp
//  FinalProject


#include "Database.hpp"



int OpenDatabase(sqlite3 * & db)
{
    int rc = 0;

    // Open Database
    cout << "Opening PairTrading.db ..." << endl;
    rc = sqlite3_open("PairTrading.db", &db);
    if (rc)
    {
        cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_close(db);
        return -1;
    }
    else
    {
        cout << "Opened PairTrading.db." << endl << endl;
    }
    
    return 0;
}


int CreateStockPairs(sqlite3 * & db)
{
    int rc = 0;
    char* error = nullptr;
    
    // Drop the table if exists
    std::string sql_dropatable = "DROP TABLE IF EXISTS StockPairs";
    if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite can't drop StockPairs table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    
    // Create the table
    std::cout << "Creating StockPairs table ..." << std::endl;
    std::string sqlCreateTable = std::string("CREATE TABLE IF NOT EXISTS StockPairs ")
        + "(ID INT NOT NULL,"
        + "Symbol1 CHAR(20) NOT NULL,"
        + "Symbol2 CHAR(20) NOT NULL,"
        + "Volatility FLOAT NOT NULL,"
        + "Profit_Loss FLOAT NOT NULL,"
        + "PRIMARY KEY(Symbol1, Symbol2)"
        + ");";

    rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
    if (rc)
    {
        std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
        sqlite3_free(error);
        return -1;
    }
    else
    {
        std::cout << "Created StockPairs table." << std::endl << std::endl;
    }
    
    return 0;
}



int CreateHistoricTables(sqlite3 * & db)
{
    int rc = 0;
    char* error = nullptr;
    
    // Drop the table if exists
    std::string sql_dropatable = "DROP TABLE IF EXISTS PairOnePrices";
    if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite can't drop PairOnePrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    
    // Create the table
    std::cout << "Creating PairOnePrices table ..." << std::endl;
    std::string sqlCreateTable = std::string("CREATE TABLE IF NOT EXISTS PairOnePrices ")
        + "(Symbol CHAR(20) NOT NULL,"
        + "Date CHAR(20) NOT NULL,"
        + "Open REAL NOT NULL,"
        + "High REAL NOT NULL,"
        + "Low REAL NOT NULL,"
        + "Close REAL NOT NULL,"
        + "Adjusted_Close REAL NOT NULL,"
        + "Volume INT NOT NULL,"
        + "PRIMARY KEY(Symbol, Date)"
        + ");";

    rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
    if (rc)
    {
        std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
        sqlite3_free(error);
        return -1;
    }
    else
    {
        std::cout << "Created PairOnePrices table." << std::endl << std::endl;
    }
    
    
    
    // Drop the table if exists
    sql_dropatable = "DROP TABLE IF EXISTS PairTwoPrices";
    if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite can't drop PairTwoPrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    
    // Create the table
    std::cout << "Creating PairTwoPrices table ..." << std::endl;
    sqlCreateTable = std::string("CREATE TABLE IF NOT EXISTS PairTwoPrices ")
        + "(Symbol CHAR(20) NOT NULL,"
        + "Date CHAR(20) NOT NULL,"
        + "Open REAL NOT NULL,"
        + "High REAL NOT NULL,"
        + "Low REAL NOT NULL,"
        + "Close REAL NOT NULL,"
        + "Adjusted_Close REAL NOT NULL,"
        + "Volume INT NOT NULL,"
        + "PRIMARY KEY(Symbol, Date)"
        + ");";

    rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
    if (rc)
    {
        std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
        sqlite3_free(error);
        return -1;
    }
    else
    {
        std::cout << "Created PairTwoPrices table." << std::endl << std::endl;
    }
    
    return 0;
}



int CreatePairPrices(sqlite3 * & db)
{
    int rc = 0;
    char* error = nullptr;
    
    // Drop the table if exists
    std::string sql_dropatable = "DROP TABLE IF EXISTS PairPrices";
    if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite can't drop PairPrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    
    // Create the table
    std::cout << "Creating PairPrices table ..." << std::endl;
    std::string sqlCreateTable = std::string("CREATE TABLE IF NOT EXISTS PairPrices ")
        + "(Symbol1 CHAR(20) NOT NULL,"
        + "Symbol2 CHAR(20) NOT NULL,"
        + "Date CHAR(20) NOT NULL,"
        + "Open1 REAL NOT NULL,"
        + "Close1 REAL NOT NULL,"
        + "Open2 REAL NOT NULL,"
        + "Close2 REAL NOT NULL,"
        + "Profit_Loss FLOAT NOT NULL,"
        + "PRIMARY KEY(Symbol1, Symbol2, Date)"
        + "FOREIGN KEY(Symbol1, Date) REFERENCES PairOnePrices(Symbol, Date)\n"
        + "ON DELETE CASCADE\n"
        + "ON UPDATE CASCADE,"
        + "FOREIGN KEY(Symbol2, Date) REFERENCES PairTwoPrices(Symbol, Date)\n"
        + "ON DELETE CASCADE\n"
        + "ON UPDATE CASCADE,"
        + "FOREIGN KEY(Symbol1, Symbol2) REFERENCES StockPairs(Symbol1, Symbol2)\n"
        + "ON DELETE CASCADE\n"
        + "ON UPDATE CASCADE"
        + ");";

    rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
    if (rc)
    {
        std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
        sqlite3_free(error);
        return -1;
    }
    else
    {
        std::cout << "Created PairPrices table." << std::endl << std::endl;
    }
    
    return 0;
}


int InsertStockPairs(sqlite3 * db, const map <string, StockPairPrices> & AllPairs)
{
    int rc = 0;
    char* error = nullptr;
    
    char sqlInsert[512];
    int counting = 1;
    
    // Execute SQL
    cout << "Inserting values into table StockPairs ..." << endl;
    for (map <string, StockPairPrices>::const_iterator itr = AllPairs.begin(); itr != AllPairs.end(); itr++)
    {
        sprintf(sqlInsert, "INSERT INTO StockPairs(ID, Symbol1, Symbol2, Volatility, Profit_Loss) VALUES(%d, \"%s\", \"%s\", %f, %f)", counting, (itr->second).GetStockPair().first.c_str(), (itr->second).GetStockPair().second.c_str(), (itr->second).GetVolatility(), 0.0);
        
        counting += 1;
        //cout << sqlInsert << endl;
        rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
        if (rc)
        {
            cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
            sqlite3_free(error);
            return -1;
        }
    }
    cout << "Inserted values into the table StockPairs." << endl << endl;
    
    return 0;
}



int InsertHistoricTables(sqlite3 * db, const Stock & Stock_, string TableName)
{
    int rc = 0;
    char* error = nullptr;
    
    char sqlInsert[512];
    
    // Execute SQL
    cout << "Inserting values into table " << TableName << " ..." << endl;
    
    const vector <TradeData> & dailyTrade = Stock_.GetTrades();
    
    for (vector <TradeData>::const_iterator itr = dailyTrade.begin(); itr != dailyTrade.end(); itr++)
    {
        string InsertQuery = "INSERT INTO " + TableName + "(Symbol, Date, Open, High, Low, Close, Adjusted_Close, Volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %ld)";
        const char * InsertQueryPtr = InsertQuery.c_str();
        
        sprintf(sqlInsert, InsertQueryPtr, Stock_.GetSymbol().c_str(), itr->GetDate().c_str(), itr->GetOpen(), itr->GetHigh(), itr->GetLow(), itr->GetClose(), itr->GetAdjClose(), itr->GetVolume());
        
        //cout << sqlInsert << endl;
        rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
        if (rc)
        {
            cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
            sqlite3_free(error);
            return -1;
        }
    }
    cout << "Inserted values into the table " << TableName << "." << endl << endl;
    
    return 0;
}




int InsertPairPrices(sqlite3 * db)
{
    int rc = 0;
    char* error = nullptr;
    
    char sqlInsert[512];
    
    // Execute SQL
    cout << "Inserting values into table PairPrices ..." << endl;

    sprintf(sqlInsert, "INSERT INTO PairPrices SELECT StockPairs.Symbol1 as Symbol1, StockPairs.Symbol2 as Symbol2, PairOnePrices.Date as Date, PairOnePrices.Open as Open1, PairOnePrices.Close as Close1, PairTwoPrices.Open as Open2, PairTwoPrices.Close as Close2, 0 as Profit_Loss FROM StockPairs, PairOnePrices, PairTwoPrices WHERE (((StockPairs.Symbol1 = PairOnePrices.Symbol) AND (StockPairs.Symbol2 = PairTwoPrices.Symbol)) AND (PairOnePrices.Date = PairTwoPrices.Date)) ORDER BY Symbol1, Symbol2");
    
    //cout << sqlInsert << endl;
    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    cout << "Inserted values into the table PairPrices." << endl << endl;
    
    return 0;
}





int DisplayTables(sqlite3 * db, string sqlSelect, int maxrows)
{
    int rc = 0;
    char* error = nullptr;
    
    char** results = NULL;
    int rows, columns;
    
    rc = sqlite3_get_table(db, sqlSelect.c_str(), &results, &rows, &columns, &error);
    if (maxrows != 0)
        rows = maxrows;
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        // Display Table
        for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                // Determine Cell Position
                int cellPosition = (rowCtr * columns) + colCtr;

                // Display Cell Value
                cout.width(12);
                cout.setf(ios::left);
                cout << results[cellPosition] << " ";
            }

            // End Line
            cout << endl;

            // Display Separator For Header
            if (0 == rowCtr)
            {
                for (int colCtr = 0; colCtr < columns; ++colCtr)
                {
                    cout.width(12);
                    cout.setf(ios::left);
                    cout << "~~~~~~~~~~~~ ";
                }
                cout << endl;
            }
        }
        if (maxrows != 0)
            cout << "..." << endl;
    }
    sqlite3_free_table(results);
    
    return 0;
}



int CloseDatabase(sqlite3 * & db)
{
    // Close Database
    sqlite3_close(db);
    cout << "Closed Database" << endl << endl;
    
    return 0;
}
