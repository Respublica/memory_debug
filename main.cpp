#include "memory_tracker.hpp"
#include <iostream>
#include <fstream>

	
std::ofstream log_file;

void exampleLog(const std::string& msg)
{
	std::cout << msg.c_str() << "\n";
	log_file << msg.c_str() << "\n";
	log_file.flush();
}

int main(int argc,char *argv[])
{
	log_file.open ("example.txt");
	if(log_file.fail()) {
		std::cerr << "Failed to open log file\n";
		exit(1);
	}
	MemoryTracker::instance().setLogFunction(exampleLog);

	int* i = new int;
	*i = 2;
	//delete i;
	MemoryTracker::instance().printMemoryLeaks();
	log_file.close();
	getchar();
	return 0;
}