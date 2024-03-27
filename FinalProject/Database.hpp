//
//  Database.hpp
//  FinalProject


#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#pragma once
#include <sqlite3.h>
#include "PairTrading.h"

class StockPairPrices;
int OpenDatabase(sqlite3 * & db);
int CloseDatabase(sqlite3 * & db);

int CreateStockPairs(sqlite3 * & db);
int CreateHistoricTables(sqlite3 * & db);
int CreatePairPrices(sqlite3 * & db);

int InsertStockPairs(sqlite3 * db, const map <string, StockPairPrices> & AllPairs);
int InsertHistoricTables(sqlite3 * db, const Stock & Stock_, string TableName);
int InsertPairPrices(sqlite3 * db);

int DisplayTables(sqlite3 * db, string sqlSelect, int maxrows = 0);

#endif /* Database_hpp */
