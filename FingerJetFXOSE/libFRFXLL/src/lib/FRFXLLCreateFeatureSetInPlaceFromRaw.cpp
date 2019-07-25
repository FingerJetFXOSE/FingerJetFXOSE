/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2019 by HID Global, Inc. All rights reserved.

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

#include "CreateFtrSet.h"

typedef FeatureExtractionObj<Engine::FeatureExtractionInPlace> FexObj;
FRFXLL_RESULT FRFXLLCreateFeatureSetInPlaceFromRaw(
  FRFXLL_HANDLE hContext,          ///< [in] Handle to a fingerprint recognition context
  unsigned char fpData[],          ///< [in] fingerprint sample, buffer is overridden during feature extraction to save memory
  size_t size,                     ///< [in] size of the sample buffer
  unsigned int width,              ///< [in] width of the image
  unsigned int height,             ///< [in] heidht of the image
  unsigned int imageResolution,    ///< [in] image resolution [DPI]
  unsigned int flags,              ///< [in] Set to 0 for default or bitwise or of any of the FRFXLL_FEX_xxx flags
  FRFXLL_HANDLE_PT phFeatureSet    ///< [out] pointer to where to put an open handle to the feature set
) {
  if (fpData == NULL)  return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (phFeatureSet == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  CheckInvalidFlagsCombinationR(flags, FRFXLL_FEX_DISABLE_ENHANCEMENT | FRFXLL_FEX_ENABLE_ENHANCEMENT);
  Ptr<const Context> ctx(hContext);
  if (!ctx) return CheckResult(FRFXLL_ERR_INVALID_HANDLE);
  Ptr<FexObj> fex(new(ctx) FexObj(ctx));
  if (!fex) return CheckResult(FRFXLL_ERR_NO_MEMORY);

  // checks for width and height coupled to resolution - required for proper operation of algorithm
  if (imageResolution < FRFXLL_EXTRACT_MIN_DPI || imageResolution > FRFXLL_EXTRACT_MAX_DPI) return FRFXLL_ERR_INVALID_IMAGE;
  if (width * 500 < FRFXLL_EXTRACT_MIN_500WIDTH * imageResolution  || width * 500 > FRFXLL_EXTRACT_MAX_500WIDTH * imageResolution) return FRFXLL_ERR_INVALID_IMAGE; // in range 0.3..1.62 in
  if (height * 500 < FRFXLL_EXTRACT_MIN_500HEIGHT * imageResolution || height * 500 > FRFXLL_EXTRACT_MAX_500HEIGHT * imageResolution) return FRFXLL_ERR_INVALID_IMAGE; // in range 0.3..2.0 in

  return fex->CreateFeatureSet(fpData, size, width, height, imageResolution, flags, phFeatureSet);
}
 
