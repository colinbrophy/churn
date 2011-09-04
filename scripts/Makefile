SRCS    := $(wildcard *.c)
TARGETS := ${SRCS:.c=}
DEPS    := $(addprefix ., ${SRCS:.c=.dep})
XDEPS   := $(wildcard ${DEPS})

CCFLAGS = -std=c89 -Wall -Wextra -pedantic
LDFLAGS =

CFG = release

ifeq ($(CFG),debug)
CCFLAGS += -g
else ifeq ($(CFG),release)
CCFLAGS += -O3 -s
endif



.PHONY: all clean distclean
all:: ${TARGETS}

ifneq (${XDEPS},)
include ${XDEPS}
endif

${DEPS}: .%.dep: %.c Makefile
	${CC} ${CCFLAGS} -MM $< | sed -e "s/\.o//g" > $@

${TARGETS}: % : %.c .%.dep Makefile ../utils.c
	${CC} ${CCFLAGS} $< ../utils.c -o $@


distclean:: clean