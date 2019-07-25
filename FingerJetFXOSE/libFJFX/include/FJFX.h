/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.

    DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of DigitalPersona, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
    For more information, please visit digitalpersona.com/fingerjetfx.
*/ 
/*
      LIBRARY:        fjfx - Fingerprint Feature Extractor

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar

      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei

      DATE:           11/08/2011
*/

/*
This library provides code that takes a bitmap image of a fingerprint and 
extracts fingerprint features, creating fingerprint minutiae data that is 
formatted according to the ANSI INSITS 381-2004 or ISO/IEC 19794-4:2005 
specifications. The standards specify the header size and layout as well 
as the maximum number of minutiae that are stored (up to 256). 

Input:  Gray-scale image (square pixels), uncompressed

Output: Fingerprint minutiae data formatted per ANSI INSITS 381-2004 
        or ISO/IEC 19794-4:2005

Feature extraction works better with larger images. If your image is too small, 
you can pad it with whitespace to meet the minimum size requirement, but quality 
will be lower with smaller images.  If the image is too large, crop it so that 
the actual fingerprint is centered.

Some fingerprints contain a lot of features and will generate larger amounts 
of fingerprint minutiae data. The ISO/ANSI standards permit a maximum 
of 256 minutiae points.  For fingers with fewer features, the actual size 
of the data will be smaller.
*/

#ifndef __fjfx_h
#define __fjfx_h

#if __cplusplus >= 201402L
#define DEPRECATED [[deprecated]]
#else
#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#else
#define DEPRECATED
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef FJFX_EXPORT
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define FJFX_EXPORT __attribute__((visibility("default")))
#else
#define FJFX_EXPORT __declspec(dllexport)
#endif
#endif // FJFX_EXPORT


// Error codes
#define FJFX_SUCCESS                         (0)     // Extraction succeeded, minutiae data is in output buffer.
#define FJFX_FAIL_IMAGE_SIZE_NOT_SUP         (1)     // Failed. Input image size was too large or too small.
#define FJFX_FAIL_EXTRACTION_UNSPEC          (2)     // Failed. Unknown error.
#define FJFX_FAIL_EXTRACTION_BAD_IMP         (3)     // Failed. No fingerprint detected in input image.
#define FJFX_FAIL_INVALID_OUTPUT_FORMAT      (7)     // Failed. Invalid output record type - only ANSI INCIT 378-2004 or ISO/IEC 19794-2:2005 are supported.
#define FJFX_FAIL_OUTPUT_BUFFER_IS_TOO_SMALL (8)     // Failed. Output buffer too small. 

// Output fingerprint minutiae data format (per CBEFF IBIA registry)
#define FJFX_FMD_ANSI_378_2004        (0x001B0201)   // ANSI INCIT 378-2004 data format
#define FJFX_FMD_ISO_19794_2_2005     (0x01010001)   // ISO/IEC 19794-2:2005 data format

// Required output buffer size
#define FJFX_FMD_BUFFER_SIZE          (34 + 256 * 6) // Output data buffer must be at least this size, in bytes (34 bytes for header + 6 bytes per minutiae point, for up to 256 minutiae points)

// Minutiae Extraction interface
// we are deprecating this entire library - no need for it
DEPRECATED
int FJFX_EXPORT fjfx_create_fmd_from_raw(
  const void            *raw_image,             // Input: image to convert.  The image must be grayscale (8 bits/pixel), no padding, bright field (dark fingerprint on white background), scan sequence consistent with ISO/IEC 19794-4:2005.
  const unsigned short  pixel_resolution_dpi,   // Must be between 300 and 1024 dpi; the resolution must be the same for horizontal and vertical size (square pixels)
  const unsigned short  height,                 // Height of the input image, in pixels. Physical height must be between 0.3 inches (7.62 mm) and 1.6 inches (40.6 mm)
  const unsigned short  width,                  // Width of the input image, in pixels. Physical height must be between 0.3 inches (7.62 mm) and 1.5 inches (38.1 mm)
  const unsigned int    output_fmd_data_format, // FJFX_FMD_ANSI_378_2004 or FJFX_FMD_ISO_19794_2_2005
  void                  *fmd,                   // Where to store resulting fingerprint minutiae data (FMD)
  unsigned int          *size_of_fmd_ptr        // Input: fmd buffer size. Output: actual size of the FMD.
);

// Misc functions
// we are deprecating this entire library - no need for it
DEPRECATED
int FJFX_EXPORT fjfx_get_pid(unsigned int *feature_extractor); // Returns 2-byte vendor ID + 2-byte product ID.
                                                   // Standard biometric component identifier per CBEFF registry
                                                   // http://www.ibia.org/cbeff/iso/
                                                   // http://www.ibia.org/base/cbeff/_biometric_org.phpx

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __fjfx_h
