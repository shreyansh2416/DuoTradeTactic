#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>

using namespace std;
class IntradayTrade
{
private:
	long timestamp;
	string datetime;
	float open;
	float high;
	float low;
	float close;
	float adjusted_close;
	int volume;
public:
	IntradayTrade(long timestamp_, string datetime_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :
		timestamp(timestamp_), datetime(datetime_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
	{}
	~IntradayTrade() {}
	friend ostream& operator << (ostream& out, const IntradayTrade& t)
	{
		out << "Timestamp: " << t.timestamp << " Datetime: " << t.datetime << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};

class Stock
{
private:
	string symbol;
	vector<IntradayTrade> trades;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	void addTrade(IntradayTrade aTrade)
	{
		trades.push_back(aTrade);
	}
	friend ostream& operator << (ostream& out, const Stock& s)
	{
		cout << "Symbol: " << s.symbol << endl;
		for (vector<IntradayTrade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
		return out;
	}
};

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int main(void)
{
	std::string readBuffer;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* myHandle;
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	myHandle = curl_easy_init();
	string url_common = "https://eodhistoricaldata.com/api/intraday/";
	string symbol = "MSFT";
	string api_token = "5ba84ea974ab42.45160048";
	string url_request = url_common + symbol + ".US?" + "&api_token=" + api_token + "&interval=5m&fmt=json";
	curl_easy_setopt(myHandle, CURLOPT_URL, url_request.c_str());
	//curl_easy_setopt(myHandle, CURLOPT_URL, "https://eodhistoricaldata.com/api/intraday/AAPL.US?api_token=5ba84ea974ab42.45160048&interval=5m&fmt=json");

	curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
	}
	else {
		//json parsing
		Json::CharReaderBuilder builder;
		Json::CharReader* reader = builder.newCharReader();
		Json::Value root;   // will contains the root value after parsing.
		string errors;

		bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
		if (not parsingSuccessful)
		{
			// Report failures and their locations in the document.
			cout << "Failed to parse JSON" << endl << readBuffer << errors << endl;
			system("pause");
			return -1;
		}
		else
		{
			std::cout << "\nSucess parsing json\n" << root << endl;
			long timestamp;
			char datetime[128];
			memset(datetime, '\0', 128);
			float open, high, low, close, adjusted_close;
			int volume;
			Stock aStock(symbol);
			int count = 0;
			for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++) {
				{
					//cout << *itr << endl;
					for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
					{
						//cout << inner.key() << ": " << *inner << endl;
						if (inner.key().asString() == "timestamp")
						{
							if (inner->isNull() || inner->isNumeric())
								timestamp = inner->asInt64();
							else
								timestamp = atol(inner->asCString());

							time_t t = timestamp;
							struct tm* tm = localtime(&t);
							sprintf(datetime, "%s", asctime(tm));
						}
						else if (inner.key() == "datetime")
							continue;
						else if (inner.key() == "open")
						{
							if (inner->isNull() || inner->isNumeric())
								open = inner->asFloat();
							else
								open = (float)atof(inner->asCString());
						}
						else if (inner.key().asString() == "high")
						{
							if (inner->isNull() || inner->isNumeric())
								high = inner->asFloat();
							else
								high = (float)atof(inner->asCString());
						}
						else if (inner.key().asString() == "low")
						{
							if (inner->isNull() || inner->isNumeric())
								low = inner->asFloat();
							else
								low = (float)atof(inner->asCString());
						}
						else if (inner.key().asString() == "close")
						{
							if (inner->isNull() || inner->isNumeric())
								close = inner->asFloat();
							else
								close = (float)atof(inner->asCString());
						}
						else if (inner.key().asString() == "adjusted_close")
						{
							if (inner->isNull() || inner->isNumeric())
								adjusted_close = inner->asFloat();
							else
								adjusted_close = (float)atof(inner->asCString());
						}
						else if (inner.key().asString() == "volume")
						{
							if (inner->isNull() || inner->isNumeric())
								volume = inner->asInt();
							else
								volume = atoi(inner->asCString());
						}
						else if (inner.key().asString() == "gmtoffset")
							continue;
						else
						{
							cout << "Invalid json field" << endl;
							system("pause");
							return -1;
						}
					}
					IntradayTrade aTrade(timestamp, datetime, open, high, low, close, adjusted_close, volume);
					aStock.addTrade(aTrade);
					count++;
				}
			}
			cout << aStock;
		}
	}
	//End a libcurl easy handle.This function must be the last function to call for an easy session
	curl_easy_cleanup(myHandle);
	return 0;
}
