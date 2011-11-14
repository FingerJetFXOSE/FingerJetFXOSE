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

PROJECTS = src/libFRFXLL src/libfjfx samples/fjfxSample

.PHONY : all clean check $(PROJECTS)

all : $(PROJECTS)
clean: $(PROJECTS)

cleandebug :  $(PROJECTS)
cleandebug : export DEBUG=1
cleandebug : export MAKECMDGOALS=clean

check: $(PROJECTS)

checkdebug :  $(PROJECTS)
checkdebug : export DEBUG=1
checkdebug : export MAKECMDGOALS=check

debug : $(PROJECTS)
debug : export DEBUG=1
debug : export MAKECMDGOALS=all

$(PROJECTS) : ; $(MAKE) -C $@ $(MAKECMDGOALS)

src/libfjfx : src/libFRFXLL
samples/fjfxSample : src/libfjfx
