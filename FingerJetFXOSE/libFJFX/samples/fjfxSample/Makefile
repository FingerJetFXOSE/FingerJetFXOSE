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
#      BINARY: fjfxSample - Sample Code for Fingerprint Feature Extractor
#
#      ALGORITHM:      Alexander Ivanisov
#                      Yi Chen
#                      Salil Prabhakar
#      IMPLEMENTATION: Alexander Ivanisov
#                      Jacob Kaminsky
#                      Lixin Wei
#      DATE:           11/08/2011
#

ifdef DEBUG
DBGDIR = /debug
CXXFLAGS += -g -O0
else
CXXFLAGS += -O2
endif

OUTDIR = ../../bin$(DBGDIR)
OUTPUT = $(OUTDIR)/fjfxSample
LIBDIR = ../../lib$(DBGDIR)
LIBS = -lfjfx
INCDIR = ../../include
VPATH += $(LIBDIR)
CFLAGS += -g -O0
CFLAGS += $(addprefix -I, $(INCDIR)) $(addprefix -L, $(LIBDIR)) $(LIBS) 
SRC = $(wildcard *.c)
HFILES = $(wildcard $(addsuffix /*.h, $(INCDIR) .))
DUMMY := $(shell test -d $(OUTDIR) || mkdir $(OUTDIR))
#OBJ = $(SRC:%.cpp=%.o)
#$(OUTPUT)($(OBJ)) : $(HFILES)
PGMDIR = ../images-pgm
ISODIR = ../templates-iso
PGM = $(wildcard $(PGMDIR)/*.pgm)
IST = $(PGM:$(PGMDIR)/%.pgm=$(OUTDIR)/%.ist)

all : $(OUTPUT)
check : all $(IST) ;

$(OUTDIR)/%.ist : $(PGMDIR)/%.pgm $(OUTPUT) ; @echo checking regression on $(<F) ; \
   LD_LIBRARY_PATH=$(LIBDIR) ./$(OUTPUT) $<  $@; \
   cmp $@ $(ISODIR)/$(@F); \
   rm $@

$(OUTDIR)/a999_01p.ist : $(PGMDIR)/a999_01p.pgm $(OUTPUT) ; @echo checking regression on $(<F) ;\
   [ "`LD_LIBRARY_PATH=$(LIBDIR) ./$(OUTPUT) $<  $@` = 'Failed feature extraction'" ]

$(OUTPUT) : $(SRC) $(LIBS) $(HFILES) Makefile; $(CC) $(SRC) $(CFLAGS) -o $@

clean : ; @rm -f $(OUTPUT)

debug : ; @$(MAKE) all
debug : export DEBUG=1

.PHONY: all clean check debug
