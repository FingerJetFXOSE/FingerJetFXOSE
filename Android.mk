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
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/libFRFXLL

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
include $(call all-subdir-makefiles)

LOCAL_MODULE    := fjfx
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/libfjfx
LOCAL_STATIC_LIBRARIES := FRFXLL

include $(BUILD_SHARED_LIBRARY)