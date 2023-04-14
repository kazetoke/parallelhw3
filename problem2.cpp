#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <bits/stdc++.h>
#include <fstream>
#include <condition_variable>

using namespace std;

struct TemperatureReading {
    double temperature;
    int timestamp;
};

class TemperatureReaders {
public:
    std::vector<TemperatureReading> readings;

    std::mutex data_mutex;
    void addTemperature(const TemperatureReading& reading) {
        std::unique_lock<std::mutex> lock(data_mutex);
        readings.push_back(reading);
    }

    void generateHourlyReport() {
        std::unique_lock<std::mutex> lock(data_mutex);
        auto compare_temperature = [](const TemperatureReading& a, const TemperatureReading& b) { //compare two temps
            return a.temperature < b.temperature;
        };

        std::sort(readings.begin(), readings.end(), compare_temperature);

        cout<<"Top 5 highest temperatures:\n";
        for (int i = 0; i < 5 && i < readings.size(); ++i) {
            cout << readings[readings.size() - 1 - i].temperature << "F at"
                      << readings[readings.size() - 1 - i].timestamp << '\n';
        }

        cout << "Top 5 lowest temperatures:\n";
        for (int i = 0; i < 5 && i < readings.size(); ++i) {
            cout << readings[i].temperature << "F at " << readings[i].timestamp << '\n';
        }

        double largest_difference = 0.0;
        int start_interval = 0;
        for (int i = 0; i < readings.size() - 10; ++i) {
            double difference = readings[i + 9].temperature - readings[i].temperature;
            if (difference > largest_difference) {
                largest_difference = difference;
                start_interval = readings[i].timestamp;
            }
        }

        cout << "Largest temperature difference observed in the 10-minute interval starting at timestamp "
                  << start_interval << " with a difference of " << largest_difference << "F\n";

        readings.clear();
    }


};

void sensor(TemperatureReaders& module, int sensor_id) {
    for (int minute = 0; minute < 60; ++minute) {
        double temperature = rand() % (70 - (-100) + 1) + (-100);
        module.addTemperature({temperature, minute});

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second, simulating 1 minute.
    }
}

int main() {
    const int num_sensors = 8;

    TemperatureReaders temperature_module;
    std::vector<std::thread> sensor_threads;

    for (int i = 0; i < num_sensors; ++i) {
        sensor_threads.push_back(std::thread(sensor, std::ref(temperature_module), i));
    }
    for (auto& thread : sensor_threads) {
    thread.join();
    }

    temperature_module.generateHourlyReport();

    return 0;

}
