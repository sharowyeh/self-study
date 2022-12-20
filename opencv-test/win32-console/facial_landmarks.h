#pragma once

// NOTE: dlib static lib required preprocessor definitions as below:
#ifndef WIN32
#define WIN32
#endif
#ifndef DLIB_DISABLE_ASSERTS
#define DLIB_DISABLE_ASSERTS
#endif
#ifndef DLIB_JPEG_SUPPORT
#define DLIB_JPEG_SUPPORT
#endif
#ifndef DLIB_PNG_SUPPORT
#define DLIB_PNG_SUPPORT
#endif
#ifndef DLIB__CMAKE_GENERATED_A_CONFIG_H_FILE
#define DLIB__CMAKE_GENERATED_A_CONFIG_H_FILE
#endif
#ifndef DLIB_CHECK_FOR_VERSION_MISMATCH
#define DLIB_CHECK_FOR_VERSION_MISMATCH DLIB_VERSION_MISMATCH_CHECK__EXPECTED_VERSION_19_24_99
#endif
#ifndef DLIB_JPEG_STATIC
#define DLIB_JPEG_STATIC
#endif
#ifndef CMAKE_INTDIR
#define CMAKE_INTDIR "Debug"
#endif

int facial_landmarks();