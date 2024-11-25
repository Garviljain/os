
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <fstream>
#include <cmath>


struct Block {
    int size;
    int start;
};


struct Allocation {
    int process_id;
    int size;
    int end_time;
};


const int MAX_ORDER = 10;  
std::vector<std::list<Block>> free_list(MAX_ORDER + 1);
std::unordered_map<int, Allocation> active_allocations;




void init_memory(int total_memory) {
    int order = std::log2(total_memory);
    free_list[order].push_back({total_memory, 0});
}


int find_order(int units) {
    return std::ceil(std::log2(units));
}




Block allocate(int process_id, int units, int current_time, int duration) {
    int order = find_order(units);
    for (int i = order; i <= MAX_ORDER; ++i) {
        if (!free_list[i].empty()) {
            Block block = free_list[i].front();
            free_list[i].pop_front();


            while (i > order) {
                --i;
                Block buddy = {block.size / 2, block.start + block.size / 2};
                free_list[i].push_back(buddy);
                block.size /= 2;
            }


            active_allocations[process_id] = {process_id, block.size, current_time + duration};
            std::cout << "Allocated " << block.size << " units to Process " << process_id << "\n";
            return block;
        }
    }
    std::cerr << "Memory allocation failed for Process " << process_id << "\n";
    return {-1, -1};  
}




void deallocate(int process_id) {
    auto it = active_allocations.find(process_id);
    if (it != active_allocations.end()) {
        Block block = {it->second.size, 0};  
        int order = find_order(block.size);
        free_list[order].push_back(block);
        std::cout << "Deallocated Process " << process_id << "\n";
        active_allocations.erase(it);
    }
}




void tick(int current_time) {
    for (auto it = active_allocations.begin(); it != active_allocations.end();) {
        if (it->second.end_time == current_time) {
            deallocate(it->second.process_id);
            it = active_allocations.erase(it); 
        } else {
            ++it;
        }
    }
}


void process_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open " << filename << "\n";
        return;
    }


    int process_id, units, request_time, duration;
    while (file >> process_id >> units >> request_time >> duration) {
        if (process_id < 0) break;  
        tick(request_time);  
        allocate(process_id, units, request_time, duration);
    }


    std::cout << "All requests processed.\n";
}


int main() {
    int total_memory = 1024; 
    init_memory(total_memory);


    process_file("buddy.dat");


    return 0;
}



