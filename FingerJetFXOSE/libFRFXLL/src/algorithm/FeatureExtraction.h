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

#ifndef __FEATUREEXTRACTION_H
#define __FEATUREEXTRACTION_H

#include "imresize.h"
#include "orimap.h"
#include "freeman.h"
#include "extract_minutia.h"
#include "matchData.h"
#include "FingerImageRecord.h"
#include "top_n.h"
#include "fft_enhance.h"

#define FRFXLL_EXTRACT_MIN_500WIDTH 196
#define FRFXLL_EXTRACT_MAX_500WIDTH 800
#define FRFXLL_EXTRACT_MIN_500HEIGHT 196
#define FRFXLL_EXTRACT_MAX_500HEIGHT 1000
#define FRFXLL_EXTRACT_MIN_DPI 300
#define FRFXLL_EXTRACT_MAX_DPI 1008

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {

  using namespace FeatureExtractionImpl;

  struct FeatureExtractionBase {
    static const size_t min_minutia = 6;

    static const size_t maxwidth = 256;
    static const size_t maxheight = 400;

    static const size_t int_resolution = 333;
    static const size_t int_resolution_min_in_place = 362; // ~= 333 * sqrt((4^2+3)/4^2)
    static const size_t ori_scale = 4;
    static const int32 imageScale = 127 * 5;
    // flags for reserved (flags) parameters
    static const uint32 flag_disable_fft_enhancement = 0x1;
    static const uint32 flag_enable_fft_enhancement  = 0x2;

    static const uint8 enh_block_bits = 5;
    static const int32 enh_spacing = 17;

    const uint8 * imgIn;
    uint8 * img;
    size_t height;
    size_t width;
    size_t size;

    int32 xOffs; // Phasemap offset
    int32 yOffs; // Phasemap offset

    uint8 * footprint;
    ori_t * ori;
    size_t ori_width;
    size_t ori_size;
    uint16 imageResolution; // In DPI
    uint8 readerCode;
    const FeatureExtractionImpl::Parameters & param;

    FeatureExtractionBase(const FeatureExtractionImpl::Parameters & param_)
      : imgIn(NULL)
      , img(NULL)
      , height(0)
      , width(0)
      , size(0)
      , xOffs(1)
      , yOffs(6)
      , footprint(NULL)
      , ori(NULL)
      , ori_width(0)
      , ori_size(0)
      , imageResolution(0)
      , readerCode(0)
      , param(param_)
    {}
    
    void SetOriSize() {
      ori_width = int((width - 1) / ori_scale + 1);
      size_t ori_height = int((size/width - 1) / ori_scale + 1);
      ori_size = ori_height * ori_width;
    }

    void WriteFootprint(Footprint & fp) {
      uint32 area = 0;
      for (int32 y = 0; y < fp.height; y++) {
        for (int32 x = 0; x < fp.width; x++) {
          size_t xi = reduce(((x * 8) * imageResolution + imageResolution / 2) / imageScale, 2);
          size_t yi = reduce(((y * 8) * imageResolution + imageResolution / 2) / imageScale, 2) * ori_width;
          bool b = (xi < ori_width) && (yi < ori_size) && (footprint[xi + yi]);
          if (b) area++;
          fp.Pixel(x,y) = b;
        }
      }
      fp.area = area * 8 * 8;
    }

    void RescaleMinutia(MatchData & md) {
      for (size_t i = 0; i < md.size(); i++) {
        md.minutia[i].position.x = int16(md.minutia[i].position.x * imageScale / imageResolution);
        md.minutia[i].position.y = int16(md.minutia[i].position.y * imageScale / imageResolution);
      }
    }

    FRFXLL_RESULT From333DpiImg(
      uint32 flags,
      size_t buffer_size,
      MatchData &md
    ) {
      ori = reinterpret_cast<ori_t *>(img + size);
      if ((flags & flag_enable_fft_enhancement) != 0) {
        size_t enh_buffer_size = size + max(ori_size * sizeof(ori_t), width << enh_block_bits);
        AssertR(enh_buffer_size + ori_size <= buffer_size, FRFXLL_ERR_INVALID_IMAGE); 
        footprint = img + enh_buffer_size;
        orientation_map_and_footprint<maxwidth, ori_scale>(width, size, img, true, NULL, footprint);
        if (! fft_enhance<enh_block_bits, enh_spacing>(img, width, size, enh_buffer_size)) {
          return FRFXLL_ERR_INTERNAL;
        }
        orientation_map_and_footprint<maxwidth, ori_scale>(width, size, img, false, ori, footprint);
      } else {
        footprint = img + size + ori_size * sizeof(ori_t);
        orientation_map_and_footprint<maxwidth, ori_scale>(width, size, img, true, ori, footprint);
      }
      freeman_phasemap<ori_scale>(width, size, img, ori, img);
      /* Patched limits of FRFXLL for NFIQ2 */
      /* original line: top_n<Minutia> top_minutia(md.minutia, md.minutia + std::min(md.capacity(), size_t(68))); */
      top_n<Minutia> top_minutia(md.minutia, md.minutia + md.capacity());
      extract_minutia<maxwidth, ori_scale>(img, width, size, footprint, top_minutia, param);
      md.numMinutia = top_minutia.size();
      top_minutia.sort();
      
      // this code fixes the angles to be ISO compliant (was in the serializer) 
      std::for_each(&md.minutia[0],&md.minutia[md.numMinutia], [](Minutia &m) {int theta = (int) m.theta; theta = -theta + 64; m.theta = (unsigned int) theta;});

      RescaleMinutia(md);

      // this code corrects for padding impact due to from the freeman phasemap (but its broken)
      std::for_each(&md.minutia[0],&md.minutia[md.numMinutia], [this](Minutia &m) {
		  m.position.x += int16(xOffs * imageScale / imageResolution);
		  m.position.y += int16(yOffs * imageScale / imageResolution);
      });
      
      WriteFootprint(md.footprint);  // this uses offset
      
      // this code has been moved from the serializer (it is broken, but works the same as in previous version)
      // we cannot rescale after shifting.. -- all rescaling has to be done before the uncrop... 
      // fixing this will break every expected feature in unit test... but we need to do it...
      // this code always scales back to 500 ppi - just as it did for every tested use case...
      // and it should really scale to image resolution...
      #define StdFmdDeserializer_Resolution 167	// this was a constant in deserializeFpData... (it needs to be fixed)
      std::for_each(&md.minutia[0],&md.minutia[md.numMinutia], [md](Minutia &m) {
//		  m.position.x = muldiv(m.position.x, 197, StdFmdDeserializer::Resolution);
//		  m.position.y = muldiv(m.position.y, 197, StdFmdDeserializer::Resolution);
		  m.position.x = muldiv(m.position.x, 197, StdFmdDeserializer_Resolution);
		  m.position.y = muldiv(m.position.y, 197, StdFmdDeserializer_Resolution);
      });

//      md.minutia_resolution_ppi = imageResolution;		// this is what it should be..
      md.minutia_resolution_ppi = 500;	// its currently broken!
      
      // ideally, this would return an error code of INSUFFICIENT_MINUTIA... (no new error codes for now)
      // the threshold should also be adjustable???
      if (md.numMinutia<min_minutia) return FRFXLL_ERR_FB_TOO_SMALL_AREA;

		// this is a measure of what the masking information found...
      if (   md.size() < param.user_feedback.minimum_number_of_minutia
          || md.footprint.area < param.user_feedback.minimum_footprint_area) {
        return FRFXLL_ERR_FB_TOO_SMALL_AREA;
      }
      return FRFXLL_OK;
    }

    template <class T_FIR>
    FRFXLL_RESULT ReadFIRHeader(
      const uint8 data[],             ///< [in] sample buffer
      size_t buffer_size              ///< [in] size of the sample buffer
    ) {
      T_FIR record(data, buffer_size);
      if (!record.IsValid()) return record;
      CheckR(Init(record.pData, buffer_size, record.Header.GetWidth(), record.Header.GetHeight(), record.Header.GetResolution()));
      CheckR(record.Header.GetDeviceCode(readerCode));
      return FRFXLL_OK;
    }

    FRFXLL_RESULT Init(
      const uint8 data[],             ///< [in] sample buffer
      size_t size_,                   ///< [in] size of the sample buffer
      uint32 width_,                  ///< [in] width of the image
      uint32 height_,                 ///< [in] heidht of the image
      uint32 imageResolution_         ///< [in] pixel resolution [DPI]
    ) {
      AssertR(size_ >= width_ * height_, FRFXLL_ERR_INVALID_IMAGE);

		// lets assure that input is ok...
      AssertR(imageResolution_ >= FRFXLL_EXTRACT_MIN_DPI, FRFXLL_ERR_INVALID_IMAGE);
      AssertR(imageResolution_ <= FRFXLL_EXTRACT_MAX_DPI, FRFXLL_ERR_INVALID_IMAGE);
      
      // lets see what scaled image dimensions would be...
      size_t width_at_500 = width_*500/imageResolution_;
      size_t height_at_500 = width_*500/imageResolution_;

      AssertR(width_at_500 >= FRFXLL_EXTRACT_MIN_500WIDTH, FRFXLL_ERR_INVALID_IMAGE);
      AssertR(height_at_500 >= FRFXLL_EXTRACT_MIN_500HEIGHT, FRFXLL_ERR_INVALID_IMAGE);

      AssertR(width_at_500 <= FRFXLL_EXTRACT_MAX_500WIDTH, FRFXLL_ERR_INVALID_IMAGE);
      AssertR(height_at_500 <= FRFXLL_EXTRACT_MAX_500HEIGHT, FRFXLL_ERR_INVALID_IMAGE);


      imgIn = data;
      width = width_;
      height = height_;
      size = width_ * height_;
      imageResolution = imageResolution_;
      xOffs = 1;
      yOffs = 6;
      return FRFXLL_OK;
    }

    FRFXLL_RESULT Resize_AnyTo333InPlaceOrBuffer(
      const uint8 in_img[],           ///< [in] input sample buffer, not preserved
      uint8 buffer[],                 ///< [] working buffer that is modified, can be the same as image buffer, or start before it
      size_t buffer_size              ///< [in] size of the working buffer
    ) {
      AssertR(width != 0 && size != 0, FRFXLL_ERR_INVALID_IMAGE);

      AssertR(height >= 32 && width >= 32, FRFXLL_ERR_INVALID_IMAGE);

      size_t in_width = width, in_height = height;
      if (imageResolution <= 550 && imageResolution >= 450) {
        // 500/333 = 1500/999 ~= 3/2
        // width  ~= in_width / (3/2) = in_width / 3 * 2 ~= in_width / 6 * 4
        width  = in_width  / 6 * 4; // width should be multiple of 4
        height = in_height / 6 * 4;
      } else {
        width  = (in_width  * int_resolution / imageResolution) & ~(ori_scale - 1);
        height = (in_height * int_resolution / imageResolution) & ~(ori_scale - 1);
      }

      if (height > maxheight) {        // center the image vertically
        size_t maxheight_in = (imageResolution <= 550 && imageResolution >= 450) ? maxheight * 3 / 2 : maxheight * imageResolution / int_resolution;
        size_t diff = in_height - maxheight_in;
        AssertR(diff > 0, FRFXLL_ERR_INTERNAL);
        in_img += (diff / 2) * in_width;
        yOffs += int32((height - maxheight) / 2);
        height = maxheight;
      }

      if (width > maxwidth) {        // center the image horizontally
        size_t maxwidth_in = (imageResolution <= 550 && imageResolution >= 450) ? maxwidth * 3 / 2 : maxwidth * imageResolution / int_resolution;
        size_t diff = in_width - maxwidth_in;
        AssertR(diff > 0, FRFXLL_ERR_INTERNAL);
        in_img += diff / 2;
        xOffs += int32((width - maxwidth) / 2);
        width = maxwidth;
      }

      size = width * height;

      AssertR(width != 0 && size != 0, FRFXLL_ERR_INVALID_IMAGE);

      SetOriSize();
      size_t buffer_size_needed = size + ori_size * (1 + sizeof(ori_t));
      AssertR(imageResolution >= int_resolution_min_in_place || in_img < buffer || in_img >= buffer + size, FRFXLL_ERR_INVALID_BUFFER); // FRFXLL_ERR_INVALID_IMAGE);
      //AssertR(imageResolution >= int_resolution || in_img < buffer || in_img >= buffer + size, FRFXLL_ERR_INVALID_BUFFER);//FRFXLL_ERR_INTERNAL); //FRFXLL_ERR_FB_TOO_SMALL_AREA);//
      AssertR(buffer_size_needed <= buffer_size, FRFXLL_ERR_INVALID_IMAGE); //FRFXLL_ERR_INVALID_BUFFER); // FRFXLL_ERR_FB_TOO_SMALL_AREA);//

      if (imageResolution <= 550 && imageResolution >= 450) {
        imresize23<maxwidth>(buffer, width, size, in_img, in_width);
      } else if (imageResolution <= int_resolution + 33 && imageResolution >= int_resolution - 33 && (in_width % 4) == 0) {
        memmove(buffer, in_img, size);
        imageResolution = muldiv(imageResolution, 5, 3);
      } else {
        size_t scale256 = imageResolution * size_t(256) / int_resolution;
        imresize(buffer, width, size, in_img, in_width, in_height, scale256);
        imageResolution = 500;
      }
      return FRFXLL_OK;
    }

    FRFXLL_RESULT ExtractMinutiae (
      const uint8 in_img[],           ///< [in] input sample buffer, not preserved
      uint8 buffer[],                 ///< [] working buffer that is modified, can be the same as image buffer, or start before it
      size_t buffer_size,             ///< [in] size of the working buffer
      uint32 flags,
      MatchData &md
    ) {
      CheckR(Resize_AnyTo333InPlaceOrBuffer(in_img, buffer, buffer_size));
      img = buffer;
      return From333DpiImg(flags, buffer_size, md);
    }
  };

  struct FeatureExtractionInPlace : public FeatureExtractionBase {

    FeatureExtractionInPlace(const FeatureExtractionImpl::Parameters & param_)
      : FeatureExtractionBase (param_)
    {}

    template <class T_FIR>
    FRFXLL_RESULT FromFIRSample(
      uint8 data[],                   ///< [in] sample buffer, modified to save memory
      size_t data_size,               ///< [in] size of the sample buffer
      uint32 flags,
      MatchData &md
    ) {
      CheckR(ReadFIRHeader<T_FIR>(data, data_size));
      AssertR(imageResolution >= int_resolution_min_in_place, FRFXLL_ERR_INVALID_IMAGE);
      return ExtractMinutiae(imgIn, data, data_size, flags, md);
    }

    FRFXLL_RESULT FromRawSample(
      uint8 data[],                   ///< [in] sample buffer
      size_t size_,                   ///< [in] size of the sample buffer
      uint32 width_,                  ///< [in] width of the image
      uint32 height_,                 ///< [in] heidht of the image
      uint32 imageResolution_,        ///< [in] pixel resolution [DPI]
      uint32 flags,
      MatchData &md
    ) {
      CheckR(Init(data, size_, width_, height_, imageResolution_));
      AssertR(imageResolution >= int_resolution_min_in_place, FRFXLL_ERR_INVALID_IMAGE);
      return ExtractMinutiae(imgIn, data, size_, flags, md);
    }

  };

  /// Contains the temporary buffer, so the size is larger, that's why we create a separate class
  struct FeatureExtraction : public FeatureExtractionBase {
    static const size_t maxsize = maxwidth * (maxheight + maxheight / 4);
    uint8 buffer[maxsize];

    FeatureExtraction(const FeatureExtractionImpl::Parameters & param_)
      : FeatureExtractionBase (param_)
    {
    }

    template <class T_FIR>
    FRFXLL_RESULT FromFIRSample(
      const uint8 data[],             ///< [in] input sample buffer, preserved
      size_t data_size,               ///< [in] size of the input sample buffer
      uint32 flags,
      MatchData &md
    ) {
      CheckR(ReadFIRHeader<T_FIR>(data, data_size));
      return ExtractMinutiae(imgIn, buffer, maxsize, flags, md);
    }

    FRFXLL_RESULT FromRawSample(
      const uint8 data[],             ///< [in] sample buffer
      size_t size_,                   ///< [in] size of the sample buffer
      uint32 width_,                  ///< [in] width of the image
      uint32 height_,                 ///< [in] heidht of the image
      uint32 imageResolution_,        ///< [in] pixel resolution [DPI]
      uint32 flags,                   ///< [in] select which features of the algorithm to use
      MatchData &md
    ) {
      CheckR(Init(data, size_, width_, height_, imageResolution_));
      return ExtractMinutiae(imgIn, buffer, maxsize, flags, md);
    }
  };

}
}
}

#endif // __FEATUREEXTRACTION_H

