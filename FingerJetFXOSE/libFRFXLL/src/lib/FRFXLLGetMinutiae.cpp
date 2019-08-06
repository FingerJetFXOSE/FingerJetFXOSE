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

#include "FpDataObj.h"

FRFXLL_RESULT FRFXLLGetMinutiaInfo(
  const FRFXLL_HANDLE hFeatureSet, ///< [in] pointer to where to put an open handle to the minutia set
  unsigned int *num_minutia,        ///< [out] pointer to set number of extracted minutia
  unsigned int *resolution_ppi        ///< [out] pointer to set number of extracted minutia
) {
  if (hFeatureSet == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (num_minutia == NULL)  return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  *num_minutia = 0;

  // casting the FRFXLL_HANDLE into a FpFtrSetObj...
  Ptr<const FpFtrSetObj> ptr(hFeatureSet);
  const MatchData& md = ptr->fpFtrSet;	// just a reference to an existing object...
  *num_minutia = (unsigned int) md.numMinutia;  

  if (resolution_ppi != NULL)  *resolution_ppi = (unsigned int) md.minutia_resolution_ppi;  

  return FRFXLL_OK;
}

FRFXLL_RESULT FRFXLLGetMinutiae(
  const FRFXLL_HANDLE hFeatureSet,    ///< [in] pointer to where to put an open handle to the minutia set
  enum FRXLL_MINUTIAE_LAYOUT layout,  ///< [in] library casts the void * into the correct type based on layout (struct FRFXLL_Minutia*?)
  unsigned int *num_minutia,		  ///< [in/out] pointer to set/get number of extracted minutia
  void *mdata					      ///< [in/out] caller allocated... calloc of number minutira * sizeof minutia struct type
) {
  if (hFeatureSet == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (layout != BASIC_19794_2_MINUTIA_STRUCT) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (num_minutia == NULL)  return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (mdata == NULL)  return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  
  // casting the void* in accordance with the layout
  struct FRFXLL_Basic_19794_2_Minutia* minutia = (struct FRFXLL_Basic_19794_2_Minutia*) mdata;
    
  // casting the FRFXLL_HANDLE into a FpFtrSetObj...
  Ptr<const FpFtrSetObj> ptr(hFeatureSet);
  const MatchData& md = ptr->fpFtrSet;	// just a reference to an existing object...
  
  // limiting the number of minutia returned to the number minutia known by the hFeatureSet
  if (*num_minutia>md.numMinutia) *num_minutia = (unsigned int) md.numMinutia;

  // copying over the minutia data into the caller supplied memory as specified by layout...
  for (unsigned int i = 0; i< *num_minutia; i++) {
	// the minutia at this point (crazy as it sounds) is stored in 333 dpi (167 ppcm) native coordinate space...
	// clearly, this code should be removed from export and here, and placed back into FeatureExtraction.
	// its broken as well, as the export simply assumes that the output resolution is 197 ppcm (500 dpi)...
	// I am doing this stepwise however... - resampling (dithering) will be handled in FeatureExtraction soon...
	  
	// first the x and y, as the code currently works in Export
	unsigned int x = md.minutia[i].position.x;
	unsigned int y = md.minutia[i].position.y;

	  
//	DIAG_MSG(printf("@C@ %s:%d GM XYATQ %u %u %u %u %u\n",__FILE__,__LINE__,x,y,md.minutia[i].theta,md.minutia[i].type,FingerJetFxOSE::FpRecEngineImpl::Embedded::StdFmdSerializer::QualityFromConfidence(md.minutia[i].conf)));
    minutia[i].x = x;
    minutia[i].y = y;
    minutia[i].a = md.minutia[i].theta;
    switch (md.minutia[i].type) {
		case Minutia::type_ridge_ending:
			minutia[i].t = RIDGE_END;
			break;
		case Minutia::type_bifurcation:
			minutia[i].t = RIDGE_BIFURCATION;
			break;
		case Minutia::type_other:
			minutia[i].t = OTHER;
			break;
		default:
			minutia[i].t = OTHER;
			break;
	}
    minutia[i].q = FingerJetFxOSE::FpRecEngineImpl::Embedded::StdFmdSerializer::QualityFromConfidence(md.minutia[i].conf);
  }
  
  return FRFXLL_OK;	
}
