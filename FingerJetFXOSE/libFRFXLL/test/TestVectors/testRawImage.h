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
      BINARY: FRFXLLSample - Sample Code for Fingerprint Feature Extractor - LowLevel API
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __TESTRAWIMAGE_H
#define __TESTRAWIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

struct raw_image {
	unsigned int width;
	unsigned int height;
	unsigned int resolution;
	unsigned char pixels[];
};

extern struct raw_image test_raw_image_500;
extern struct raw_image test_raw_image_333;

extern const unsigned char *ftrs_E_04_08_01[4]; // consintent set of feature sets
extern const unsigned char *ftrsFromDiffFingers0[4]; // inconsintent set of feature sets

#ifdef __cplusplus
}
#endif

#endif // __TESTRAWIMAGE_H
