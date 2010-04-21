#pragma once 

#if 1
#define DBG(code) code 
#else
#define DBG(code) 
#endif

#define ASSERT(x) DBG(do { if (!(x)){ debug("assertion %s failed",#x); } } while(0))

#define debug(...) DBG(__dbg(__FUNCTION__, __LINE__, __VA_ARGS__))

void __dbg(const char *function, int lineno, const char *fmt, ...);
