#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <iomanip>
#include <algorithm>

struct Request {
	int time;
	int size;
	int duration;
};

struct Block {
	int start;
	int end;
	bool allocated;
	int endTime;
	int allocatedSize;
};

void readRequests(const std::string& filename, int& memorySize, std::deque<Request>& requests) {
	std::ifstream infile(filename);
	if (!infile) {
    	std::cerr << "Error opening file: " << filename << std::endl;
    	exit(1);
	}
	infile >> memorySize;
	while (true) {
    	Request req;
    	infile >> req.time >> req.size >> req.duration;
    	if (req.time == -1 && req.size == -1 && req.duration == -1) {
        	break;
    	}
    	requests.push_back(req);
	}
}

void printMemoryStatus(int successful, int total, int memorySize, const std::vector<Block>& blocks) {
	double successRate = (total > 0) ? (100.0 * successful / total) : 0.0;

	int totalUsed = 0;
	int totalFree = 0;
	int lastEnd = -1;

	for (const auto& block : blocks) {
    	if (block.allocated) {
        	totalUsed += block.allocatedSize;
        	lastEnd = block.end;
    	} else {
        	if (lastEnd != -1 && block.start > lastEnd + 1) {
            	totalFree += (block.start - lastEnd - 1);
        	}
        	lastEnd = block.end;
    	}
	}
	if (lastEnd != memorySize - 1) {
    	totalFree += (memorySize - 1 - lastEnd);
	}

	double externalFragmentation = (totalFree * 100.0) / memorySize;
	double internalFragmentation = ((totalUsed - totalFree) * 100.0) / memorySize;

	std::cout << "Allocation success rate: " << std::fixed << std::setprecision(2) << successRate << "%\n";
	std::cout << "External fragmentation: " << externalFragmentation << "%\n";
	std::cout << "Internal fragmentation: " << internalFragmentation << "%\n";
}

bool allocateBlock(std::vector<Block>& blocks, int size, int currentTime, int& lastFitIndex, std::function<bool(const Block&, int)> fitStrategy) {
	int n = blocks.size();
	int startIdx = lastFitIndex;
	bool allocated = false;

	for (int i = 0; i < n; ++i) {
    	int idx = (startIdx + i) % n;
    	Block& block = blocks[idx];
    	if (!block.allocated && fitStrategy(block, size)) {
        	if ((block.end - block.start + 1) > size) {
            	blocks.push_back({block.start + size, block.end, false, 0, 0});
            	block.end = block.start + size - 1;
        	}
        	block.allocated = true;
        	block.allocatedSize = size;
        	block.endTime = currentTime + size;
        	lastFitIndex = (idx + 1) % n;
        	allocated = true;
        	break;
    	}
	}

	return allocated;
}

int main() {
	int memorySize;
	std::deque<Request> requests;
	readRequests("alloc.dat", memorySize, requests);

	std::vector<Block> blocks = { {0, memorySize - 1, false, 0, 0} };
	int lastFitIndex = 0;

	int successfulFirstFit = 0, successfulBestFit = 0, successfulWorstFit = 0, successfulNextFit = 0;
	int requestCount = 0;

	auto firstFitStrategy = [](const Block& block, int size) {
    	return (block.end - block.start + 1) >= size;
	};

	auto bestFitStrategy = [memorySize](const Block& block, int size) {
    	return (block.end - block.start + 1) >= size;
	};

	auto worstFitStrategy = [memorySize](const Block& block, int size) {
    	return (block.end - block.start + 1) >= size;
	};

	while (!requests.empty()) {
    	Request currentRequest = requests.front();
    	requests.pop_front();

    	for (auto& block : blocks) {
        	if (block.allocated && block.endTime <= currentRequest.time) {
            	block.allocated = false;
            	block.allocatedSize = 0;
        	}
    	}

    	bool allocatedFirstFit = allocateBlock(blocks, currentRequest.size, currentRequest.time, lastFitIndex, firstFitStrategy);
    	bool allocatedBestFit = allocateBlock(blocks, currentRequest.size, currentRequest.time, lastFitIndex, bestFitStrategy);
    	bool allocatedWorstFit = allocateBlock(blocks, currentRequest.size, currentRequest.time, lastFitIndex, worstFitStrategy);
    	bool allocatedNextFit = allocateBlock(blocks, currentRequest.size, currentRequest.time, lastFitIndex, worstFitStrategy);  
    	if (allocatedFirstFit) ++successfulFirstFit;
    	if (allocatedBestFit) ++successfulBestFit;
    	if (allocatedWorstFit) ++successfulWorstFit;
    	if (allocatedNextFit) ++successfulNextFit;

    	++requestCount;
    	if (requestCount % 10 == 0) {
        	std::cout << "After " << requestCount << " requests:\n";
        	std::cout << "First Fit:\n";
        	printMemoryStatus(successfulFirstFit, requestCount, memorySize, blocks);
        	std::cout << "Best Fit:\n";
        	printMemoryStatus(successfulBestFit, requestCount, memorySize, blocks);
        	std::cout << "Worst Fit:\n";
        	printMemoryStatus(successfulWorstFit, requestCount, memorySize, blocks);
        	std::cout << "Next Fit:\n";
        	printMemoryStatus(successfulNextFit, requestCount, memorySize, blocks);
    	}
	}

	return 0;
}
