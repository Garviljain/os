#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

struct Process {
    int id, priority, arrival_time;
    vector<int> bursts;
    int current_burst = 0;
    int time_remaining = 0;
    int wait_time = 0;
    string history;
};

vector<Process> processes;
queue<int> ready_queue;
vector<int> io_queue;

void read_input() {
    ifstream file("proc.dat");
    string line;
    getline(file, line);
    int n = stoi(line);
    
    for (int i = 0; i < n; ++i) {
        getline(file, line);
        stringstream ss(line);
        Process p;
        string token;
        getline(ss, token, ',');
        p.id = stoi(token);
        getline(ss, token, ',');
        p.priority = stoi(token);
        getline(ss, token, ',');
        p.arrival_time = stoi(token);
        while (getline(ss, token, ',')) {
            int burst = stoi(token);
            if (burst == -1) break;
            p.bursts.push_back(burst);
        }
        processes.push_back(p);
    }
    file.close();
}

void update_io(int time) {
    vector<int> remaining_io;
    for (int i = 0; i < io_queue.size(); ++i) {
        int pid = io_queue[i];
        processes[pid].bursts[processes[pid].current_burst]--;
        processes[pid].history += "R";
        if (processes[pid].bursts[processes[pid].current_burst] == 0) {
            processes[pid].current_burst++;
            ready_queue.push(pid);
        } else {
            remaining_io.push_back(pid);
        }
    }
    io_queue = remaining_io;
}

void fcfs() {
    int time = 0;
    while (!ready_queue.empty() || !io_queue.empty()) {
        update_io(time);
        if (!ready_queue.empty()) {
            int pid = ready_queue.front();
            ready_queue.pop();
            Process &p = processes[pid];
            if (p.arrival_time > time) {
                time = p.arrival_time;
            }
            p.history += string(time - p.history.size(), 'W');
            p.history += "C";
            time++;
            p.bursts[p.current_burst]--;
            if (p.bursts[p.current_burst] == 0) {
                p.current_burst++;
                if (p.current_burst < p.bursts.size()) {
                    io_queue.push(pid);
                }
            } else {
                ready_queue.push(pid);
            }
        } else {
            time++;
        }
    }
}

void sjf_preemptive() {
    int time = 0;
    while (!ready_queue.empty() || !io_queue.empty()) {
        update_io(time);
        if (!ready_queue.empty()) {
            sort(ready_queue.front(), ready_queue.back(), [](int a, int b) {
                return processes[a].bursts[processes[a].current_burst] < processes[b].bursts[processes[b].current_burst];
            });
            int pid = ready_queue.front();
            ready_queue.pop();
            Process &p = processes[pid];
            if (p.arrival_time > time) {
                time = p.arrival_time;
            }
            p.history += string(time - p.history.size(), 'W');
            p.history += "C";
            time++;
            p.bursts[p.current_burst]--;
            if (p.bursts[p.current_burst] == 0) {
                p.current_burst++;
                if (p.current_burst < p.bursts.size()) {
                    io_queue.push(pid);
                }
            } else {
                ready_queue.push(pid);
            }
        } else {
            time++;
        }
    }
}

void priority_preemptive() {
    int time = 0;
    while (!ready_queue.empty() || !io_queue.empty()) {
        update_io(time);
        if (!ready_queue.empty()) {
            sort(ready_queue.front(), ready_queue.back(), [](int a, int b) {
                return processes[a].priority < processes[b].priority;
            });
            int pid = ready_queue.front();
            ready_queue.pop();
            Process &p = processes[pid];
            if (p.arrival_time > time) {
                time = p.arrival_time;
            }
            p.history += string(time - p.history.size(), 'W');
            p.history += "C";
            time++;
            p.bursts[p.current_burst]--;
            if (p.bursts[p.current_burst] == 0) {
                p.current_burst++;
                if (p.current_burst < p.bursts.size()) {
                    io_queue.push(pid);
                }
            } else {
                ready_queue.push(pid);
            }
        } else {
            time++;
        }
    }
}

void round_robin() {
    int time = 0;
    while (!ready_queue.empty() || !io_queue.empty()) {
        update_io(time);
        if (!ready_queue.empty()) {
            int pid = ready_queue.front();
            ready_queue.pop();
            Process &p = processes[pid];
            if (p.arrival_time > time) {
                time = p.arrival_time;
            }
            p.history += string(time - p.history.size(), 'W');
            p.history += "C";
            time++;
            p.bursts[p.current_burst]--;
            if (p.bursts[p.current_burst] == 0) {
                p.current_burst++;
                if (p.current_burst < p.bursts.size()) {
                    io_queue.push(pid);
                }
            } else {
                ready_queue.push(pid);
            }
        } else {
            time++;
        }
    }
}

void display_history() {
    for (auto &p : processes) {
        cout << "Process " << p.id << ": " << p.history << endl;
    }
}

int main() {
    read_input();
    ready_queue.push(0);
    fcfs();
    display_history();
    return 0;
}

