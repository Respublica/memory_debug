#ifndef MEMORY_DEBUG_H_
#define MEMORY_DEBUG_H_

#ifdef _DEBUG
/**
 * Global overrides of the new and delete operators for memory tracking.
 *
 */
#include <new>
#include <exception>

// Prints all heap and reference leaks to stdout.
extern void printMemoryLeaks();

#ifdef _MSC_VER
#pragma warning( disable : 4290 ) // C++ exception specification ignored.
#endif

// global new/delete operator overloads
void* operator new (std::size_t size, const char* file, int line);
void* operator new[] (std::size_t size, const char* file, int line);
void* operator new (std::size_t size) throw(std::bad_alloc);
void* operator new[] (std::size_t size) throw(std::bad_alloc);
void* operator new (std::size_t size, const std::nothrow_t&) throw();
void* operator new[] (std::size_t size, const std::nothrow_t&) throw();
void operator delete (void* p) throw();
void operator delete[] (void* p) throw();
void operator delete (void* p, const char* file, int line) throw();
void operator delete[] (void* p, const char* file, int line) throw();

#ifdef _MSC_VER
#pragma warning( default : 4290 )
#endif

// Re-define new to use versions with file and line number so it's make everything transparent :)
#define DEBUG_NEW new (__FILE__, __LINE__)
#define new DEBUG_NEW

#if defined(WIN32)
// Sets whether stack traces are tracked on memory allocations or not.
void setTrackStackTrace(bool trackStackTrace);
// Toggles stack trace tracking on memory allocations.
void toggleTrackStackTrace();
#endif // #if defined(WIN32)

#endif // #ifdef _DEBUG
#endif // MEMORY_DEBUG_H_