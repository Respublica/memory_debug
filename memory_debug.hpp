#ifndef MEMORY_DEBUG_H_
#define MEMORY_DEBUG_H_

#ifdef _DEBUG
/**
 * Global overrides of the new and delete operators for memory tracking.
 *
 */

#include <new>
#include <exception>
//#include <cstddef>


#ifdef _MSC_VER
#pragma warning( disable : 4290 ) // C++ exception specification ignored.
#endif

// global new/delete operator overloads
void* operator new (std::size_t size, const char* file = __FILE__, int line = __LINE__);
void* operator new[] (std::size_t size, const char* file = __FILE__, int line = __LINE__);
void* operator new (std::size_t size) throw(std::bad_alloc);
void* operator new[] (std::size_t size) throw(std::bad_alloc);
void* operator new (std::size_t size, const std::nothrow_t&) throw();
void* operator new[] (std::size_t size, const std::nothrow_t&) throw();
void operator delete (void* p) throw();
void operator delete[] (void* p) throw();
void operator delete (void* p, const char* file = __FILE__, int line = __LINE__) throw();
void operator delete[] (void* p, const char* file = __FILE__, int line = __LINE__) throw();

#ifdef _MSC_VER
#pragma warning( default : 4290 )
#endif





#endif // #ifdef _DEBUG

#endif // MEMORY_DEBUG_H_