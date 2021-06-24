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
      LIBRARY: fjfx - Fingerprint Feature Extractor

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#include <string.h>
#include "FJFX.h"
#include "FRFXLL.h"

#define CBEFF (0x00330502)

struct dpHandle {
  FRFXLL_HANDLE h;

  explicit dpHandle(FRFXLL_HANDLE _h = NULL) : h(_h) {}

  ~dpHandle() {
    if (h)
      Close();
  }

  FRFXLL_RESULT Close() {
    FRFXLL_RESULT rc = FRFXLL_OK;
    if (h) {
      rc = FRFXLLCloseHandle(&h);
    }
    h = NULL;
    return rc;
  }

  operator FRFXLL_HANDLE() const  { return h; }
  FRFXLL_HANDLE* operator &()     { return &h; }
};

#define Check(x, err) { if ((x) < FRFXLL_OK) return err; }
#define CheckFx(x)    Check(x, FJFX_FAIL_EXTRACTION_UNSPEC);


// Minutiae Extraction interface
int fjfx_create_fmd_from_raw(
  const void *raw_image,
  const unsigned short dpi,
  const unsigned short height,
  const unsigned short width,
  const unsigned int output_format,
  void   *fmd,
  unsigned int *size_of_fmd_ptr
) {
  if (fmd == NULL)       return FJFX_FAIL_EXTRACTION_UNSPEC;
  if (raw_image == NULL) return FJFX_FAIL_EXTRACTION_BAD_IMP;
  if (width > 2000 || height > 2000)                         return FJFX_FAIL_IMAGE_SIZE_NOT_SUP;
  if (dpi < 300 || dpi > 1024)                               return FJFX_FAIL_IMAGE_SIZE_NOT_SUP;
  if (width * 500 < 150 * dpi  || width * 500 > 812 * dpi)   return FJFX_FAIL_IMAGE_SIZE_NOT_SUP; // in range 0.3..1.62 in
  if (height * 500 < 150 * dpi || height * 500 > 1000 * dpi) return FJFX_FAIL_IMAGE_SIZE_NOT_SUP; // in range 0.3..2.0 in
  size_t size = size_of_fmd_ptr ? *size_of_fmd_ptr : FJFX_FMD_BUFFER_SIZE;
  if (size < FJFX_FMD_BUFFER_SIZE)                           return FJFX_FAIL_OUTPUT_BUFFER_IS_TOO_SMALL;
  FRFXLL_DATA_TYPE dt = 0;
  switch (output_format) {
    case FJFX_FMD_ANSI_378_2004:    dt = FRFXLL_DT_ANSI_FEATURE_SET; break;
    case FJFX_FMD_ISO_19794_2_2005: dt = FRFXLL_DT_ISO_FEATURE_SET; break;
    default:
      return FJFX_FAIL_INVALID_OUTPUT_FORMAT;
  }
  dpHandle hContext, hFtrSet;
  CheckFx( FRFXLLCreateLibraryContext(&hContext) );
  switch ( FRFXLLCreateFeatureSetFromRaw(hContext, reinterpret_cast<const unsigned char *>(raw_image), width * height, width, height, dpi, FRFXLL_FEX_ENABLE_ENHANCEMENT, &hFtrSet ) ) {
    case FRFXLL_OK: 
      break;
    case FRFXLL_ERR_FB_TOO_SMALL_AREA:
      return FJFX_FAIL_EXTRACTION_BAD_IMP;
    default: 
      return FJFX_FAIL_EXTRACTION_UNSPEC;
  }
  const unsigned short dpcm = (dpi * 100 + 50) / 254;
  const unsigned char finger_quality  = 60;  // Equivalent to NFIQ value 3 
  const unsigned char finger_position = 0;   // Unknown finger
  const unsigned char impression_type = 0;   // Live-scan plain
  FRFXLL_OUTPUT_PARAM_ISO_ANSI param = {sizeof(FRFXLL_OUTPUT_PARAM_ISO_ANSI), CBEFF, finger_position, 0, dpcm, dpcm, width, height, 0, finger_quality, impression_type};
  unsigned char * tmpl = reinterpret_cast<unsigned char *>(fmd);
  CheckFx( FRFXLLExport(hFtrSet, dt, &param, tmpl, &size) );
  if (size_of_fmd_ptr) *size_of_fmd_ptr = (unsigned int)size;
  CheckFx( FRFXLLCloseHandle(&hFtrSet) );
  CheckFx( FRFXLLCloseHandle(&hContext) );
  return FJFX_SUCCESS;
}

// Misc functions
int get_pid(unsigned int *feature_extractor) {
  *feature_extractor = CBEFF;
  return FJFX_SUCCESS;
}
