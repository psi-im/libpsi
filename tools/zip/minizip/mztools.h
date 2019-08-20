/*
  Additional tools for Minizip
  Code: Xavier Roche '2004
  License: Same as ZLIB (www.gzip.org)
*/

#ifndef _zip_tools_H
#define _zip_tools_H

#include "unzip.h"

#ifndef _ZLIB_H
#include "zlib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Repair a ZIP file (missing central directory)
   file: file to recover
   fileOut: output file after recovery
   fileOutTmp: temporary file name used for recovery
*/
extern int ZEXPORT unzRepair(const char* file,
                             const char* fileOut,
                             const char* fileOutTmp,
                             uLong* nRecovered,
                             uLong* bytesRecovered);

#ifdef __cplusplus
}
#endif

#endif // _zip_tools_H
