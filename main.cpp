#include <stdio.h>
#include "memory_debug.hpp"


int main(int argc,char *argv[])
{
	setTrackStackTrace(true);
	int* i = new int;
	*i = 2;
	//delete i;
	printMemoryLeaks();
	getchar();
	return 0;
}