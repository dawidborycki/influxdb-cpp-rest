#include <influxdb_raw_db_utf8.h>
#include <influxdb_simple_api.h>
#include <influxdb_line.h>
#include <influxdb_simple_async_api.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace influxdb::api;

double* generateSignal(int length) {
    auto phaseStep = 2 * M_PI / length;

    auto signal = new double[length];

    for (int i = 0; i < length; i++) {
        auto phase = 1.0 * i * phaseStep;        

        signal[i] = sin(phase);
    }

    return signal;
}

int main(int argc, char* argv[])
{
    try
    {
        const char* url = "http://localhost:8086";
        std::string dbName = "iotdb";
        influxdb::raw::db_utf8 db(url, dbName);
        influxdb::api::simple_db api(url, dbName);        

        // Create database
        api.create();
        
        // Generate sine wave data
        const int N = 16;
        auto data = generateSignal(N);        

        // Add sine wave to the measurement
        std::string measurementName = "sineWave";
        std::string columnNameSignal = "signal";
        std::string columnNameSignalReversed = "signalReversed";

        for(int i = 0; i < N; i++) {            
            api.insert(line
                (measurementName, key_value_pairs(), key_value_pairs(columnNameSignal, data[i]))
                (measurementName, key_value_pairs(), key_value_pairs(columnNameSignalReversed, -data[i])));
        }

        // Delete signal
        delete data;

        // Query and display data from the database            
        auto query = "SELECT " + columnNameSignalReversed + " FROM " + dbName + ".." + measurementName;        
        std::cout << std::endl << db.get(query) << std::endl;

        // Drop database 
        api.drop();                                
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}