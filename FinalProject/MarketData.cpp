//
//  MarketData.cpp
//  FinalProject


#include "MarketData.hpp"


//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int PullMarketData(const std::string & url_request, std::string & read_buffer)
{
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);

    //creating session handle
    CURL * handle;

    // Store the result of CURLís webpage retrieval, for simple error checking.
    CURLcode result;

    // notice the lack of major error-checking, for brevity
    handle = curl_easy_init();

    //after creating handle we will start transfering webpage
    //curl_easy_setopt is used to tell libcurl how to behave.
    //By setting the appropriate options, the application can change libcurl's behavior.
    
    curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
    
    //adding a user agent
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

    // send all data to this function
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

    // we pass our 'chunk' struct to the callback function
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);

    //perform a blocking file transfer
    result = curl_easy_perform(handle);
    
    
    // check for errors
    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return -1;
    }
    
    //std::cout << read_buffer << std::endl;
    
    curl_easy_cleanup(handle);
    
    return 0;
}



int PopulateDailyTrades(const std::string & read_buffer, Stock & Stock_)
{
    //json parsing
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value root;   // will contains the root value after parsing.
    std::string errors;

    bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);
    if (not parsingSuccessful)
    {
        // Report failures and their locations in the document.
        std::cout << "Failed to parse JSON" << std::endl << read_buffer << errors << std::endl;
        return -1;
    }
    
    //std::cout << "\nSucess parsing json\n" << root << std::endl;
    std::cout << "Sucess parsing json" << std::endl;
    std::string Date;
    float Open, High, Low, Close, Adjusted_Close;
    long Volume;
    
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++) // iterate the Json list
    {
        Date = (*itr)["date"].asString();    // read value from Json Object, it is a C++ map
        Open = (*itr)["open"].asFloat();
        High = (*itr)["high"].asFloat();
        Low = (*itr)["low"].asFloat();
        Close = (*itr)["close"].asFloat();
        Adjusted_Close = (*itr)["adjusted_close"].asFloat();
        Volume = (*itr)["volume"].asInt64();
        TradeData aTrade(Date, Open, High, Low, Close, Adjusted_Close, Volume);
        Stock_.addTrade(aTrade);
    }
    
    return 0;
}





vector<vector<string>> CSVReader::getData()
{
    // Parses through csv file line by line and returns the data
    // in vector of vector of strings.
    ifstream file(fileName);
    vector<vector<string> > dataList;
    string line = "";
    long iter = 1;
    // Iterate through each line and split the content
    while (getline(file, line))
    {
        vector<string> vec;
        string word;
        
        stringstream s(line);
        // read every column data of a row and
        // store it in a string variable, 'word'
        while (getline(s, word, ',')) {
          
            // add all the column data
            // of a row to a vector
            vec.push_back(word);
        }
        
        dataList.push_back(vec);
        if(iter == MaxCounter)
        {
            break;
        }
        iter += 1;
        
    }
    // Close the File
    file.close();
    return dataList;
}



int PopulatePairNamesCSV(string filename, map <string, StockPairPrices> & AllPairs)
{
    CSVReader CsvFile(filename);
    vector<vector<string>> DataCsv = CsvFile.getData();
    
    string temp;
    vector<string> Rows;
    StockPairPrices StocksNames;
    string Key;
    
    //string s2 = "\r";
    
    for(int ite1=0; ite1 < DataCsv.size(); ite1++)
    {
        Rows = DataCsv[ite1];
        temp = Rows[1];
        temp.erase(std::remove(temp.begin(), temp.end(), '\r'), temp.end());
        
        StocksNames.SetStockPair(Rows[0], temp);
        
        Key = Rows[0] + " & " + temp;
        
        AllPairs[Key] = StocksNames;
    }
    
    return 0;
}


