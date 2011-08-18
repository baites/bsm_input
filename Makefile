# Set CPPFLAGS environment variable for debugging:
#
# 	export CPPFLAGS=-gdwarf-2
#
# Created by Samvel Khalatyan, Aug 03, 2011
# Copyright 2011, All rights reserved

CXX ?= g++

# Subsystems that have compilable libraries
#
subsys   = 
lib      = ./lib/libbsm_input.so.1.2

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file except programs listed in PROGS
#
heads = $(filter-out %.pb.h,$(wildcard ./interface/*.h))
srcs = $(filter-out %.pb.cc,$(wildcard ./src/*.cc))

objs = $(foreach obj,${srcs},$(addprefix ./obj/,$(patsubst %.cc,%.o,$(notdir ${obj}))))

protocs = $(wildcard proto/*.proto)
messages = $(foreach pb,${protocs},$(patsubst proto/%.proto,message/%.pb.h,${pb}))
protocobjs = $(foreach pb,${protocs},$(patsubst proto/%.proto,obj/%.o,${pb}))

# List of programs with main functions to be filtered out of objects
#
progs = $(foreach prog,$(wildcard ./src/*.cpp),$(addprefix ./bin/bsm_,$(patsubst ./src/%.cpp,%,${prog})))

CPPFLAGS += ${debug} -fPIC -pipe -Wall -I../ -I$(shell root-config --incdir)
LDFLAGS += $(shell root-config --libs) -lprotobuf -lboost_system -lboost_filesystem
ifeq ($(shell uname),Linux)
	LDFLAGS  +=  -L/usr/lib64
else
	CPPFLAGS += -I/opt/local/include
	LDFLAGS  += -L/opt/local/lib
endif

#CXXFLAGS = ${DEBUG} -fPIC -pipe -Wall -I../  -I/opt/local/include/ -I${BOOST_ROOT}/include -I${ROOTSYS}/include
#LIBS     = -L/usr/lib64 -L${PROTOBUFLIB} -lprotobuf -L${BOOST_ROOT}lib -lboost_system -lboost_filesystem
#LDFLAGS  = `root-config --libs` -L/usr/lib64 -L/opt/local/lib -lprotobuf -L${BOOST_ROOT}lib -lboost_system -lboost_filesystem

# Rules to be always executed: empty ones
#
.PHONY: lib

lib: ${lib}

all: prog

pb: ${protocobjs}

obj: ${objs}

prog: ${progs}


# Protocol Buffers
#
${messages}: message/%.pb.h: proto/%.proto
	@echo "[+] Generating Protocol Buffers $@ ..."
	protoc -I=proto --cpp_out message $(patsubst message/%.pb.h,proto/%.proto,$@)
	@pushd ./interface &> /dev/null; ln -fs ../$@; popd &> /dev/null
	@pushd src &> /dev/null; ln -fs ../$(patsubst %.h,%.cc,$@); popd &> /dev/null
	@echo

${protocobjs}: ${messages}
	@echo "[+] Compiling Protocol Buffers $@ ..."
	${CXX} ${CPPFLAGS} -I./message -c $(patsubst obj/%.o,message/%.pb.cc,$@) -o $@
	@echo



# Regular compilcation
#
${objs}: obj/%.o: src/%.cc interface/%.h ${protocobjs}
	@echo "[+] Compiling objects $@ ..."
	${CXX} ${CPPFLAGS} -c $(addprefix ./src/,$(patsubst %.o,%.cc,$(notdir $@))) -o $@
	@echo



# Libraries
#
${lib}: ${objs}
	@echo "[+] Generating Library $@ ..."
	$(eval lib_name=$(notdir $@))
	${CXX} -shared -W1,-soname,${lib_name} ${LDFLAGS} -o $@ ${objs} ${protocobjs}
	@cd ./lib; ln -fs ${lib_name} $(basename ${lib_name}); ln -fs $(basename ${lib_name}) $(basename $(basename ${lib_name}))

# Executables
#
${progs}: bin/bsm_%: src/%.cpp ${lib}
	@echo "[+] Compiling programs $@ ..."
	$(eval prog_name=$(patsubst bin/bsm_%,%,$@))
	${CXX} ${CPPFLAGS} -c src/${prog_name}.cpp -o ./obj/${prog_name}.o
	${CXX} ${LDFLAGS} ${lib} ./obj/${prog_name}.o -o $@
	@echo



# Cleaning
#
cleanbin:
ifneq ($(strip ${progs}),)
	rm -f ./bin/bsm_*
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f ./message/*.pb.{h,cc}
	rm -f ./interface/*.pb.h
	rm -f ./src/*.pb.cc
	rm -f ./lib/*
