UNAME ?= $(shell uname)
R3_CONFIGURE_OPT ?=

CFLAGS := -O3 -g -Wall -fpic

C_SO_NAME := r3.so
LDFLAGS := -shared

BUILDER_IMAGE = lua-resty-libr3-builder

# on Mac OS X, one should set instead:
# for Mac OS X environment, use one of options
ifeq ($(UNAME),Darwin)
	LDFLAGS := -bundle -undefined dynamic_lookup
	R3_CONFIGURE_OPT := --host=x86_64
endif

OBJS := lua_r3.o
R3_FOLDER := r3
R3_CONGIGURE := $(R3_FOLDER)/configure
R3_STATIC_LIB := $(R3_FOLDER)/.libs/libr3.a

.PHONY: default
default: compile

### clean:        Remove generated files
.PHONY: clean
clean:
	rm -rf $(R3_FOLDER)
	rm -f $(C_SO_NAME) $(OBJS) ${R3_CONGIGURE}

### compile:      Compile library
.PHONY: compile

compile: ${R3_FOLDER} ${R3_CONGIGURE} ${R3_STATIC_LIB} $(C_SO_NAME)

${OBJS} : %.o : %.c
	$(CC) $(CFLAGS) -c $<

${C_SO_NAME} : ${OBJS}
	$(CC) $(LDFLAGS) $(OBJS) $(R3_FOLDER)/.libs/libr3.a -o $@ -lpcre  

${R3_FOLDER} :
	git clone -b 2.0.3-iresty https://github.com/iresty/r3.git

${R3_CONGIGURE} :
	cd $(R3_FOLDER) && ./autogen.sh

${R3_STATIC_LIB} :
	cd $(R3_FOLDER) && ./configure $(R3_CONFIGURE_OPT) && make

### help:         Show Makefile rules
.PHONY: help
help:
	@echo Makefile rules:
	@echo
	@grep -E '^### [-A-Za-z0-9_]+:' Makefile | sed 's/###/   /'
