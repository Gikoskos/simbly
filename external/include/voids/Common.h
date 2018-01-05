 /********************
 *  Common.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


//LIBRARY INTERNAL HEADER. DO NOT INCLUDE THIS SEPARATELY.

#ifndef VOIDS_COMMON_H__
#define VOIDS_COMMON_H__

//if we're compiling on windows
#ifdef _WIN32

//if we're building the library and this header isn't
//just included from someone who uses the library
# ifdef BUILD_VOIDS_LIB

#  ifdef _MSC_VER //disable compiler warning bug on MSVC where it warns about
#   pragma warning(disable : 4127) //do{}while(0) macros
#  endif //_MSC_VER

#  ifdef EXPORT_API_VOIDS_DLL
#   define VOIDS_API __declspec(dllexport)
#  else //EXPORT_API_VOIDS_DLL
#   define VOIDS_API
#  endif //not EXPORT_API_VOIDS_DLL

//if the library is included on a project that uses it
# else //BUILD_VOIDS_LIB

#  ifdef IMPORT_API_VOIDS_DLL
#   define VOIDS_API __declspec(dllimport)
#  else //IMPORT_API_VOIDS_DLL
#   define VOIDS_API
#  endif //not IMPORT_API_VOIDS_DLL

# endif //not BUILD_VOIDS_LIB

#else //_WIN32

# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define VOIDS_API    __attribute__ ((visibility ("default")))
#  else
#   define VOIDS_API
#  endif
# else //__GNUC__
#  define VOIDS_API
# endif //not __GNUC__

#endif //not _WIN32

//global macros are library internal and shouldn't pollute the user's macro namespace
#ifdef BUILD_VOIDS_LIB

#define VOIDS_SALT 8999 //using a prime number as seed for xxhash and mersenne twister

#define SAVE_ERR(err, tmp_err) \
    if (err) \
        *(err) = tmp_err;

#endif //BUILD_VOIDS_LIB


#include <stddef.h>

//Key-value pair type definition
typedef struct _KeyValuePair {
    void *pData;
    void *pKey;
} KeyValuePair;

typedef void (*vdsUserDataFunc)(void *pData);
typedef int (*vdsUserCompareFunc)(const void *pKey1, const void *pKey2);
typedef size_t (*vdsUserHashFunc)(size_t key_hash, size_t array_len);

typedef enum _vdsTreeTraversal {
    VDS_PRE_ORDER,
    VDS_IN_ORDER,
    VDS_POST_ORDER,
    VDS_BREADTH_FIRST, //level-order
    VDS_EULER
} vdsTreeTraversal;

typedef enum _vdsHeapProperty {
    VDS_MAX_HEAP,
    VDS_MIN_HEAP
} vdsHeapProperty;

//libvoids library error codes
typedef enum _vdsErrCode {
    VDS_SUCCESS = 0x0,
    VDS_MALLOC_FAIL,
    VDS_KEY_EXISTS,
    VDS_BUFFER_FULL,
    VDS_BUFFER_EMPTY,
    VDS_INVALID_ARGS
} vdsErrCode;

//ErrorHandling.c functions are declared here instead of using another header file
VOIDS_API const char *VdsErrString(vdsErrCode err);

#endif //VOIDS_COMMON_H__
