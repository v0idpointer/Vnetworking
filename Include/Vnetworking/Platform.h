/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_PLATFORM_H_
#define _NE_PLATFORM_H_

#if defined(_WIN32)
#define NE_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#error "Vnetworking: Cannot build: macOS is not supported."
#elif defined(__linux__)
#error "Vnetworking: Cannot build: Linux is not supported."
#else
#error "Vnetworking: Cannot build: unknown platform."
#endif

#endif // _NE_PLATFORM_H_