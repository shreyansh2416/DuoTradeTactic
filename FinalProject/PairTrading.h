//
//  Stock.h
//  FinalProject


#pragma once

#ifndef PairTrading_h
#define PairTrading_h

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;


class TradeData
{
protected:
    string Date;
    double Open;
    double High;
    double Low;
    double Close;
    double AdjClose;
    long Volume;
    
public:
    TradeData() : Date(""), Open(0), High(0), Low(0), Close(0), AdjClose(0), Volume(0) {}
    TradeData(string Date_, double Open_, double High_, double Low_, double Close_, double AdjClose_, double Volume_):
        Date(Date_), Open(Open_), High(High_), Low(Low_), Close(Close_), AdjClose(AdjClose_), Volume(Volume_) {}
    TradeData(const TradeData & TradeData) : Date(TradeData.Date), Open(TradeData.Open), High(TradeData.High), Low(TradeData.Low), Close(TradeData.Close), AdjClose(TradeData.AdjClose), Volume(TradeData.Volume) {}
    
    TradeData operator=(const TradeData & TradeData)
    {
        Date = TradeData.Date;
        Open = TradeData.Open;
        High = TradeData.High;
        Low = TradeData.Low;
        Close = TradeData.Close;
        AdjClose = TradeData.AdjClose;
        Volume = TradeData.Volume;
        return *this;
    }
    
    ~TradeData() {}
    
    string GetDate() const {return Date;}
    double GetOpen() const {return Open;}
    double GetHigh() const {return High;}
    double GetLow() const {return Low;}
    double GetClose() const {return Close;}
    double GetAdjClose() const { return AdjClose;}
    long GetVolume() const {return Volume;}
    
    friend ostream & operator << (ostream & out, const TradeData & t)
    {
        out << "Date: " << t.Date << " Open: " << t.Open << " High: " << t.High << " Low: " << t.Low << " Close: " << t.Close << " Adjusted_Close: "<< t.AdjClose << " Volume: " << t.Volume << endl;
        return out;
    }
};



class Stock
{
private:
    string Symbol;
    vector <TradeData> Trades;
    
public:
    Stock() : Symbol("") { Trades.clear(); }
    Stock(string Symbol_): Symbol(Symbol_) { Trades.clear(); }
    Stock(const Stock & Stock_) { memcpy(this, & Stock_, sizeof(Stock_)); }
    
    ~Stock() {}
    
    string GetSymbol() const { return Symbol; }
    
    void addTrade(const TradeData & aTrade) { Trades.push_back(aTrade); }
    const vector <TradeData> & GetTrades(void) const { return Trades; }
    
    friend ostream & operator << (ostream & out, const Stock & s)
    {
        out << "Symbol: " << s.Symbol << endl;
        for(vector <TradeData>::const_iterator itr = s.Trades.begin(); itr != s.Trades.end(); itr++)
            out << *itr;
        return out;
    }
};


struct PairPrice
{
    double Open1;
    double Close1;
    double Open2;
    double Close2;
    double ProfitLoss;
    
    PairPrice() : Open1(0), Close1(0), Open2(0), Close2(0), ProfitLoss(0) {}
    PairPrice(double Open1_, double Close1_, double Open2_, double Close2_, double ProfitLoss_) : Open1(Open1_), Close1(Close1_), Open2(Open2_), Close2(Close2_), ProfitLoss(ProfitLoss_) {}
};


class StockPairPrices
{
private:
    pair <string, string> stockPair;
    double volatility;
    double k;
    double FinalPnL;
    map <string, PairPrice> dailyPairPrices;
    
public:
    StockPairPrices() { volatility = 0; k = 0; }
    StockPairPrices(pair <string, string> stockPair_) { stockPair = stockPair_; volatility = 0; k = 0;}
    
    void SetStockPair(pair <string, string> stockPair_) { stockPair = stockPair_; }
    void SetStockPair(string Stock1, string Stock2) { stockPair.first = Stock1; stockPair.second = Stock2; }
    void SetDailyPairPrice(string Date_, PairPrice pairPrice_)
    {
        dailyPairPrices.insert(pair<string, PairPrice> (Date_, pairPrice_));
    }
    void SetVolatility(double volatility_){ volatility = volatility_;}
    void SetK(double k_){ k = k_;}
    void SetFinalPnL(double FinalPnL_){ FinalPnL = FinalPnL_;}
    void UpdateProfitLoss( string Date_, double ProfitLoss_ )
    {
        dailyPairPrices[Date_].ProfitLoss = ProfitLoss_;
    }
    pair<string, string> GetStockPair() const { return stockPair; }
    map<string, PairPrice> GetDailyPrices() const { return dailyPairPrices; }
    double GetVolatility() const { return volatility; }
    double GetK() const { return k; }
    double GetFinalPnL() const { return FinalPnL; }
};






#endif /* PairTrading_h */
