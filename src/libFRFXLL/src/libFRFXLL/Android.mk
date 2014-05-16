#   FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition
#
#   Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.
#
#   DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
#   or trademarks of DigitalPersona, Inc. in the United States and other
#   countries.
#
#   FingerJetFX OSE is open source software that you may modify and/or
#   redistribute under the terms of the GNU Lesser General Public License
#   as published by the Free Software Foundation, either version 3 of the 
#   License, or (at your option) any later version, provided that the 
#   conditions specified in the COPYRIGHT.txt file provided with this 
#   software are met.
#
#   For more information, please visit digitalpersona.com/fingerjetfx.

#
#      LIBRARY: fjfx - Fingerprint Feature Extractor
#
#      ALGORITHM:      Alexander Ivanisov
#                      Yi Chen
#                      Salil Prabhakar
#      IMPLEMENTATION: Alexander Ivanisov
#                      Jacob Kaminsky
#                      Lixin Wei
#      DATE:           11/08/2011
#
#      ANDROID BUILD SYSTEM FILES: Heiko Witte

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(call all-subdir-makefiles)

LOCAL_MODULE    := FRFXLL
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include/ \
                    $(LOCAL_PATH)/../algorithm

LOCAL_SRC_FILES := FRFXLLCreateContext.cpp \
                   FRFXLLCreateFeatureSet.cpp \
                   FRFXLLCreateFeatureSetFromRaw.cpp \
                   FRFXLLCreateFeatureSetInPlace.cpp \
                   FRFXLLCreateFeatureSetInPlaceFromRaw.cpp \
                   FRFXLLCreateLibraryContext.cpp \
                   FRFXLLDuplicateHandle.cpp \
                   FRFXLLExport.cpp \
                   FRFXLLGetLibraryVersion.cpp

include $(BUILD_STATIC_LIBRARY)