#pragma once

#ifdef WIN32
   #ifdef mscAlgorithmPaint_EXPORTS
       #define MSCALGORITHMPAINT_EXPORT __declspec(dllexport)
   #else
       #define MSCALGORITHMPAINT_EXPORT __declspec(dllimport)
   #endif
#else
   #define MSCALGORITHMPAINT_EXPORT
#endif
