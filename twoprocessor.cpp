
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>


struct Process {
   int arrivalTime;
   int processId;
   int burstTime;
   int priority;
   int startTime;
   int endTime;




   bool operator>(const Process &p) const {
       return (priority > p.priority) || (priority == p.priority && arrivalTime > p.arrivalTime);
   }
};


void readProcessData(const std::string &filename, std::vector<Process> &processes) {
   std::ifstream file(filename);
   if (!file) {
       std::cerr << "Error opening file!" << std::endl;
       return;
   }


   int arrival, id, burst, priority;
   char comma;
   while (file >> arrival >> comma >> id >> comma >> burst >> comma >> priority) {
       if (arrival < 0) break;
       processes.push_back({arrival, id, burst, priority, -1, -1});
   }
   file.close();
}


void printGanttChart(const std::vector<Process> &completed, const std::string &processorName) {
   std::cout << "\nGantt Chart for " << processorName << ":\n";
   for (const auto &p : completed) {
       std::cout << "| P" << p.processId << " ";
   }
   std::cout << "|\n";
}


void calculateAndPrintStats(const std::vector<Process> &completed) {
   int totalTurnaround = 0;
   int totalWaiting = 0;


   std::cout << "\nProcess-wise details:\n";
   std::cout << "Process ID | Turnaround Time | Waiting Time\n";


   for (const auto &p : completed) {
       int turnaroundTime = abs(p.endTime - p.arrivalTime);
       int waitingTime = abs(turnaroundTime - p.burstTime);


       totalTurnaround += turnaroundTime;
       totalWaiting += waitingTime;


       std::cout << "P" << p.processId
                 << "           " << std::setw(8) << turnaroundTime
                 << "              " << std::setw(8) << waitingTime << "\n";
   }


   std::cout << "\nAverage Turnaround Time: " << (totalTurnaround / (double)completed.size()) << "\n";
   std::cout << "Average Waiting Time: " << (totalWaiting / (double)completed.size()) << "\n";
}
void scheduleSingleQueue(const std::vector<Process> &processes) {
   std::priority_queue<Process, std::vector<Process>, std::greater<Process>> queue;
   std::vector<Process> completed;
   int timeProcessor1 = 0, timeProcessor2 = 0;


  
   for (const auto &process : processes) {
       queue.push(process);
   }


   while (!queue.empty()) {
       Process current = queue.top();
       queue.pop();


       if (timeProcessor1 <= timeProcessor2) {
           current.startTime = std::max(timeProcessor1, current.arrivalTime);
           current.endTime = current.startTime + current.burstTime / 2; // Processor 1 is twice as fast
           timeProcessor1 = current.endTime;
       } else {
           current.startTime = std::max(timeProcessor2, current.arrivalTime);
           current.endTime = current.startTime + current.burstTime; // Processor 2 speed
           timeProcessor2 = current.endTime;
       }


       completed.push_back(current);
   }


   printGanttChart(completed, "Single Queue (Both Processors)");
   calculateAndPrintStats(completed);
}


void scheduleTwoQueues(const std::vector<Process> &processes) {
   std::priority_queue<Process, std::vector<Process>, std::greater<Process>> queue1, queue2;
   std::vector<Process> completed1, completed2;
   int timeProcessor1 = 0, timeProcessor2 = 0;




   for (const auto &process : processes) {
       if (process.priority <= 5) {
           queue1.push(process);    // Higher priority for Processor 1
       } else {
           queue2.push(process);    // Lower priority for Processor 2
       }
   }




   while (!queue1.empty()) {
       Process current = queue1.top();
       queue1.pop();


       current.startTime = std::max(timeProcessor1, current.arrivalTime);
       current.endTime = current.startTime + current.burstTime / 2;
       timeProcessor1 = current.endTime;


       completed1.push_back(current);
   }


    while (!queue2.empty()) {
       Process current = queue2.top();
       queue2.pop();


       current.startTime = std::max(timeProcessor2, current.arrivalTime);
       current.endTime = current.startTime + current.burstTime; // Processor 2 speed
       timeProcessor2 = current.endTime;


       completed2.push_back(current);
   }


   printGanttChart(completed1, "Processor 1");
   calculateAndPrintStats(completed1);


   printGanttChart(completed2, "Processor 2");
   calculateAndPrintStats(completed2);
}




int main() {
   std::vector<Process> processes;
   readProcessData("sched2.dat", processes);


   std::cout << "Version 1: Single Queue Scheduling\n";
   scheduleSingleQueue(processes);


   std::cout << "\nVersion 2: Separate Queue Scheduling\n";
   scheduleTwoQueues(processes);


   return 0;
}


