# Set CPPFLAGS environment variable for debugging:
#
# 	export CPPFLAGS=-gdwarf-2
#
# Created by Samvel Khalatyan, Aug 03, 2011
# Copyright 2011, All rights reserved

CXX ?= g++

# Subsystems that have compilable libraries
subsys   = 
lib      = libbsm_input.so.1.2

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file except programs listed in PROGS
heads    = $(wildcard ./interface/*.h)
srcs     = $(filter-out %.pb.cc,$(wildcard ./src/*.cc))

objs       = $(foreach obj,$(addprefix ./obj/,$(patsubst %.cc,%.o,$(notdir ${srcs}))),${obj})

protocs    = $(wildcard ./proto/*.proto)
messages   = $(subst ./proto/,./message/,$(patsubst %.proto,%.pb.h,${protocs}))
protocobjs = $(subst ./proto/,./obj/,$(patsubst %.proto,%.o,${protocs}))

# List of programs with main functions to be filtered out of objects
progs    = $(patsubst ./src/%.cpp,%,$(wildcard ./src/*.cpp))

CPPFLAGS += ${debug} -fPIC -pipe -Wall -I../ -I$(shell root-config --incdir)
LDFLAGS  += $(shell root-config --libs) -lprotobuf -lboost_system -lboost_filesystem
ifeq ($(shell uname),Linux)
	LDFLAGS  +=  -L/usr/lib64
else
	CPPFLAGS += -I/opt/local/include
	LDFLAGS  += -L/opt/local/lib
endif

# Rules to be always executed: empty ones
.PHONY: all

all: pb obj lib

help:
	@echo "make <rule>"
	@echo
	@echo "Rules"
	@echo "-----"
	@echo
	@echo "  all        compile executables"
	@echo

pb: ${messages} ${protocobjs}

obj: ${objs}

lib: ${lib}

prog: ${progs}



# Protocol Buffers
#
${messages}:
	@echo "[+] Generating Protocol Buffers ..."
	protoc -I=proto --cpp_out message $(patsubst message/%.pb.h,proto/%.proto,$@)
	@pushd ./interface &> /dev/null; ln -s ../$@; popd &> /dev/null
	@pushd src &> /dev/null; ln -s ../$(patsubst %.h,%.cc,$@); popd &> /dev/null
	@echo

${protocobjs}: ${messages}
	@echo "[+] Compiling Protocol Buffers ..."
	${CXX} ${CPPFLAGS} -I./message -c $(addprefix ./message/,$(patsubst %.o,%.pb.cc,$(notdir $@))) -o $@
	@echo



# Regular compilcation
#
${objs}: pb ${srcs} ${heads}
	@echo "[+] Compiling objects ..."
	${CXX} ${CPPFLAGS} -c $(addprefix ./src/,$(patsubst %.o,%.cc,$(notdir $@))) -o $@
	@echo



# Libraries
#
${lib}: ${objs}
	@echo "[+] Generating Library ..."
	${CXX} -shared -W1,-soname,$(basename $@) ${LDFLAGS} -o $(addprefix ./lib/,$@) ${objs} ${protocobjs}
	@cd ./lib; ln -fs $@ $(basename $@); ln -fs $(basename $@) $(basename $(basename $@))
	@echo



# Executables
#
${progs}: ${objs} 
	@echo "[+] Compiling programs ..."
	${CXX} ${CPPFLAGS} -c src/$@.cpp -o ./obj/$@.o
	${CXX} ${LDFLAGS} $(addprefix ./lib/,${lib}) ./obj/$@.o -o ./bin/bsm_$@
	@echo



# Cleaning
#
cleanbin:
ifneq ($(strip ${progs}),)
	rm -f $(addprefix ./bin/test_,${progs})
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f ./message/*.pb.{h,cc}
	rm -f ./interface/*.pb.h
	rm -f ./src/*.pb.cc
	rm -f $(addprefix ./lib/,$(basename $(basename ${lib}))*)
