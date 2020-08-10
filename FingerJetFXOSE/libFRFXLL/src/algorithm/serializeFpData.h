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

#ifndef __SERIALIZEFPDATA_H
#define __SERIALIZEFPDATA_H

#include <algorithm> // sort
#include "member_comp.h"
#include "dpfr.h"
#include "dpError.h"
#include "matchData.h"
#include "intmath.h"

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {

      struct FtrCode {
        static const uint16 DPFP_CODE_BITS_USED_MASK    = 0x8000;
        static const uint16 DPFP_ENCRYPTED_MASK         = 0x4000;
        static const uint16 DPFP_LITTLE_ENDIAN_MASK     = 0x2000;
        // DPFP_MORE_FEATURES_FOLLOW indicates that after the current template,
        // there are more.  The address of the next template should be calculated by
        // adding the address of compressedData and compressedDataLen + 1 (for the end marker).
        static const uint16 DPFP_MORE_FEATURES_FOLLOW   = 0x1000;
        static const uint16 DPFP_EXTENDED_FEATURES      = 0x0800;
        static const uint16 DPFP_ALLOW_LEARNING         = 0x0400;
        static const uint16 DPFP_XTEA_MASK              = 0x0200;
        static const uint16 DPFP_PLAIN = 47;      //These remain for backward compatibility.  See FP_ENCRYPTED_MASK and
        static const uint16 DPFP_ENCRYPTED = 74;  //FP_IS_ENCRYPTED below for current usage.
        uint16 value;

        bool BytesSwapped() const { // should work for any byte order
          if (value == DPFP_PLAIN || value == DPFP_ENCRYPTED) return false;
          if (value == (DPFP_PLAIN << 8) || value == (DPFP_ENCRYPTED << 8)) return true; 
          if (value & DPFP_CODE_BITS_USED_MASK) return false;
          if (value & (DPFP_CODE_BITS_USED_MASK >> 8)) return true;
          return false;
        }

        bool IsValid() const {
          if (value == DPFP_PLAIN || value == DPFP_ENCRYPTED) return true;
          return (value & DPFP_CODE_BITS_USED_MASK) != 0;
        }
        bool IsEncrypted() const { return (value & DPFP_CODE_BITS_USED_MASK) ? (value & DPFP_ENCRYPTED_MASK) != 0 : value != DPFP_PLAIN; }
        bool IsTeaEncrypted() const { return IsEncrypted() && !(value & DPFP_XTEA_MASK); }
        bool MoreViewsFollow() const { return (value & DPFP_MORE_FEATURES_FOLLOW) != 0; }
        bool HasEFBs() const { return (value & DPFP_EXTENDED_FEATURES) != 0; }
        FtrCode(uint16 code) : value(code) {}
        FtrCode() 
          : value(DPFP_CODE_BITS_USED_MASK | DPFP_ENCRYPTED_MASK | DPFP_LITTLE_ENDIAN_MASK | DPFP_EXTENDED_FEATURES) 
        {}
      };

      inline bool ValidFingerPosition(uint16 fingerPosition) { return fingerPosition <= 10; };
      inline bool ValidResolution(uint16 resolution) { return resolution >= 99 && resolution <= 1000; };
      inline bool ValidImageSize(uint16 imageSize) { return imageSize >= 16 && imageSize < (1<<14); };

      struct Writer : public HResult {
        unsigned char * data;
        size_t size;
        size_t cur;
        bool dataInBigEndian;
        bool reversed; // data written in the stream will be reversed

        Writer(unsigned char * data_,  size_t size_, bool dataInBigEndian_ = false)
          : data(data_), size(size_), cur(0), dataInBigEndian(dataInBigEndian_), reversed(false) {
        };
        virtual FRFXLL_RESULT putch(int32 c) { 
          if (!data) {
          } else if (eof()) {
            data = NULL;
          } else {
            data[cur] = uint8(c & 0xff);  
          }
          cur++;
          return GetResult();
        }
        virtual bool eof() const { return data ? cur >= size : false; }
        bool bad() const { return rc < FRFXLL_OK; }
        bool writeInBigEndian() const {
          return dataInBigEndian != reversed;
        }
        Writer & SetError(FRFXLL_RESULT rc_) {
          if (!bad()) rc = rc_;
          return *this;
        }
        Writer & operator << (uint8 x) {
          putch(x);
          return *this;
        }
        Writer & operator << (int8 x) {
          putch(x);
          return *this;
        }
        Writer & operator << (uint16 x) {
          if (writeInBigEndian()) {
            putch(x >> 8);
            putch(x);
          } else {
            putch(x);
            putch(x >> 8);
          }
          return *this;
        }
        Writer & operator << (int16 x) {
          return *this << (uint16)x;
        }
        Writer & operator << (uint32 x) {
          if (writeInBigEndian()) {
            *this << (uint16)(x >> 16) << (uint16)(x);
          } else {
            *this << (uint16)(x) << (uint16)(x >> 16);
          }
          return *this;
        }
        Writer & operator << (int32 x) {
          return *this << (uint32)x;
        }
      };
      struct Reversed {
        Writer & w;
        bool oldReversed;

        Reversed(Writer & w_, bool reversed = true) 
          : w(w_)
          , oldReversed(w_.reversed) 
        {
          w.reversed = reversed;
        }
        ~Reversed() { w.reversed = oldReversed; }
      };

      inline void Reverse(unsigned char data[], size_t size) {
        for (size_t i = 0; i < --size; i++) {
          unsigned char t = data[i];
          data[i] = data[size];
          data[size] = t;
        }
      }

      struct StdFmdSerializer {
        static const uint16 ImageSizeX = 250;
        static const uint16 ImageSizeY = 400;
        static const uint16 DefaultResolution = 197; // 500DPI / 2.54

        unsigned char * const dataOut;
        const size_t dataOutSize;
        size_t recordHeaderLength;
        size_t viewDataOffset;
        size_t viewDataLength;
        size_t viewEDBOffset;
        size_t viewEDBLength;

        uint32 CBEFF;
        uint16 fingerPosition;
        uint16 viewNumber;
        uint16 resolutionX;
        uint16 resolutionY;
        uint16 imageSizeX;
        uint16 imageSizeY;
        uint8  rotation;
        uint8  fingerQuality;
        uint8  impressionType;

        StdFmdSerializer(unsigned char * dataOut_,
                         const size_t & dataOutSize_,
                         bool & isExtended_)
          : dataOut(dataOut_)
          , dataOutSize(dataOutSize_)
        {
          viewDataOffset =  0;
          viewDataLength =  0;
          viewEDBOffset =   0;
          viewEDBLength =   0;
          CBEFF =           0x00330000;
          fingerPosition =  0;
          viewNumber =      0;
          resolutionX =     DefaultResolution;
          resolutionY =     DefaultResolution;
          imageSizeX  =     FRFXLL_IMAGE_SIZE_NOT_SPECIFIED;
          imageSizeY  =     FRFXLL_IMAGE_SIZE_NOT_SPECIFIED;
          rotation    =     0;
          fingerQuality  =  0;
          impressionType =  0; // live scan
        }
        static uint8 QualityFromConfidence(uint8 confidence) {
          return min<uint8>((confidence + 1) / 2, 100);
        }
        virtual void WriteTotalLength(Writer & wr, size_t len) {}; // in ISO 4 bytes in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
        virtual void WriteDeviceInfo(Writer & wr, const MatchData & md) {};
        virtual void WriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {};
        virtual void WriteTheta(Writer & wr, uint8 theta) {};
        void ReWriteTotalLength(Writer & wr, size_t len) {
          size_t saved = wr.cur;
          WriteTotalLength(wr, len);
          wr.cur = saved;
        }
        void ReWriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {
          size_t saved = wr.cur;
          WriteImageSize(wr, sizeX, sizeY);
          wr.cur = saved;
        }
        void ReWriteQuality(Writer & wr, uint8 quality) {
          size_t saved = wr.cur;
          wr.cur = viewDataOffset + 2;
          wr << quality;
          wr.cur = saved;
        }
        void WriteMinutiae(Writer & wr, const MatchData & md, unsigned int num) {
          uint8 minQ = 0xff;
          uint16 sizeX = 0;
          uint16 sizeY = 0;

          unsigned int mr_ppcm = muldiv(md.minutia_resolution_ppi,197,500);
                    
          for ( size_t i = 0; i < num; i++ ) {
            Point pos = md.minutia[i].position;
            
            if (resolutionX != mr_ppcm) {
				pos.x = muldiv(pos.x,resolutionX,mr_ppcm);
			}
            if (resolutionY != mr_ppcm) {
				pos.y = muldiv(pos.y,resolutionY,mr_ppcm);
			}

            if ( pos.x > sizeX ) sizeX = pos.x;
            if ( pos.y > sizeY ) sizeY = pos.y;

            wr << uint16((pos.x & 0x3FFF) | (md.minutia[i].type << 14));    // X
            wr << uint16(pos.y & 0x3FFF);                                   // Y
            WriteTheta(wr, md.minutia[i].theta);                            // Theta
            uint8 currQ = QualityFromConfidence(md.minutia[i].conf);
            wr << currQ;                                                    // Quality
            minQ = currQ < minQ ? currQ : minQ;
          }
          ReWriteImageSize(wr, imageSizeX != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED ? imageSizeX : (sizeX + 8), imageSizeY != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED ? imageSizeY : (sizeY + 8));
          ReWriteQuality(wr, minQ);          
        }
        void WriteRecordHeaders(Writer & wr, const MatchData & md) {
          wr.cur = 0;
          wr << uint32(0x464D5200);                                         // Format Identifier
          wr << uint32(0x20323000);                                         // Version of this standard
          WriteTotalLength(wr, 0);                                          // Length of total record in bytes
          WriteDeviceInfo(wr, md);                                          // Capture device/system information
          wr << uint16(imageSizeX);                                         // Image Size in X
          wr << uint16(imageSizeY);                                         // Image Size in Y
          wr << uint16(resolutionX);                                        // X (horizontal) Resolution
          wr << uint16(resolutionY);                                        // Y (vertical) Resolution
          wr << uint8(0);                                                   // Number of Finger Views
          wr << uint8(0);                                                   // Reserved byte
        }
        void WriteViewData(Writer & wr, const MatchData & md) {
          wr.cur = viewDataOffset;
          wr << uint8(fingerPosition);                                      // Finger Position
          wr << uint8((viewNumber<<4) + (impressionType & 0x0F));           // View Number
                                                                            // Impression Type
          wr << fingerQuality;                                              // Finger Quality
          size_t avail = wr.size - wr.cur;                                  // calculate available space
          size_t num = ( ( avail - 3 ) / 6 );                               // maximum minutae to write (already sorted by confidence)
          num = num < md.numMinutia ? num : md.numMinutia;                  // not more than the number of minutiae found, max 255 see MatchData
          wr << uint8(num);                                                 // Number of Minutiae
          WriteMinutiae(wr, md, num);                                       // 6 bytes per minutae
          WriteViewEDBHeaders(wr);                                          // 2 bytes
          size_t saved = wr.cur;
          wr.cur = viewDataOffset - 2;
          wr << uint8(1);                                                   // Number of Finger Views
          wr.cur = saved;
        }
        void WriteViewEDBHeaders(Writer & wr) {
          wr << uint16(viewEDBLength);                                      // Extended Data Block Length
          //wr << uint8(0x01) << uint8(0x01);                               // Extended Data Area Type Code
          //wr << uint16(0);                                                // Extended Data Area Length
        }
        void ComputeLengthAndOffset(size_t & size, const MatchData & md) {
          viewDataOffset = recordHeaderLength;
          viewDataLength = 6 + 6 * md.numMinutia; // 4 = view header length; 2 = EDB Length length
          viewEDBOffset = viewDataOffset + viewDataLength;
          viewEDBLength = 0;
          size = recordHeaderLength + viewDataLength + viewEDBLength;
        }
        FRFXLL_RESULT UnpackParameters(const void * parameters) {
          if ( NULL == parameters ) return FRFXLL_OK;
          const FRFXLL_OUTPUT_PARAM_ISO_ANSI * pParams = reinterpret_cast<const FRFXLL_OUTPUT_PARAM_ISO_ANSI *>(parameters);
          if ( NULL == pParams ) return FRFXLL_ERR_INVALID_PARAM;
          uint8 version = 0;
          switch ( pParams->length ) {
            case sizeof(FRFXLL_OUTPUT_PARAM_ISO_ANSI):
              version = 1;
              break;
            default:
              return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1 && pParams->CBEFF != FRFXLL_CBEFF_NOT_SPECIFIED) CBEFF = pParams->CBEFF;
          if (version >= 1 && pParams->fingerPosition != FRFXLL_FINGER_POSITION_NOT_SPECIFIED) fingerPosition = pParams->fingerPosition;
          if ( !ValidFingerPosition(fingerPosition) ) return FRFXLL_ERR_INVALID_PARAM;
          if (version >= 1 && pParams->viewNumber != FRFXLL_VIEW_NUMBER_NOT_SPECIFIED)   viewNumber = pParams->viewNumber;
          if (version >= 1 && pParams->resolutionX != FRFXLL_RESOLUTION_NOT_SPECIFIED) resolutionX = pParams->resolutionX;
          if ( !ValidResolution(resolutionX) ) return FRFXLL_ERR_INVALID_PARAM;
          if (version >= 1 && pParams->resolutionY != FRFXLL_RESOLUTION_NOT_SPECIFIED) resolutionY = pParams->resolutionY;
          if ( !ValidResolution(resolutionY) ) return FRFXLL_ERR_INVALID_PARAM;
          if (version >= 1 && pParams->imageSizeX != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED) {
            imageSizeX = pParams->imageSizeX;
            if ( !ValidImageSize(imageSizeX) ) return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1 && pParams->imageSizeY != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED) {
            imageSizeY = pParams->imageSizeY;
            if ( !ValidImageSize(imageSizeY) ) return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1) rotation = pParams->rotation;
          if (version >= 1) {
            if (pParams->fingerQuality > 100) return FRFXLL_ERR_INVALID_PARAM;
            fingerQuality = pParams->fingerQuality;
          }
          if (version >= 1) {
            if (pParams->impressionType > 8)  return FRFXLL_ERR_INVALID_PARAM;
            impressionType = pParams->impressionType;
          }
          return FRFXLL_OK;
        }
        FRFXLL_RESULT SerializeFeatureSet(
          size_t & size,
          const MatchData & md0,
          const void * parameters,
          bool fixedSize,
          FtrCode ftrCode
        ) {
          FRFXLL_RESULT rc = UnpackParameters(parameters);
          if ( rc < FRFXLL_OK ) return rc;
          MatchData md(md0);
          if (rotation) {
            md.RotateAndShift(rotation);
          }

          ComputeLengthAndOffset(size, md);
          if ( NULL == dataOut ) return FRFXLL_OK;
          if ( dataOutSize < recordHeaderLength ) return FRFXLL_ERR_MORE_DATA;

          size_t   totalLength = 0;
          Writer   wr(dataOut, dataOutSize, true);
          rc = FRFXLL_ERR_MORE_DATA;
          WriteRecordHeaders(wr, md);
          WriteViewData(wr, md);
          rc = size > wr.cur ? FRFXLL_ERR_MORE_DATA : FRFXLL_OK;
          size = wr.cur;
          ReWriteTotalLength(wr, size); // in ISO 4 bytes; in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
          return rc;
        }
      };

      struct IsoFmdSerializer : public StdFmdSerializer {
        IsoFmdSerializer(unsigned char * dataOut_,
                         const size_t & dataOutSize_,
                         bool & isExtended_)
                         : StdFmdSerializer(dataOut_, dataOutSize_, isExtended_) {
          recordHeaderLength = 24;
        }
        virtual void WriteTotalLength(Writer & wr, size_t len) { // in ISO 4 bytes
          wr.cur = 8;
          wr << uint32(len);
        }
        virtual void WriteDeviceInfo(Writer & wr, const MatchData & md) {
          wr << uint8(0) << uint8(0);
        }
        virtual void WriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {
          wr.cur = 14;
          wr << sizeX;
          wr << sizeY;
        }
        virtual void WriteTheta(Writer & wr, uint8 theta) {
          wr << uint8(theta);
        }
      };

      struct AnsiFmdSerializer : public StdFmdSerializer {
        AnsiFmdSerializer(unsigned char * dataOut_,
                          const size_t & dataOutSize_,
                          bool & isExtended_)
                          : StdFmdSerializer(dataOut_, dataOutSize_, isExtended_) {
          recordHeaderLength = 26;
        }
        virtual void WriteTotalLength(Writer & wr, size_t len) { // in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
          wr.cur = 8;
          wr << uint16(len); // 2 bytes should be enough
        }
        virtual void WriteDeviceInfo(Writer & wr, const MatchData & md) {
          wr << CBEFF;                                      // CBEFF product identifier
          wr << uint8(0) << uint8(0);
        }
        virtual void WriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {
          wr.cur = 16;
          wr << sizeX;
          wr << sizeY;
        }
        virtual void WriteTheta(Writer & wr, uint8 theta) {
          theta = muldiv(theta, 180, 256);
          wr << uint8(theta);
        }
      };

      inline FRFXLL_RESULT WriteIsoFeatures(
        unsigned char dataOut[], 
        size_t & size, 
        const MatchData & md,
        const void * parameters,
        bool isExtended,
        bool fixedSize = false,
        bool isTemplate = false,
        FtrCode ftrCode = FtrCode()
      ) {
        IsoFmdSerializer isoFS(dataOut, size, isExtended);
        return isoFS.SerializeFeatureSet(size, md, parameters, fixedSize, ftrCode);
      }

      inline FRFXLL_RESULT WriteAnsiFeatures(
        unsigned char dataOut[], 
        size_t & size, 
        const MatchData & md, 
        const void * parameters,
        bool isExtended,
        bool fixedSize = false,
        bool isTemplate = false,
        FtrCode ftrCode = FtrCode()
      ) {
        AnsiFmdSerializer ansiFS(dataOut, size, isExtended);
        return ansiFS.SerializeFeatureSet(size, md, parameters, fixedSize, ftrCode);
      }

    }
  }
}
#endif
