TARGET  := churn
SRCS    := $(wildcard *.c)
OBJS    := ${SRCS:.c=.o}
DEPS    := $(addprefix ., ${SRCS:.c=.dep})
XDEPS   := $(wildcard ${DEPS})

CCFLAGS = -std=c89 -Wall -Wextra -pedantic
LDFLAGS =

CFG = debug

ifeq ($(CFG),debug)
CCFLAGS += -g
LDFLAGS += -g
else ifeq ($(CFG),release)
CCFLAGS += -O3 -s
LDFLAGS += -O3
endif



.PHONY: all clean distclean
all:: ${TARGET}

ifneq (${XDEPS},)
include ${XDEPS}
endif

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.c .%.dep
	${CC} ${CCFLAGS} -o $@ -c $<

${DEPS}: .%.dep: %.c Makefile
	${CC} ${CCFLAGS} -MM $< > $@

clean::
	-rm -f *~ *.o *.dep ${TARGET}

distclean:: clean