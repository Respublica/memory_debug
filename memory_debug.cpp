#ifdef _DEBUG


#include "memory_tracker.hpp"

#include <new>
#include <exception>



#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

void* operator new (std::size_t size, const char* file, int line)
{
    return MemoryTracker::debugAlloc(size, file, line);
}

void* operator new[] (std::size_t size, const char* file, int line)
{
    return operator new (size, file, line);
}

void* operator new (std::size_t size) throw(std::bad_alloc)
{
    return operator new (size, "", 0);
}

void* operator new[] (std::size_t size) throw(std::bad_alloc)
{
    return operator new (size, "", 0);
}

void* operator new (std::size_t size, const std::nothrow_t&) throw()
{
    return operator new (size, "", 0);
}

void* operator new[] (std::size_t size, const std::nothrow_t&) throw()
{
    return operator new (size, "", 0);
}

void operator delete (void* p) throw()
{
    MemoryTracker::debugFree(p);
}

void operator delete[] (void* p) throw()
{
    operator delete (p);
}

void operator delete (void* p, const char* file, int line) throw()
{
    operator delete (p);
}

void operator delete[] (void* p, const char* file, int line) throw()
{
    operator delete (p);
}

#ifdef _MSC_VER
#pragma warning( default : 4290 )
#endif


#endif // _DEBUG