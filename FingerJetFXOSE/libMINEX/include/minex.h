/*
    Minex 3 Wrapper for FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2019 by HID Global, Inc. All rights reserved.

    HID, HID Global, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of HID Global, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
*/ 
/*
      LIBRARY:        fjfx_mnx - Minex Testable Fingerprint Feature Extractor

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
                      Greg Cannon

      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
                      Ralph Lessmann
                      Greg Cannon

      DATE:           6/6/2019
*/


#ifndef __fjfx_mnx_h
#define __fjfx_mnx_h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef FJFX_MNX_EXPORT
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define FJFX_MNX_EXPORT __attribute__((visibility("default")))
#else
#define FJFX_MNX_EXPORT __declspec(dllexport)
#endif
#endif // FJFX_EXPORT

#include "stdint.h"

// Error codes
#define MINEX_RET_SUCCESS 0
#define MINEX_RET_BAD_IMAGE_SIZE 1
#define MINEX_RET_FAILURE_UNSPECIFIED 2
#define MINEX_RET_FAILURE_BAD_IMPRESSION 3
#define MINEX_RET_FAILURE_NULL_TEMPLATE 4
#define MINEX_RET_FAILURE_BAD_VERIFICATION_TEMPLATE 5
#define MINEX_RET_FAILURE_BAD_ENROLLMENT_TEMPLATE 6

// Minutiae Extraction interface
int32_t FJFX_MNX_EXPORT create_template(
	const uint8_t *raw_image,
	const uint8_t finger_quality,
	const uint8_t finger_position,
	const uint8_t impression_type,
	const uint16_t height,
	const uint16_t width,
	uint8_t *output_template);

// Misc functions

int32_t FJFX_MNX_EXPORT get_pids(uint32_t *template_generator,uint32_t *template_matcher);
int32_t FJFX_MNX_EXPORT get_native_minutia_resolution(uint32_t *native_resolution);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __fjfx_mnx_h
