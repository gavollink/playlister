/****************************************************************************
 * utils.h
 *
 * String manipulation, etc.
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see LICENSE file in this distribution.
 */
#ifndef UTILS_H
#define UTILS_H 1
#include <stdlib.h>      // malloc, realloc, rand, size_t
#include <locale.h>      // setlocale()
#include <dirent.h>      // opendir(), readdir()
#include "utarray.h"
#include "configure.h"   // Created by Makefile from CONFIGURE.h

/***************************************
 * Dan J. Bernstien code (Public Domain) -- included in this package.
 *    https://cr.yp.to/software.html
 */
#include "djb/str.h"

/* THIS SHOULD NEVER BE MORE THAN 63 strlen()  */
#ifndef PLAYLISTER_VERSION
#define PLAYLISTER_VERSION "1.02.00"
#endif

#if 1 == HAS_ARC4RANDOM     /* DEFINED (or not) in configure.h */
#include <bsd/stdlib.h>
#define configrandseed()
#define configrand() arc4random()
#elif 1 == HAS_SRANDDEV     /* DEFINED (or not) in configure.h */
#define configrandseed() sranddev()
#define configrand() rand()
#else
#include <time.h>        // time()
#define configrandseed() srand(time(NULL))
#define configrand() rand()
#endif /* HAS_SRANDOMDEV | HAS_SRANDDEV */

enum LogLevel {
    FATAL,
    ERROR,
    WARN,
    INFO,
    VERBOSE,
    DEBUG,
    DUMP
};

#ifndef UTILS_C
    extern const char * const LogString[];
#endif

void     initUtils       (void);
char   * replaceString   (char *str, const char *search
                            , const char *replace, size_t ssz);
char   * prependString   (char *str, const char *pre, size_t ssz);
int      removeStringIdx (char *str, size_t index, size_t ssz);
int      removeString    (char *str, const char *needle, size_t ssz);
char   * replCharString  (char *str, const char out
                            , const char in, size_t ssz);
char   * replHexString   (char *str, const char out
                            , const char in, size_t ssz);
int      URIunescape     (char *str);
char   * checkFileExists (char *filename, size_t fnamesize);
char   * tryFindMatch    (char *filename, char *portion);
void     randomUTarray   (UT_array *orig);
void     myfatal         (const char* text, ...);
void     myerror         (const char* text, ...);
void     mywarning       (const char* text, ...);
void     myprint         (const char* text, ...);
void     mydebug         (const char* text, ...);
void     extradebug      (const char* text, ...);
void     superdebug      (const char* text, ...);
char   * str_strn        (const char *haystack, const char *needle
                            , size_t len);

#endif /* UTILS_H */
/**
 * vim: sw=4 ts=4 expandtab
 * EOF: utils.h
 */
