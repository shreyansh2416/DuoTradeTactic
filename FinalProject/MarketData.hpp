//
//  MarketData.hpp
//  FinalProject



#ifndef MarketData_hpp
#define MarketData_hpp

#include <stdio.h>
#include <fstream>
#include "curl/curl.h"
#include "json/json.h"
#include "PairTrading.h"


int PullMarketData(const std::string & url_request, std::string & read_buffer);
int PopulateDailyTrades(const std::string & read_buffer, Stock & Stock_);
int PopulatePairNamesCSV(string filename, map <string, StockPairPrices> & AllPairs);


class CSVReader
{
    string fileName;
    string delimeter;
    long MaxCounter;
public:
    CSVReader(string filename, long MaxCount = 0, string delm = ",") : fileName(filename), delimeter(delm), MaxCounter(MaxCount)
    { }
    // Function to fetch data from a CSV File
    vector<vector<string>> getData();
};


#endif /* MarketData_hpp */
