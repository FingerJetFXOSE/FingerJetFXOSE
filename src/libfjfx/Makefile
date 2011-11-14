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

VERSION = ../version.mak
include $(VERSION)

ifdef DEBUG
DBGDIR = /debug
CXXFLAGS += -g -O0
else
CXXFLAGS += -O2  -Wl,--strip-all
endif

OUTDIR = ../../lib$(DBGDIR)
OUTFILE = libfjfx.so
VERSFILE = fjfx.vers
OUTPUT = $(OUTDIR)/$(OUTFILE).$(MAJOR).$(MINOR).$(REVISION)
LIBDIR = ../libFRFXLL/lib$(DBGDIR)
LIBS = -lFRFXLL

INCDIR = ../../include ../libFRFXLL/include
VPATH += $(LIBDIR)
CXXFLAGS += $(addprefix -I,$(INCDIR)) \
  -fno-exceptions -fno-rtti -fno-stack-protector -fPIC \
  -shared -nodefaultlibs -Wl,--version-script=$(VERSFILE) \
  -Wl,-soname,$(OUTFILE).$(MAJOR) \
  $(addprefix -L,$(LIBDIR)) -Wl,--whole-archive $(LIBS)
  
SRC = $(wildcard *.cpp)
HFILES = $(wildcard $(addsuffix /*.h, $(INCDIR) .))
OBJ = $(SRC:%.cpp=%.o)
DUMMY := $(shell test -d $(OUTDIR) || mkdir $(OUTDIR))

all : $(OUTPUT) $(OUTDIR)/$(OUTFILE).$(MAJOR) $(OUTDIR)/$(OUTFILE)

$(OUTPUT) : $(SRC) $(HFILES) Makefile $(VERSFILE); $(CXX) $(CXXFLAGS) $(SRC) -o $@
$(OUTDIR)/$(OUTFILE).$(MAJOR) : $(VERSION) ; @ln -fTsv $(OUTFILE).$(MAJOR).$(MINOR).$(REVISION) $(OUTDIR)/$(OUTFILE).$(MAJOR)
$(OUTDIR)/$(OUTFILE) : $(VERSION) ; @ln -fTsv $(OUTFILE).$(MAJOR) $(OUTDIR)/$(OUTFILE)

clean : ; @rm -f $(OUTPUT) $(OUTDIR)/$(OUTFILE).$(MAJOR) $(OUTDIR)/$(OUTFILE)

check : all ;

debug : ; @$(MAKE) all
debug : export DEBUG=1

.PHONY : all clean check debug
