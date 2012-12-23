#ifndef BASIC_TYPES_HPP_
#define BASIC_TYPES_HPP_



#if (defined(__x86_64) || defined(__ia64) || defined(__ppc64__) || \
     defined(_WIN64) || defined(_M_IA64)) && \
    !(defined(_LP64) || defined(__lp64))
    typedef unsigned long long  address_type;
#else
    typedef unsigned long       address_type;
#endif

typedef unsigned char byte;
typedef unsigned int  uint;


#endif // BASIC_TYPES_HPP_