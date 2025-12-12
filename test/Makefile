PROGRAM := test_pa
INCDIR := ../include/
SRCDIR := ./
LIBDIR := lib
OUTDIR := build
TARGET := $(OUTDIR)/$(PROGRAM)
SRCS = $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/$(LIBDIR)/*.cc)
OBJS = $(addprefix $(OUTDIR)/, $(patsubst %.cc, %.o, $(SRCS)))

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)
ROOTGLIBS = $(shell root-config --glibs)

CXX = g++
CXXFLAGS = -Wall -I$(INCDIR)
#$(ROOTCFLAGS) 
CXXLIBS = $(ROOTLIBS) $(ROOTGLIBS) -lMinuit2

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^

$(OUTDIR)/%.o : %.cc
	@if [ ! -e `dirname $@` ] ; then mkdir -p `dirname $@`; fi
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OUTDIR)
