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
      BINARY: fjfxSample - Sample Code for Fingerprint Feature Extractor

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <FJFX.h>

int main(int argc, char ** argv) {
  FILE *fp = 0;
  int height, width, gray;
  unsigned int size;
  void * image = 0;
  size_t n;
  int err;
  unsigned char tmpl[FJFX_FMD_BUFFER_SIZE] = {0};
  
  if(argc != 3 || argv[1] == NULL || argv[2] == NULL) {
    printf("Fingerprint Minutia Extraction\n"
           "Usage: %s <image.pgm> <fmd.ist>\n"
           "where <image.pgm> is the binary PGM (P5) file that containing 500DPI 8-bpp grayscale figerprint image\n"
           "      <fmd.ist> is the file where to write fingerprint minutia data in ISO/IEC 19794-2 2005 format\n", argv[0]);
    return -1;
  }

  fp = fopen(argv[1], "rb");
  if (fp == 0) {
    printf("Cannot open image file: %s\n", argv[1]);
    return 9;
  }
  n = fscanf(fp, "P5%d%d%d", &width, &height, &gray); 
  if (n != 3 || 
      gray > 256 || width > 0xffff || height > 0xffff || 
      gray <= 1 || width < 32 || height < 32) {
    printf("Image file %s is in unsupported format\n", argv[1]);
    fclose(fp);
    return 10;
  }
  
  size = width * height;
  image = malloc(size);
  if (image == 0) {
    printf("Cannot allocate image buffer: image size is %dx%d", width, height);
    if(fp != 0) {
      fclose(fp); fp = 0;
    }
    return 12;
  }
  
  n = fread(image, 1, size, fp);
  fclose(fp); fp = 0;
  if (n != size) {
    printf("Image file %s is too short\n", argv[1]);
    free(image);
    return 11;
  }

  size = FJFX_FMD_BUFFER_SIZE;
  err = fjfx_create_fmd_from_raw(image, 500, height, width, FJFX_FMD_ISO_19794_2_2005, tmpl, &size);
  free(image); image = 0;
  if (err != FJFX_SUCCESS) {
    printf("Failed feature extraction\n");
    return err;
  }
  
  fp = fopen(argv[2], "wb");
  if (fp == 0) {
    printf("Cannot create output file: %s\n", argv[2]);
    return 14;
  }
  n = fwrite(tmpl, 1, size, fp);
  fclose(fp);
  if (n != size) {
    printf("Cannot write output file of size %d\n", (int)size);
    free(image);
    return 15;
  }
  return 0;
}
