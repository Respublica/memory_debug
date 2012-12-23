#ifndef MEMORY_TRACKER_HPP_
#define MEMORY_TRACKER_HPP_


#include <cstddef> // std::size_t
#include <mutex>
#include "base.hpp"


class MemoryTracker
{
public:
	static const int kMaxStackFrames = 16; // maximum number of stack's frame allowed to capture

	struct MemoryAllocationRecord
	{
		address_type address;			   // address returned to the caller after allocation
		uint size;						   // size of the allocation request
		const char* file;                  // source file of allocation request
		int line;                          // source line of the allocation request
		MemoryAllocationRecord* next;      // pointer to next record in our d-linked list
		MemoryAllocationRecord* prev;      // pointer to previous record in our d-linked list
		uint pc[kMaxStackFrames];		   // captured frames of the allocation request
	};

	// default constructor
	MemoryTracker();
	// destructor
	~MemoryTracker();
	void printMemoryLeaks();

	void* debugAlloc(std::size_t size, const char* file, int line);
	void  debugFree(void* p);

	int allocation_count() const;
	static MemoryTracker& instance();
private:
	// dissalow class copy and assignment
	MemoryTracker(const MemoryTracker&);    
	MemoryTracker &operator=(const MemoryTracker&);

	void printStackTrace(MemoryAllocationRecord* rec);
	void recordStackTrace(MemoryAllocationRecord* rec);

	MemoryAllocationRecord* memoryAllocations_;
	int memoryAllocationCount_;
	std::mutex list_mutex_;
};



#endif // MEMORY_TRACKER_HPP_