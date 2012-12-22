#include <stdio.h>
#include "memory_tracker.hpp"


int main(int argc,char *argv[])
{
	MemoryTracker::setTrackStackTrace(true);
	int* i = new int;
	*i = 2;
	//delete i;
	MemoryTracker::printMemoryLeaks();
	getchar();
	return 0;
}