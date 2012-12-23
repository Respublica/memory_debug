#include <stdio.h>
#include "memory_tracker.hpp"


int main(int argc,char *argv[])
{
	MemoryTracker::instance().setTrackStackTrace(false);
	int* i = new int;
	*i = 2;
	//delete i;
	MemoryTracker::instance().printMemoryLeaks();
	getchar();
	return 0;
}