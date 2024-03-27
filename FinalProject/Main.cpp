



#include <stdio.h>
#include <algorithm>
#include "PairTrading.h"
#include "MarketData.hpp"
#include "Database.hpp"
#include "Calculations.hpp"


int main(void)
{
    sqlite3 * db = NULL;
    if (OpenDatabase(db) != 0) return -1;
    
    bool bCompleted = false;
    char selection;
    
    //map <string, Stock> stockMap;
    map <string, StockPairPrices> AllPairs;
    
    string BackTestStartDate = "2021-01-02";
    
    while(!bCompleted)
    {
        cout << endl;
        cout << "------------------------------------------------------------------------------------" << endl;
        cout << "------------------------------------------------------------------------------------" << endl;
        cout << "Menu" << endl;
        cout << "========" << endl;
        cout << "A - Create and Populate Pair Table" << endl;
        cout << "B - Retrieve and Populate Historical Data for Each Stock" << endl;
        cout << "C - Create PairPrices Table" << endl;
        cout << "D - Enter BackTest Date & Calculate Volatility" << endl;
        cout << "E - Back Test" << endl;
        cout << "F - Calculate Profit and Loss for Each Pair" << endl;
        cout << "G - Manual Testing" << endl;
        cout << "H - Drop All the Tables" << endl;
        cout << "X - Exit" << endl << endl;
        
        cout << "Enter selection: ";
        cin >> selection;
        
        switch (selection) {
            case 'A':
            case 'a':
            {
                cout << endl;
                if (CreateStockPairs(db) != 0)
                    return -1;
                
                if (PopulatePairNamesCSV("PairTrading.csv", AllPairs) != 0)
                    return -1;
                
                //for (int Itr = 0; Itr < AllPairs.size(); ++Itr)
                //    cout << "Stock1: " << AllPairs[Itr].GetStockPair().first << " Stock2: " << AllPairs[Itr].GetStockPair().second << endl;
                
                if (InsertStockPairs(db, AllPairs) != 0)
                    return -1;
                
                cout << "Retrieving Top values of table StockPairs ..." << endl;
                cout << endl;
                string sqlSelect = "SELECT * FROM StockPairs;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                break;
            }
            case 'B':
            case 'b':
            {
                cout << endl;
                string start_date = "2011-01-01";
                string end_date = "2021-02-26";
                
                string daily_url_common = "https://eodhistoricaldata.com/api/eod/";
                string api_token = "6015ac078111a9.52171409";
                
                vector <string> TempStocks(2);
                string TableHistoricName;
                
                vector <string> PairOneList;
                vector <string> PairTwoList;
                
                if (CreateHistoricTables(db) != 0)
                    return -1;
                
                for (map <string, StockPairPrices>::iterator itr = AllPairs.begin(); itr != AllPairs.end(); itr++)
                {
                    TempStocks[0] = ((itr->second).GetStockPair()).first;
                    TempStocks[1] = ((itr->second).GetStockPair()).second;
                    
                    PairOneList.push_back(TempStocks[0]);
                    PairTwoList.push_back(TempStocks[1]);
                    
                    for (vector<string>::iterator itr2 = TempStocks.begin(); itr2 != TempStocks.end(); itr2++)
                    {
                        if (itr2 == TempStocks.begin())
                        {
                            TableHistoricName = "PairOnePrices";
                            if (count(PairOneList.begin(), PairOneList.end(), *itr2) > 1)
                            {
                                PairOneList.pop_back();
                                continue;
                            }
                        }
                        else
                        {
                            TableHistoricName = "PairTwoPrices";
                            if (count(PairTwoList.begin(), PairTwoList.end(), *itr2) > 1)
                            {
                                PairTwoList.pop_back();
                                continue;
                            }
                        }
                        
                        Stock aStock(*itr2);
                        
                        cout << endl;
                        cout << "Stock Name: " << *itr2 << endl;
                        
                        string daily_url_request = daily_url_common + *itr2 + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
                        
                        string daily_market_data;
                        
                        if (PullMarketData(daily_url_request, daily_market_data) != 0)
                            return -1;
                        if (PopulateDailyTrades(daily_market_data, aStock) != 0)
                            return -1;
                        
                        //stockMap[*itr2] = aStock;
                            
                        if (InsertHistoricTables(db, aStock, TableHistoricName) != 0)
                            return -1;
                        
                        //cout << aStock << endl;
                    }
                }
                
                cout << "Retrieving Top values of table PairOnePrices ..." << endl;
                cout << endl;
                string sqlSelect = "SELECT * FROM PairOnePrices;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                cout << endl;
                cout << "Retrieving Top values of table PairTwoPrices ..." << endl;
                cout << endl;
                sqlSelect = "SELECT * FROM PairTwoPrices;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                break;
            }
            case 'C':
            case 'c':
            {
                cout << endl;
                if (CreatePairPrices(db) != 0)
                    return -1;
                
                if (InsertPairPrices(db) != 0)
                    return -1;
                
                cout << "Retrieving Top values of table PairPrices ..." << endl;
                cout << endl;
                string sqlSelect = "SELECT * FROM PairPrices;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                break;
            }
            case 'D':
            case 'd':
            {
                cout << endl;
                cout << "Enter a BackTest Date (YYYY-mm-dd): ";
                cin >> BackTestStartDate;
                
                cout << endl;
                if (CalculateVolatility(db, AllPairs, BackTestStartDate) != 0)
                    return -1;
                
                cout << "Retrieving Top values of table StockPairs ..." << endl;
                cout << endl;
                string sqlSelect = "SELECT * FROM StockPairs;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                break;
            }
            case 'E':
            case 'e':
            {
                cout << endl;
                if (CalculateBackTest(db, AllPairs) != 0)
                    return -1;
                
                cout << "Retrieving Top values of table PairPrices ..." << endl;
                cout << endl;
                string sqlSelect = "SELECT * FROM PairPrices;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                break;
            }
            case 'F':
            case 'f':
            {
                cout << endl;
                if (CalculateEachPnL(db, AllPairs, BackTestStartDate) != 0)
                    return -1;
                
                cout << "Retrieving Top values of table StockPairs ..." << endl;
                cout << endl;
                string sqlSelect = "SELECT * FROM StockPairs;";
                if (DisplayTables(db, sqlSelect, 5) != 0)
                    return -1;
                
                break;
            }
            case 'G':
            case 'g':
            {
                cout << endl;
                double open1d2, open2d2, close1d1, close2d1, close1d2, close2d2,
                    k_pair, N1=10000, N2, ProfitLoss, vol_pair;
                int LongShort;
                string pair_select;
                
                cout << "Select Stock Pair" << endl;
                for (auto const &pair: AllPairs) {
                    cout << pair.first << endl;
                }
                cout << endl;
                cin.ignore();
                while (AllPairs.find(pair_select) == AllPairs.end()) {
                    cout << "Enter pair exactly as printed above: ";
                    getline(cin, pair_select);
                }
                vol_pair = AllPairs[pair_select].GetVolatility();
                
                cout << endl;
                cout << "Choose k: ";
                cin >> k_pair;
                cout << endl;
                cout << endl;
                
                cout << "Enter Day 1 Close Prices: " << endl;
                cout << "Enter Close Price for First Stock: ";
                cin >> close1d1;
                cout << "Enter Close Price for Second Stock: ";
                cin >> close2d1;
                cout << endl;
                cout << endl;
                
                cout << "Enter Day 2 Open Prices" << endl;
                cout << "Enter Open Price for First Stock: ";
                cin >> open1d2;
                cout << "Enter Open Price for Second Stock: ";
                cin >> open2d2;
                cout << endl;
                cout << endl;
                
                cout << "Enter Day 2 Close Prices" << endl;
                cout << "Enter Close Price for First Stock: ";
                cin >> close1d2;
                cout << "Enter Close Price for Second Stock: ";
                cin >> close2d2;
                cout << endl;

                if (abs(close1d1/close2d1 - open1d2/open2d2) > (vol_pair * k_pair))
                    LongShort = -1;
                else
                    LongShort = 1;
                N2 = N1 * (open1d2 / open2d2);
                ProfitLoss = (-LongShort * N1 * (open1d2 - close1d2)) + (LongShort * N2 * (open2d2 - close2d2));
                
                cout << endl << "Simmulated Results" << endl;
                if (LongShort == -1) {
                    cout << "Short Position Taken in First Stock: 10000 Shares" << endl;
                    cout << "Long Postion Taken in Second Stock: " << N2 << endl;
                }
                else{
                    cout << "Long Position Taken in First Stock: 10,000 Shares" << endl;
                    cout << "Short Postion Taken in Second Stock: " << N2 << " Shares" <<  endl;
                }
                cout << "The Profit and Loss is: " << ProfitLoss << endl;
                break;
            }
            case 'X':
            case 'x':
            {
                CloseDatabase(db);
                bCompleted = true;
                break;
            }
            default:
                cout << "Choose one of the Menu options." << endl;
                break;
        }
        
    }
    
    return 0;
}
