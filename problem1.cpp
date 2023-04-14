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

struct Present{
    int tag;
};

class LinkedList{
public:
    void insert(Present present) {
        std::unique_lock<std::mutex> lock(list_mutex);
        auto it = std::upper_bound(list.begin(), list.end(), present, [](const Present& left, const Present& right) { //comparing
            return left.tag < right.tag;
        });
        list.insert(it, present);
    }

    bool contains(int tag){
        std::unique_lock<std::mutex> lock(list_mutex);
        return std::any_of(list.begin(), list.end(), [tag](const Present& present) {
            return present.tag == tag;
        });
    }

    Present remove(){
        std::unique_lock<std::mutex> lock(list_mutex);
        if (list.empty()) {
            return {-1};
        }
        Present present = list.front();
        list.pop_front();
        return present;
    }


    std::list<Present> list;

    std::mutex list_mutex;
};

void servant(LinkedList& concurrent_list, std::vector<Present>& presents) {
    for (auto& present : presents) {
        concurrent_list.insert(present);
        if (concurrent_list.contains(present.tag)) {
            Present removed_present = concurrent_list.remove();
            if (removed_present.tag != -1) {
                std::cout << "Thank you card written in present: " << removed_present.tag << std::endl;
            }
        }
    }
}

int main() {
    const int num_presents = 5000;
    const int num_servants = 4;

    LinkedList concurrent_list;
    std::vector<std::vector<Present>> all_presents(num_servants);

    for (int i = 0; i < num_presents; ++i) {
        all_presents[i % num_servants].push_back({ i });
    }

    std::vector<std::thread> servant_threads;
    for (int i = 0; i < num_servants; ++i) {
        servant_threads.push_back(std::thread(servant, std::ref(concurrent_list), std::ref(all_presents[i])));
    }

    for (auto& thread : servant_threads) {
        thread.join();
    }

    return 0;
}
