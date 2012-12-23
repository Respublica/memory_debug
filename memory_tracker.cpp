#include "memory_tracker.hpp"

#include <cstdio>
#include <cstdarg>

#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")
#endif

//------------------------------------------------------------------------------
/**
*/
MemoryTracker& MemoryTracker::instance()
{  
    static MemoryTracker instance;
    return instance;  
}

//------------------------------------------------------------------------------
/**
*/
MemoryTracker::MemoryTracker() :
memoryAllocationCount_(0),
memoryAllocations_(0)
{
}

//------------------------------------------------------------------------------
/**
*/
MemoryTracker::~MemoryTracker()
{
}

//------------------------------------------------------------------------------
/**
*/
void* MemoryTracker::debugAlloc(std::size_t size, const char* file, int line)
{
    // Allocate memory + size for a MemoryAlloctionRecord
    byte* mem = (byte*)malloc(size + sizeof(MemoryAllocationRecord));

    MemoryAllocationRecord* rec = (MemoryAllocationRecord*)mem;

    // Move memory pointer past record
    mem += sizeof(MemoryAllocationRecord);

    rec->address = (address_type)mem;
    rec->size = size;
    rec->file = file;
    rec->line = line;
    rec->next = memoryAllocations_;
    rec->prev = 0;

    // Capture the stack frame (up to kMaxStackFrames) 
	this->recordStackTrace(rec);

	this->list_mutex_.lock();
	// add our allocation record to our double linked list and
	// increment our internal counter in a thread safe maner
    if (this->memoryAllocations_)
        this->memoryAllocations_->prev = rec;
    this->memoryAllocations_ = rec;
    ++this->memoryAllocationCount_;

	this->list_mutex_.unlock();

    return mem;
}

//------------------------------------------------------------------------------
/**
*/
void MemoryTracker::debugFree(void* p)
{
    if (p == 0)
        return;

    // Backup passed in pointer to access memory allocation record
    void* mem = ((byte*)p) - sizeof(MemoryAllocationRecord);

    MemoryAllocationRecord* rec = (MemoryAllocationRecord*)mem;

    // Sanity check: ensure that address in record matches passed in address
    if (rec->address != (address_type)p)
    {
        printf("CORRUPTION: Attempting to free memory address with invalid memory allocation record.\n");
        return;
    }


    // Link this item out of our doublel linked list
	this->list_mutex_.lock();
    if (this->memoryAllocations_ == rec)
        this->memoryAllocations_ = rec->next;
    if (rec->prev)
        rec->prev->next = rec->next;
    if (rec->next)
        rec->next->prev = rec->prev;
    --this->memoryAllocationCount_;
	this->list_mutex_.unlock();

    // Free the address from the original alloc location (before mem allocation record)
    free(mem);
}

//------------------------------------------------------------------------------
/**
*/
#ifdef WIN32
void MemoryTracker::recordStackTrace(MemoryAllocationRecord* rec)
{
	// little trick to make sure we call SymInitialize only one time during tracking process
    static bool initialized = false;
    if (!initialized)
    {
		if (!SymInitialize(GetCurrentProcess(), NULL, true))
			printf("Stack trace tracking will not work.\n");

        initialized = true;
     }
    
	// Get the current context (state of EBP, EIP, ESP registers).
    static CONTEXT context;
    RtlCaptureContext(&context);
    
    static STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

        // Initialize the stack frame based on the machine architecture.
#ifdef _M_IX86
	static const DWORD machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
	#error "Machine architecture not supported!"
#endif

	// Walk up the stack and store the program counters.
    memset(rec->pc, 0, sizeof(rec->pc));
    for (int i = 0; i < kMaxStackFrames; i++)
    {
		rec->pc[i] = stackFrame.AddrPC.Offset;
        if (!StackWalk64(machineType, GetCurrentProcess(), GetCurrentThread(), &stackFrame, &context, NULL, SymFunctionTableAccess64,							 SymGetModuleBase64, NULL))
        {
			break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void MemoryTracker::printStackTrace(MemoryAllocationRecord* rec)
{
    const unsigned int bufferSize = 512;
    // Resolve the program counter to the corresponding function names.
    unsigned int pc;
    for (int i = 0; i < kMaxStackFrames; i++)
    {
        // Check to see if we are at the end of the stack trace.
        pc = rec->pc[i];
        if (pc == 0)
            break;

        // Get the function name.
        byte buffer[sizeof(IMAGEHLP_SYMBOL64) + bufferSize];
        IMAGEHLP_SYMBOL64* symbol = (IMAGEHLP_SYMBOL64*)buffer;
        DWORD64 displacement;
        memset(symbol, 0, sizeof(IMAGEHLP_SYMBOL64) + bufferSize);
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        symbol->MaxNameLength = bufferSize;
        if (!SymGetSymFromAddr64(GetCurrentProcess(), pc, &displacement, symbol))
        {
            printf("STACK TRACE: <unknown location>\n");
        }
        else
        {
            symbol->Name[bufferSize - 1] = '\0';

            // Check if we need to go further up the stack.
            if (strncmp(symbol->Name, "operator new", 12) == 0 ||
				strncmp(symbol->Name, "MemoryTracker::debugAlloc", 25) == 0 ||
				strncmp(symbol->Name, "MemoryTracker::debugFree", 24) == 0 )
            {
                // ignore theses symbols.
            }
            else
            {
                // Get the file and line number.
                if (pc != 0)
                {
                    IMAGEHLP_LINE64 line;
                    DWORD displacement;
                    memset(&line, 0, sizeof(line));
                    line.SizeOfStruct = sizeof(line);
                    if (!SymGetLineFromAddr64(GetCurrentProcess(), pc, &displacement, &line))
                    {
                        printf("STACK TRACE: %s - <unknown file>:<unknown line number>\n", symbol->Name);
                    }
                    else
                    {
                        const char* file = strrchr(line.FileName, '\\');
                        if(!file) 
                            file = line.FileName;
                        else
                            file++;
                        
                        printf("STACK TRACE: %s - %s:%d\n", symbol->Name, file, line.LineNumber);
                    }
                }
            }
        }
    }
}

#else // !WIN32

void MemoryTracker::recordStackTrace(MemoryAllocationRecord* rec)
{
}
	
void MemoryTracker::printStackTrace(MemoryAllocationRecord* rec)
{
}
#endif // #ifdef WIN32

//------------------------------------------------------------------------------
/**
*/
void MemoryTracker::printMemoryLeaks()
{
    // Dump general heap memory leaks
	if (this->allocation_count() == 0)
    {
        printf("All HEAP allocations successfully cleaned up (no leaks detected).\n");
    }
    else
    {
        printf("WARNING: %d HEAP allocations still active in memory.\n", this->allocation_count());

        MemoryAllocationRecord* rec = memoryAllocations_;
        while (rec)
        {
			printf("LEAK: HEAP allocation leak at address %#x of size %d:\n", rec->address, rec->size);
            printStackTrace(rec);
			rec = rec->next;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int MemoryTracker::allocation_count() const
{
	return this->memoryAllocationCount_;
}
