#ifndef MEMORY_TRACKER_H_
#define MEMORY_TRACKER_H_


#include <cstddef> // std::size_t


class MemoryTracker
{
public:
	static const int kMaxStackFrames = 16; // maximum number of stack's frame allowed to capture

	struct MemoryAllocationRecord
	{
		unsigned long address;			   // address returned to the caller after allocation
		unsigned int size;                 // size of the allocation request
		const char* file;                  // source file of allocation request
		int line;                          // source line of the allocation request
		MemoryAllocationRecord* next;      // pointer to next record in our d-linked list
		MemoryAllocationRecord* prev;      // pointer to previous record in our d-linked list
		unsigned int pc[kMaxStackFrames];  // captured frames of the allocation request
	};

	static void printMemoryLeaks();
	static void setTrackStackTrace(bool trackStackTrace);
	static void toggleTrackStackTrace();

	static void* debugAlloc(std::size_t size, const char* file, int line);
	static void  debugFree(void* p);
private:
	// dissalow class copy and assignment
	MemoryTracker(const MemoryTracker&);    
	MemoryTracker &operator=(const MemoryTracker&);
	static void printStackTrace(MemoryAllocationRecord* rec);

	static bool trackStackTrace_; 
	static MemoryAllocationRecord* memoryAllocations_;
	static int memoryAllocationCount_;
};








#endif // MEMORY_TRACKER_H_