

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
using namespace std;
const int NUM_PHILOSOPHERS = 5;
mutex forks[NUM_PHILOSOPHERS];  

void philosopher(int id) {
	default_random_engine generator;
	uniform_int_distribution<int> distribution(1, 1000);

	while (true) {
    	cout << "Philosopher " << id << " is thinking.\n";
    	this_thread::sleep_for(std::chrono::milliseconds(distribution(generator)));

    	int leftFork = id;
    	int rightFork = (id + 1) % NUM_PHILOSOPHERS;

    	if (id % 2 == 0) {
        	forks[leftFork].lock();
        	forks[rightFork].lock();
    	} else {
        	forks[rightFork].lock();
        	forks[leftFork].lock();
    	}

    	cout << "Philosopher " << id << " is eating  with " << leftFork << "and" << rightFork <<"\n";
    	this_thread::sleep_for(chrono::milliseconds(distribution(generator)));

    	// forks unlock
    	forks[leftFork].unlock();
    	forks[rightFork].unlock();
	}
}

int main() {
	thread philosophers[NUM_PHILOSOPHERS];

    
	for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
    	philosophers[i] = thread(philosopher, i);
	}

   
	for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
    	philosophers[i].join();
	}

	return 0;
}


