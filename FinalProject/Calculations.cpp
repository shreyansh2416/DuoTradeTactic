//
//  Calculations.cpp
//  FinalProject


#include "Calculations.hpp"
#include <cmath>


int CalculateVolatility(sqlite3 * db, map <string, StockPairPrices> & AllPairs, string BackTestStartDate)
{
    int rc = 0;
    char* error = nullptr;
    
    string calculate_variance = string("Update StockPairs SET Volatility = ")
    + "(SELECT (AVG((Close1/Close2) * (Close1/Close2)) - AVG(Close1/Close2) * AVG(Close1/Close2)) "
    + "FROM PairPrices "
    + "WHERE StockPairs.Symbol1 = PairPrices.Symbol1 AND StockPairs.Symbol2 = PairPrices.Symbol2 AND "
    + "PairPrices.Date < \'" + BackTestStartDate + "\');";
    
    rc = sqlite3_exec(db, calculate_variance.c_str(), NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    
    
    char** results = NULL;
    int rows, columns;
    string Stock1, Stock2;
    vector <double> Volatilities;
    char calculate_vol[512];
    
    string sqlSelect = string("SELECT Symbol1, Symbol2, Volatility FROM StockPairs;");
    
    rc = sqlite3_get_table(db, sqlSelect.c_str(), &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
        {
            Stock1 = results[(rowCtr * columns) + 0];
            Stock2 = results[(rowCtr * columns) + 1];
            
            Volatilities.push_back(sqrt(stod(results[(rowCtr * columns) + 2])));
            AllPairs[Stock1 + " & " + Stock2].SetVolatility(Volatilities.back());
            
            sprintf(calculate_vol, "Update StockPairs SET Volatility = %f WHERE Symbol1 = \"%s\" AND Symbol2 = \"%s\";", Volatilities.back(), Stock1.c_str(), Stock2.c_str());
            
            rc = sqlite3_exec(db, calculate_vol, NULL, NULL, &error);
            if (rc)
            {
                cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
                sqlite3_free(error);
                return -1;
            }
        }
    }
    
    return 0;
}



int CalculateBackTest(sqlite3 * db, map <string, StockPairPrices> & AllPairs)
{
    int rc = 0;
    char* error = nullptr;
    
    char** results = NULL;
    int rows, columns;
    
    string Stock1, Stock2;
    double VolPair;
    double kPair;
    
    double Close1d1, Close2d1;
    double Open1d2, Close1d2, Open2d2, Close2d2, ProfitLoss;
    
    int LongShort; // 1 is Long, -1 is Short
    double N1 = 10000;
    double N2;
    
    double Kvalue = 1;
    
    
    for (map <string, StockPairPrices>::iterator itr = AllPairs.begin(); itr != AllPairs.end(); itr++)
    {
        Stock1 = ((itr->second).GetStockPair()).first;
        Stock2 = ((itr->second).GetStockPair()).second;
        
        (itr->second).SetK(Kvalue);
        
        VolPair = (itr->second).GetVolatility();
        kPair = (itr->second).GetK();
        
        string sqlSelect = string("SELECT Symbol1, Symbol2, Date, Open1, Close1, Open2, Close2 FROM PairPrices ")
            + "WHERE Symbol1 = \'" + Stock1 + "\' AND Symbol2 = \'" + Stock2 + "\';";
        
        rc = sqlite3_get_table(db, sqlSelect.c_str(), &results, &rows, &columns, &error);
        if (rc)
        {
            cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
            sqlite3_free(error);
        }
        else
        {
            Close1d1 = stod(results[(1 * columns) + 4]);
            Close2d1 = stod(results[(1 * columns) + 6]);
            ProfitLoss = 0;
            
            PairPrice aPair(stod(results[(1 * columns) + 3]), Close1d1, stod(results[(1 * columns) + 5]), Close2d1, ProfitLoss);
            
            (itr->second).SetDailyPairPrice(results[(1 * columns) + 2], aPair);
            
            char calculate_PnL[512];
            
            for (int rowCtr = 2; rowCtr <= rows; ++rowCtr)
            {
                Open1d2 = stod(results[(rowCtr * columns) + 3]);
                Close1d2 = stod(results[(rowCtr * columns) + 4]);
                Open2d2 = stod(results[(rowCtr * columns) + 5]);
                Close2d2 = stod(results[(rowCtr * columns) + 6]);
                
                if (abs(Close1d1/Close2d1 - Open1d2/Open2d2) > (VolPair * kPair))
                    LongShort = -1;
                else
                    LongShort = 1;
                
                N2 = N1 * (Open1d2 / Open2d2);
                ProfitLoss = (-LongShort * N1 * (Open1d2 - Close1d2)) + (LongShort * N2 * (Open2d2 - Close2d2));
                
                PairPrice aPair(Open1d2, Close1d2, Open2d2, Close2d2, ProfitLoss);
                
                (itr->second).SetDailyPairPrice(results[(rowCtr * columns) + 2], aPair);
                
                Close1d1 = stod(results[(rowCtr * columns) + 4]);
                Close2d1 = stod(results[(rowCtr * columns) + 6]);
                
                sprintf(calculate_PnL, "Update PairPrices SET Profit_Loss = %f WHERE Date = \"%s\" AND Symbol1 = \"%s\" AND Symbol2 = \"%s\";", ProfitLoss, results[(rowCtr * columns) + 2], results[(rowCtr * columns) + 0], results[(rowCtr * columns) + 1]);
                
                rc = sqlite3_exec(db, calculate_PnL, NULL, NULL, &error);
                if (rc)
                {
                    cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
                    sqlite3_free(error);
                    return -1;
                }
                
            }
        }
        sqlite3_free_table(results);
    }
    
    return 0;
}


int CalculateEachPnL(sqlite3 * db, map <string, StockPairPrices> & AllPairs, string BackTestStartDate)
{
    int rc = 0;
    char* error = nullptr;
    
    string Stock1, Stock2;
    
    for (map <string, StockPairPrices>::iterator itr = AllPairs.begin(); itr != AllPairs.end(); itr++)
    {
        Stock1 = ((itr->second).GetStockPair()).first;
        Stock2 = ((itr->second).GetStockPair()).second;
        
        map <string, PairPrice> dailyPairPrices = (itr->second).GetDailyPrices();
        
        double FinalPnL = 0;
        
        for (map <string, PairPrice>::iterator itr2 = dailyPairPrices.begin(); itr2 != dailyPairPrices.end(); itr2++)
        {
            if(itr2->first < BackTestStartDate)
                continue;
            FinalPnL += (itr2->second).ProfitLoss;
        }
        
        char calculate_FinalPnL[512];
        
        sprintf(calculate_FinalPnL, "Update StockPairs SET Profit_Loss = %f WHERE Symbol1 = \"%s\" AND Symbol2 = \"%s\";", FinalPnL, Stock1.c_str(), Stock2.c_str());
        
        rc = sqlite3_exec(db, calculate_FinalPnL, NULL, NULL, &error);
        if (rc)
        {
            cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
            sqlite3_free(error);
            return -1;
        }
    }
    
    return 0;
}



