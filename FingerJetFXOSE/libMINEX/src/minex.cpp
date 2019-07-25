/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2019 by HID Global, Inc. All rights reserved.

    HID Global, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of HID Global, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
    For more information, please visit digitalpersona.com/fingerjetfx.
*/


#include <string.h>
#include "FRFXLL.h"

#include <stdio.h>
#include <random>
#include "stdint.h"

#define DP_CBEFF (0x00330502)

// NIST expects that they receive a CENTOS 7.6 shared library
// it does not make sense to compile this for windows, android, etc...

#ifndef FJFX_MNX_EXPORT
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define FJFX_MNX_EXPORT __attribute__((visibility("default")))
#else
#define FJFX_MNX_EXPORT
#endif // GNUC
#endif // FJFX_EXPORT

// Error codes
#define MINEX_RET_SUCCESS 0
#define MINEX_RET_BAD_IMAGE_SIZE 1
#define MINEX_RET_FAILURE_UNSPECIFIED 2
#define MINEX_RET_FAILURE_BAD_IMPRESSION 3
#define MINEX_RET_FAILURE_NULL_TEMPLATE 4
#define MINEX_RET_FAILURE_BAD_VERIFICATION_TEMPLATE 5
#define MINEX_RET_FAILURE_BAD_ENROLLMENT_TEMPLATE 6

// name demangling
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int32_t FJFX_MNX_EXPORT create_template(
	const uint8_t *raw_image,
	const uint8_t finger_quality,
	const uint8_t finger_position,
	const uint8_t impression_type,
	const uint16_t height,
	const uint16_t width,
	uint8_t *output_template);

int32_t FJFX_MNX_EXPORT match_templates(
	const uint8_t *verification_template,
	const uint8_t *enrollment_template,
	float *similarity);

int32_t FJFX_MNX_EXPORT get_pids(
	uint32_t *template_generator,
	uint32_t *template_matcher);

int32_t FJFX_MNX_EXPORT get_native_minutia_resolution(
	uint32_t *native_resolution);

#ifdef __cplusplus
}
#endif // __cplusplus

static uint32_t map_fjet_errorcodes_to_minex3(FRFXLL_RESULT fjerr) {
	switch(fjerr) {
		case FRFXLL_OK:
			return MINEX_RET_SUCCESS;
		case FRFXLL_ERR_FB_TOO_SMALL_AREA:
			return MINEX_RET_BAD_IMAGE_SIZE;
		case FRFXLL_ERR_INVALID_IMAGE:
			return MINEX_RET_BAD_IMAGE_SIZE;
		default:
			return MINEX_RET_FAILURE_UNSPECIFIED;		
	}
	return MINEX_RET_FAILURE_UNSPECIFIED;		
}

// the minex test passes a buffer of a specified length...
// max # minutia is 128, and 32 bytes for ansi 378:2004 overhead
// the api does not request a size populated by client
// the size is obtained by examining the template itself...
// this is an ANSI 378:2004 template
// that means that the size is encoded in 16 bits at offset 8...
#define MINEX_BUFFER_LENGTH 32+128*6	// 800

int32_t create_template(
	const uint8_t *raw_image,
	const uint8_t finger_quality,
	const uint8_t finger_position,
	const uint8_t impression_type,
	const uint16_t height,
	const uint16_t width,
	uint8_t *output_template) {

//	printf("@@@ %s:%s:%d [WH %u %u]\n",__FILE__,__func__,__LINE__,width,height);
		
	if (raw_image==nullptr)	return MINEX_RET_FAILURE_UNSPECIFIED;
	if (output_template==nullptr)	return MINEX_RET_FAILURE_UNSPECIFIED;
	
	size_t img_size = height;
	img_size *= width;

	FRFXLL_HANDLE ctx,mobj;
	FRFXLL_RESULT rc = FRFXLLCreateLibraryContext(&ctx);

	rc = FRFXLLCreateFeatureSetFromRaw(ctx, raw_image, img_size, width, height, 500, FRFXLL_FEX_ENABLE_ENHANCEMENT,&mobj);
	if (rc!=FRFXLL_OK) {
		rc = FRFXLLCreateEmptyFeatureSet(ctx,&mobj);
		if (rc!=FRFXLL_OK) {
			return map_fjet_errorcodes_to_minex3(rc);
		}
	}
	
	FRFXLLCloseHandle(&ctx);
	size_t esize = MINEX_BUFFER_LENGTH;
	memset(output_template,0,MINEX_BUFFER_LENGTH);
	
	FRFXLL_OUTPUT_PARAM_ISO_ANSI format_params;
	format_params.length = sizeof(FRFXLL_OUTPUT_PARAM_ISO_ANSI);
	format_params.CBEFF = 0;	// required by MINEX
	format_params.fingerPosition = finger_position;
	format_params.viewNumber = 0;
	format_params.resolutionX = 197;
	format_params.resolutionY = 197;
	format_params.imageSizeX = width;
	format_params.imageSizeY = height;
	format_params.rotation = 0;
	format_params.fingerQuality = finger_quality;
	format_params.impressionType = impression_type;

	rc = FRFXLLExport(mobj, FRFXLL_DT_ANSI_FEATURE_SET, &format_params, output_template, &esize);
//	printf("encoded size=%zu,serialized size=%u\n",esize,(output_template[8]<<8)+output_template[9]);
	FRFXLLCloseHandle(&mobj);
  
	return map_fjet_errorcodes_to_minex3(rc);
}

int32_t match_templates(const uint8_t *verification_template, const uint8_t *enrollment_template, float *similarity) {
	*similarity = -1;
	return (0);
}

int32_t get_pids(
	uint32_t *template_generator,
	uint32_t *template_matcher) {
		*template_generator = DP_CBEFF;
		*template_generator = 0x0033FFFF;	// for now...  (I NEED A VERSION <> 5.2)
		*template_matcher = 0;
		return MINEX_RET_SUCCESS;
		
}

int32_t get_native_minutia_resolution(
	uint32_t *native_resolution) {
		*native_resolution = 333;
		return MINEX_RET_SUCCESS;
		
}
