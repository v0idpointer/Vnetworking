/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_EXPORTS_H_
#define _NE_EXPORTS_H_

#ifdef NE_BUILD_CORE_DLL
#define VNETCOREAPI __declspec(dllexport)
#else
#define VNETCOREAPI __declspec(dllimport)
#endif

#ifdef NE_BUILD_HTTP_DLL
#define VNETHTTPAPI __declspec(dllexport)
#else
#define VNETHTTPAPI __declspec(dllimport)
#endif

#pragma warning (disable: 4251)
#pragma warning (disable: 4275)

#endif // _NE_EXPORTS_H_