
CC ?= gcc
BUILD_DIR ?= ./build
SRC_DIR ?= ./src
TEST_DIR ?= ./test
TEST_BUILD_DIR ?= ${BUILD_DIR}/test
TEST_MAKEFILE = ${TEST_BUILD_DIR}/MakefileTestSupport
OBJ ?= ${BUILD_DIR}/obj
OBJ_DIR = ${OBJ}
CMOCK = /Users/peterco/Projects/Personal/terminals/cmock
# Trying to get at the internal.h and other headers files required for the mock web server to compile
# INCLUDEPATHS = -I$$PATH_TO_LIBMHD_INCLUDES  -I ../libmicrohttpd//libmicrohttpd-0.9.59/src/include/ -I ../libmicrohttpd/libmicrohttpd-0.9.59/
INCLUDEPATHS = -I$$PATH_TO_LIBMHD_INCLUDES
LIBPATHS = -L$$PATH_TO_LIBMHD_LIBS
LDFLAGS = -lmicrohttpd

default: all

all: setup test ${BUILD_DIR}/main run

setup:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${OBJ}
	ruby $(CMOCK)/scripts/create_makefile.rb --silent

clean:
	rm -rf ${BUILD_DIR}

${BUILD_DIR}/main: ${SRC_DIR}/main.c ${SRC_DIR}/web_server.c
	${CC} $^ -o $@  ${INCLUDEPATHS} ${LIBPATHS} ${LDFLAGS}

run:
	./build/main || true

test: setup

-include ${TEST_MAKEFILE}
